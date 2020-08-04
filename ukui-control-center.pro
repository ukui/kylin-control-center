TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = \
    registeredQDbus \
    plugins\
    shell \
#    tastenbrett \


TRANSLATIONS += \
    shell/res/i18n/zh_CN.ts \
    shell/res/i18n/tr.ts \
    shell/res/i18n/bo.ts


# Automating generation .qm files from .ts files
CONFIG(release, debug|release) {
    !system($$PWD/translate_generation.sh): error("Failed to generate translation")
}

isEmpty(PREFIX) {
    PREFIX = /usr
}

qm_files.path = $${PREFIX}/share/ukui-control-center/shell/res/i18n/
qm_files.files = shell/res/i18n/*.qm

INSTALLS += qm_files
