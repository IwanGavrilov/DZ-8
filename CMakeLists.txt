cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

project(bayan VERSION ${PROJECT_VESRION})

configure_file(version.h.in version.h)

add_executable(bayan_cli bayan.cpp)
add_library(bayan lib.cpp)
add_library(loger loger.cpp)

set_target_properties(bayan_cli loger PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bayan PRIVATE "${CMAKE_BINARY_DIR}")

target_link_libraries(bayan_cli PRIVATE bayan)
target_link_libraries(bayan_cli PRIVATE loger)

if (MSVC)
    target_compile_options(bayan_cli PRIVATE
        /W4
    )
    target_compile_options(bayan PRIVATE
        /W4
    )
else ()
    target_compile_options(bayan_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    # target_compile_options(bulk PRIVATE
    #   -Wall -Wextra -pedantic -Werror
    #)
endif()

install(TARGETS bayan_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
