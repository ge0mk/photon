#include <math/vector.hpp>
#include <opengl/window.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace opengl {
	Window::Window(math::ivec2 size, const std::string &title)
	 : glfw::Window(size, title) {
		makeContextCurrent();
		if(!gladLoadGL()) {
			spdlog::error("couldn't load gl!");
		}
		glViewport(0, 0, size.x, size.y);
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