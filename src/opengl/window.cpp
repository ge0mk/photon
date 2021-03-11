#include <math/vector.hpp>
#include <opengl/window.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace opengl {
	Window::Window(int width, int height, std::string title)
	 : glfw::Window(width, height, title) {
		makeContextCurrent();
		if(!gladLoadGL()) {
			spdlog::error("couldn't load gl!");
		}
		glViewport(0, 0, width, height);
	}
	Window::~Window() {}

	int Window::exec() {
		while(!windowShouldClose()) {
			pollEvents();
			update();
			render();
			swapBuffers();
		}
		return 0;
	}

	void Window::update() {}

	void Window::render() {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Window::onFramebufferResized(math::ivec2 size) {
		glViewport(0, 0, size.x, size.y);
	}
}