configure_file(
  "com.ubuntu.developer.boghison.mixcloud_mixcloudscope-settings.ini"
  "${CMAKE_BINARY_DIR}/src/com.ubuntu.developer.boghison.mixcloud_mixcloudscope-settings.ini"
)
# Install the scope ini file
install(
  FILES
    "com.ubuntu.developer.boghison.mixcloud_mixcloudscope.ini"
    "com.ubuntu.developer.boghison.mixcloud_mixcloudscope-settings.ini"
  DESTINATION ${SCOPE_INSTALL_DIR}
)

# Install the scope images
install(
  FILES
    "art.png"
    "logo.svg"
    "mixcloud.svg"
  DESTINATION
    "${SCOPE_INSTALL_DIR}"
)

