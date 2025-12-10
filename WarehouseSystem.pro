QT += core gui widgets

CONFIG += c++17

TARGET = WarehouseSystem
TEMPLATE = app

SOURCES += \
    main.cpp \
    loginwindow.cpp \
    registrationwindow.cpp \
    user.cpp \
    employee.cpp \
    administrator.cpp \
    usermanager.cpp \
    mainwindow.cpp \
    warehousewindow.cpp \
    sectionwindow.cpp \
    product.cpp \
    operation.cpp \
    operationshistorywindow.cpp \
    deliverywindow.cpp \
    shipmentformwindow.cpp \
    transferformwindow.cpp \
    accountmanagementwindow.cpp \
    accountmanagementwindow.cpp \
    adduserwindow.cpp \
    exceptionhandler.cpp

HEADERS += \
    loginwindow.h \
    registrationwindow.h \
    user.h \
    employee.h \
    administrator.h \
    usermanager.h \
    mainwindow.h \
    warehousewindow.h \
    sectionwindow.h \
    product.h \
    operation.h \
    operationshistorywindow.h \
    deliverywindow.h \
    shipmentformwindow.cpp \
    transferformwindow.h \
    accountmanagementwindow.h \
    accountmanagementwindow.h \
    adduserwindow.h \
    customlist.h \
    exceptionhandler.h

# Установка кодировки
QMAKE_CXXFLAGS += -finput-charset=UTF-8
DEFINES += QT_USE_QSTRINGBUILDER

# Отключаем некоторые предупреждения если нужно
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

DISTFILES += \
    CMakeLists.txt
