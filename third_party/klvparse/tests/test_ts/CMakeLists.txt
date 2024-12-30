get_filename_component(TARGET ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB SOURCES "*.cpp")
set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable("${TARGET}" ${SOURCES})
target_include_directories(
  "${TARGET}" PUBLIC ${INCLUDE_DIRS} 
)
target_link_libraries("${TARGET}" PRIVATE klvparse::klvparse gstreamer::gstreamer)
