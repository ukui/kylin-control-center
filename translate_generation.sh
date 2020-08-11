#!/bin/bash

ts_list=(`ls shell/res/i18n/*.ts`)
source /etc/os-release
version=(`echo $ID`)

for ts in "${ts_list[@]}"
do
    printf "\nprocess ${ts}\n"
    if [ "$version" == "fedora" ];then
        lrelease-qt5 "${ts}"
    else
	lrelease "${ts}"
    fi
done
