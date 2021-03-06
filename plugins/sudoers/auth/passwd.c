/*
 * Copyright (c) 1999-2005, 2010-2014 Todd C. Miller <Todd.Miller@courtesan.com>
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
 *
 * Sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F39502-99-1-0512.
 */

#include <config.h>

#include <sys/types.h>
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
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif /* HAVE_STRINGS_H */
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <pwd.h>

#include "sudoers.h"
#include "sudo_auth.h"

#define DESLEN			13
#define HAS_AGEINFO(p, l)	(l == 18 && p[DESLEN] == ',')

int
sudo_passwd_init(struct passwd *pw, sudo_auth *auth)
{
    debug_decl(sudo_passwd_init, SUDOERS_DEBUG_AUTH, sudoers_debug_instance)

#ifdef HAVE_SKEYACCESS
    if (skeyaccess(pw, user_tty, NULL, NULL) == 0)
	debug_return_int(AUTH_FAILURE);
#endif
    sudo_setspent();
    auth->data = sudo_getepw(pw);
    sudo_endspent();
    debug_return_int(AUTH_SUCCESS);
}

int
sudo_passwd_verify(struct passwd *pw, char *pass, sudo_auth *auth)
{
    char sav, *epass;
    char *pw_epasswd = auth->data;
    size_t pw_len;
    int matched = 0;
    debug_decl(sudo_passwd_verify, SUDOERS_DEBUG_AUTH, sudoers_debug_instance)

    pw_len = strlen(pw_epasswd);

#ifdef HAVE_GETAUTHUID
    /* Ultrix shadow passwords may use crypt16() */
    epass = (char *) crypt16(pass, pw_epasswd);
    if (epass != NULL && strcmp(pw_epasswd, epass) == 0)
	debug_return_int(AUTH_SUCCESS);
#endif /* HAVE_GETAUTHUID */

    /*
     * Truncate to 8 chars if standard DES since not all crypt()'s do this.
     * If this turns out not to be safe we will have to use OS #ifdef's (sigh).
     */
    sav = pass[8];
    if (pw_len == DESLEN || HAS_AGEINFO(pw_epasswd, pw_len))
	pass[8] = '\0';

    /*
     * Normal UN*X password check.
     * HP-UX may add aging info (separated by a ',') at the end so
     * only compare the first DESLEN characters in that case.
     */
    epass = (char *) crypt(pass, pw_epasswd);
    pass[8] = sav;
    if (epass != NULL) {
	if (HAS_AGEINFO(pw_epasswd, pw_len) && strlen(epass) == DESLEN)
	    matched = !strncmp(pw_epasswd, epass, DESLEN);
	else
	    matched = !strcmp(pw_epasswd, epass);
    }

    debug_return_int(matched ? AUTH_SUCCESS : AUTH_FAILURE);
}

int
sudo_passwd_cleanup(pw, auth)
    struct passwd *pw;
    sudo_auth *auth;
{
    char *pw_epasswd = auth->data;
    debug_decl(sudo_passwd_cleanup, SUDOERS_DEBUG_AUTH, sudoers_debug_instance)

    if (pw_epasswd != NULL) {
	memset_s(pw_epasswd, SUDO_CONV_REPL_MAX, 0, strlen(pw_epasswd));
	sudo_efree(pw_epasswd);
    }
    debug_return_int(AUTH_SUCCESS);
}
