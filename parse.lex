%{
/*
 *  CU sudo version 1.4
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
 * parse.lex -- lexigraphical analyzer for sudo.
 *
 * Chris Jepeway <jepeway@cs.utk.edu>
 */

#ifndef lint
static char rcsid[] = "$Id$";
#endif /* lint */

#include "config.h"

#ifdef STDC_HEADERS
#include <stdlib.h>
#endif /* STDC_HEADERS */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#if defined(HAVE_MALLOC_H) && !defined(STDC_HEADERS)
#include <malloc.h>
#endif /* HAVE_MALLOC_H && !STDC_HEADERS */
#include <ctype.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "sudo.h"
#include <options.h>
#include "y.tab.h"

#undef yywrap		/* guard against a yywrap macro */

extern YYSTYPE yylval;
extern int clearaliases;
int sudolineno = 1;
static int sawspace = 0;
static int max_args;
static int num_args;

static void fill		__P((char *, int));
static void fill_cmnd		__P((char *, int));
static void fill_args		__P((char *, int, int));
extern void reset_aliases	__P((void));
extern void yyerror		__P((char *));

/* realloc() to size + COMMANDARGINC to make room for command args */
#define COMMANDARGINC	64

#ifdef TRACELEXER
#define LEXTRACE(msg)	fputs(msg, stderr)
#else
#define LEXTRACE(msg)
#endif
%}

OCTET			[[:digit:]]{1,3}
DOTTEDQUAD		{OCTET}(\.{OCTET}){3}
WORD			[[:alnum:]_-]+

%e	4000
%p	6000
%k	3500

%s	GOTCMND
%s	GOTRUNAS

%%
[ \t]+			{			/* throw away space/tabs */
			    sawspace = TRUE;	/* but remember for fill_args */
			}

\\\n			{ 
			    sawspace = TRUE;	/* remember for fill_args */
			    ++sudolineno;
			    LEXTRACE("\n\t");
			}			/* throw away EOL after \ */

<GOTCMND>\\[:\,=\\\" \t] {
			    LEXTRACE("QUOTEDCHAR ");
			    fill_args(yytext + 1, 1, sawspace);
			    sawspace = FALSE;
			}

<GOTCMND>[:\,=\n]	{
			    BEGIN INITIAL;
			    unput(*yytext);
			    return(COMMAND);
			}			/* end of command line args */

\n			{ 
			    ++sudolineno; 
			    LEXTRACE("\n");
			    return(COMMENT);
			}			/* return newline */

<INITIAL>#.*\n		{
			    ++sudolineno;
			    LEXTRACE("\n");
			    return(COMMENT);
			}			/* return comments */

<GOTCMND>\"[^\n]*\"	{
			    /* XXX - should be able to span lines? */
			    LEXTRACE("ARG ");
			    fill_args(yytext+1, yyleng-2, sawspace);
			    sawspace = FALSE;
			}			/* quoted command line arg */

<GOTCMND>[^:\,= \t\n]+ {
			    LEXTRACE("ARG ");
			    fill_args(yytext, yyleng, sawspace);
			    sawspace = FALSE;
			  }			/* a command line arg */

\,			{
			    LEXTRACE(", ");
			    return(',');
			}			/* return ',' */

\!			{
			    return('!');		/* return '!' */
			}

=			{
			    LEXTRACE("= ");
			    return('=');
			}			/* return '=' */

:			{
			    LEXTRACE(": ");
			    return(':');
			}			/* return ':' */

\.			{
			    return('.');
			}

NOPASSWD:		{ 
				/* XXX - is this the best way? */
				/* cmnd does not require passwd for this user */
			    	LEXTRACE("NOPASSWD ");
			    	return(NOPASSWD);
			}

\+{WORD}		{
			    /* netgroup */
			    fill(yytext, yyleng);
			    return(NETGROUP);
			}

\%{WORD}		{
			    /* UN*X group */
			    fill(yytext, yyleng);
			    return(USERGROUP);
			}

{DOTTEDQUAD}(\/{DOTTEDQUAD})? {
			    fill(yytext, yyleng);
			    LEXTRACE("NTWKADDR ");
			    return(NTWKADDR);
			}

[[:alpha:]][[:alnum:]_-]*(\.{WORD})+ {
			    fill(yytext, yyleng);
			    LEXTRACE("FQHOST ");
			    return(FQHOST);
			}

\(			{
				/* XXX - what about '(' in command args? */
				BEGIN GOTRUNAS;
				LEXTRACE("RUNAS ");
				return (RUNAS);
			}

<GOTRUNAS>[[:upper:]][[:upper:][:digit:]_]* {
			    /* User_Alias that user can run command as or ALL */
			    fill(yytext, yyleng);
			    if (strcmp(yytext, "ALL") == 0) {
				LEXTRACE("ALL ");
				return(ALL);
			    } else {
				LEXTRACE("ALIAS ");
				return(ALIAS);
			    }
			}

<GOTRUNAS>#?{WORD}	{
			    /* username/uid that user can run command as */
			    fill(yytext, yyleng);
			    LEXTRACE("NAME ");
			    return(NAME);
			}

<GOTRUNAS>\)		BEGIN INITIAL; /* XXX - will newlines be treated correctly? */


\/[^\,:=\\ \t\n#]+	{
			    /* directories can't have args... */
			    if (yytext[yyleng - 1] == '/') {
				LEXTRACE("COMMAND ");
				fill_cmnd(yytext, yyleng);
				return(COMMAND);
			    } else {
				BEGIN GOTCMND;
				LEXTRACE("COMMAND ");
				fill_cmnd(yytext, yyleng);
			    }
			}			/* a pathname */

[[:upper:]][[:upper:][:digit:]_]*	{
			    fill(yytext, yyleng);
			    if (strcmp(yytext, "ALL") == 0) {
				LEXTRACE("ALL ");
				return(ALL);
			    }
			    LEXTRACE("ALIAS ");
			    return(ALIAS);
			}

[[:alpha:]][[:alnum:]_-]*	{
			    int l;

			    fill(yytext, yyleng);
			    if (strcmp(yytext, "Host_Alias") == 0) {
				LEXTRACE("HOSTALIAS ");
				return(HOSTALIAS);
			    }
			    if (strcmp(yytext, "Cmnd_Alias") == 0) {
				LEXTRACE("CMNDALIAS ");
				return(CMNDALIAS);
			    }
			    if (strcmp(yytext, "User_Alias") == 0) {
				LEXTRACE("USERALIAS ");
				return(USERALIAS);
			    }
			    l = yyleng - 1;
			    if (isalpha(yytext[l]) || isdigit(yytext[l])) {
				/* NAME is what RFC1034 calls a label */
				LEXTRACE("NAME ");
				return(NAME);
			    }

			    return(ERROR);
			}

.			{
			    return(ERROR);
			}	/* parse error */

%%
static void fill(s, len)
    char *s;
    int len;
{
    yylval.string = (char *) malloc(len + 1);
    if (yylval.string == NULL)
	yyerror("unable to allocate memory");

    /* copy the string and NULL-terminate it */
    (void) strncpy(yylval.string, s, len);
    yylval.string[len] = '\0';
}


static void fill_cmnd(s, len)
    char *s;
    int len;
{
    num_args = max_args = 0;

    yylval.command.cmnd = (char *) malloc(len + 1);
    if (yylval.command.cmnd == NULL)
	yyerror("unable to allocate memory");

    /* copy the string and NULL-terminate it */
    (void) strncpy(yylval.command.cmnd, s, len);
    yylval.command.cmnd[len] = '\0';

    yylval.command.args = NULL;
}


static void fill_args(s, len, startnew)
    char *s;
    int len;
    int startnew;
{
    num_args += startnew;

    if (num_args >= max_args) {
	max_args += COMMANDARGINC;
	if (yylval.command.args == NULL)
	    yylval.command.args = (char **) malloc(max_args);
	else
	    yylval.command.args = (char **) realloc(yylval.command.args,
						    max_args);
	if (yylval.command.args == NULL)
	    yyerror("unable to allocate memory");
    }

    yylval.command.args[num_args-1] = (char *) malloc(len + 1);
    if (yylval.command.args[num_args-1] == NULL)
	yyerror("unable to allocate memory");

    /* copy the string and NULL-terminate it */
    (void) strncpy(yylval.command.args[num_args-1], s, len);
    yylval.command.args[num_args-1][len] = '\0';

    /* NULL-terminate the argument vector */
    yylval.command.args[num_args] = (char *)NULL;
}


int yywrap()
{
#ifdef YY_NEW_FILE
    YY_NEW_FILE;
#endif /* YY_NEW_FILE */

    /* don't reset the aliases if called by testsudoers */
    if (clearaliases)
	reset_aliases();

    return(TRUE);
}
