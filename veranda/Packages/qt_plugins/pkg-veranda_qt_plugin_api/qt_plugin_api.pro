QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = veranda
TEMPLATE = app

INCLUDEPATH += include \
               # Hack to make box2d work with autocomplete in qtcreator
               ../pkg_Box2D/include \
               ../pkg_CatchTesting/include

DISTFILES += \
    package.xml \
    CMakeLists.txt

HEADERS += \
    include/veranda_qt_plugins/world_object_component_plugin.h