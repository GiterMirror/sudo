#define def_syslog              (sudo_defs_table[0].sd_un.ival)
#define I_SYSLOG                0
#define def_syslog_goodpri      (sudo_defs_table[1].sd_un.ival)
#define I_SYSLOG_GOODPRI        1
#define def_syslog_badpri       (sudo_defs_table[2].sd_un.ival)
#define I_SYSLOG_BADPRI         2
#define def_long_otp_prompt     (sudo_defs_table[3].sd_un.flag)
#define I_LONG_OTP_PROMPT       3
#define def_ignore_dot          (sudo_defs_table[4].sd_un.flag)
#define I_IGNORE_DOT            4
#define def_mail_always         (sudo_defs_table[5].sd_un.flag)
#define I_MAIL_ALWAYS           5
#define def_mail_badpass        (sudo_defs_table[6].sd_un.flag)
#define I_MAIL_BADPASS          6
#define def_mail_no_user        (sudo_defs_table[7].sd_un.flag)
#define I_MAIL_NO_USER          7
#define def_mail_no_host        (sudo_defs_table[8].sd_un.flag)
#define I_MAIL_NO_HOST          8
#define def_mail_no_perms       (sudo_defs_table[9].sd_un.flag)
#define I_MAIL_NO_PERMS         9
#define def_tty_tickets         (sudo_defs_table[10].sd_un.flag)
#define I_TTY_TICKETS           10
#define def_lecture             (sudo_defs_table[11].sd_un.tuple)
#define I_LECTURE               11
#define def_lecture_file        (sudo_defs_table[12].sd_un.str)
#define I_LECTURE_FILE          12
#define def_authenticate        (sudo_defs_table[13].sd_un.flag)
#define I_AUTHENTICATE          13
#define def_root_sudo           (sudo_defs_table[14].sd_un.flag)
#define I_ROOT_SUDO             14
#define def_log_host            (sudo_defs_table[15].sd_un.flag)
#define I_LOG_HOST              15
#define def_log_year            (sudo_defs_table[16].sd_un.flag)
#define I_LOG_YEAR              16
#define def_shell_noargs        (sudo_defs_table[17].sd_un.flag)
#define I_SHELL_NOARGS          17
#define def_set_home            (sudo_defs_table[18].sd_un.flag)
#define I_SET_HOME              18
#define def_always_set_home     (sudo_defs_table[19].sd_un.flag)
#define I_ALWAYS_SET_HOME       19
#define def_path_info           (sudo_defs_table[20].sd_un.flag)
#define I_PATH_INFO             20
#define def_fqdn                (sudo_defs_table[21].sd_un.flag)
#define I_FQDN                  21
#define def_insults             (sudo_defs_table[22].sd_un.flag)
#define I_INSULTS               22
#define def_requiretty          (sudo_defs_table[23].sd_un.flag)
#define I_REQUIRETTY            23
#define def_env_editor          (sudo_defs_table[24].sd_un.flag)
#define I_ENV_EDITOR            24
#define def_rootpw              (sudo_defs_table[25].sd_un.flag)
#define I_ROOTPW                25
#define def_runaspw             (sudo_defs_table[26].sd_un.flag)
#define I_RUNASPW               26
#define def_targetpw            (sudo_defs_table[27].sd_un.flag)
#define I_TARGETPW              27
#define def_use_loginclass      (sudo_defs_table[28].sd_un.flag)
#define I_USE_LOGINCLASS        28
#define def_set_logname         (sudo_defs_table[29].sd_un.flag)
#define I_SET_LOGNAME           29
#define def_stay_setuid         (sudo_defs_table[30].sd_un.flag)
#define I_STAY_SETUID           30
#define def_preserve_groups     (sudo_defs_table[31].sd_un.flag)
#define I_PRESERVE_GROUPS       31
#define def_loglinelen          (sudo_defs_table[32].sd_un.ival)
#define I_LOGLINELEN            32
#define def_timestamp_timeout   (sudo_defs_table[33].sd_un.ival)
#define I_TIMESTAMP_TIMEOUT     33
#define def_passwd_timeout      (sudo_defs_table[34].sd_un.ival)
#define I_PASSWD_TIMEOUT        34
#define def_passwd_tries        (sudo_defs_table[35].sd_un.ival)
#define I_PASSWD_TRIES          35
#define def_umask               (sudo_defs_table[36].sd_un.mode)
#define I_UMASK                 36
#define def_logfile             (sudo_defs_table[37].sd_un.str)
#define I_LOGFILE               37
#define def_mailerpath          (sudo_defs_table[38].sd_un.str)
#define I_MAILERPATH            38
#define def_mailerflags         (sudo_defs_table[39].sd_un.str)
#define I_MAILERFLAGS           39
#define def_mailto              (sudo_defs_table[40].sd_un.str)
#define I_MAILTO                40
#define def_mailsub             (sudo_defs_table[41].sd_un.str)
#define I_MAILSUB               41
#define def_badpass_message     (sudo_defs_table[42].sd_un.str)
#define I_BADPASS_MESSAGE       42
#define def_timestampdir        (sudo_defs_table[43].sd_un.str)
#define I_TIMESTAMPDIR          43
#define def_timestampowner      (sudo_defs_table[44].sd_un.str)
#define I_TIMESTAMPOWNER        44
#define def_exempt_group        (sudo_defs_table[45].sd_un.str)
#define I_EXEMPT_GROUP          45
#define def_passprompt          (sudo_defs_table[46].sd_un.str)
#define I_PASSPROMPT            46
#define def_passprompt_override (sudo_defs_table[47].sd_un.flag)
#define I_PASSPROMPT_OVERRIDE   47
#define def_runas_default       (sudo_defs_table[48].sd_un.str)
#define I_RUNAS_DEFAULT         48
#define def_secure_path         (sudo_defs_table[49].sd_un.str)
#define I_SECURE_PATH           49
#define def_editor              (sudo_defs_table[50].sd_un.str)
#define I_EDITOR                50
#define def_listpw              (sudo_defs_table[51].sd_un.tuple)
#define I_LISTPW                51
#define def_verifypw            (sudo_defs_table[52].sd_un.tuple)
#define I_VERIFYPW              52
#define def_noexec              (sudo_defs_table[53].sd_un.flag)
#define I_NOEXEC                53
#define def_noexec_file         (sudo_defs_table[54].sd_un.str)
#define I_NOEXEC_FILE           54
#define def_ignore_local_sudoers (sudo_defs_table[55].sd_un.flag)
#define I_IGNORE_LOCAL_SUDOERS  55
#define def_closefrom           (sudo_defs_table[56].sd_un.ival)
#define I_CLOSEFROM             56
#define def_closefrom_override  (sudo_defs_table[57].sd_un.flag)
#define I_CLOSEFROM_OVERRIDE    57
#define def_setenv              (sudo_defs_table[58].sd_un.flag)
#define I_SETENV                58
#define def_env_reset           (sudo_defs_table[59].sd_un.flag)
#define I_ENV_RESET             59
#define def_env_check           (sudo_defs_table[60].sd_un.list)
#define I_ENV_CHECK             60
#define def_env_delete          (sudo_defs_table[61].sd_un.list)
#define I_ENV_DELETE            61
#define def_env_keep            (sudo_defs_table[62].sd_un.list)
#define I_ENV_KEEP              62
#define def_role                (sudo_defs_table[63].sd_un.str)
#define I_ROLE                  63
#define def_type                (sudo_defs_table[64].sd_un.str)
#define I_TYPE                  64

enum def_tupple {
	never,
	once,
	always,
	any,
	all
};
