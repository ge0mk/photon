#include "vector.hpp"
#include <opengl/app.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace opengl {
	Application::Application(int width, int height, std::string title)
	 : glfw::Application(width, height, title) {
	 	makeContextCurrent();
		if(!gladLoadGL()) {
			spdlog::error("couldn't load gl!");
		}
		glViewport(0, 0, width, height);
	}
	Application::~Application() {}

	int Application::exec() {
		while(!windowShouldClose()) {
			pollEvents();
			update();
			render();
			swapBuffers();
		}
		return 0;
	}

	void Application::update() {}

	void Application::render() {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Application::onFramebufferResized(math::ivec2 size) {
		glViewport(0, 0, size.x, size.y);
	}
}