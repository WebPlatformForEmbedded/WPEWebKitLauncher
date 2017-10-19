# - Try to find wpe-webkit.
# Once done, this will define
#
#  WPE_WEBKIT_INCLUDE_DIRS - the bcm_host include directories
#  WPE_WEBKIT_LIBRARIES - link these to use wpe-webkit.
#
# Copyright (C) 2015 Igalia S.L.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND ITS CONTRIBUTORS ``AS
# IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ITS
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

find_package(PkgConfig)
pkg_check_modules(PC_WPE_WEBKIT wpe-webkit)

find_path(WPE_WEBKIT_INCLUDE_DIR
    NAMES WPE/WebKit.h
    HINTS ${PC_WPE_WEBKIT_INCLUDEDIR} ${PC_WPE_WEBKIT_INCLUDE_DIRS}
    PATH_SUFFIXES wpe-0.1
)
set(WPE_WEBKIT_INCLUDE_DIRS ${WPE_WEBKIT_INCLUDE_DIR} ${WPE_WEBKIT_INCLUDE_DIR}/WPE)

find_library(WPE_WEBKIT_LIBRARIES
    NAMES WPEWebKit
    HINTS ${PC_WPE_WEBKIT_LIBDIR} ${PC_WPE_WEBKIT_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(WPE_WEBKIT DEFAULT_MSG WPE_WEBKIT_LIBRARIES)

mark_as_advanced(WPE_WEBKIT_INCLUDE_DIRS WPE_WEBKIT_LIBRARIES)
