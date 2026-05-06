cmake_minimum_required(VERSION 3.14)

# Read version from the `version` file so we have a single source of truth.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version" INQUIRE_VERSION_RAW LIMIT_COUNT 1)
string(STRIP "${INQUIRE_VERSION_RAW}" INQUIRE_VERSION)

project(inquire
    VERSION   ${INQUIRE_VERSION}
    LANGUAGES CXX
    DESCRIPTION "Production-quality interactive CLI prompts for C++11"
    HOMEPAGE_URL "https://github.com/IamK77/inquire"
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ---- options ----
option(INQUIRE_BUILD_TESTS    "Build inquire's unit tests"        ${PROJECT_IS_TOP_LEVEL})
option(INQUIRE_BUILD_EXAMPLES "Build inquire's interactive demo"  ${PROJECT_IS_TOP_LEVEL})
option(INQUIRE_BUILD_STATIC   "Build inquire as a static library" ON)
option(INQUIRE_INSTALL        "Generate install rules"            ${PROJECT_IS_TOP_LEVEL})
option(INQUIRE_ENABLE_ASAN    "Build with AddressSanitizer"       OFF)
option(INQUIRE_ENABLE_UBSAN   "Build with UndefinedBehaviorSanitizer" OFF)
option(INQUIRE_ENABLE_COVERAGE "Build with gcov coverage flags"   OFF)
option(INQUIRE_WARNINGS_AS_ERRORS "Treat warnings as errors"      OFF)

# ---- common settings ----
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ---- header-only single-header target ----
add_library(inquire_header INTERFACE)
add_library(Inquire::header ALIAS inquire_header)
target_include_directories(inquire_header INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(inquire_header INTERFACE cxx_std_11)

# ---- modular static library (faster compile for large projects) ----
if(INQUIRE_BUILD_STATIC)
    set(INQUIRE_LIB_SOURCES
        src/Inquire/inquire.cpp
        src/Inquire/utils/colorful.cpp
        src/Inquire/utils/console.cpp
        src/Inquire/utils/encode.cpp
        src/Inquire/utils/func.cpp
        src/Inquire/utils/renderer.cpp
        src/Inquire/utils/tty_guard.cpp
    )
    add_library(inquire STATIC ${INQUIRE_LIB_SOURCES})
    add_library(Inquire::inquire ALIAS inquire)
    target_include_directories(inquire
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Inquire>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/Inquire/utils
    )
    target_compile_features(inquire PUBLIC cxx_std_11)
    set_target_properties(inquire PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        POSITION_INDEPENDENT_CODE ON
    )

    if(MSVC)
        target_compile_options(inquire PRIVATE /W4 /utf-8)
        if(INQUIRE_WARNINGS_AS_ERRORS)
            target_compile_options(inquire PRIVATE /WX)
        endif()
    else()
        target_compile_options(inquire PRIVATE
            -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor
        )
        if(INQUIRE_WARNINGS_AS_ERRORS)
            target_compile_options(inquire PRIVATE -Werror)
        endif()
    endif()
endif()

# ---- sanitizer / coverage flags (apply to all targets) ----
if(INQUIRE_ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()
if(INQUIRE_ENABLE_UBSAN)
    add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=undefined)
endif()
if(INQUIRE_ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# ---- examples ----
if(INQUIRE_BUILD_EXAMPLES)
    add_executable(inquire_example example/example.cpp)
    target_link_libraries(inquire_example PRIVATE Inquire::header)
endif()

# ---- tests ----
if(INQUIRE_BUILD_TESTS)
    enable_testing()
    add_executable(inquire_test tests/test.cpp)
    target_link_libraries(inquire_test PRIVATE Inquire::header)
    add_test(NAME inquire_unit_tests COMMAND inquire_test)
    set_tests_properties(inquire_unit_tests PROPERTIES
        FAIL_REGULAR_EXPRESSION "FAIL:"
        TIMEOUT 30
    )
endif()

# ---- install rules ----
if(INQUIRE_INSTALL)
    install(FILES include/inquire.hpp
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(TARGETS inquire_header
            EXPORT  inquireTargets)

    if(INQUIRE_BUILD_STATIC)
        install(TARGETS inquire
                EXPORT  inquireTargets
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()

    install(EXPORT inquireTargets
            FILE        inquireTargets.cmake
            NAMESPACE   Inquire::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/inquire)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/inquireConfigVersion.cmake"
        VERSION       ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    configure_package_config_file(
        cmake/inquireConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/inquireConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/inquire)

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/inquireConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/inquireConfigVersion.cmake"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/inquire)

    configure_file(cmake/inquire.pc.in
                   "${CMAKE_CURRENT_BINARY_DIR}/inquire.pc" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/inquire.pc"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

# ---- summary ----
message(STATUS "inquire ${PROJECT_VERSION} configured:")
message(STATUS "  build static lib  : ${INQUIRE_BUILD_STATIC}")
message(STATUS "  build tests       : ${INQUIRE_BUILD_TESTS}")
message(STATUS "  build examples    : ${INQUIRE_BUILD_EXAMPLES}")
message(STATUS "  install           : ${INQUIRE_INSTALL}")
message(STATUS "  asan / ubsan      : ${INQUIRE_ENABLE_ASAN} / ${INQUIRE_ENABLE_UBSAN}")
message(STATUS "  coverage          : ${INQUIRE_ENABLE_COVERAGE}")
