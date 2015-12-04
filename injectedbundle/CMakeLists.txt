add_library(WPEInjectedBundle SHARED
    main.cpp
)

target_include_directories(WPEInjectedBundle PUBLIC
    ${WPE_WEBKIT_INCLUDE_DIRS}
)
target_link_libraries(WPEInjectedBundle
    ${WPE_WEBKIT_LIBRARIES}
)

install(TARGETS WPEInjectedBundle DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
