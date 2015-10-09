cmake_minimum_required(VERSION 2.8)
project(WPELauncher)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(WPELauncher
    main.cpp
)

find_package(BCMHost REQUIRED)
find_package(WPEWebKit REQUIRED)

target_include_directories(WPELauncher PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${BCM_HOST_INCLUDE_DIRS}
    ${WPE_WEBKIT_INCLUDE_DIRS}
)
target_link_libraries(WPELauncher
    ${BCM_HOST_LIBRARIES}
    ${WPE_WEBKIT_LIBRARIES}
)

install(TARGETS WPELauncher DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
