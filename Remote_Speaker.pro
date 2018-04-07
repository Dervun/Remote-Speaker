TEMPLATE = app
TARGET = Remote_Speaker

QT += multimedia core gui
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG +=  c++11


SOURCES += main.cpp \
    GUI/mainwindow.cpp \
    GUI/LayoutManagement/layoutmanager.cpp \
    GUI/LayoutManagement/receivermanager.cpp \
    GUI/LayoutManagement/sendermanager.cpp \
    Network/soundreceiver.cpp \
    Network/soundsender.cpp

HEADERS  += \
    GUI/mainwindow.h \
    GUI/LayoutManagement/layoutmanager.h \
    GUI/LayoutManagement/receivermanager.h \
    GUI/LayoutManagement/sendermanager.h \
    Network/soundreceiver.h \
    Network/soundsender.h

FORMS    += \
    GUI/mainwindow.ui

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/drawable-ldpi/icon.png

RESOURCES += \
    resources.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

TRANSLATIONS += translations/ts/apptr_ru.ts \
    translations/ts/apptr_fr.ts
