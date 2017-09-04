add_executable(WPELauncher
    input.cpp
    keyboardeventhandler.cpp
    main.cpp
)

target_include_directories(WPELauncher PUBLIC
    ${GLib_INCLUDE_DIRS}
    ${LIBINPUT_INCLUDE_DIRS}
    ${LIBUDEV_INCLUDE_DIRS}
    ${LIBXKBCOMMON_INCLUDE_DIRS}
    ${WPE_WEBKIT_INCLUDE_DIRS}
)
target_link_libraries(WPELauncher
    ${GLib_LIBRARIES}
    ${LIBINPUT_LIBRARIES}
    ${LIBUDEV_LIBRARIES}
    ${LIBXKBCOMMON_LIBRARIES}
    ${WPE_WEBKIT_LIBRARIES}
)

install(TARGETS WPELauncher DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
