QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets axcontainer

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ExportTable.cpp \
    ItemTable.cpp \
    cellitemchkbox.cpp \
    cellitemspinbox.cpp \
    lineedit.cpp \
    main.cpp \
    mainwindow.cpp \
    matchtable.cpp \
    newitemdialog.cpp \
    parser.cpp \
    settingdialog.cpp \
    spinboxdelegate.cpp

HEADERS += \
    ExportTable.h \
    ItemTable.h \
    cellitemchkbox.h \
    cellitemspinbox.h \
    lineedit.h \
    mainwindow.h \
    matchtable.h \
    newitemdialog.h \
    parser.h \
    settingdialog.h \
    spinboxdelegate.h

FORMS += \
    mainwindow.ui \
    newitemdialog.ui \
    newitemdialog.ui \
    settingdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qtresource.qrc

RC_ICONS = money_512.ico
