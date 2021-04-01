cmake_minimum_required(VERSION 3.16.3)
project(photon)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#build type specifig args
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# used library components
set(OPENGL 1)
set(IMGUI 1)

# build glfw
if(GLFW OR OPENGL OR VULKAN OR PHOTON_FULL)
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

	add_subdirectory(glfw)
endif()

# freetype
add_subdirectory(freetype)

# compile definitions
add_compile_definitions(SPDLOG_COMPILED_LIB IMGUI_IMPL_OPENGL_LOADER_GLAD LTC_SOURCE LTC_NO_ROLC)
if(WIN32)
	add_compile_definitions(_GLFW_WIN32 WITH_WINMM WINDOWS)
elseif(UNIX)
	add_compile_definitions(_GLFW_X11 WITH_ALSA UNIX)
endif()

# photon src
add_library(photon)
if(WIN32)
	target_include_directories(photon PUBLIC ENV{VULKAN_SDK}/Include)
endif()

add_subdirectory(src)
target_include_directories(photon PUBLIC include)

# linking
target_link_libraries(photon PUBLIC freetype)

if(GLFW OR OPENGL OR VULKAN OR PHOTON_FULL)
	target_link_libraries(photon PUBLIC glfw)
endif()

if(WIN32)
	if(AUDIO OR PHOTON_FULL)
		target_link_libraries(photon PUBLIC winmm)
	endif()
elseif(UNIX)
	target_link_libraries(photon PUBLIC dl m pthread)
	if(AUDIO OR PHOTON_FULL)
		target_link_libraries(photon PUBLIC asound)
	endif()
endif()

# resources
add_subdirectory(assets assets_tmp)

# main program
add_executable(platformer
	src/platformer.cpp
	src/camera.cpp
	src/chunk.cpp
	src/entity.cpp
	src/particles.cpp
	src/player.cpp
	src/resources.cpp
	src/rigidbody.cpp
	src/text.cpp
	src/tile.cpp
	src/world.cpp
)
add_dependencies(platformer assets)
target_link_libraries(platformer PUBLIC photon)
