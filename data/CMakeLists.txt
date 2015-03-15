INSTALL(DIRECTORY images DESTINATION ${CMAKE_INSTALL_PREFIX}/data)
INSTALL(DIRECTORY oa DESTINATION ${CMAKE_INSTALL_PREFIX}/data)
INSTALL(
  FILES
    settings/com.ubuntu.developer.boghison.mixcloud_scope-settings.ini
  DESTINATION
    ${SCOPE_INSTALL_DIR}
)
