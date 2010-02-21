#
# Copyright (c) 2010 Todd C. Miller <Todd.Miller@courtesan.com>
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
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# XXX - add plugins/sudoers
SUBDIRS = src plugins/sample doc

all install: config.status
	for d in $(SUBDIRS); do (cd $$d && $(MAKE) $@); done

config.status:
	@if [ ! -s config.status ]; then \
		echo "Please run configure first"; \
		exit 1; \
	fi

clean: config.status
	-rm -f compat/*.o
	for d in $(SUBDIRS); do (cd $$d && $(MAKE) $@); done

mostlyclean: clean

distclean: config.status
	-rm -rf compat/*.o pathnames.h config.h config.status config.cache \
		config.log libtool stamp-h* autom4te.cache
	for d in $(SUBDIRS); do (cd $$d && $(MAKE) $@); done

cleandir: distclean

clobber: distclean

realclean: distclean
