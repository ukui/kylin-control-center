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

extern "C" {

#include <stdio.h>
#include <stdbool.h>
#include <cstring>

#include <crypt.h>

}

#define SHADOW_FILE "/etc/shadow"
#define BUFF_SIZE 256
#define SIZE 256

static bool checkpwd(char * name, char * currentPwd);

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    Widget w;
//    w.show();
//    return a.exec();
    if (3 == argc){
        if (checkpwd(argv[1], argv[2]))
            printf("Success\n");
    }

    return 0;
}

static bool checkpwd(char * username, char * currentPwd){

    char buf[BUFF_SIZE] = {0};  /*缓冲区*/
    FILE *fp = NULL/*,*fd = NULL,*fp_check_shadow = NULL*/;           /*文件指针*/
    int len = 0;             /*行字符个数*/
    int line_len = 0;
    char name[SIZE]={0};
    char pwd[SIZE]={0};
    char tmp_1[SIZE]={0};
    char tmp_2[SIZE]={0};
    char tmp_3[SIZE]={0};
    char tmp_4[SIZE]={0};
    char tmp_5[SIZE]={0};
    char tmp_6[SIZE]={0};
    char tmp_7[SIZE]={0};

    if((fp = fopen(SHADOW_FILE,"r")) == NULL){
        perror("/etc/shadow fail to read\n");
        return false;
    }

    while(fgets(buf,BUFF_SIZE,fp) != NULL){
        line_len = strlen(buf);
        len += line_len;
        sscanf(buf,"%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]",name,pwd,tmp_1,tmp_2,tmp_3,tmp_4,tmp_5,tmp_6,tmp_7);
        //printf("line_name:%s  username:%s   pwd:%s\n",name,username,pwd);

        if(0 == strcmp(name,username)){
            char * encrypted = NULL;
            encrypted = crypt(currentPwd, pwd);
            if ( encrypted && strcmp(encrypted, pwd) == 0){
                return true;
            } else {
                return false;
            }
        }
    }


    return false;
}
