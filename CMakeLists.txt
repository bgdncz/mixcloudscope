PROJECT(mixcloud)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

SET(CMAKE_AUTOMOC ON)
SET(UBUNTU_MANIFEST_PATH "manifest.json" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")
SET(UBUNTU_PROJECT_TYPE "Scope" CACHE INTERNAL "Tells QtCreator this is a Scope project")
SET(CMAKE_INSTALL_PREFIX /)
SET(SCOPE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/scope)

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
    net-cpp>=1.1.0
    REQUIRED
)
FIND_PACKAGE(Qt5Core REQUIRED)
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS} ${SCOPE_INCLUDE_DIRS})
execute_process(
  COMMAND dpkg-architecture -qDEB_HOST_ARCH
  OUTPUT_VARIABLE CLICK_ARCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
configure_file(manifest.json ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
INSTALL(FILES com.ubuntu.developer.boghison.mixcloud_scope.ini DESTINATION ${SCOPE_INSTALL_DIR})
configure_file(
  "${CMAKE_SOURCE_DIR}/com.ubuntu.developer.boghison.mixcloud_scope.ini"
  "${CMAKE_BINARY_DIR}/src/com.ubuntu.developer.boghison.mixcloud_scope.ini"
)
INSTALL(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/manifest.json
    mixcloud.apparmor
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}
)

ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(po)
