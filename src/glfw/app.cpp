#include <glfw/app.hpp>

namespace glfw {
	Application::Application(int width, int height, std::string title)
	 : Window(width,  height, title) {}
	Application::~Application() {}

	int Application::exec() {
		while(!windowShouldClose()) {
			pollEvents();
		}
		return 0;
	}
}