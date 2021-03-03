target_include_directories(photon PRIVATE ${CMAKE_SOURCE_DIR}/include/stb)

file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} SRCS *.c *.cpp)
foreach(child ${children})
	target_sources(photon PRIVATE ${child})
endforeach()