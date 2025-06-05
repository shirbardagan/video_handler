set(TARGET klvparse)
file(GLOB SOURCES 
    "klvparse/byte/*.cpp"
    "klvparse/klv/*.cpp"
    "klvparse/stanag/*.cpp"
    "klvparse/stanag/common/*.cpp"
    "klvparse/stanag/security/*.cpp"
    "klvparse/stanag/vmti/*.cpp"
    "klvparse/stanag/vmti/vtarget/*.cpp"
    "klvparse/stanag/vmti/vtarget/vobject/*.cpp"
    "klvparse/stanag/vmti/vtarget/vtracker/*.cpp"
)

file(GLOB HEADERS 
    "klvparse/byte/*.h"
    "klvparse/klv/*.h"
    "klvparse/stanag/*.h"
    "klvparse/stanag/common/*.h"
    "klvparse/stanag/security/*.h"
    "klvparse/stanag/vmti/*.h"
    "klvparse/stanag/vmti/vtarget/*.h"
    "klvparse/stanag/vmti/vtarget/vobject/*.h"
    "klvparse/stanag/vmti/vtarget/vtracker/*.h"
)

set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

add_library("${TARGET}" STATIC ${SOURCES})
target_include_directories("${TARGET}" PUBLIC ${INCLUDE_DIRS})
target_link_libraries("${TARGET}" PUBLIC nlohmann_json::nlohmann_json)
if(WIN32)
  target_link_libraries("${TARGET}" PUBLIC wsock32 ws2_32)
else()
  target_compile_options("${TARGET}" PUBLIC -fPIC)
endif()
# target_compile_options("${TARGET}" PUBLIC -fPIC)
set_target_properties("${TARGET}" PROPERTIES CXX_STANDARD 17)
# set_target_properties("${TARGET}" PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS "${TARGET}" 
LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
)
install(DIRECTORY ""
DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
FILES_MATCHING
PATTERN "*.h"
)
configure_file(klvparse.pc.in klvparse.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/klvparse.pc" 
    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"
)

add_library(klvparse::klvparse ALIAS klvparse)