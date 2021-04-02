add_custom_target(assets)

add_custom_command(TARGET assets COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/assets)

add_custom_command(
	TARGET assets
	COMMAND python3 -m pip install Pillow
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/player
)

add_custom_command(
	TARGET assets
	COMMAND python3 "spritesheet.py"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/player
)

file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} SRCS *.glsl *.png *.ttf)
foreach(child ${children})
	add_custom_command(
		TARGET assets
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${child} ${CMAKE_BINARY_DIR}/assets/${child}
	)
endforeach()

add_custom_command(
	TARGET assets
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/player/player.png ${CMAKE_BINARY_DIR}/assets/player.png
)


#add_custom_command(TARGET assets COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/assets)
#add_custom_command(TARGET assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/assets)
