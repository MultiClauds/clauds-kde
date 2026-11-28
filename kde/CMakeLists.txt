# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.22)

# The version is in the VERSION file at the repository root (Semantic Versioning, x.y.z). The release
# workflow writes the version of a release into it before it builds. Do not write it anywhere else.
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../VERSION CLAUDS_VERSION_FROM_FILE LIMIT_COUNT 1)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../VERSION)
if(NOT CLAUDS_VERSION_FROM_FILE MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
    message(FATAL_ERROR "VERSION must contain a version like 1.2.3, got '${CLAUDS_VERSION_FROM_FILE}'")
endif()

project(clauds-kde VERSION ${CLAUDS_VERSION_FROM_FILE} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

option(CLAUDS_BUILD_APP "Build the tray app (needs Qt Quick, Qt WebEngine)" ON)
option(CLAUDS_WITH_KF6 "Use KDE Frameworks 6 (StatusNotifierItem, Notifications, CoreAddons, Crash)" ON)
option(BUILD_TESTING "Build the tests" ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Concurrent)

# ---- Core: Qt Core only. All file and process logic. Unit-tested. ----
add_library(clauds-core STATIC
    src/core/Account.h
    src/core/AccountStore.cpp src/core/AccountStore.h
    src/core/AutoSwitch.cpp src/core/AutoSwitch.h
    src/core/ClaudeProcess.cpp src/core/ClaudeProcess.h
    src/core/SessionLinker.cpp src/core/SessionLinker.h
    src/core/Usage.cpp src/core/Usage.h
)
target_include_directories(clauds-core PUBLIC src/core)
target_link_libraries(clauds-core PUBLIC Qt6::Core Qt6::Concurrent)
target_compile_definitions(clauds-core PUBLIC QT_NO_KEYWORDS)

if(CLAUDS_BUILD_APP)
    add_subdirectory(src/app)
endif()

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# ---- Package: cpack -G DEB (for Kubuntu 26.04) ----
set(CPACK_PACKAGE_NAME "clauds")
set(CPACK_PACKAGE_VENDOR "Clauds KDE port")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Switch the Claude desktop app between accounts (KDE Plasma)")
set(CPACK_PACKAGE_CONTACT "https://github.com/LemcheNET/clauds-kde/issues")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "claude-desktop, qml6-module-org-kde-desktop")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qml6-module-qtquick-controls, qml6-module-qtquick-layouts, qml6-module-qtquick-templates, qml6-module-qtquick-window")
include(CPack)
