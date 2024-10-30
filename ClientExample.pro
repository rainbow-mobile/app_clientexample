QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG+=no_keywords
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# set home dir
HOME = $$system(echo $HOME)
message($$HOME)

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    api_get.h \
    api_post.h \
    mainwindow.h \
    socket.h \
    socket.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += \
    $$HOME/socket.io-client-cpp/build/include
DEPENDPATH += $$HOME/socket.io-client-cpp/build/lib

INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG(release, debug|release): LIBS += -L/usr/local/lib/ -lsioclient
else:CONFIG(debug, debug|release): LIBS += -L/usr/local/lib/ -lsioclient

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build/lib/Release/ -lsioclient
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build/lib/Debug/ -lsioclient
