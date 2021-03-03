#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <math/matrix.hpp>
#include <math/vector.hpp>
#include <math/quaternion.hpp>
#include <glad/glad.h>
#include <GLFW/window.hpp>
#include <photonvk/photonvk.hpp>
#include <spdlog/spdlog.h>

#include "glhelper.hpp"
#include "gui.hpp"

int main(int argc, char *argv[]) {
	std::vector<std::string> args(argv, argv + argc);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	Window win(512, 512, args[0]);
	win.setEventCallback([](Event *event) {
		dispatch<ResizeEvent>(event, [](ResizeEvent *event){
			glViewport(0, 0, event->width, event->height);
		});
		state.handleInputEvent(event);
	});

	win.makeContextCurrent();
	gladLoadGL();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	math::vec4 clearColor = {.r = 0.05f, .g = 0.1f, .b = 0.05f, .a = 1.0f};
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

	unsigned prog = loadProgram("gui");

	unsigned VBO, EBO, VAO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(math::vec3)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(math::vec3) + sizeof(math::vec4)));
	glEnableVertexAttribArray(2);

	math::mat4 model, view, proj;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while(!win.shouldClose()) {
		state.defaultStyle.rounding = sin(glfwGetTime()) / 2.0 + 0.5;
		auto [vertices, indices] = frame([&]() {
			widget([](){
				Style n = state.defaultStyle;
				n.palette.background = math::vec4(0.2, 0.2, 0.2, 1.0);
				n.border = math::vec4(8);
				n.rounding = sin(glfwGetTime()) / 2.0 + 0.5;
				state.pushStyle(n);
				widget([](){});
				state.popStyle();
			});
		});
		//break;
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_DYNAMIC_DRAW);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(prog);

		view = math::mat4().scale(math::vec3(1,-1,1)).translate(math::vec3(-1, -1, 0)).scale(math::vec3(math::vec2(2) / win.size(), -0.1));
		glUniformMatrix4fv(0, 1, GL_FALSE, model.data());
		glUniformMatrix4fv(1, 1, GL_FALSE, view.data());
		glUniformMatrix4fv(2, 1, GL_FALSE, proj.data());

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		win.swapBuffers();
		win.pollEvents();
	}
	return 0;
}