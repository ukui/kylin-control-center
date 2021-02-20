/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <security/pam_appl.h>
#include <sys/mman.h>
static int toParent = 0;
static int fromChild = 0;

typedef struct pam_message PAM_MESSAGE;
typedef struct pam_response PAM_RESPONSE;

static void
writeData(int fd, const void *buf, ssize_t count)
{
    if(write(fd, buf, count) != count)
    printf("write to parent failed:  %s\n",strerror(errno));
}

static void
writeString(int fd, const char *data)
{
    int length = data ? strlen(data) : -1;
    writeData(fd, &length, sizeof(length));
    if(data)
        writeData(fd, data, sizeof(char) * length);
}

static int
readData(int fd, void *buf, size_t count)
{
    ssize_t nRead = read(fd, buf, count);
    if(nRead < 0)
    printf("read data failed:  %s\n",strerror(errno));
    return nRead;
}

static char *
readString(int fd)
{
    int length;

    if(readData(fd, &length, sizeof(length)) <= 0)
        return NULL;
    if(length <= 0)
        return NULL;

    char *value = (char *)malloc(sizeof(char) * (length + 1));
    readData(fd, value, length);
    value[length] = '\0';

    return value;
}

static int
pam_conversation(int msgLength, const struct pam_message **msg,
                PAM_RESPONSE **resp, void */*appData*/)
{
    PAM_RESPONSE *response = (PAM_RESPONSE*)calloc(msgLength,sizeof(PAM_RESPONSE));

    int authComplete = 0;
    writeData(toParent, (const void*)&authComplete, sizeof(authComplete));
    writeData(toParent, (const void*)&msgLength, sizeof(msgLength));
    //发送pam消息
    for(int i = 0; i < msgLength; i++)
    {
        const struct pam_message *m = msg[i];
        writeData(toParent, (const void *)&m->msg_style, sizeof(m->msg_style));
        writeString(toParent, m->msg);
    }
    //读取响应
    for(int i = 0; i < msgLength; i++)
    {
        PAM_RESPONSE *r = &response[i];
        readData(fromChild, &r->resp_retcode, sizeof(r->resp_retcode));
        r->resp = readString(fromChild);
    }
    *resp = response;
    return PAM_SUCCESS;
}

static void
_authenticate(const char *userName)
{
//    printf("authenticate  %s\n",userName);

    pam_handle_t *pamh = NULL;
    char *newUser;
    int ret;
    int authRet;
    struct pam_conv conv;

    conv.conv = pam_conversation;
    conv.appdata_ptr = NULL;

    ret = pam_start("control-center", userName, &conv, &pamh);
    if(ret != PAM_SUCCESS)
    {
        printf("failed to start PAM:  = %s\n", pam_strerror(NULL, ret));
    }

    authRet = pam_authenticate(pamh, 0);

    ret = pam_get_item(pamh, PAM_USER, (const void **)&newUser);
    if(ret != PAM_SUCCESS)
    {
        pam_end(pamh, 0);
        printf("failed to get username\n");
    }
    free(newUser);
    newUser = NULL;
//    fprintf(stderr, "authentication result: %d\n", authRet);

    // 发送认证结果
    int authComplete = 1;
    writeData(toParent, (const void*)&authComplete, sizeof(authComplete));
    writeData(toParent, (const void *)&authRet, sizeof(authRet));

    /* ---认证完成\n*/
    _exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        return EXIT_FAILURE;
    }
    toParent = atoi (argv[1]);
    fromChild = atoi (argv[2]);
    if (toParent == 0 || fromChild == 0)
    {
        printf ("Invalid file descriptors %s %s\n", argv[2], argv[3]);
        return EXIT_FAILURE;
    }
    mlockall (MCL_CURRENT | MCL_FUTURE);
    fcntl (toParent, F_SETFD, FD_CLOEXEC);
    fcntl (fromChild, F_SETFD, FD_CLOEXEC);


    _authenticate(argv[3]);

}
