/*
 * Copyright (c) 2010 Todd C. Miller <Todd.Miller@courtesan.com>
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
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif /* HAVE_STRINGS_H */
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if TIME_WITH_SYS_TIME
# include <time.h>
#endif
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <pwd.h>

#include "sudoers.h"

static void *group_handle;
static struct sudoers_group_plugin *group_plugin;

/*
 * Load the specified plugin and run its init function.
 * Returns -1 if unable to open the plugin, else it returns
 * the value from the plugin's init function.
 */
int
group_plugin_load(char *plugin_info)
{
    struct stat sb;
    char *args, path[PATH_MAX], savedch;
    char **argv = NULL;
    size_t len;
    int rc;

    /*
     * Fill in .so path and split out args (if any).
     */
    path[0] = '\0';
    if (plugin_info[0] != '/')
	strlcpy(path, _PATH_SUDO_PLUGIN_DIR, sizeof(path));
    if ((args = strpbrk(plugin_info, " \t")) != NULL) {
	savedch = *args;
	*args = '\0';
    }
    len = strlcat(path, plugin_info, sizeof(path));
    if (args != NULL)
	*args++ = savedch;
    if (len >= sizeof(path)) {
	warningx("%s%s: %s", _PATH_SUDO_PLUGIN_DIR, plugin_info,
	    strerror(ENAMETOOLONG));
	return -1;
    }

    /* Sanity check plugin path. */
    if (stat(path, &sb) != 0) {
	warning("%s", path);
	return -1;
    }
    if (sb.st_uid != ROOT_UID) {
	warningx("%s must be owned by uid %d", path, ROOT_UID);
	return -1;
    }
    if ((sb.st_mode & (S_IWGRP|S_IWOTH)) != 0) {
	warningx("%s must be only be writable by owner", path);
	return -1;
    }

    /* Open plugin and map in symbol. */
    group_handle = dlopen(path, RTLD_LAZY);
    if (!group_handle) {
	warningx("unable to dlopen %s: %s", path, dlerror());
	return -1;
    }
    group_plugin = dlsym(group_handle, "group_plugin");
    if (group_plugin == NULL) {
	warningx("unable to find symbol \"group_plugin\" in %s", path);
	return -1;
    }

    if (GROUP_API_VERSION_GET_MAJOR(group_plugin->version) != GROUP_API_VERSION_MAJOR) {
	warningx("%s: incompatible group plugin major version %d, expected %d",
	    path, GROUP_API_VERSION_GET_MAJOR(group_plugin->version),
	    GROUP_API_VERSION_MAJOR);
	return -1;
    }

    /*
     * Split args into a vector if specified.
     */
    if (args != NULL) {
	int ac = 0, wasblank = TRUE;
	char *cp;

        for (cp = args; *cp != '\0'; cp++) {
            if (isblank((unsigned char)*cp)) {
                wasblank = TRUE;
            } else if (wasblank) {
                wasblank = FALSE;
                ac++;
            }
        }
	if (ac != 0) 	{
	    argv = emalloc2(ac, sizeof(char *));
	    ac = 0;
	    for ((cp = strtok(args, " \t")); cp; (cp = strtok(NULL, " \t")))
		argv[ac++] = cp;
	}
    }

    rc = (group_plugin->init)(GROUP_API_VERSION, sudo_printf, argv);

    efree(argv);

    return rc;
}

void
group_plugin_unload(void)
{
    (group_plugin->cleanup)();
    dlclose(group_handle);
    group_handle = NULL;
}

int
group_plugin_query(const char *user, const char *group,
    const struct passwd *pwd)
{
    return (group_plugin->query)(user, group, pwd);
}
