TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = \
    registeredQDbus \
    plugins\
    shell \
    tastenbrett \


TRANSLATIONS += \
    shell/res/i18n/zh_CN.ts \
    shell/res/i18n/tr.ts \
    shell/res/i18n/bo.ts

