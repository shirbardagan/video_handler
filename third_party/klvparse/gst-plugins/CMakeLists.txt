# set(VERSION 0.0.1)
set(PLUGINS_FOLDERS "gstklvparse" "gstklvencode")

set(PLUGIN_COMPILE_DEFENITIONS
    "PACKAGE=\"DSPIP\""
    "GST_PACKAGE_NAME=\"DSPIP\""
    "GST_PACKAGE_ORIGIN=\"DSPIP\""
    "GST_LICENSE=\"LGPL\""
    "PACKAGE_VERSION=\"${VERSION}\""
)
find_package(gstreamer)
pkg_get_variable(PLUGIN_INSTALL_DIR "gstreamer-1.0" "pluginsdir")
message("PLUGIN_INSTALL_DIR=${PLUGIN_INSTALL_DIR}")


foreach(FOLDER ${PLUGINS_FOLDERS})
  add_subdirectory("${FOLDER}")
endforeach()
