#include <photon.hpp>

int main(int argc, const char *argv[]) {
	glfw::init();
	atexit(glfw::cleanup);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	Engine game;
	return game.run();
}