/*
 * Copyright (c) 2009-2010 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <config.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif /* STDC_HEADERS */
#ifdef HAVE_STRING_H
# include <string.h>
#else
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif /* HAVE_STRING_H */
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if TIME_WITH_SYS_TIME
# include <time.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#ifdef HAVE_ZLIB
# include <zlib.h>
#endif

#include "sudoers.h"

union io_fd {
    FILE *f;
#ifdef HAVE_ZLIB
    gzFile g;
#endif
    void *v;
};

struct script_buf {
    int len; /* buffer length (how much read in) */
    int off; /* write position (how much already consumed) */
    char buf[16 * 1024];
};

#define IOFD_STDIN	0
#define IOFD_STDOUT	1
#define IOFD_STDERR	2
#define IOFD_TTYIN	3
#define IOFD_TTYOUT	4
#define IOFD_TIMING	5
#define IOFD_MAX	6

#define SESSID_MAX	2176782336U

static struct timeval last_time;
static union io_fd io_fds[IOFD_MAX];
extern struct io_plugin sudoers_io;

void
io_nextid(void)
{
    struct stat sb;
    char buf[32], *ep;
    int fd, i, ch;
    unsigned long id = 0;
    int len;
    ssize_t nread;
    char pathbuf[PATH_MAX];

    /*
     * Create _PATH_SUDO_IO_LOGDIR if it doesn't already exist.
     */
    if (stat(_PATH_SUDO_IO_LOGDIR, &sb) != 0) {
	if (mkdir(_PATH_SUDO_IO_LOGDIR, S_IRWXU) != 0)
	    log_error(USE_ERRNO, "Can't mkdir %s", _PATH_SUDO_IO_LOGDIR);
    } else if (!S_ISDIR(sb.st_mode)) {
	log_error(0, "%s exists but is not a directory (0%o)",
	    _PATH_SUDO_IO_LOGDIR, (unsigned int) sb.st_mode);
    }

    /*
     * Open sequence file
     */
    len = snprintf(pathbuf, sizeof(pathbuf), "%s/seq", _PATH_SUDO_IO_LOGDIR);
    if (len <= 0 || len >= sizeof(pathbuf)) {
	errno = ENAMETOOLONG;
	log_error(USE_ERRNO, "%s/seq", pathbuf);
    }
    fd = open(pathbuf, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
    if (fd == -1)
	log_error(USE_ERRNO, "cannot open %s", pathbuf);
    lock_file(fd, SUDO_LOCK);

    /* Read seq number (base 36). */
    nread = read(fd, buf, sizeof(buf));
    if (nread != 0) {
	if (nread == -1)
	    log_error(USE_ERRNO, "cannot read %s", pathbuf);
	id = strtoul(buf, &ep, 36);
	if (buf == ep || id >= SESSID_MAX)
	    log_error(0, "invalid sequence number %s", pathbuf);
    }
    id++;

    /*
     * Convert id to a string and stash in sudo_user.sessid.
     * Note that that least significant digits go at the end of the string.
     */
    for (i = 5; i >= 0; i--) {
	ch = id % 36;
	id /= 36;
	buf[i] = ch < 10 ? ch + '0' : ch - 10 + 'A';
    }
    buf[6] = '\n';

    /* Stash id logging purposes */
    memcpy(sudo_user.sessid, buf, 6);
    sudo_user.sessid[6] = '\0';

    /* Rewind and overwrite old seq file. */
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1 || write(fd, buf, 7) != 7)
	log_error(USE_ERRNO, "Can't write to %s", pathbuf);
    close(fd);
}

static int
build_idpath(char *pathbuf, size_t pathsize)
{
    struct stat sb;
    int i, len;

    if (sudo_user.sessid[0] == '\0')
	log_error(0, "tried to build a session id path without a session id");

    /*
     * Path is of the form /var/log/sudo-session/00/00/01.
     */
    len = snprintf(pathbuf, pathsize, "%s/%c%c/%c%c/%c%c", _PATH_SUDO_IO_LOGDIR,
	sudo_user.sessid[0], sudo_user.sessid[1], sudo_user.sessid[2],
	sudo_user.sessid[3], sudo_user.sessid[4], sudo_user.sessid[5]);
    if (len <= 0 && len >= pathsize) {
	errno = ENAMETOOLONG;
	log_error(USE_ERRNO, "%s/%s", _PATH_SUDO_IO_LOGDIR, sudo_user.sessid);
    }

    /*
     * Create the intermediate subdirs as needed.
     */
    for (i = 6; i > 0; i -= 3) {
	pathbuf[len - i] = '\0';
	if (stat(pathbuf, &sb) != 0) {
	    if (mkdir(pathbuf, S_IRWXU) != 0)
		log_error(USE_ERRNO, "Can't mkdir %s", pathbuf);
	} else if (!S_ISDIR(sb.st_mode)) {
	    log_error(0, "%s: %s", pathbuf, strerror(ENOTDIR));
	}
	pathbuf[len - i] = '/';
    }

    return(len);
}

static void *
open_io_fd(char *pathbuf, int len, const char *suffix, int docompress)
{
    void *vfd = NULL;
    int fd;

    pathbuf[len] = '\0';
    strlcat(pathbuf, suffix, PATH_MAX);
    fd = open(pathbuf, O_CREAT|O_EXCL|O_WRONLY, S_IRUSR|S_IWUSR);
    if (fd != -1) {
	fcntl(fd, F_SETFD, FD_CLOEXEC);
#ifdef HAVE_ZLIB
	if (docompress)
	    vfd = gzdopen(fd, "w");
	else
#endif
	    vfd = fdopen(fd, "w");
    }
    return vfd;
}

static int
sudoers_io_open(unsigned int version, sudo_conv_t conversation,
    sudo_printf_t plugin_printf, char * const settings[],
    char * const user_info[], int argc, char * const argv[],
    char * const user_env[])
{
    char pathbuf[PATH_MAX];
    FILE *io_logfile;
    int len;

    if (!sudo_conv)
	sudo_conv = conversation;
    if (!sudo_printf)
	sudo_printf = plugin_printf;

    /* If we have no command (because -V was specified) just return. */
    if (argc == 0)
	return TRUE;

    if (!def_log_input && !def_log_output && !def_use_pty)
	return FALSE;

    /*
     * Build a path containing the session id split into two-digit subdirs,
     * so ID 000001 becomes /var/log/sudo-session/00/00/01.
     */
    len = build_idpath(pathbuf, sizeof(pathbuf));
    if (len == -1)
	return -1;

    if (mkdir(pathbuf, S_IRUSR|S_IWUSR|S_IXUSR) != 0)
	log_error(USE_ERRNO, "Can't mkdir %s", pathbuf);

    /*
     * We create 7 files: a log file, a timing file and 5 for input/output.
     */
    io_logfile = open_io_fd(pathbuf, len, "/log", FALSE);
    if (io_logfile == NULL)
	log_error(USE_ERRNO, "Can't create %s", pathbuf);

    io_fds[IOFD_TIMING].v = open_io_fd(pathbuf, len, "/timing", def_compress_io);
    if (io_fds[IOFD_TIMING].v == NULL)
	log_error(USE_ERRNO, "Can't create %s", pathbuf);

    if (def_log_input) {
	io_fds[IOFD_TTYIN].v = open_io_fd(pathbuf, len, "/ttyin", def_compress_io);
	if (io_fds[IOFD_TTYIN].v == NULL)
	    log_error(USE_ERRNO, "Can't create %s", pathbuf);

	io_fds[IOFD_STDIN].v = open_io_fd(pathbuf, len, "/stdin", def_compress_io);
	if (io_fds[IOFD_STDIN].v == NULL)
	    log_error(USE_ERRNO, "Can't create %s", pathbuf);
    } else {
	/* No input logging. */
	sudoers_io.log_ttyin = NULL;
	sudoers_io.log_stdin = NULL;
    }

    if (def_log_output) {
	io_fds[IOFD_TTYOUT].v = open_io_fd(pathbuf, len, "/ttyout", def_compress_io);
	if (io_fds[IOFD_TTYOUT].v == NULL)
	    log_error(USE_ERRNO, "Can't create %s", pathbuf);

	io_fds[IOFD_STDOUT].v = open_io_fd(pathbuf, len, "/stdout", def_compress_io);
	if (io_fds[IOFD_STDOUT].v == NULL)
	    log_error(USE_ERRNO, "Can't create %s", pathbuf);

	io_fds[IOFD_STDERR].v = open_io_fd(pathbuf, len, "/stderr", def_compress_io);
	if (io_fds[IOFD_STDERR].v == NULL)
	    log_error(USE_ERRNO, "Can't create %s", pathbuf);
    } else {
	/* No output logging. */
	sudoers_io.log_ttyout = NULL;
	sudoers_io.log_stdout = NULL;
	sudoers_io.log_stderr = NULL;
    }

    gettimeofday(&last_time, NULL);

    /* XXX - log more stuff?  window size? environment? */
    /* XXX - use passed in argv instead of using sudoers policy info. */
    fprintf(io_logfile, "%ld:%s:%s:%s:%s\n", last_time.tv_sec, user_name,
        runas_pw->pw_name, runas_gr ? runas_gr->gr_name : "", user_tty);
    fprintf(io_logfile, "%s\n", user_cwd);
    fprintf(io_logfile, "%s%s%s\n", user_cmnd, user_args ? " " : "",
        user_args ? user_args : "");
    fclose(io_logfile);

    return TRUE;
}

static void
sudoers_io_close(int exit_status, int error)
{
    int i;

    for (i = 0; i < IOFD_MAX; i++) {
#ifdef HAVE_ZLIB
	if (def_compress_io)
	    gzclose(io_fds[i].g);
	else
#endif
	    fclose(io_fds[i].f);
    }
}

static int
sudoers_io_version(int verbose)
{
    struct sudo_conv_message msg;
    struct sudo_conv_reply repl;
    char *str;

    easprintf(&str, "Sudoers I/O plugin version %s\n", PACKAGE_VERSION);

    /* Call conversation function */
    memset(&msg, 0, sizeof(msg));
    msg.msg_type = SUDO_CONV_INFO_MSG;
    msg.msg = str;
    memset(&repl, 0, sizeof(repl));
    sudo_conv(1, &msg, &repl);
    free(str);

    return TRUE;
}

static int
sudoers_io_log(const char *buf, unsigned int len, int idx)
{
    struct timeval now, tv;

    gettimeofday(&now, NULL);

#ifdef HAVE_ZLIB
    if (def_compress_io)
	gzwrite(io_fds[idx].g, buf, len);
    else
#endif
	fwrite(buf, 1, len, io_fds[idx].f);
    timersub(&now, &last_time, &tv);
#ifdef HAVE_ZLIB
    if (def_compress_io)
	gzprintf(io_fds[IOFD_TIMING].g, "%d %f %d\n", idx,
	    tv.tv_sec + ((double)tv.tv_usec / 1000000), len);
    else
#endif
	fprintf(io_fds[IOFD_TIMING].f, "%d %f %d\n", idx,
	    tv.tv_sec + ((double)tv.tv_usec / 1000000), len);
    last_time.tv_sec = now.tv_sec;
    last_time.tv_usec = now.tv_usec;

    return TRUE;
}

static int
sudoers_io_log_ttyin(const char *buf, unsigned int len)
{
    return sudoers_io_log(buf, len, IOFD_TTYIN);
}

static int
sudoers_io_log_ttyout(const char *buf, unsigned int len)
{
    return sudoers_io_log(buf, len, IOFD_TTYOUT);
}

static int
sudoers_io_log_stdin(const char *buf, unsigned int len)
{
    return sudoers_io_log(buf, len, IOFD_STDIN);
}

static int
sudoers_io_log_stdout(const char *buf, unsigned int len)
{
    return sudoers_io_log(buf, len, IOFD_STDOUT);
}

static int
sudoers_io_log_stderr(const char *buf, unsigned int len)
{
    return sudoers_io_log(buf, len, IOFD_STDERR);
}

struct io_plugin sudoers_io = {
    SUDO_IO_PLUGIN,
    SUDO_API_VERSION,
    sudoers_io_open,
    sudoers_io_close,
    sudoers_io_version,
    sudoers_io_log_ttyin,
    sudoers_io_log_ttyout,
    sudoers_io_log_stdin,
    sudoers_io_log_stdout,
    sudoers_io_log_stderr
};
