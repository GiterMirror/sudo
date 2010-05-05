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
#include <sys/wait.h>

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
# if defined(HAVE_MEMORY_H) && !defined(STDC_HEADERS)
#  include <memory.h>
# endif
# include <string.h>
#else
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif /* HAVE_STRING_H */
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <fcntl.h>
#include <limits.h>
#include <grp.h>
#include <pwd.h>
#include <stdarg.h>

#include <sudo_plugin.h>
#include <compat.h>
#include <missing.h>

/*
 * Sample plugin module that allows any user who knows the password
 * ("test") to run any command as root.  Since there is no credential
 * caching the validate and invalidate functions are NULL.
 */

#ifdef __TANDEM
# define ROOT_UID       65535
#else
# define ROOT_UID       0
#endif

#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0
#undef ERROR
#define ERROR -1

static struct plugin_state {
    char **envp;
    char * const *settings;
    char * const *user_info;
} plugin_state;
static sudo_conv_t sudo_conv;
static sudo_printf_t sudo_log;
static FILE *input, *output;
static uid_t runas_uid = ROOT_UID;
static gid_t runas_gid = -1;

/*
 * Allocate storage for a name=value string and return it.
 */
static char *
fmt_string(const char *var, const char *val)
{
    size_t var_len = strlen(var);
    size_t val_len = strlen(val);
    char *cp, *str;

    cp = str = malloc(var_len + 1 + val_len + 1);
    if (str != NULL) {
	memcpy(cp, var, var_len);
	cp += var_len;
	*cp++ = '=';
	memcpy(cp, val, val_len);
	cp += val_len;
	*cp = '\0';
    }

    return(str);
}

/*
 * Plugin policy open function.
 */
static int
policy_open(unsigned int version, sudo_conv_t conversation,
    sudo_printf_t sudo_printf, char * const settings[],
    char * const user_info[], char * const user_env[])
{
    char * const *ui;
    struct passwd *pw;
    const char *runas_user = NULL;
    struct group *gr;
    const char *runas_group = NULL;

    if (!sudo_conv)
	sudo_conv = conversation;
    if (!sudo_log)
	sudo_log = sudo_printf;

    if (SUDO_API_VERSION_GET_MAJOR(version) != SUDO_API_VERSION_MAJOR) {
	sudo_log(SUDO_CONV_ERROR_MSG,
	    "the sample plugin requires API version %d.x\n",
	    SUDO_API_VERSION_MAJOR);
	return ERROR;
    }

    /* Only allow commands to be run as root. */
    for (ui = settings; *ui != NULL; ui++) {
	if (strncmp(*ui, "runas_user=", sizeof("runas_user=") - 1) == 0) {
	    runas_user = *ui + sizeof("runas_user=") - 1;
	}
	if (strncmp(*ui, "runas_group=", sizeof("runas_group=") - 1) == 0) {
	    runas_group = *ui + sizeof("runas_group=") - 1;
	}
#if !defined(HAVE_GETPROGNAME) && !defined(HAVE___PROGNAME)
	if (strncmp(*ui, "progname=", sizeof("progname=") - 1) == 0) {
	    setprogname(*ui + sizeof("progname=") - 1);
	}
#endif
    }
    if (runas_user != NULL) {
	if ((pw = getpwnam(runas_user)) == NULL) {
	    sudo_log(SUDO_CONV_ERROR_MSG, "unknown user %s\n", runas_user);
	    return 0;
	}
	runas_uid = pw->pw_uid;
    }
    if (runas_group != NULL) {
	if ((gr = getgrnam(runas_group)) == NULL) {
	    sudo_log(SUDO_CONV_ERROR_MSG, "unknown group %s\n", runas_group);
	    return 0;
	}
	runas_gid = gr->gr_gid;
    }

    /* Plugin state. */
    plugin_state.envp = (char **)user_env;
    plugin_state.settings = settings;
    plugin_state.user_info = user_info;

    return 1;
}

/*
 * Plugin policy check function.
 * Simple example that prompts for a password, hard-coded to "test".
 */
static int 
policy_check(int argc, char * const argv[],
    char *env_add[], char **command_info_out[],
    char **argv_out[], char **user_env_out[])
{
    struct sudo_conv_message msg;
    struct sudo_conv_reply repl;
    char **command_info;
    int i = 0;

    if (!argc || argv[0] == NULL) {
	sudo_log(SUDO_CONV_ERROR_MSG, "no command specified\n");
	return FALSE;
    }
    /* Only allow fully qualified paths to keep things simple. */
    if (argv[0][0] != '/') {
	sudo_log(SUDO_CONV_ERROR_MSG,
	    "only fully qualified pathnames may be specified\n");
	return FALSE;
    }

    /* Prompt user for password via conversation function. */
    memset(&msg, 0, sizeof(msg));
    msg.msg_type = SUDO_CONV_PROMPT_ECHO_OFF;
    msg.msg = "Password: ";
    memset(&repl, 0, sizeof(repl));
    sudo_conv(1, &msg, &repl);
    if (repl.reply == NULL) {
	sudo_log(SUDO_CONV_ERROR_MSG, "missing password\n");
	return FALSE;
    }
    if (strcmp(repl.reply, "test") != 0) {
	sudo_log(SUDO_CONV_ERROR_MSG, "incorrect password\n");
	return FALSE;
    }

    /* No changes to argv or envp */
    *argv_out = (char **)argv;
    *user_env_out = plugin_state.envp;

    /* Setup command info. */
    command_info = calloc(32, sizeof(char *));
    if (command_info == NULL) {
	sudo_log(SUDO_CONV_ERROR_MSG, "out of memory\n");
	return ERROR;
    }
    if ((command_info[i++] = fmt_string("command", argv[0])) == NULL ||
	asprintf(&command_info[i++], "runas_euid=%ld", (long)runas_uid) == -1 ||
	asprintf(&command_info[i++], "runas_uid=%ld", (long)runas_uid) == -1) {
	sudo_log(SUDO_CONV_ERROR_MSG, "out of memory\n");
	return ERROR;
    }
    if (runas_gid != -1) {
	if (asprintf(&command_info[i++], "runas_gid=%ld", (long)runas_gid) == -1 ||
	    asprintf(&command_info[i++], "runas_egid=%ld", (long)runas_gid) == -1) {
	    sudo_log(SUDO_CONV_ERROR_MSG, "out of memory\n");
	    return ERROR;
	}
    }
#ifdef USE_TIMEOUT
    command_info[i++] = "timeout=30";
#endif

    *command_info_out = command_info;

    return TRUE;
}

static int
policy_list(int argc, char * const argv[], int verbose, const char *list_user)
{
    /*
     * List user's capabilities.
     */
    sudo_log(SUDO_CONV_INFO_MSG, "Validated users may run any command\n");
    return TRUE;
}

static int
policy_version(int verbose)
{
    sudo_log(SUDO_CONV_INFO_MSG, "Sample policy plugin version %s\n", PACKAGE_VERSION);
    return TRUE;
}

static void
policy_close(int exit_status, int error)
{
    /*
     * The policy might log the command exit status here.
     * In this example, we just print a message.
     */
    if (error) {
	sudo_log(SUDO_CONV_ERROR_MSG, "Command error: %s\n", strerror(error));
    } else {
        if (WIFEXITED(exit_status)) {
	    sudo_log(SUDO_CONV_INFO_MSG, "Command exited with status %d\n",
		WEXITSTATUS(exit_status));
        } else if (WIFSIGNALED(exit_status)) {
	    sudo_log(SUDO_CONV_INFO_MSG, "Command killed by signal %d\n",
		WTERMSIG(exit_status));
	}
    }
}

static int
io_open(unsigned int version, sudo_conv_t conversation,
    sudo_printf_t sudo_printf, char * const settings[],
    char * const user_info[], char * const user_env[])
{
    int fd;
    char path[PATH_MAX];

    if (!sudo_conv)
	sudo_conv = conversation;
    if (!sudo_log)
	sudo_log = sudo_printf;

    /* Open input and output files. */
    snprintf(path, sizeof(path), "/var/tmp/sample-%u.output",
	(unsigned int)getpid());
    fd = open(path, O_WRONLY|O_CREAT|O_EXCL, 0644);
    if (fd == -1)
	return FALSE;
    output = fdopen(fd, "w");

    snprintf(path, sizeof(path), "/var/tmp/sample-%u.input",
	(unsigned int)getpid());
    fd = open(path, O_WRONLY|O_CREAT|O_EXCL, 0644);
    if (fd == -1)
	return FALSE;
    input = fdopen(fd, "w");

    return TRUE;
}

static void
io_close(int exit_status, int error)
{
    fclose(input);
    fclose(output);
}

static int
io_version(int verbose)
{
    sudo_log(SUDO_CONV_INFO_MSG, "Sample I/O plugin version %s\n",
	PACKAGE_VERSION);
    return TRUE;
}

static int
io_log_input(const char *buf, unsigned int len)
{
    fwrite(buf, len, 1, input);
    return TRUE;
}

static int
io_log_output(const char *buf, unsigned int len)
{
    fwrite(buf, len, 1, output);
    return TRUE;
}

struct policy_plugin sample_policy = {
    SUDO_POLICY_PLUGIN,
    SUDO_API_VERSION,
    policy_open,
    policy_close,
    policy_version,
    policy_check,
    policy_list,
    NULL, /* validate */
    NULL /* invalidate */
};

/*
 * Note: This plugin does not differentiate between tty and pipe I/O.
 *       It all gets logged to the same file.
 */
struct io_plugin sample_io = {
    SUDO_IO_PLUGIN,
    SUDO_API_VERSION,
    io_open,
    io_close,
    io_version,
    io_log_input,	/* tty input */
    io_log_output,	/* tty output */
    io_log_input,	/* command stdin if not tty */
    io_log_output,	/* command stdout if not tty */
    io_log_output	/* command stderr if not tty */
};
