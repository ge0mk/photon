target_include_directories(photon PRIVATE ${CMAKE_SOURCE_DIR}/include/tomcrypt)

file(GLOB_RECURSE children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} SRCS *.c *.cpp)
foreach(child ${children})
	target_sources(photon PRIVATE ${child})
endforeach()