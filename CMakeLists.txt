cmake_minimum_required(VERSION 3.5)

project(ExperimentHeadMachine VERSION 0.9 LANGUAGES CXX)
include(CTest)
include(Dart)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#privides x64d glog path
#install x64r glog for youself
list(APPEND CMAKE_PREFIX_PATH "D:/LIBARAY/glog-master/build-glog-master-Desktop_Qt_6_4_3_MinGW_64_bit-Debug/install" )
#How to install? download glog src code first of course.
#for instance, on windouws:
#1.cmake -S <glog-path> -B buildr
#2.cmake --build buildr --target install --config Release -D CMAKE_INSTALL_PREFIX=<...>
#:-D CMAKE_INSTALL_PREFIX=<...> is optional,by default system lib path

foreach(path ${CMAKE_PREFIX_PATH})
     message("cmake prefix path:${path}")
endforeach()

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/appCaches/defaults")
file(COPY "defaults/defaultChannelArg.json" "defaults/defaultExSettings.json"
     "defaults/ExDocumentTemplate.json"
     DESTINATION "${CMAKE_BINARY_DIR}/appCaches/defaults")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Charts)

find_package(glog REQUIRED)

add_subdirectory("EnhancedChartViewSystem")
add_subdirectory("PrintSystem")
add_subdirectory("SYSBackEnd")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
	firstwindow.h firstwindow.cpp firstwindow.ui
	connectinputdialog.h connectinputdialog.cpp connectinputdialog.ui
	channelargdialog.cpp channelargdialog.h  channelargdialog.ui
	projectunitdialog.h projectunitdialog.cpp projectunitdialog.ui
	datamanager.h datamanager.cpp
	displaysettingdialog.h displaysettingdialog.cpp displaysettingdialog.ui
	exargsettingdialog.cpp exargsettingdialog.h exargsettingdialog.ui
	mousesettingdialog.cpp mousesettingdialog.h mousesettingdialog.ui
	exsettingreplydialog.hpp exsettingreplydialog.cpp exsettingreplydialog.ui
	uimanager.hpp uimanager.cpp
	shockreplaytable.hpp shockreplaytable.cpp shockreplaytable.ui
	timezonetablewidget.hpp timezonetablewidget.cpp timezonetablewidget.ui
	printdialog.hpp printdialog.cpp printdialog.ui
	exdocumentinformationdialog.hpp exdocumentinformationdialog.cpp exdocumentinformationdialog.ui
	experimentdocument.hpp experimentdocument.cpp
	publicDefs.hpp

)




if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ExperimentHeadMachine
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ExperimentHeadMachine APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ExperimentHeadMachine SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
	add_executable(ExperimentHeadMachine
            ${PROJECT_SOURCES}
	)
    endif()
endif()

target_link_libraries(ExperimentHeadMachine PRIVATE
     Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network
     Qt${QT_VERSION_MAJOR}::Charts glog::glog EnhancedChartViewSystem PrintSystem SYSBackEnd)



set_target_properties(ExperimentHeadMachine PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ExperimentHeadMachine
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ExperimentHeadMachine)
endif()
