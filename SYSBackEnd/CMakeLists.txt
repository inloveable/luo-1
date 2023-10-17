cmake_minimum_required(VERSION 3.5)

project(SYSBackEnd VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_PREFIX_PATH  "D:/LIBARAY/glog-master/build-glog-master-Desktop_Qt_6_4_3_MinGW_64_bit-Debug/install" )
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Network Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network Core )


find_package(glog REQUIRED)

link_libraries(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libQT_API2_0.a)

set(PROJECT_SOURCES
     fft.cpp
     fft.h
     iir.hpp
     iir.cpp
     math_util.cpp
     math_util.h
     srs.h
     srs.cpp
     sysbackend.hpp sysbackend.cpp
     networkmanager.h networkmanager.cpp
     pvibapi.hpp pvibapi.cpp


     3rdparty/ApiPvib2_0.h
     3rdparty/atomicops.h
     3rdparty/readerwritercircularbuffer.h
     3rdparty/readerwriterqueue.h

     ../publicDefs.hpp

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(SYSBackEnd OBJECT
	MANUAL_FINALIZATION
	${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SYSBackEnd APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
	add_library(SYSBackEnd OBJECT
	    ${PROJECT_SOURCES}
	)
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
	add_library(SYSBackEnd OBJECT
	    ${PROJECT_SOURCES}
	)
    endif()
endif()

target_link_libraries(SYSBackEnd PRIVATE Qt${QT_VERSION_MAJOR}::Core
     Qt${QT_VERSION_MAJOR}::Network
     ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libQT_API2_0.a
     glog::glog
)
target_include_directories(SYSBackEnd PUBLIC .)
set_target_properties(SYSBackEnd PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS SYSBackEnd
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)

endif()
