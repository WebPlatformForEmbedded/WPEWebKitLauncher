cmake_minimum_required(VERSION 2.8)
project(WPELauncher)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(WPEWebKit REQUIRED)

find_package(PkgConfig)
pkg_search_module(PC_GLIB REQUIRED glib-2.0)

set(GLIB_LIBRARIES )
foreach(lib ${PC_GLIB_LIBRARIES})
    find_library(GLIB_LIBRARY
        NAMES ${lib}
        HINTS ${PC_GLIB_LIBDIR} ${PC_GLIB_LIBRARY_DIRS}
        )
    list(APPEND GLIB_LIBRARIES ${GLIB_LIBRARY})
endforeach()

set(GLIB_INCLUDE_DIRS ${PC_GLIB_INCLUDE_DIRS})

file(WRITE ${CMAKE_BINARY_DIR}/test_atomic.cpp
     "#include <atomic>\n"
     "int main() { std::atomic<int64_t> i(0); i++; return 0; }\n")
try_compile(ATOMIC_BUILD_SUCCEEDED ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/test_atomic.cpp)
if (NOT ATOMIC_BUILD_SUCCEEDED)
    list(APPEND WPE_WEBKIT_LIBRARIES atomic)
endif ()
file(REMOVE ${CMAKE_BINARY_DIR}/test_atomic.cpp)

add_subdirectory(injectedbundle)
add_subdirectory(launcher)
