QT += core gui widgets

CONFIG += c++17

TARGET = WarehouseSystem
TEMPLATE = app

SOURCES += \
    appexception.cpp \
    fileexception.cpp \
    filehandler.cpp \
    inputvalidator.cpp \
    main.cpp \
    loginwindow.cpp \
    registrationwindow.cpp \
    user.cpp \
    employee.cpp \
    administrator.cpp \
    usermanager.cpp \
    mainwindow.cpp \
    validationexception.cpp \
    warehousewindow.cpp \
    sectionwindow.cpp \
    product.cpp \
    operation.cpp \
    operationshistorywindow.cpp \
    deliverywindow.cpp \
    shipmentformwindow.cpp \
    transferformwindow.cpp \
    accountmanagementwindow.cpp \
    adduserwindow.cpp \
    deletionrecord.cpp \
    deletionhistory.cpp

HEADERS += \
    appexception.h \
    fileexception.h \
    filehandler.h \
    inputvalidator.h \
    loginwindow.h \
    registrationwindow.h \
    user.h \
    employee.h \
    administrator.h \
    usermanager.h \
    mainwindow.h \
    validationexception.h \
    warehousewindow.h \
    sectionwindow.h \
    product.h \
    operation.h \
    operationshistorywindow.h \
    deliverywindow.h \
    shipmentformwindow.h \
    transferformwindow.h \
    accountmanagementwindow.h \
    adduserwindow.h \
    customlist.h \
    deletionrecord.h \
    deletionhistory.h

# Установка кодировки
QMAKE_CXXFLAGS += -finput-charset=UTF-8
DEFINES += QT_USE_QSTRINGBUILDER

# Отключаем некоторые предупреждения если нужно
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

DISTFILES += \
    CMakeLists.txt
