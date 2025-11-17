cmake_minimum_required(VERSION 3.16)

project(Warehouse1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Warehouse1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        errorhandler.cpp errorhandler.h loginwindow.cpp loginwindow.h main.cpp mainwindow.cpp mainwindow.h operation.cpp operation.h product.cpp product.h registrationwindow.cpp registrationwindow.h sectionwindow.cpp sectionwindow.h user.cpp user.h usermanager.cpp usermanager.h WarehouseSystem.pro warehousewindow.cpp warehousewindow.h
        employee.h employee.cpp
        administrator.h administrator.cpp
        operationshistorywindow.h operationshistorywindow.cpp
        deliverywindow.h deliverywindow.cpp
        shipmentformwindow.h shipmentformwindow.cpp
        transferformwindow.h transferformwindow.cpp
        accountmanagementwindow.h accountmanagementwindow.cpp
        adduserwindow.h adduserwindow.cpp


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Warehouse1 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Warehouse1 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Warehouse1
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Warehouse1 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Warehouse1)
endif()
set_target_properties(Warehouse1 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Warehouse1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Warehouse1)
endif()
