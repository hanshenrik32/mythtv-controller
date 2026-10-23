cmake_minimum_required(VERSION 3.16)

project(mythtv-controller
    VERSION 0.50
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Brug Debug som standard ved Makefile/Ninja-builds.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(X11 REQUIRED)
find_package(fmt REQUIRED)

pkg_check_modules(SDL REQUIRED IMPORTED_TARGET sdl)
pkg_check_modules(SDL_IMAGE REQUIRED IMPORTED_TARGET SDL_image)
pkg_check_modules(XML2 REQUIRED IMPORTED_TARGET libxml-2.0)
pkg_check_modules(CURL REQUIRED IMPORTED_TARGET libcurl)
pkg_check_modules(JSONCPP REQUIRED IMPORTED_TARGET jsoncpp)
pkg_check_modules(MEDIAINFO REQUIRED IMPORTED_TARGET libmediainfo)
pkg_check_modules(TORRENT REQUIRED IMPORTED_TARGET libtorrent-rasterbar)
pkg_check_modules(VLC REQUIRED IMPORTED_TARGET libvlc)
pkg_check_modules(ICAL REQUIRED IMPORTED_TARGET libical)

# Accepter både MySQL og MariaDB.
pkg_search_module(MYSQL REQUIRED IMPORTED_TARGET
    mysqlclient
    libmariadb
    mariadb
)

# Biblioteker uden pkg-config her.
find_library(DEVIL_LIBRARY NAMES IL)
find_library(GLC_LIBRARY NAMES GLC)
find_library(LIRC_LIBRARY NAMES lirc_client)

foreach(lib DEVIL_LIBRARY GLC_LIBRARY LIRC_LIBRARY)
    if(NOT ${lib})
        message(FATAL_ERROR "Kunne ikke finde biblioteket: ${lib}")
    endif()
endforeach()

# FMOD SDK. Kan ændres med -DFMOD_ROOT=/sti/til/sdk
set(FMOD_ROOT
    "/opt/mythtv-controller/fmodstudioapi20311linux"
    CACHE PATH "FMOD SDK directory"
)

# Vælg arkitektur efter målplatformen.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
    set(FMOD_ARCH arm64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    set(FMOD_ARCH arm)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(FMOD_ARCH x86_64)
else()
    set(FMOD_ARCH x86)
endif()

find_path(FMOD_INCLUDE_DIR
    NAMES fmod.hpp
    HINTS
        "${FMOD_ROOT}/api/core/inc"
        "/usr/local/include/fmodex"
)

find_library(FMOD_LIBRARY
    NAMES fmod
    HINTS "${FMOD_ROOT}/api/core/lib/${FMOD_ARCH}"
)

if(NOT FMOD_INCLUDE_DIR OR NOT FMOD_LIBRARY)
    message(FATAL_ERROR
        "FMOD blev ikke fundet. Angiv -DFMOD_ROOT=/sti/til/sdk"
    )
endif()

set(SOURCES
    main.cpp
    renderer.cpp
    shader.cpp
    utility.cpp
    readjpg.cpp
    myctrl_readwebfile.cpp
    myth_ttffont.cpp
    myctrl_stream.cpp
    myctrl_radio.cpp
    myth_setupsql.cpp
    myctrl_tidal2.cpp
    myctrl_mplaylist.cpp
    myctrl_music.cpp
    myctrl_movie.cpp
    myctrl_torrent.cpp
    myctrl_spotify.cpp
    myctrl_recorded.cpp
    myctrl_tvprg.cpp
    myth_vlcplayer.cpp
    myth_saver.cpp
    checknet.cpp
    myth_setup.cpp
    mongoose-master/mongoose.c
    json-parser/json.c
)

# Den gamle Makefile kørte også .c-filer gennem g++.
# Bevar den adfærd for kompatibilitet.
set_source_files_properties(
    mongoose-master/mongoose.c
    json-parser/json.c
    PROPERTIES LANGUAGE CXX
)

add_executable(mythtv-controller ${SOURCES})

target_include_directories(mythtv-controller PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${FMOD_INCLUDE_DIR}"
    /usr/include/lirc
)

target_link_libraries(mythtv-controller PRIVATE
    Threads::Threads
    OpenGL::GL
    OpenGL::GLU
    GLUT::GLUT
    GLEW::GLEW
    Freetype::Freetype
    X11::X11
    fmt::fmt

    PkgConfig::SDL
    PkgConfig::SDL_IMAGE
    PkgConfig::XML2
    PkgConfig::MYSQL
    PkgConfig::CURL
    PkgConfig::JSONCPP
    PkgConfig::MEDIAINFO
    PkgConfig::TORRENT
    PkgConfig::VLC
    PkgConfig::ICAL

    "${DEVIL_LIBRARY}"
    "${GLC_LIBRARY}"
    "${LIRC_LIBRARY}"
    "${FMOD_LIBRARY}"
    m
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(mythtv-controller PRIVATE
        -Wno-format-overflow
        -Wformat-truncation=2
        -Wregister
        "$<$<CONFIG:Debug>:-O0>"
        "$<$<CONFIG:Debug>:-ggdb>"
    )
endif()

# Slå til, hvis programmet kun skal køre på denne CPU.
option(ENABLE_NATIVE_CPU "Enable -march=native" OFF)

if(ENABLE_NATIVE_CPU AND
   CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(mythtv-controller PRIVATE -march=native)
endif()

# Sørg for, at FMOD også kan findes efter installation.
get_filename_component(FMOD_LIBRARY_DIR "${FMOD_LIBRARY}" DIRECTORY)

set_target_properties(mythtv-controller PROPERTIES
    BUILD_RPATH "${FMOD_LIBRARY_DIR}"
    INSTALL_RPATH "${FMOD_LIBRARY_DIR}"
)

# Buildnummer: øges efter en vellykket linkning.
set(BUILD_NUMBER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build-number.txt")

if(NOT EXISTS "${BUILD_NUMBER_FILE}")
    file(WRITE "${BUILD_NUMBER_FILE}" "0\n")
endif()

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/increment-build-number.cmake" [=[
file(READ "${BUILD_NUMBER_FILE}" number)
string(STRIP "${number}" number)

if(NOT number MATCHES "^[0-9]+$")
    message(FATAL_ERROR "Ugyldigt buildnummer: ${number}")
endif()

math(EXPR number "${number} + 1")
file(WRITE "${BUILD_NUMBER_FILE}" "${number}\n")
]=])

add_custom_command(TARGET mythtv-controller POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
        "-DBUILD_NUMBER_FILE=${BUILD_NUMBER_FILE}"
        -P "${CMAKE_CURRENT_BINARY_DIR}/increment-build-number.cmake"
    VERBATIM
)

# Kompatible targetnavne fra den gamle Makefile.
add_custom_target(compile DEPENDS mythtv-controller)
add_custom_target(Debug DEPENDS mythtv-controller)

# Installation: samme systemplaceringer som før.
set(APP_DATA_DIR "/opt/mythtv-controller" CACHE PATH
    "Application data directory"
)
set(APP_BIN_DIR "/usr/bin" CACHE PATH
    "Executable directory"
)
set(APP_CONFIG_DIR "/etc" CACHE PATH
    "Configuration directory"
)

install(TARGETS mythtv-controller
    RUNTIME DESTINATION "${APP_BIN_DIR}"
)

install(DIRECTORY images
    DESTINATION "${APP_DATA_DIR}"
)

# Install tema
foreach(n RANGE 1 10)
    install(DIRECTORY "tema${n}"
        DESTINATION "${APP_DATA_DIR}"
    )
    install(FILES "tema${n}.json"
        DESTINATION "${APP_DATA_DIR}"
    )
endforeach()

install(DIRECTORY DESTINATION "${APP_DATA_DIR}/images/radiostations")
install(DIRECTORY DESTINATION "${APP_DATA_DIR}/images/mythnetvision")
install(DIRECTORY DESTINATION "${APP_DATA_DIR}/convert/hires")

install(FILES mythtv-controller.png
    DESTINATION "${APP_DATA_DIR}"
)

install(FILES mythtv-controller.desktop
    DESTINATION /usr/share/applications
)

# Bevar en eksisterende konfiguration.
install(CODE "
    if(NOT EXISTS \"\$ENV{DESTDIR}${APP_CONFIG_DIR}/mythtv-controller.conf\")
        file(INSTALL
            DESTINATION \"${APP_CONFIG_DIR}\"
            TYPE FILE
            FILES \"${CMAKE_CURRENT_SOURCE_DIR}/mythtv-controller.conf\"
        )
    endif()
")

set(FMOD_SDK_NAME "fmodstudioapi20311linux")

set(FMOD_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/${FMOD_SDK_NAME}"
    CACHE PATH "FMOD SDK, som skal installeres"
)

set(FMOD_INSTALL_DIR "/opt/mythtv-controller/${FMOD_SDK_NAME}")

add_custom_target(installsound
    COMMAND "${CMAKE_COMMAND}" -E echo
        "Installerer FMOD fra ${FMOD_SOURCE_DIR}"

    COMMAND "${CMAKE_COMMAND}" -E make_directory
        "/opt/mythtv-controller"

    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${FMOD_SOURCE_DIR}"
        "${FMOD_INSTALL_DIR}"

    COMMAND /usr/bin/touch
        "/etc/mythtv-controller.conf"

    COMMAND /usr/bin/chmod 644
        "/etc/mythtv-controller.conf"

    COMMAND "${CMAKE_COMMAND}" -E echo
        "FMOD er installeret i ${FMOD_INSTALL_DIR}"

    VERBATIM
)

