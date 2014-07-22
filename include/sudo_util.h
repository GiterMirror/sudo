/*
 * Copyright (c) 2013-2014 Todd C. Miller <Todd.Miller@courtesan.com>
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

#ifndef _SUDO_UTIL_H
#define _SUDO_UTIL_H

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# include "compat/stdbool.h"
#endif /* HAVE_STDBOOL_H */

/*
 * Macros for operating on struct timeval.
 */
#define sudo_timevalclear(tv)	((tv)->tv_sec = (tv)->tv_usec = 0)

#define sudo_timevalisset(tv)	((tv)->tv_sec || (tv)->tv_usec)

#define sudo_timevalcmp(tv1, tv2, op)					       \
    (((tv1)->tv_sec == (tv2)->tv_sec) ?					       \
	((tv1)->tv_usec op (tv2)->tv_usec) :				       \
	((tv1)->tv_sec op (tv2)->tv_sec))

#define sudo_timevaladd(tv1, tv2, tv3)					       \
    do {								       \
	(tv3)->tv_sec = (tv1)->tv_sec + (tv2)->tv_sec;			       \
	(tv3)->tv_usec = (tv1)->tv_usec + (tv2)->tv_usec;		       \
	if ((tv3)->tv_usec >= 1000000) {				       \
	    (tv3)->tv_sec++;						       \
	    (tv3)->tv_usec -= 1000000;					       \
	}								       \
    } while (0)

#define sudo_timevalsub(tv1, tv2, tv3)					       \
    do {								       \
	(tv3)->tv_sec = (tv1)->tv_sec - (tv2)->tv_sec;			       \
	(tv3)->tv_usec = (tv1)->tv_usec - (tv2)->tv_usec;		       \
	if ((tv3)->tv_usec < 0) {					       \
	    (tv3)->tv_sec--;						       \
	    (tv3)->tv_usec += 1000000;					       \
	}								       \
    } while (0)

#ifndef TIMEVAL_TO_TIMESPEC
# define TIMEVAL_TO_TIMESPEC(tv, ts)					       \
    do {								       \
	(ts)->tv_sec = (tv)->tv_sec;					       \
	(ts)->tv_nsec = (tv)->tv_usec * 1000;				       \
    } while (0)
#endif

/*
 * Macros for operating on struct timespec.
 */
#define sudo_timespecclear(ts)	((ts)->tv_sec = (ts)->tv_nsec = 0)

#define sudo_timespecisset(ts)	((ts)->tv_sec || (ts)->tv_nsec)

#define sudo_timespeccmp(ts1, ts2, op)					       \
    (((ts1)->tv_sec == (ts2)->tv_sec) ?					       \
	((ts1)->tv_nsec op (ts2)->tv_nsec) :				       \
	((ts1)->tv_sec op (ts2)->tv_sec))

#define sudo_timespecadd(ts1, ts2, ts3)					       \
    do {								       \
	(ts3)->tv_sec = (ts1)->tv_sec + (ts2)->tv_sec;			       \
	(ts3)->tv_nsec = (ts1)->tv_nsec + (ts2)->tv_nsec;		       \
	while ((ts3)->tv_nsec >= 1000000000) {				       \
	    (ts3)->tv_sec++;						       \
	    (ts3)->tv_nsec -= 1000000000;				       \
	}								       \
    } while (0)

#define sudo_timespecsub(ts1, ts2, ts3)					       \
    do {								       \
	(ts3)->tv_sec = (ts1)->tv_sec - (ts2)->tv_sec;			       \
	(ts3)->tv_nsec = (ts1)->tv_nsec - (ts2)->tv_nsec;		       \
	while ((ts3)->tv_nsec < 0) {					       \
	    (ts3)->tv_sec--;						       \
	    (ts3)->tv_nsec += 1000000000;				       \
	}								       \
    } while (0)

#ifndef TIMESPEC_TO_TIMEVAL
# define TIMESPEC_TO_TIMEVAL(tv, ts)					       \
    do {								       \
	(tv)->tv_sec = (ts)->tv_sec;					       \
	(tv)->tv_usec = (ts)->tv_nsec / 1000;				       \
    } while (0)
#endif

/*
 * Macros to extract ctime and mtime as timevals.
 */
#ifdef HAVE_ST_MTIM
# ifdef HAVE_ST__TIM
#  define ctim_get(_x, _y)	TIMESPEC_TO_TIMEVAL((_y), &(_x)->st_ctim.st__tim)
#  define mtim_get(_x, _y)	TIMESPEC_TO_TIMEVAL((_y), &(_x)->st_mtim.st__tim)
# else
#  define ctim_get(_x, _y)	TIMESPEC_TO_TIMEVAL((_y), &(_x)->st_ctim)
#  define mtim_get(_x, _y)	TIMESPEC_TO_TIMEVAL((_y), &(_x)->st_mtim)
# endif
#else
# ifdef HAVE_ST_MTIMESPEC
#  define ctim_get(_x, _y)	TIMESPEC_TO_TIMEVAL((_y), &(_x)->st_ctimespec)
#  define mtim_get(_x, _y)	TIMESPEC_TO_TIMEVAL((_y), &(_x)->st_mtimespec)
# else
#  define ctim_get(_x, _y)	do { (_y)->tv_sec = (_x)->st_ctime; (_y)->tv_usec = 0; } while (0)
#  define mtim_get(_x, _y)	do { (_y)->tv_sec = (_x)->st_mtime; (_y)->tv_usec = 0; } while (0)
# endif /* HAVE_ST_MTIMESPEC */
#endif /* HAVE_ST_MTIM */

/*
 * Macros to quiet gcc's warn_unused_result attribute.
 */
#ifdef __GNUC__
# define ignore_result(x) do {						       \
    __typeof__(x) y = (x);						       \
    (void)y;								       \
} while(0)
#else
# define ignore_result(x)	(void)(x)
#endif

/* aix.c */
__dso_public int aix_prep_user_v1(char *user, const char *tty);
#define aix_prep_user(_a, _b) aix_prep_user_v1((_a), (_b))
__dso_public int aix_restoreauthdb_v1(void);
#define aix_restoreauthdb() aix_restoreauthdb_v1()
__dso_public int aix_setauthdb_v1(char *user);
#define aix_setauthdb(_a) aix_setauthdb_v1((_a))

/* gidlist.c */
__dso_public int sudo_parse_gids_v1(const char *gidstr, const gid_t *basegid, GETGROUPS_T **gidsp);
#define sudo_parse_gids(_a, _b, _c) sudo_parse_gids_v1((_a), (_b), (_c))

/* key_val.c */
__dso_public char *sudo_new_key_val_v1(const char *key, const char *value);
#define sudo_new_key_val(_a, _b) sudo_new_key_val_v1((_a), (_b))

/* locking.c */
#define SUDO_LOCK	1		/* lock a file */
#define SUDO_TLOCK	2		/* test & lock a file (non-blocking) */
#define SUDO_UNLOCK	4		/* unlock a file */
__dso_public bool sudo_lock_file_v1(int fd, int action);
#define sudo_lock_file(_a, _b) sudo_lock_file_v1((_a), (_b))

/* parseln.c */
__dso_public ssize_t sudo_parseln_v1(char **buf, size_t *bufsize, unsigned int *lineno, FILE *fp);
#define sudo_parseln(_a, _b, _c, _d) sudo_parseln_v1((_a), (_b), (_c), (_d))

/* progname.c */
__dso_public void initprogname(const char *);

/* setgroups.c */
__dso_public int sudo_setgroups_v1(int ngids, const GETGROUPS_T *gids);
#define sudo_setgroups(_a, _b) sudo_setgroups_v1((_a), (_b))

/* strtobool.c */
__dso_public int sudo_strtobool_v1(const char *str);
#define sudo_strtobool(_a) sudo_strtobool_v1((_a))

/* strtoid.c */
__dso_public id_t sudo_strtoid_v1(const char *str, const char *sep, char **endp, const char **errstr);
#define sudo_strtoid(_a, _b, _c, _d) sudo_strtoid_v1((_a), (_b), (_c), (_d))

/* strtomode.c */
__dso_public int sudo_strtomode_v1(const char *cp, const char **errstr);
#define sudo_strtomode(_a, _b) sudo_strtomode_v1((_a), (_b))

/* term.c */
__dso_public bool sudo_term_cbreak_v1(int fd);
#define sudo_term_cbreak(_a) sudo_term_cbreak_v1((_a))
__dso_public bool sudo_term_copy_v1(int src, int dst);
#define sudo_term_copy(_a, _b) sudo_term_copy_v1((_a), (_b))
__dso_public bool sudo_term_noecho_v1(int fd);
#define sudo_term_noecho(_a) sudo_term_noecho_v1((_a))
__dso_public bool sudo_term_raw_v1(int fd, int isig);
#define sudo_term_raw(_a, _b) sudo_term_raw_v1((_a), (_b))
__dso_public bool sudo_term_restore_v1(int fd, bool flush);
#define sudo_term_restore(_a, _b) sudo_term_restore_v1((_a), (_b))

/* ttysize.c */
__dso_public void sudo_get_ttysize_v1(int *rowp, int *colp);
#define sudo_get_ttysize(_a, _b) sudo_get_ttysize_v1((_a), (_b))

#endif /* _SUDO_UTIL_H */
