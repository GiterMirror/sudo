#ifndef lint
/*static char yysccsid[] = "from: @(#)yaccpar	1.9 (Berkeley) 02/21/93";*/
static char yyrcsid[]
#if __GNUC__ >= 2
  __attribute__ ((unused))
#endif /* __GNUC__ >= 2 */
  = "$OpenBSD: skeleton.c,v 1.26 2006/04/20 16:51:32 deraadt Exp $";
#endif
#include <stdlib.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
#define YYPREFIX "yy"
#line 2 "gram.y"
/*
 * Copyright (c) 1996, 1998-2005, 2007
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
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F39502-99-1-0512.
 */

#include <config.h>

#include <sys/types.h>
#include <sys/param.h>
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
#if defined(YYBISON) && defined(HAVE_ALLOCA_H) && !defined(__GNUC__)
# include <alloca.h>
#endif /* YYBISON && HAVE_ALLOCA_H && !__GNUC__ */

#include "sudo.h"
#include "parse.h"

#ifndef lint
__unused static const char rcsid[] = "$Sudo$";
#endif /* lint */

/*
 * Globals
 */
extern int sudolineno;
extern char *sudoers;
int parse_error;
int pedantic = FALSE;
int verbose = FALSE;
int errorlineno = -1;
char *errorfile = NULL;

struct defaults *defaults;
struct userspec *userspecs;

/*
 * Local protoypes
 */
static void  add_defaults	__P((int, struct member *, struct defaults *));
static void  add_userspec	__P((struct member *, struct privilege *));
       void  yyerror		__P((const char *));

void
yyerror(s)
    const char *s;
{
    /* Save the line the first error occurred on. */
    if (errorlineno == -1) {
	errorlineno = sudolineno ? sudolineno - 1 : 0;
	errorfile = estrdup(sudoers);
    }
    if (verbose && s != NULL) {
#ifndef TRACELEXER
	(void) fprintf(stderr, ">>> %s: %s near line %d <<<\n", sudoers, s,
	    sudolineno ? sudolineno - 1 : 0);
#else
	(void) fprintf(stderr, "<*> ");
#endif
    }
    parse_error = TRUE;
}
#line 101 "gram.y"
#ifndef YYSTYPE_DEFINED
#define YYSTYPE_DEFINED
typedef union {
    struct cmndspec *cmndspec;
    struct defaults *defaults;
    struct member *member;
    struct privilege *privilege;
    struct sudo_command command;
    struct cmndtag tag;
    char *string;
    int tok;
} YYSTYPE;
#endif /* YYSTYPE_DEFINED */
#line 132 "y.tab.c"
#define COMMAND 257
#define ALIAS 258
#define DEFVAR 259
#define NTWKADDR 260
#define NETGROUP 261
#define USERGROUP 262
#define WORD 263
#define DEFAULTS 264
#define DEFAULTS_HOST 265
#define DEFAULTS_USER 266
#define DEFAULTS_RUNAS 267
#define DEFAULTS_CMND 268
#define RUNAS 269
#define NOPASSWD 270
#define PASSWD 271
#define NOEXEC 272
#define EXEC 273
#define MONITOR 274
#define NOMONITOR 275
#define SETENV 276
#define NOSETENV 277
#define ALL 278
#define COMMENT 279
#define HOSTALIAS 280
#define CMNDALIAS 281
#define USERALIAS 282
#define RUNASALIAS 283
#define ERROR 284
#define YYERRCODE 256
#if defined(__cplusplus) || defined(__STDC__)
const short yylhs[] =
#else
short yylhs[] =
#endif
	{                                        -1,
    0,    0,   21,   21,   22,   22,   22,   22,   22,   22,
   22,   22,   22,   22,   22,   22,    4,    4,    3,    3,
    3,    3,    3,   19,   19,   18,   10,   10,    8,    8,
    8,    8,    8,    2,    2,    1,    6,    6,   14,   14,
   13,   13,   11,   11,   15,   15,   15,   15,   15,   20,
   20,   20,   20,   20,   20,   20,   20,   20,    5,    5,
    5,   24,   24,   27,    9,    9,   25,   25,   28,    7,
    7,   26,   26,   29,   23,   23,   30,   17,   17,   12,
   12,   16,   16,   16,   16,   16,
};
#if defined(__cplusplus) || defined(__STDC__)
const short yylen[] =
#else
short yylen[] =
#endif
	{                                         2,
    0,    1,    1,    2,    1,    2,    2,    2,    2,    2,
    2,    2,    3,    3,    3,    3,    1,    3,    1,    2,
    3,    3,    3,    1,    3,    3,    1,    2,    1,    1,
    1,    1,    1,    1,    3,    3,    1,    2,    0,    2,
    1,    3,    1,    2,    1,    1,    1,    1,    1,    0,
    2,    2,    2,    2,    2,    2,    2,    2,    1,    1,
    1,    1,    3,    3,    1,    3,    1,    3,    3,    1,
    3,    1,    3,    3,    1,    3,    3,    1,    3,    1,
    2,    1,    1,    1,    1,    1,
};
#if defined(__cplusplus) || defined(__STDC__)
const short yydefred[] =
#else
short yydefred[] =
#endif
	{                                      0,
    0,   82,   84,   85,   86,    0,    0,    0,    0,    0,
   83,    5,    0,    0,    0,    0,    0,    0,   78,   80,
    0,    0,    3,    6,    0,    0,   17,    0,   29,   32,
   31,   33,   30,    0,   27,    0,   65,    0,   45,   47,
   48,   49,   46,    0,   41,    0,   43,   61,   60,   59,
    0,   37,   70,    0,    0,    0,   62,    0,    0,   67,
    0,    0,   75,    0,    0,   72,   81,    0,    0,   24,
    0,    4,    0,    0,    0,   20,    0,   28,    0,    0,
    0,   44,    0,    0,   38,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   79,    0,    0,   21,   22,
   23,   18,   66,   42,   71,    0,   63,    0,   68,    0,
   76,    0,   73,    0,   34,    0,   50,   25,    0,    0,
    0,   35,   51,   52,   53,   54,   57,   58,   55,   56,
   36,
};
#if defined(__cplusplus) || defined(__STDC__)
const short yydgoto[] =
#else
short yydgoto[] =
#endif
	{                                      18,
  115,  116,   27,   28,   52,   53,   54,   35,   69,   37,
   45,   19,   46,  117,   47,   20,   21,   70,   71,  121,
   22,   23,   62,   56,   59,   65,   57,   60,   66,   63,
};
#if defined(__cplusplus) || defined(__STDC__)
const short yysindex[] =
#else
short yysindex[] =
#endif
	{                                    466,
 -266,    0,    0,    0,    0,  -21,  513,   -7,  517,  188,
    0,    0, -244, -239, -234, -233, -231,    0,    0,    0,
  -29,  466,    0,    0,    9, -226,    0,   -9,    0,    0,
    0,    0,    0, -219,    0,  -26,    0,  -24,    0,    0,
    0,    0,    0, -212,    0,  -23,    0,    0,    0,    0,
 -241,    0,    0,  -22,  -25,  -18,    0,  -13,  -15,    0,
   -5,    2,    0,   -3,    3,    0,    0,   -7,  -38,    0,
    6,    0, -198, -196, -195,    0,  -21,    0,  513,   -9,
   -9,    0,  517,   -9,    0,  188,   -9,  513, -244,  188,
 -239,   -7, -234,  517, -233,    0, -200,  513,    0,    0,
    0,    0,    0,    0,    0,   28,    0,   30,    0,   31,
    0,   32,    0,  517,    0,   33,    0,    0,   32, -200,
  -31,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,};
#if defined(__cplusplus) || defined(__STDC__)
const short yyrindex[] =
#else
short yyrindex[] =
#endif
	{                                     73,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   78,    0,    0,    1,    0,    0,  169,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  197,    0,    0,  225,    0,
    0,  253,    0,    0,  298,    0,    0,    0,    0,    0,
  326,    0,    0,    0,    0,    0,    0,    0,    0,  354,
  382,    0,    0,  410,    0,    0,  438,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  267,    0,    0,    0,
    0,    0,    0,    0,    0,   29,    0,   57,    0,   85,
    0,  113,    0,    0,    0,  141,    0,    0,  480,  267,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,};
#if defined(__cplusplus) || defined(__STDC__)
const short yygindex[] =
#else
short yygindex[] =
#endif
	{                                      0,
  -41,    0,    4,   17,   37,  -83,  -10,   48,   -2,   10,
    8,   15,  -86,    0,   49,   75,   -8,   -4,    0,    0,
    0,   74,    0,    0,    0,    0,   11,    7,   12,   13,
};
#define YYTABLESIZE 795
#if defined(__cplusplus) || defined(__STDC__)
const short yytable[] =
#else
short yytable[] =
#endif
	{                                      38,
   19,   51,  105,   34,   36,   79,   26,  112,   26,   26,
   26,   26,   24,   55,   68,   48,   49,   79,   58,   68,
   83,   86,   97,   61,   64,   17,    2,  119,   64,    3,
    4,    5,   76,   19,   77,   88,   50,  131,   29,   89,
   30,   31,   91,   32,   19,   39,   11,   90,   40,   41,
   42,   74,   80,   75,   81,   92,   69,   94,   33,   93,
   95,   64,   84,   98,   99,   43,  100,  101,  114,   73,
   87,   79,    1,   86,   68,   83,  120,    2,  122,  108,
  102,   78,   96,  110,   77,  106,   64,   85,  103,   69,
  104,   67,   82,  118,    0,   72,    0,  109,    0,  107,
    0,    0,    0,    0,    0,  111,  113,    0,    0,    0,
    0,    0,   74,    0,   69,    0,    0,   77,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   26,    0,   77,    0,    0,   74,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   12,    0,
   74,    0,    0,   26,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    9,    0,   26,    0,
    0,   12,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   51,    0,    0,    0,   10,   48,   49,    0,   29,    9,
   30,   31,   25,   32,   25,   25,   25,   25,  123,  124,
  125,  126,  127,  128,  129,  130,   50,    0,   33,    0,
    2,    0,    8,    3,    4,    5,   19,   10,   19,    0,
    0,   19,   19,   19,   19,   19,   19,   19,   19,    0,
   11,    0,    0,    0,    0,    0,    0,    0,   19,   19,
   19,   19,   19,   19,   64,    8,   64,    0,    0,   64,
   64,   64,   64,   64,   64,   64,   64,   11,    0,   39,
    0,    0,    0,    0,    0,    0,   64,   64,   64,   64,
   64,   64,   69,    0,   69,    0,    0,   69,   69,   69,
   69,   69,   69,   69,   69,    7,    0,    0,    0,    0,
   11,    0,    0,    0,   69,   69,   69,   69,   69,   69,
   77,    0,   77,    0,    0,   77,   77,   77,   77,   77,
   77,   77,   77,   15,    0,    0,    0,    0,    7,    0,
    0,    0,   77,   77,   77,   77,   77,   77,   74,    0,
   74,    0,    0,   74,   74,   74,   74,   74,   74,   74,
   74,   13,    0,    0,    0,    0,   15,    0,    0,    0,
   74,   74,   74,   74,   74,   74,   26,    0,   26,    0,
    0,   26,   26,   26,   26,   26,   26,   26,   26,   14,
    0,    0,    0,    0,   13,    0,    0,    0,   26,   26,
   26,   26,   26,   26,   12,    0,   12,    0,    0,   12,
   12,   12,   12,   12,   12,   12,   12,   16,    0,    0,
    0,    0,   14,    0,   48,   49,   12,   12,   12,   12,
   12,   12,    9,    0,    9,    0,    0,    9,    9,    9,
    9,    9,    9,    9,    9,   50,    0,    0,    0,    0,
   16,    0,    0,    0,    9,    9,    9,    9,    9,    9,
   10,    0,   10,    0,    0,   10,   10,   10,   10,   10,
   10,   10,   10,    0,    0,    0,    0,    0,   17,    0,
    0,    0,   10,   10,   10,   10,   10,   10,    8,    0,
    8,    0,   40,    8,    8,    8,    8,    8,    8,    8,
    8,    0,    0,   39,   39,    0,    0,    0,    0,    0,
    8,    8,    8,    8,    8,    8,   39,   39,   39,   39,
   39,   39,   39,   39,   39,   34,    0,    0,    0,   44,
    0,    0,    0,   11,    0,   11,    0,    0,   11,   11,
   11,   11,   11,   11,   11,   11,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   11,   11,   11,   11,   11,
   11,    7,    0,    7,    0,    0,    7,    7,    7,    7,
    7,    7,    7,    7,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    7,    7,    7,    7,    7,    7,   15,
    0,   15,    0,    0,   15,   15,   15,   15,   15,   15,
   15,   15,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   15,   15,   15,   15,   15,   15,   13,    0,   13,
    0,    0,   13,   13,   13,   13,   13,   13,   13,   13,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   13,
   13,   13,   13,   13,   13,   14,    0,   14,    0,    0,
   14,   14,   14,   14,   14,   14,   14,   14,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   14,   14,   14,
   14,   14,   14,   16,    0,   16,    0,    0,   16,   16,
   16,   16,   16,   16,   16,   16,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   16,   16,   16,   16,   16,
   16,    1,    0,    2,    0,    0,    3,    4,    5,    6,
    7,    8,    9,   10,    0,    0,   40,   40,    0,    0,
    0,    0,    0,   11,   12,   13,   14,   15,   16,   40,
   40,   40,   40,   40,   40,   40,   40,   40,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   29,    0,   30,   31,   39,   32,    0,   40,   41,   42,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   33,    0,    0,    0,   43,
};
#if defined(__cplusplus) || defined(__STDC__)
const short yycheck[] =
#else
short yycheck[] =
#endif
	{                                       8,
    0,   33,   86,   33,    7,   44,   33,   94,   33,   33,
   33,   33,  279,  258,   44,  257,  258,   44,  258,   44,
   44,   44,   61,  258,  258,   33,  258,  114,    0,  261,
  262,  263,  259,   33,   44,   61,  278,  121,  258,   58,
  260,  261,   58,  263,   44,  258,  278,   61,  261,  262,
  263,   43,   36,   45,   38,   61,    0,   61,  278,   58,
   58,   33,   46,   58,  263,  278,  263,  263,  269,   61,
   54,   44,    0,   44,   44,   44,   44,    0,  120,   90,
   77,   34,   68,   92,    0,   88,   58,   51,   79,   33,
   83,   17,   44,   98,   -1,   22,   -1,   91,   -1,   89,
   -1,   -1,   -1,   -1,   -1,   93,   95,   -1,   -1,   -1,
   -1,   -1,    0,   -1,   58,   -1,   -1,   33,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    0,   -1,   58,   -1,   -1,   33,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,
   58,   -1,   -1,   33,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,   58,   -1,
   -1,   33,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   33,   -1,   -1,   -1,    0,  257,  258,   -1,  258,   33,
  260,  261,  259,  263,  259,  259,  259,  259,  270,  271,
  272,  273,  274,  275,  276,  277,  278,   -1,  278,   -1,
  258,   -1,    0,  261,  262,  263,  256,   33,  258,   -1,
   -1,  261,  262,  263,  264,  265,  266,  267,  268,   -1,
  278,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  278,  279,
  280,  281,  282,  283,  256,   33,  258,   -1,   -1,  261,
  262,  263,  264,  265,  266,  267,  268,    0,   -1,   33,
   -1,   -1,   -1,   -1,   -1,   -1,  278,  279,  280,  281,
  282,  283,  256,   -1,  258,   -1,   -1,  261,  262,  263,
  264,  265,  266,  267,  268,    0,   -1,   -1,   -1,   -1,
   33,   -1,   -1,   -1,  278,  279,  280,  281,  282,  283,
  256,   -1,  258,   -1,   -1,  261,  262,  263,  264,  265,
  266,  267,  268,    0,   -1,   -1,   -1,   -1,   33,   -1,
   -1,   -1,  278,  279,  280,  281,  282,  283,  256,   -1,
  258,   -1,   -1,  261,  262,  263,  264,  265,  266,  267,
  268,    0,   -1,   -1,   -1,   -1,   33,   -1,   -1,   -1,
  278,  279,  280,  281,  282,  283,  256,   -1,  258,   -1,
   -1,  261,  262,  263,  264,  265,  266,  267,  268,    0,
   -1,   -1,   -1,   -1,   33,   -1,   -1,   -1,  278,  279,
  280,  281,  282,  283,  256,   -1,  258,   -1,   -1,  261,
  262,  263,  264,  265,  266,  267,  268,    0,   -1,   -1,
   -1,   -1,   33,   -1,  257,  258,  278,  279,  280,  281,
  282,  283,  256,   -1,  258,   -1,   -1,  261,  262,  263,
  264,  265,  266,  267,  268,  278,   -1,   -1,   -1,   -1,
   33,   -1,   -1,   -1,  278,  279,  280,  281,  282,  283,
  256,   -1,  258,   -1,   -1,  261,  262,  263,  264,  265,
  266,  267,  268,   -1,   -1,   -1,   -1,   -1,   33,   -1,
   -1,   -1,  278,  279,  280,  281,  282,  283,  256,   -1,
  258,   -1,   33,  261,  262,  263,  264,  265,  266,  267,
  268,   -1,   -1,  257,  258,   -1,   -1,   -1,   -1,   -1,
  278,  279,  280,  281,  282,  283,  270,  271,  272,  273,
  274,  275,  276,  277,  278,   33,   -1,   -1,   -1,   33,
   -1,   -1,   -1,  256,   -1,  258,   -1,   -1,  261,  262,
  263,  264,  265,  266,  267,  268,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  278,  279,  280,  281,  282,
  283,  256,   -1,  258,   -1,   -1,  261,  262,  263,  264,
  265,  266,  267,  268,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  278,  279,  280,  281,  282,  283,  256,
   -1,  258,   -1,   -1,  261,  262,  263,  264,  265,  266,
  267,  268,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  278,  279,  280,  281,  282,  283,  256,   -1,  258,
   -1,   -1,  261,  262,  263,  264,  265,  266,  267,  268,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  278,
  279,  280,  281,  282,  283,  256,   -1,  258,   -1,   -1,
  261,  262,  263,  264,  265,  266,  267,  268,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  278,  279,  280,
  281,  282,  283,  256,   -1,  258,   -1,   -1,  261,  262,
  263,  264,  265,  266,  267,  268,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  278,  279,  280,  281,  282,
  283,  256,   -1,  258,   -1,   -1,  261,  262,  263,  264,
  265,  266,  267,  268,   -1,   -1,  257,  258,   -1,   -1,
   -1,   -1,   -1,  278,  279,  280,  281,  282,  283,  270,
  271,  272,  273,  274,  275,  276,  277,  278,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  258,   -1,  260,  261,  258,  263,   -1,  261,  262,  263,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  278,   -1,   -1,   -1,  278,
};
#define YYFINAL 18
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 284
#if YYDEBUG
#if defined(__cplusplus) || defined(__STDC__)
const char * const yyname[] =
#else
char *yyname[] =
#endif
	{
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,0,0,0,0,0,0,"'+'","','","'-'",0,0,0,0,0,0,0,0,0,0,0,0,"':'",0,0,
"'='",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"COMMAND","ALIAS","DEFVAR","NTWKADDR","NETGROUP","USERGROUP","WORD","DEFAULTS",
"DEFAULTS_HOST","DEFAULTS_USER","DEFAULTS_RUNAS","DEFAULTS_CMND","RUNAS",
"NOPASSWD","PASSWD","NOEXEC","EXEC","MONITOR","NOMONITOR","SETENV","NOSETENV",
"ALL","COMMENT","HOSTALIAS","CMNDALIAS","USERALIAS","RUNASALIAS","ERROR",
};
#if defined(__cplusplus) || defined(__STDC__)
const char * const yyrule[] =
#else
char *yyrule[] =
#endif
	{"$accept : file",
"file :",
"file : line",
"line : entry",
"line : line entry",
"entry : COMMENT",
"entry : error COMMENT",
"entry : userlist privileges",
"entry : USERALIAS useraliases",
"entry : HOSTALIAS hostaliases",
"entry : CMNDALIAS cmndaliases",
"entry : RUNASALIAS runasaliases",
"entry : DEFAULTS defaults_list",
"entry : DEFAULTS_USER userlist defaults_list",
"entry : DEFAULTS_RUNAS runaslist defaults_list",
"entry : DEFAULTS_HOST hostlist defaults_list",
"entry : DEFAULTS_CMND cmndlist defaults_list",
"defaults_list : defaults_entry",
"defaults_list : defaults_list ',' defaults_entry",
"defaults_entry : DEFVAR",
"defaults_entry : '!' DEFVAR",
"defaults_entry : DEFVAR '=' WORD",
"defaults_entry : DEFVAR '+' WORD",
"defaults_entry : DEFVAR '-' WORD",
"privileges : privilege",
"privileges : privileges ':' privilege",
"privilege : hostlist '=' cmndspeclist",
"ophost : host",
"ophost : '!' host",
"host : ALIAS",
"host : ALL",
"host : NETGROUP",
"host : NTWKADDR",
"host : WORD",
"cmndspeclist : cmndspec",
"cmndspeclist : cmndspeclist ',' cmndspec",
"cmndspec : runasspec cmndtag opcmnd",
"opcmnd : cmnd",
"opcmnd : '!' cmnd",
"runasspec :",
"runasspec : RUNAS runaslist",
"runaslist : oprunasuser",
"runaslist : runaslist ',' oprunasuser",
"oprunasuser : runasuser",
"oprunasuser : '!' runasuser",
"runasuser : ALIAS",
"runasuser : ALL",
"runasuser : NETGROUP",
"runasuser : USERGROUP",
"runasuser : WORD",
"cmndtag :",
"cmndtag : cmndtag NOPASSWD",
"cmndtag : cmndtag PASSWD",
"cmndtag : cmndtag NOEXEC",
"cmndtag : cmndtag EXEC",
"cmndtag : cmndtag SETENV",
"cmndtag : cmndtag NOSETENV",
"cmndtag : cmndtag MONITOR",
"cmndtag : cmndtag NOMONITOR",
"cmnd : ALL",
"cmnd : ALIAS",
"cmnd : COMMAND",
"hostaliases : hostalias",
"hostaliases : hostaliases ':' hostalias",
"hostalias : ALIAS '=' hostlist",
"hostlist : ophost",
"hostlist : hostlist ',' ophost",
"cmndaliases : cmndalias",
"cmndaliases : cmndaliases ':' cmndalias",
"cmndalias : ALIAS '=' cmndlist",
"cmndlist : opcmnd",
"cmndlist : cmndlist ',' opcmnd",
"runasaliases : runasalias",
"runasaliases : runasaliases ':' runasalias",
"runasalias : ALIAS '=' runaslist",
"useraliases : useralias",
"useraliases : useraliases ':' useralias",
"useralias : ALIAS '=' userlist",
"userlist : opuser",
"userlist : userlist ',' opuser",
"opuser : user",
"opuser : '!' user",
"user : ALIAS",
"user : ALL",
"user : NETGROUP",
"user : USERGROUP",
"user : WORD",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
/* LINTUSED */
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
#line 511 "gram.y"
/*
 * Add a list of defaults structures to the defaults list.
 * The binding, if non-NULL, specifies a list of hosts, users, or
 * runas users the entries apply to (specified by the type).
 */
static void
add_defaults(type, binding, defs)
    int type;
    struct member *binding;
    struct defaults *defs;
{
    struct defaults *d;

    /*
     * Set type and binding (who it applies to) for new entries.
     */
    for (d = defs; d != NULL; d = d->next) {
	d->type = type;
	d->binding = binding;
    }
    if (defaults == NULL)
	defaults = defs;
    else
	LIST_APPEND(defaults, defs);
}

/*
 * Allocate a new struct userspec, populate it, and insert it at the
 * and of the userspecs list.
 */
static void
add_userspec(members, privs)
    struct member *members;
    struct privilege *privs;
{
    struct userspec *u;

    u = emalloc(sizeof(*u));
    u->user = members;
    u->privileges = privs;
    u->last = NULL;
    u->next = NULL;
    if (userspecs == NULL)
	userspecs = u;
    else
	LIST_APPEND(userspecs, u);
}

/*
 * Free up space used by data structures from a previous parser run and sets
 * the current sudoers file to path.
 */
void
init_parser(path, quiet)
    char *path;
    int quiet;
{
    struct defaults *d;
    struct member *m, *lastbinding;
    struct userspec *us;
    struct privilege *priv;
    struct cmndspec *cs;
    VOID *next;

    for (us = userspecs; us != NULL; us = next) {
	for (m = us->user; m != NULL; m = next) {
	    next = m->next;
	    efree(m->name);
	    efree(m);
	}
	for (priv = us->privileges; priv != NULL; priv = next) {
	    for (m = priv->hostlist; m != NULL; m = next) {
		next = m->next;
		efree(m->name);
		efree(m);
	    }
	    for (cs = priv->cmndlist; cs != NULL; cs = next) {
		for (m = cs->runaslist; m != NULL; m = next) {
		    next = m->next;
		    efree(m->name);
		    efree(m);
		}
		efree(cs->cmnd->name);
		efree(cs->cmnd);
		next = cs->next;
		efree(cs);
	    }
	    next = priv->next;
	    efree(priv);
	}
	next = us->next;
	efree(us);
    }
    userspecs = NULL;

    lastbinding = NULL;
    for (d = defaults; d != NULL; d = next) {
	if (d->binding != lastbinding) {
	    for (m = d->binding; m != NULL; m = next) {
		next = m->next;
		efree(m->name);
		efree(m);
	    }
	    lastbinding = d->binding;
	}
	next = d->next;
	efree(d->var);
	efree(d->val);
	efree(d);
    }
    defaults = NULL;

    init_aliases();

    efree(sudoers);
    sudoers = estrdup(path);

    parse_error = FALSE;
    errorlineno = -1;
    sudolineno = 1;
    verbose = !quiet;
}
#line 664 "y.tab.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
#if defined(__cplusplus) || defined(__STDC__)
static int yygrowstack(void)
#else
static int yygrowstack()
#endif
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        goto bail;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        goto bail;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
bail:
    if (yyss)
            free(yyss);
    if (yyvs)
            free(yyvs);
    yyss = yyssp = NULL;
    yyvs = yyvsp = NULL;
    yystacksize = 0;
    return -1;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
#if defined(__cplusplus) || defined(__STDC__)
yyparse(void)
#else
yyparse()
#endif
{
    int yym, yyn, yystate;
#if YYDEBUG
#if defined(__cplusplus) || defined(__STDC__)
    const char *yys;
#else /* !(defined(__cplusplus) || defined(__STDC__)) */
    char *yys;
#endif /* !(defined(__cplusplus) || defined(__STDC__)) */

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif /* YYDEBUG */

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 166 "gram.y"
{ ; }
break;
case 5:
#line 174 "gram.y"
{
			    ;
			}
break;
case 6:
#line 177 "gram.y"
{
			    yyerrok;
			}
break;
case 7:
#line 180 "gram.y"
{
			    add_userspec(yyvsp[-1].member, yyvsp[0].privilege);
			}
break;
case 8:
#line 183 "gram.y"
{
			    ;
			}
break;
case 9:
#line 186 "gram.y"
{
			    ;
			}
break;
case 10:
#line 189 "gram.y"
{
			    ;
			}
break;
case 11:
#line 192 "gram.y"
{
			    ;
			}
break;
case 12:
#line 195 "gram.y"
{
			    add_defaults(DEFAULTS, NULL, yyvsp[0].defaults);
			}
break;
case 13:
#line 198 "gram.y"
{
			    add_defaults(DEFAULTS_USER, yyvsp[-1].member, yyvsp[0].defaults);
			}
break;
case 14:
#line 201 "gram.y"
{
			    add_defaults(DEFAULTS_RUNAS, yyvsp[-1].member, yyvsp[0].defaults);
			}
break;
case 15:
#line 204 "gram.y"
{
			    add_defaults(DEFAULTS_HOST, yyvsp[-1].member, yyvsp[0].defaults);
			}
break;
case 16:
#line 207 "gram.y"
{
			    add_defaults(DEFAULTS_CMND, yyvsp[-1].member, yyvsp[0].defaults);
			}
break;
case 18:
#line 213 "gram.y"
{
			    LIST_APPEND(yyvsp[-2].defaults, yyvsp[0].defaults);
			    yyval.defaults = yyvsp[-2].defaults;
			}
break;
case 19:
#line 219 "gram.y"
{
			    NEW_DEFAULT(yyval.defaults, yyvsp[0].string, NULL, TRUE);
			}
break;
case 20:
#line 222 "gram.y"
{
			    NEW_DEFAULT(yyval.defaults, yyvsp[0].string, NULL, FALSE);
			}
break;
case 21:
#line 225 "gram.y"
{
			    NEW_DEFAULT(yyval.defaults, yyvsp[-2].string, yyvsp[0].string, TRUE);
			}
break;
case 22:
#line 228 "gram.y"
{
			    NEW_DEFAULT(yyval.defaults, yyvsp[-2].string, yyvsp[0].string, '+');
			}
break;
case 23:
#line 231 "gram.y"
{
			    NEW_DEFAULT(yyval.defaults, yyvsp[-2].string, yyvsp[0].string, '-');
			}
break;
case 25:
#line 237 "gram.y"
{
			    LIST_APPEND(yyvsp[-2].privilege, yyvsp[0].privilege);
			    yyval.privilege = yyvsp[-2].privilege;
			}
break;
case 26:
#line 243 "gram.y"
{
			    struct cmndtag tags;
			    struct privilege *p = emalloc(sizeof(*p));
			    struct cmndspec *cs;
			    p->hostlist = yyvsp[-2].member;
			    p->cmndlist = yyvsp[0].cmndspec;
			    tags.nopasswd = tags.noexec = tags.monitor =
				tags.setenv = UNSPEC;
			    /* propagate tags */
			    for (cs = yyvsp[0].cmndspec; cs != NULL; cs = cs->next) {
				if (cs->tags.nopasswd == UNSPEC)
				    cs->tags.nopasswd = tags.nopasswd;
				if (cs->tags.noexec == UNSPEC)
				    cs->tags.noexec = tags.noexec;
				if (cs->tags.setenv == UNSPEC)
				    cs->tags.setenv = tags.setenv;
				if (cs->tags.monitor == UNSPEC)
				    cs->tags.monitor = tags.monitor;
				memcpy(&tags, &cs->tags, sizeof(tags));
			    }
			    p->last = NULL;
			    p->next = NULL;
			    yyval.privilege = p;
			}
break;
case 27:
#line 269 "gram.y"
{
			    yyval.member = yyvsp[0].member;
			    yyval.member->negated = FALSE;
			}
break;
case 28:
#line 273 "gram.y"
{
			    yyval.member = yyvsp[0].member;
			    yyval.member->negated = TRUE;
			}
break;
case 29:
#line 279 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, ALIAS);
			}
break;
case 30:
#line 282 "gram.y"
{
			    NEW_MEMBER(yyval.member, NULL, ALL);
			}
break;
case 31:
#line 285 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, NETGROUP);
			}
break;
case 32:
#line 288 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, NTWKADDR);
			}
break;
case 33:
#line 291 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, WORD);
			}
break;
case 35:
#line 297 "gram.y"
{
			    LIST_APPEND(yyvsp[-2].cmndspec, yyvsp[0].cmndspec);
			    yyval.cmndspec = yyvsp[-2].cmndspec;
			}
break;
case 36:
#line 303 "gram.y"
{
			    struct cmndspec *cs = emalloc(sizeof(*cs));
			    cs->runaslist = yyvsp[-2].member;
			    cs->tags = yyvsp[-1].tag;
			    cs->cmnd = yyvsp[0].member;
			    cs->last = NULL;
			    cs->next = NULL;
			    yyval.cmndspec = cs;
			}
break;
case 37:
#line 314 "gram.y"
{
			    yyval.member = yyvsp[0].member;
			    yyval.member->negated = FALSE;
			}
break;
case 38:
#line 318 "gram.y"
{
			    yyval.member = yyvsp[0].member;
			    yyval.member->negated = TRUE;
			}
break;
case 39:
#line 324 "gram.y"
{
			    yyval.member = NULL;
			}
break;
case 40:
#line 327 "gram.y"
{
			    yyval.member = yyvsp[0].member;
			}
break;
case 42:
#line 333 "gram.y"
{
			    LIST_APPEND(yyvsp[-2].member, yyvsp[0].member);
			    yyval.member = yyvsp[-2].member;
			}
break;
case 43:
#line 339 "gram.y"
{
			    yyval.member = yyvsp[0].member;
			    yyval.member->negated = FALSE;
			}
break;
case 44:
#line 343 "gram.y"
{
			    yyval.member = yyvsp[0].member;
			    yyval.member->negated = TRUE;
			}
break;
case 45:
#line 349 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, ALIAS);
			}
break;
case 46:
#line 352 "gram.y"
{
			    NEW_MEMBER(yyval.member, NULL, ALL);
			}
break;
case 47:
#line 355 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, NETGROUP);
			}
break;
case 48:
#line 358 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, USERGROUP);
			}
break;
case 49:
#line 361 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, WORD);
			}
break;
case 50:
#line 366 "gram.y"
{
			    yyval.tag.nopasswd = yyval.tag.noexec = yyval.tag.monitor =
				yyval.tag.setenv = UNSPEC;
			}
break;
case 51:
#line 370 "gram.y"
{
			    yyval.tag.nopasswd = TRUE;
			}
break;
case 52:
#line 373 "gram.y"
{
			    yyval.tag.nopasswd = FALSE;
			}
break;
case 53:
#line 376 "gram.y"
{
			    yyval.tag.noexec = TRUE;
			}
break;
case 54:
#line 379 "gram.y"
{
			    yyval.tag.noexec = FALSE;
			}
break;
case 55:
#line 382 "gram.y"
{
			    yyval.tag.setenv = TRUE;
			}
break;
case 56:
#line 385 "gram.y"
{
			    yyval.tag.setenv = FALSE;
			}
break;
case 57:
#line 388 "gram.y"
{
			    yyval.tag.monitor = TRUE;
			}
break;
case 58:
#line 391 "gram.y"
{
			    yyval.tag.monitor = FALSE;
			}
break;
case 59:
#line 396 "gram.y"
{
			    NEW_MEMBER(yyval.member, NULL, ALL);
			}
break;
case 60:
#line 399 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, ALIAS);
			}
break;
case 61:
#line 402 "gram.y"
{
			    struct sudo_command *c = emalloc(sizeof(*c));
			    c->cmnd = yyvsp[0].command.cmnd;
			    c->args = yyvsp[0].command.args;
			    NEW_MEMBER(yyval.member, (char *)c, COMMAND);
			}
break;
case 64:
#line 414 "gram.y"
{
			    char *s;
			    if ((s = alias_add(yyvsp[-2].string, HOSTALIAS, yyvsp[0].member)) != NULL) {
				yyerror(s);
				YYERROR;
			    }
			}
break;
case 66:
#line 424 "gram.y"
{
			    LIST_APPEND(yyvsp[-2].member, yyvsp[0].member);
			    yyval.member = yyvsp[-2].member;
			}
break;
case 69:
#line 434 "gram.y"
{
			    char *s;
			    if ((s = alias_add(yyvsp[-2].string, CMNDALIAS, yyvsp[0].member)) != NULL) {
				yyerror(s);
				YYERROR;
			    }
			}
break;
case 71:
#line 444 "gram.y"
{
			    LIST_APPEND(yyvsp[-2].member, yyvsp[0].member);
			    yyval.member = yyvsp[-2].member;
			}
break;
case 74:
#line 454 "gram.y"
{
			    char *s;
			    if ((s = alias_add(yyvsp[-2].string, RUNASALIAS, yyvsp[0].member)) != NULL) {
				yyerror(s);
				YYERROR;
			    }
			}
break;
case 77:
#line 467 "gram.y"
{
			    char *s;
			    if ((s = alias_add(yyvsp[-2].string, USERALIAS, yyvsp[0].member)) != NULL) {
				yyerror(s);
				YYERROR;
			    }
			}
break;
case 79:
#line 477 "gram.y"
{
			    LIST_APPEND(yyvsp[-2].member, yyvsp[0].member);
			    yyval.member = yyvsp[-2].member;
			}
break;
case 80:
#line 483 "gram.y"
{
			    yyval.member = yyvsp[0].member;
			    yyval.member->negated = FALSE;
			}
break;
case 81:
#line 487 "gram.y"
{
			    yyval.member = yyvsp[0].member;
			    yyval.member->negated = TRUE;
			}
break;
case 82:
#line 493 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, ALIAS);
			}
break;
case 83:
#line 496 "gram.y"
{
			    NEW_MEMBER(yyval.member, NULL, ALL);
			}
break;
case 84:
#line 499 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, NETGROUP);
			}
break;
case 85:
#line 502 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, USERGROUP);
			}
break;
case 86:
#line 505 "gram.y"
{
			    NEW_MEMBER(yyval.member, yyvsp[0].string, WORD);
			}
break;
#line 1325 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    if (yyss)
            free(yyss);
    if (yyvs)
            free(yyvs);
    yyss = yyssp = NULL;
    yyvs = yyvsp = NULL;
    yystacksize = 0;
    return (1);
yyaccept:
    if (yyss)
            free(yyss);
    if (yyvs)
            free(yyvs);
    yyss = yyssp = NULL;
    yyvs = yyvsp = NULL;
    yystacksize = 0;
    return (0);
}
