#ifndef RUNPASSWD_H
#define RUNPASSWD_H


struct PasswdHandler;

typedef struct PasswdHandler PasswdHandler;

typedef void (*PasswdCallback) (PasswdHandler * passwd_handler, GError * error, const gpointer user_data);

/* Error codes */
typedef enum {
    PASSWD_ERROR_REJECTED,		/* New password is not secure enough */
    PASSWD_ERROR_AUTH_FAILED,	/* Wrong old password, or PAM failure */
    PASSWD_ERROR_REAUTH_FAILED,	/* Password has changed since first authentication */
    PASSWD_ERROR_BACKEND,		/* Backend error */
    PASSWD_ERROR_UNKNOWN		/* General error */
} PasswdError;

PasswdHandler *passwd_init                ();

void           passwd_destroy             (PasswdHandler *passwd_handler);

void           passwd_authenticate        (PasswdHandler *passwd_handler,
                                           const char    *current_password,
                                           PasswdCallback cb,
                                           gpointer       user_data);

gboolean       passwd_change_password     (PasswdHandler *passwd_handler,
                                           const char    *new_password,
                                           PasswdCallback cb,
                                           const gpointer user_data);

#endif // RUNPASSWD_H
