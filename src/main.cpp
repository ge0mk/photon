#include <math/math.hpp>
#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <glfw/window.hpp>

#include <opengl/buffer.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/mesh.hpp>
#include <opengl/program.hpp>
#include <opengl/texture.hpp>
#include <opengl/vao.hpp>

#include <thread>

#include "text.hpp"

class Game : public glfw::Window {
public:
	Game(math::ivec2 res, const std::string &title) : glfw::Window(res, title) {
		makeContextCurrent();
		if(!gladLoadGL()) {
			spdlog::error("couldn't load gl!");
		}
		glClearColor(0.0, 0.5, 0.2, 1.0);

		textRenderer = std::shared_ptr<TextRenderer>(new TextRenderer(freetype::Font("res/jetbrains-mono.ttf")));
		textRenderer->createObject("Hello World!", math::mat4().translate(math::vec3(-1,0,0)).scale(0.0045), math::vec4(1));
	}

	void update() {
		if(getKey(GLFW_KEY_ESCAPE)) {
			setWindowShouldClose();
		}
	}

	void render() {
		if(resized) {
			glViewport(0, 0, width, height);
			resized = false;
		}
		glClear(GL_COLOR_BUFFER_BIT);

		// render stuff
		textRenderer->render(mat4());

		swapBuffers();
	}

	void physics() {}
	void particles() {}

	void renderui() {
		textRenderer->update();
	}

	int exec() override {
		guiThread = std::thread([this](){
			while(!windowShouldClose()) {
				renderui();
			}
		});
		physicsThread = std::thread([this](){
			while(!windowShouldClose()) {
				physics();
			}
		});
		particleThread = std::thread([this](){
			while(!windowShouldClose()) {
				particles();
			}
		});

		while(!windowShouldClose()) {
			pollEvents();
			update();
			render();
		}

		guiThread.join();
		physicsThread.join();
		particleThread.join();
		return 0;
	}

	void onFramebufferResized(math::ivec2 size) override {
		resized = true;
		width = size.x, height = size.y;
	}

private:
	std::atomic<bool> resized;
	std::atomic<int> width;
	std::atomic<int> height;

	std::thread renderThread;
	std::thread physicsThread;
	std::thread particleThread;
	std::thread guiThread;

	std::shared_ptr<TextRenderer> textRenderer;
};

int main(int argc, const char *argv[]) {
	glfw::init();
	freetype::init();

	atexit([](){
		glfw::cleanup();
		freetype::cleanup();
	});

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	Game game(math::ivec2(1080, 720), "Multi threading!");
	return game.run();
}