#-------------------------------------------------
#
# Project created by QtCreator 2018-04-11T17:23:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mypaint-test
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += mypaint \
               jsonc

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mypaint/brushmodes.c \
    mypaint/fifo.c \
    mypaint/helpers.c \
    mypaint/libmypaint.c \
    mypaint/mypaint.c \
    mypaint/mypaint-brush.c \
    mypaint/mypaint-brush-settings.c \
    mypaint/mypaint-fixed-tiled-surface.c \
    mypaint/mypaint-mapping.c \
    mypaint/mypaint-rectangle.c \
    mypaint/mypaint-surface.c \
    mypaint/mypaint-tiled-surface.c \
    mypaint/operationqueue.c \
    mypaint/rng-double.c \
    mypaint/tilemap.c \
    mypaint/utils.c \
    jsonc/arraylist.c \
    jsonc/debug.c \
    jsonc/json_c_version.c \
    jsonc/json_object.c \
    jsonc/json_object_iterator.c \
    jsonc/json_pointer.c \
    jsonc/json_tokener.c \
    jsonc/json_util.c \
    jsonc/json_visit.c \
    jsonc/libjson.c \
    jsonc/linkhash.c \
    jsonc/printbuf.c \
    jsonc/random_seed.c \
    jsonc/strerror_override.c \
    mypaint/glib/mypaint-brush.c

HEADERS += \
        mainwindow.h \
    mypaint/brushmodes.h \
    mypaint/brushsettings-gen.h \
    mypaint/config.h \
    mypaint/fifo.h \
    mypaint/helpers.h \
    mypaint/mypaint.h \
    mypaint/mypaint-brush.h \
    mypaint/mypaint-brush-settings.h \
    mypaint/mypaint-brush-settings-gen.h \
    mypaint/mypaint-config.h \
    mypaint/mypaint-fixed-tiled-surface.h \
    mypaint/mypaint-glib-compat.h \
    mypaint/mypaint-mapping.h \
    mypaint/mypaint-rectangle.h \
    mypaint/mypaint-surface.h \
    mypaint/mypaint-tiled-surface.h \
    mypaint/operationqueue.h \
    mypaint/rng-double.h \
    mypaint/tiled-surface-private.h \
    mypaint/tilemap.h \
    mypaint/utils.h \
    jsonc/arraylist.h \
    jsonc/bits.h \
    jsonc/debug.h \
    jsonc/json.h \
    jsonc/json_c_version.h \
    jsonc/json_inttypes.h \
    jsonc/json_object.h \
    jsonc/json_object_iterator.h \
    jsonc/json_object_private.h \
    jsonc/json_pointer.h \
    jsonc/json_tokener.h \
    jsonc/json_util.h \
    jsonc/json_visit.h \
    jsonc/linkhash.h \
    jsonc/math_compat.h \
    jsonc/printbuf.h \
    jsonc/random_seed.h \
    jsonc/snprintf_compat.h \
    jsonc/strdup_compat.h \
    jsonc/strerror_override.h \
    jsonc/strerror_override_private.h \
    jsonc/vasprintf_compat.h \
    mypaint/glib/mypaint-brush.h

FORMS += \
        mainwindow.ui
