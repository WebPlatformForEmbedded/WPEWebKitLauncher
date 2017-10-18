add_executable(WPELauncher
    main.cpp
)

target_include_directories(WPELauncher PUBLIC
    ${GLib_INCLUDE_DIRS}
    ${WPE_WEBKIT_INCLUDE_DIRS}
)
set(WPE_LIBRARIES )
foreach(lib ${WPE_WEBKIT_LIBRARIES})
    find_library(WPE_LIBRARY
        NAMES lib
        HINTS ${PC_WPE_WEBKIT_LIBDIR} ${PC_WPE_WEBKIT_LIBRARY_DIRS}
        )
    list(APPEND WPE_LIBRARIES ${WPE_LIBRARY})
endforeach()

target_link_libraries(WPELauncher
    ${GLib_LIBRARIES}
    ${WPE_LIBRARIES}
)

install(TARGETS WPELauncher DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
