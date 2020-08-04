#!/bin/bash

ts_list=(`ls shell/res/i18n/*.ts`)

for ts in "${ts_list[@]}"
do
    printf "\nprocess ${ts}\n"
    lrelease "${ts}"
done
