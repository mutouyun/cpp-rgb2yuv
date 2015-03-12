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
    ../include/detail/basic_concept.hxx \
    ../include/detail/scope_block.hxx \
    ../include/detail/buffer_creator.hxx \
    ../include/detail/pixel_convertor.hxx \
    ../include/detail/pixel_walker.hxx \
    ../include/detail/pixel_iterator.hxx \
    ../include/rgb2yuv_old.hpp \
    ../include/rgb2yuv.hpp
