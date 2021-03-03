target_include_directories(photon PRIVATE ${CMAKE_SOURCE_DIR}/include/imgui)

file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(child ${children})
	if(${child} MATCHES ".+[(\.cpp)(\.c)]")
		target_sources(photon PRIVATE ${child})
	endif()
endforeach()