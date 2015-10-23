cmake_minimum_required(VERSION 2.8)
project(WPELauncher)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(WPELauncher
    main.cpp
)

#find_package(BCMHost REQUIRED)
find_package(GDL REQUIRED)
find_package(WPEWebKit REQUIRED)
find_package(GLIB REQUIRED)

target_include_directories(WPELauncher PUBLIC
    ${CMAKE_SOURCE_DIR}
#    ${BCM_HOST_INCLUDE_DIRS}
    ${GDL_INCLUDE_DIRS}
    ${WPE_WEBKIT_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
)
target_link_libraries(WPELauncher
#    ${BCM_HOST_LIBRARIES}
    ${GDL_LIBRARIES}
    ${WPE_WEBKIT_LIBRARIES}
    ${GLIB_LIBRARIES}
    "WPEWebKit"
)

install(TARGETS WPELauncher DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
