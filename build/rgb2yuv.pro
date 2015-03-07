TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/bin/Debug
}
CONFIG(release, debug|release) {
    DEFINES += NDEBUG
    DESTDIR = $$PWD/bin/Release
}

INCLUDEPATH += \
    ./ \
    ../test

SOURCES += \
    ../test/test.cpp

HEADERS += \
    ../include/rgb2yuv.hpp
