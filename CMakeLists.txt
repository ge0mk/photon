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

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# used library components
set(OPENGL 1)
set(IMGUI 1)

if(GLFW OR OPENGL OR VULKAN OR PHOTON_FULL) # build glfw
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

	add_subdirectory(glfw)
endif()

add_subdirectory(freetype)

if(WIN32)
	add_compile_definitions(_GLFW_WIN32 WITH_WINMM WINDOWS)
elseif(UNIX)
	add_compile_definitions(_GLFW_X11 WITH_ALSA UNIX)
endif()

add_compile_definitions(SPDLOG_COMPILED_LIB IMGUI_IMPL_OPENGL_LOADER_GLAD LTC_SOURCE LTC_NO_ROLC)

add_library(photon)
if(WIN32)
	target_include_directories(photon PUBLIC ENV{VULKAN_SDK}/Include)
else()
	target_compile_options(photon PUBLIC -g)
endif()

add_subdirectory(src)
target_include_directories(photon PUBLIC include)

if(WIN32)
	target_link_libraries(photon PUBLIC glfw freetype winmm)
elseif(UNIX)
	target_link_libraries(photon PUBLIC dl m pthread glfw freetype asound)
endif()

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
	src/world.cpp)
target_link_libraries(platformer PUBLIC photon)

add_executable(test src/main.cpp src/text.cpp)
target_link_libraries(test PUBLIC photon)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})