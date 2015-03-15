SET(SCOPE_SOURCES
  scopehelper.cpp
  preview.cpp
  query.cpp
  scope.cpp
)

SET(SCOPE_HEADERS
  scopehelper.h
  preview.h
  query.h
  scope.h
)

ADD_LIBRARY(
  scope-static OBJECT
  ${SCOPE_SOURCES}
  ${SCOPE_HEADERS}
)

# Ensure we export all the symbols
SET_TARGET_PROPERTIES(
  scope-static
  PROPERTIES
    LINK_FLAGS "-Wl,--export-all-symbols"
)

ADD_LIBRARY(
  scope SHARED
  $<TARGET_OBJECTS:scope-static>
)

TARGET_LINK_LIBRARIES(
  scope
  ${SCOPE_LDFLAGS}
)

SET_TARGET_PROPERTIES(
  scope
  PROPERTIES
    OUTPUT_NAME "com.ubuntu.developer.boghison.mixcloud_scope"
)

install(
  TARGETS scope
  LIBRARY DESTINATION ${SCOPE_INSTALL_DIR}
)

