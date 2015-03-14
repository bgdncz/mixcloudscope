INSTALL(DIRECTORY images DESTINATION ${CMAKE_INSTALL_PREFIX}/data)
INSTALL(DIRECTORY oa DESTINATION ${CMAKE_INSTALL_PREFIX}/data)
INSTALL(
  FILES
    screenshots/screenshot.png
    settings/com.ubuntu.developer.boghison.mixcloud_scope-settings.ini
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}
)