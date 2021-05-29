#include <QCoreApplication>

#include <glib.h>
#include "run-passwd2.h"

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

    passwd_handler = passwd_init();

    passwd_change_password(passwd_handler, argv[1], argv[2], chpasswd_cb, NULL);

    return a.exec();
}

static void
auth_cb (PasswdHandler *passwd_handler, GError *error, gpointer user_data)
{

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


    if (!error) {

        qApp->exit(0);
    } else {

        passwd_destroy (passwd_handler);

        qApp->exit(1);
    }

}
