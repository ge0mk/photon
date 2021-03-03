cmake_minimum_required(VERSION 3.17)
project(photon)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# build glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)

if(WIN32)
	add_compile_definitions(_GLFW_WIN32 WITH_WINMM)
elseif(UNIX)
	add_compile_definitions(_GLFW_X11 WITH_ALSA)
endif()

add_compile_definitions(SPDLOG_COMPILED_LIB IMGUI_IMPL_OPENGL_LOADER_GLAD LTC_SOURCE LTC_NO_ROLC)

add_library(photon)

add_subdirectory(src)
target_compile_options(photon PUBLIC -g)
target_include_directories(photon PUBLIC include)

if(WIN32)
	target_link_libraries(photon PUBLIC win32 glfw winmm)
elseif(UNIX)
	target_link_libraries(photon PUBLIC dl m pthread glfw asound)
endif()

add_executable(app src/main.cpp src/meshes.cpp src/photon.cpp)
target_link_libraries(app PUBLIC photon)

add_executable(platformer src/platformer.cpp)
target_link_libraries(platformer PUBLIC photon)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})