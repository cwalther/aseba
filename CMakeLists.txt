# kate: replace-tabs off; tab-width 4; indent-width 4; tab-indents true; indent-mode normal
# vim: ts=4:sw=4:noexpandtab
cmake_minimum_required(VERSION 2.6)

project(aseba)
enable_testing()

include(ParseVersion.cmake)

add_definitions(-Wall)
add_definitions(-DASEBA_ASSERT)

# Remove -Wl,--no-undefined which CMake 3.0.2 (on OpenSUSE 13.2) adds to the
# linker options when building shared libs. That breaks building libs that use
# callbacks that will be provided by other libs when the executable is linked.
if (DEFINED CMAKE_SHARED_LINKER_FLAGS)
	STRING(REPLACE "-Wl,--no-undefined" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
endif (DEFINED CMAKE_SHARED_LINKER_FLAGS)

# Dashel
find_package(dashel REQUIRED)
include_directories(${dashel_INCLUDE_DIRS})

# Enki
find_path(ENKI_INCLUDE_DIR enki/PhysicalEngine.h)
find_library(ENKI_LIBRARY enki)
find_library(ENKI_VIEWER_LIBRARY enkiviewer)
if (ENKI_INCLUDE_DIR AND ENKI_LIBRARY AND ENKI_VIEWER_LIBRARY)
	set(ENKI_FOUND 1)
	message(STATUS "Enki found")
else (ENKI_INCLUDE_DIR AND ENKI_LIBRARY AND ENKI_VIEWER_LIBRARY)
	message(STATUS "Enki *not* found, simulation targets disabled")
endif (ENKI_INCLUDE_DIR AND ENKI_LIBRARY AND ENKI_VIEWER_LIBRARY)


# Aseba itself
set(LIB_INSTALL_DIR lib CACHE FILEPATH "Where to install libraries")
set(LIB_VERSION_MAJOR 1) # Must be bumped for incompatible ABI changes
set(LIB_VERSION_MINOR 4)
set(LIB_VERSION_PATCH 0)
set(LIB_VERSION_STRING ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

add_subdirectory(common)

add_subdirectory(transport)

set(ASEBA_CORE_LIBRARIES asebadashelplugins asebacommon ${dashel_LIBRARIES})

add_subdirectory(switches)

add_subdirectory(vm)

add_subdirectory(compiler)

add_subdirectory(clients)

add_subdirectory(targets)

add_subdirectory(examples)

add_subdirectory(tests)
