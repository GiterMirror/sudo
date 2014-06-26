/*
 * Copyright (c) 2010, 2011, 2013, 2014
 *	Todd C. Miller <Todd.Miller@courtesan.com>
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

#ifndef _SUDO_FILEOPS_H
#define _SUDO_FILEOPS_H

/*
 * Flags for lock_file()
 */
#define SUDO_LOCK	1		/* lock a file */
#define SUDO_TLOCK	2		/* test & lock a file (non-blocking) */
#define SUDO_UNLOCK	4		/* unlock a file */

struct timeval;

__dso_public bool sudo_lock_file(int, int);
__dso_public ssize_t  sudo_parseln(char **buf, size_t *bufsize, unsigned int *lineno, FILE *fp);

#endif /* _SUDO_FILEOPS_H */
