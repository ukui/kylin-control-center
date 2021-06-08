#include <QCoreApplication>

#include <glib.h>
#include "run-passwd.h"

PasswdHandler *passwd_handler = NULL;

static void auth_cb (PasswdHandler *passwd_handler, GError *error, gpointer user_data);
static void chpasswd_cb (PasswdHandler *passwd_handler, GError *error, gpointer user_data);

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        return -1;
    }

    QCoreApplication a(argc, argv);

    passwd_handler = passwd_init ();

    passwd_authenticate (passwd_handler, argv[1], auth_cb, argv[2]);

    return a.exec();
}

static void
auth_cb (PasswdHandler *passwd_handler,
         GError        *error,
         gpointer       user_data)
{
    char * pwd = (char*) user_data;

    if (error){
        g_warning("%s", error->message);
        return;
    }

    passwd_change_password (passwd_handler, pwd, chpasswd_cb, NULL);

}

/**
 * @brief chpasswd_cb
 * @param passwd_handler
 * @param error
 * @param user_data
 */
static void
chpasswd_cb (PasswdHandler *passwd_handler,
             GError        *error,
             gpointer       user_data)
{
//    char *primary_text;
    char *secondary_text;

    if (!error) {
        //finish_password_change (TRUE);
//        primary_text = "Success";
        secondary_text = "";

        printf("%s\n", secondary_text);

        qApp->exit(0);
    } else {
//        primary_text = "Failed";
        secondary_text = error->message;

        char ** lines = g_strsplit(secondary_text, "\n", -1);

        printf("%s\n", lines[0]);

        passwd_destroy (passwd_handler);

        qApp->exit(1);
    }

}
