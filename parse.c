/*
 *  CU sudo version 1.3.1
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Please send bugs, changes, problems to sudo-bugs@cs.colorado.edu
 *
 *******************************************************************
 *
 * parse.c -- sudo parser frontend and comparison routines.
 *
 * Chris Jepeway <jepeway@cs.utk.edu>
 */

#ifndef lint
static char rcsid[] = "$Id$";
#endif /* lint */

#include "config.h"

#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#ifdef HAVE_MALLOC_H 
#include <malloc.h>
#endif /* HAVE_MALLOC_H */ 
#include <ctype.h>
#include <sys/param.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#include "sudo.h"
#include "options.h"

extern FILE *yyin, *yyout;

/*
 * Globals
 */
int parse_error = FALSE;

/*
 * this routine is called from the sudo.c module and tries to validate
 * the user, host and command triplet.
 */
int validate(check_cmnd)
    int check_cmnd;
{
    FILE *sudoers_fp;
    int i, return_code;

    /* become sudoers file owner */
    set_perms(PERM_SUDOERS);

    if ((sudoers_fp = fopen(_PATH_SUDO_SUDOERS, "r")) == NULL) {
	perror(_PATH_SUDO_SUDOERS);
	log_error(NO_SUDOERS_FILE);
	exit(1);
    }
    yyin = sudoers_fp;
    yyout = stdout;

    return_code = yyparse();

    /*
     * don't need to keep this open...
     */
    (void) fclose(sudoers_fp);

    /* relinquish extra privs */
    set_perms(PERM_ROOT);
    set_perms(PERM_USER);

    if (return_code || parse_error)
	return(VALIDATE_ERROR);

    if (top == 0)
	/*
	 * nothing on the top of the stack =>
	 * user doesn't appear in sudoers
	 */
	return(VALIDATE_NO_USER);

    /*
     * Only check the actual command if the check_cmnd
     * flag is set.  It is not set for the "validate"
     * and "list" pseudo-commands.  Always check the
     * host and user.
     */
    if (check_cmnd == FALSE)
	while (top) {
	    if (host_matches == TRUE)
		/* user may always do validate or list on allowed hosts */
		return(VALIDATE_OK);
	    top--;
	}
    else
	while (top) {
	    if (host_matches == TRUE)
		if (cmnd_matches == TRUE)
		    /* user was granted access to cmnd on host */
		    return(VALIDATE_OK);
		else if (cmnd_matches == FALSE)
		    /* user was explicitly denied acces to cmnd on host */
		    return(VALIDATE_NOT_OK);
	    top--;
	}

    /*
     * we popped everything off the stack =>
     * user was mentioned, but not explicitly
     * granted nor denied access => say no
     */
    return(VALIDATE_NOT_OK);
}



/*
 * If path doesn't end in /, return TRUE iff cmnd & path name the same inode;
 * otherwise, return TRUE if cmnd names one of the inodes in path
 */
int
path_matches(cmnd, path)
    char *cmnd, *path;
{
    int plen;
    struct stat pst;
    DIR *dirp;
    struct dirent *dent;
    char buf[MAXCOMMANDLENGTH+1];
    static char *c;

    /* only need to stat cmnd once since it never changes */
    if (cmnd_st.st_dev == 0) {
	if (stat(cmnd, &cmnd_st) < 0)
	    return(FALSE);
	if ((c = strrchr(cmnd, '/')) == NULL)
	    c = cmnd;
	else
	    c++;
    }

    plen = strlen(path);
    if (path[plen - 1] != '/') {
#ifdef FAST_MATCH
	char *p;

	/* only proceed if the basenames of cmnd and path are the same */
	if ((p = strrchr(path, '/')) == NULL)
	    p = path;
	else
	    p++;
	if (strcmp(c, p))
	    return(FALSE);
#endif /* FAST_MATCH */

	if (stat(path, &pst) < 0)
	    return(FALSE);
	return(cmnd_st.st_dev == pst.st_dev && cmnd_st.st_ino == pst.st_ino);
    }

    /* grot through path's directory entries, looking for cmnd */
    dirp = opendir(path);
    if (dirp == NULL)
	return(FALSE);

    while ((dent = readdir(dirp)) != NULL) {
	strcpy(buf, path);
	strcat(buf, dent->d_name);
#ifdef FAST_MATCH
	/* only stat if basenames are not the same */
	if (strcmp(c, dent->d_name))
	    continue;
#endif /* FAST_MATCH */
	if (stat(buf, &pst) < 0)
	    continue;
	if (cmnd_st.st_dev == pst.st_dev && cmnd_st.st_ino == pst.st_ino)
	    break;
    }

    closedir(dirp);
    return(dent != NULL);
}



int
addr_matches(n)
    char *n;
{
    int i;
    struct in_addr addr;

    addr.s_addr = inet_addr(n);

    for (i = 0; i < num_interfaces; i++)
	if (interfaces[i].addr.s_addr == addr.s_addr ||
	    (interfaces[i].addr.s_addr & interfaces[i].netmask.s_addr)
	    == addr.s_addr)
	    return(TRUE);

    return(FALSE);
}
