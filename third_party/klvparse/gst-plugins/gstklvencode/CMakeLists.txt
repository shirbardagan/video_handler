set(TARGET gstklvencode)

file(GLOB "SOURCES" "*.c" "*.cpp")
set("INCLUDE_DIRS" "${CMAKE_CURRENT_SOURCE_DIR}")

add_library("${TARGET}" SHARED ${SOURCES})
target_include_directories("${TARGET}" PUBLIC ${INCLUDE_DIRS})
target_link_libraries("${TARGET}" PRIVATE 
    gstreamer::gstreamer
    klvparse::klvparse
)

target_compile_definitions("${TARGET}" PRIVATE ${PLUGIN_COMPILE_DEFENITIONS})
install(TARGETS "${TARGET}" LIBRARY DESTINATION "${PLUGIN_INSTALL_DIR}")
install(TARGETS "${TARGET}" RUNTIME DESTINATION "${PLUGIN_INSTALL_DIR}")
