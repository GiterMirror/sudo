#define COMMAND 257
#define ALIAS 258
#define DEFVAR 259
#define NTWKADDR 260
#define NETGROUP 261
#define USERGROUP 262
#define WORD 263
#define DIGEST 264
#define DEFAULTS 265
#define DEFAULTS_HOST 266
#define DEFAULTS_USER 267
#define DEFAULTS_RUNAS 268
#define DEFAULTS_CMND 269
#define NOPASSWD 270
#define PASSWD 271
#define NOEXEC 272
#define EXEC 273
#define SETENV 274
#define NOSETENV 275
#define LOG_INPUT 276
#define NOLOG_INPUT 277
#define LOG_OUTPUT 278
#define NOLOG_OUTPUT 279
#define ALL 280
#define COMMENT 281
#define HOSTALIAS 282
#define CMNDALIAS 283
#define USERALIAS 284
#define RUNASALIAS 285
#define ERROR 286
#define TYPE 287
#define ROLE 288
#define PRIVS 289
#define LIMITPRIVS 290
#define MYSELF 291
#define SHA224_TOK 292
#define SHA256_TOK 293
#define SHA384_TOK 294
#define SHA512_TOK 295
#ifndef YYSTYPE_DEFINED
#define YYSTYPE_DEFINED
typedef union {
    struct cmndspec *cmndspec;
    struct defaults *defaults;
    struct member *member;
    struct runascontainer *runas;
    struct privilege *privilege;
    struct sudo_digest *digest;
    struct sudo_command command;
    struct cmndtag tag;
    struct selinux_info seinfo;
    struct solaris_privs_info privinfo;
    char *string;
    int tok;
} YYSTYPE;
#endif /* YYSTYPE_DEFINED */
extern YYSTYPE sudoerslval;
