#!/usr/bin/env perl
#
# Copyright (c) 2011-2013 Todd C. Miller <Todd.Miller@courtesan.com>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

use File::Temp qw/ :mktemp  /;
use Fcntl;
use warnings;

die "usage: $0 Makefile ...\n" unless $#ARGV >= 0;

my @incpaths;
my %dir_vars;
my %implicit;
my %generated;

# Read in MANIFEST fail if present
my %manifest;
if (open(MANIFEST, "<MANIFEST")) {
    while (<MANIFEST>) {
	chomp;
	next unless /([^\/]+\.[cly])$/;
	$manifest{$1} = $_;
    }
}

foreach (@ARGV) {
    mkdep($_);
}

sub mkdep {
    my $file = $_[0];
    $file =~ s:^\./+::;		# strip off leading ./

    my $makefile;
    if (open(MF, "<$file")) {
	local $/;		# enable "slurp" mode
	$makefile = <MF>;
    } else {
	warn "$0: $file: $!\n";
	return undef;
    }
    close(MF);

    # New makefile, minus the autogenerated dependencies
    my $separator = "# Autogenerated dependencies, do not modify";
    my $new_makefile = $makefile;
    $new_makefile =~ s/${separator}.*$//s;
    $new_makefile .= "$separator\n";

    # Old makefile, join lines with continuation characters
    $makefile =~ s/\\\n//mg;

    # Expand some configure bits
    $makefile =~ s:\@DEV\@::g;
    $makefile =~ s:\@COMMON_OBJS\@:aix.lo event_poll.lo event_select.lo:;
    $makefile =~ s:\@SUDO_OBJS\@:openbsd.o preload.o selinux.o sesh.o solaris.o sudo_noexec.lo:;
    $makefile =~ s:\@SUDOERS_OBJS\@:bsm_audit.lo linux_audit.lo ldap.lo sssd.lo:;
    # XXX - fill in AUTH_OBJS from contents of the auth dir instead
    $makefile =~ s:\@AUTH_OBJS\@:afs.lo aix_auth.lo bsdauth.lo dce.lo fwtk.lo getspwuid.lo kerb5.lo pam.lo passwd.lo rfc1938.lo secureware.lo securid5.lo sia.lo:;
    $makefile =~ s:\@LTLIBOBJS\@:closefrom.lo dlopen.lo fnmatch.lo getaddrinfo.lo getcwd.lo getgrouplist.lo getline.lo getprogname.lo getopt_long.lo glob.lo isblank.lo memrchr.lo memset_s.lo mksiglist.lo mksigname.lo mktemp.lo pw_dup.lo sig2str.lo siglist.lo signame.lo snprintf.lo strlcat.lo strlcpy.lo strsignal.lo utimes.lo globtest.o fnm_test.o:;

    # Parse OBJS lines
    my %objs;
    while ($makefile =~ /^[A-Z0-9_]*OBJS\s*=\s*(.*)/mg) {
	foreach (split/\s+/, $1) {
	    next if /^\$[\(\{].*[\)\}]$/; # skip included vars for now
	    $objs{$_} = 1;
	}
    }

    # Find include paths
    @incpaths = ();
    while ($makefile =~ /-I(\S+)/mg) {
	push(@incpaths, $1) unless $1 eq ".";
    }

    # Check for generated files
    if ($makefile =~ /GENERATED\s*=\s*(.+)$/m) {
	foreach (split(/\s+/, $1)) {
	    $generated{$_} = 1;
	}
    }

    # Values of srcdir, top_srcdir, top_builddir, incdir
    %dir_vars = ();
    $file =~ m:^(.*)/+[^/]+:;
    $dir_vars{'srcdir'} = $1 || '.';
    $dir_vars{'devdir'} = $dir_vars{'srcdir'};
    $dir_vars{'authdir'} = $dir_vars{'srcdir'} . "/auth";
    $dir_vars{'top_srcdir'} = '.';
    #$dir_vars{'top_builddir'} = '.';
    $dir_vars{'incdir'} = 'include';

    # Find implicit rules for generated .o and .lo files
    %implicit = ();
    while ($makefile =~ /^\.c\.(l?o):\s*\n\t+(.*)$/mg) {
	$implicit{$1} = $2;
    }

    # Find existing .o and .lo dependencies
    my %old_deps;
    while ($makefile =~ /^(\w+\.l?o):\s*(\S+\.c)/mg) {
	$old_deps{$1} = $2;
    }

    # Sort files so we do .lo files first
    foreach my $obj (sort keys %objs) {
	next unless $obj =~ /(\S+)\.(l?o)$/;
	if ($2 eq "o" && exists($objs{"$1.lo"})) {
	    # If we have both .lo and .o files, make the .o depend on the .lo
	    $new_makefile .= sprintf("%s: %s.lo\n", $obj, $1);
	} else {
	    # Use old depenencies when mapping objects to their source.
	    # If no old depenency, use the MANIFEST file to find the source.
	    my $src = $1 . '.c';
	    my $ext = $2;
	    if (exists $old_deps{$obj}) {
		$src = $old_deps{$obj};
	    } elsif (exists $manifest{$src}) {
		$src = $manifest{$src};
		foreach (sort { length($b) <=> length($a) } keys %dir_vars) {
		    next if $_ eq "devdir";
		    last if $src =~ s:^\Q$dir_vars{$_}/\E:\$\($_\)/:;
		}
	    } else {
		warn "$file: unable to find source for $obj\n";
	    }
	    my $imp = $implicit{$ext};
	    $imp =~ s/\$</$src/g;

	    my $deps = sprintf("%s: %s %s", $obj, $src,
		join(' ', find_depends($src)));
	    if (length($deps) > 80) {
		my $off = 0;
		my $indent = length($obj) + 2;
		while (length($deps) - $off > 80 - $indent) {
		    my $pos;
		    if ($off != 0) {
			$new_makefile .= ' ' x $indent;
			$pos = rindex($deps, ' ', $off + 80 - $indent - 2);
		    } else {
			$pos = rindex($deps, ' ', $off + 78);
		    }
		    $new_makefile .= substr($deps, $off, $pos - $off) . " \\\n";
		    $off = $pos + 1;
		}
		$new_makefile .= ' ' x $indent;
		$new_makefile .= substr($deps, $off) . "\n";
	    } else {
		$new_makefile .= "$deps\n";
	    }
	    $new_makefile .= "\t$imp\n";
	}
    }

    my $newfile = $file . ".new";
    if (!open(MF, ">$newfile")) {
	warn("cannot open $newfile: $!\n");
    } else {
	print MF $new_makefile || warn("cannot write $newfile: $!\n");
	close(MF) || warn("cannot close $newfile: $!\n");;
	rename($newfile, $file);
    }
}

exit(0);

sub find_depends {
    my $src = $_[0];
    my ($deps, $code, %headers);

    if ($src !~ /\//) {
	# XXX - want build dir not src dir
	$src = "$dir_vars{'srcdir'}/$src";
    }

    # resolve $(srcdir) etc.
    foreach (keys %dir_vars) {
	$src =~ s/\$[\(\{]$_[\)\}]/$dir_vars{$_}/g;
    }

    # find open source file and find headers used by it
    if (!open(FILE, "<$src")) {
	warn "unable to open $src\n";
	return "";
    }
    local $/;		# enable "slurp" mode
    $code = <FILE>;
    close(FILE);

    # find all headers
    while ($code =~ /^#\s*include\s+["<](\S+)[">]/mg) {
	my ($hdr, $hdr_path) = find_header($1);
	if (defined($hdr)) {
	    $headers{$hdr} = 1;
	    # Look for other includes in the .h file
	    foreach (find_depends($hdr_path)) {
		$headers{$_} = 1;
	    }
	}
    }

    sort keys %headers;
}

# find the path to a header file
# returns path or undef if not found
sub find_header {
    my $hdr = $_[0];

    # Look for .h.in files in top_builddir and build dir
    return ("\$(top_builddir\)/$hdr", "./${hdr}.in") if -r "./${hdr}.in";
    return ("./$hdr", "$dir_vars{'srcdir'}/${hdr}.in") if -r "$dir_vars{'srcdir'}/${hdr}.in";

    if (exists $generated{$hdr}) {
	my $hdr_path = $dir_vars{'devdir'} . '/' . $hdr;
	return ('$(devdir)/' . $hdr, $hdr_path) if -r $hdr_path;
    }
    foreach my $inc (@incpaths) {
	my $hdr_path = "$inc/$hdr";
	# resolve variables in include path
	foreach (keys %dir_vars) {
	    next if $_ eq "devdir";
	    $hdr_path =~ s/\$[\(\{]$_[\)\}]/$dir_vars{$_}/g;
	}
	return ("$inc/$hdr", $hdr_path) if -r $hdr_path;
    }

    undef;
}
