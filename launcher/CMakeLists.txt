pkg_check_modules(WPEBackend wpe-0.2)

add_executable(WPELauncher
    main.cpp
)

target_include_directories(WPELauncher PUBLIC
    ${WPEBackend_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${WPE_WEBKIT_INCLUDE_DIRS}
)

target_link_libraries(WPELauncher
    ${WPEBackend_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${WPE_WEBKIT_LIBRARIES}
)

install(TARGETS WPELauncher DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
