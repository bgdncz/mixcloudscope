PROJECT(mixcloud)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

set(CMAKE_AUTOMOC ON)
set(UBUNTU_MANIFEST_PATH "manifest.json" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")
set(UBUNTU_PROJECT_TYPE "Scope" CACHE INTERNAL "Tells QtCreator this is a Scope project")

ADD_DEFINITIONS(
  -fno-permissive
  -std=c++11
  -pedantic
  -Wall
  -Wextra
  -fPIC
  -DQT_NO_KEYWORDS
)

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(
    SCOPE
    libunity-scopes>=0.6.0
    REQUIRED
)
FIND_PACKAGE(Qt5Core REQUIRED)

INSTALL(
  FILES
    com.ubuntu.developer.boghison.mixcloud_scope.ini
    manifest.json
    mixcloud.apparmor
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}
)

ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(src)