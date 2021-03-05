add_subdirectory(utils)
add_subdirectory(spdlog)
add_subdirectory(stb)
add_subdirectory(math)

if(AUDIO OR PHOTON_FULL)
	add_subdirectory(soloud)
endif()

if(SQLITE OR PHOTON_FULL)
	add_subdirectory(sqlite)
endif()

if(OPENGL OR PHOTON_FULL)
	add_subdirectory(glad)
	add_subdirectory(opengl)
endif()

if(VULKAN OR PHOTON_FULL)
	add_subdirectory(vulkan)
endif()

if(IMGUI OR PHOTON_FULL)
	add_subdirectory(imgui)
endif()

if(VULKAN OR OPENGL OR GLFW OR PHOTON_FULL)
	if(VULKAN)
		add_compile_definitions(GLFW_ENABLE_VULKAN)
	endif()
	add_subdirectory(glfw)
endif()

if(NETWORK OR PHOTON_FULL)
	add_subdirectory(network)
	add_subdirectory(tomcrypt)
	add_subdirectory(tommath)
endif()
