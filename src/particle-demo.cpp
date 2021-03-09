#include <imgui/app.hpp>

#include "resources.hpp"
#include "particles.hpp"

class Game : public imgui::Application {
public:
	Game() : imgui::Application(1080, 720, "Particles!") {
		glClearColor(0,0,0,0);
		maximizeWindow();
		glClear(GL_COLOR_BUFFER_BIT);

		cameraInfoUBO.bindBase(0);
		cameraInfoUBO.setData({mat4(), mat4()});

		modelInfoUBO.bindBase(1);
		modelInfoUBO.setData({mat4(), mat4()});
	}

	void update() override {
		particles.update(glfwGetTime(), dt, nullptr);
		if(particles.size() < 1000) {
			particles.spawn({vec2(float(rand()) / float(RAND_MAX) * 2 - 1, 2), 0, 0, 0, vec2(0, -1), 0, 0, vec2(0.002, 0.05)});
		}
		for(Particle &p : particles) {
			if(p.pos.y < -1.1) {
				p.pos = vec2(float(rand()) / float(RAND_MAX) * 2 - 1, 2);
				p.speed = 0;
			}
		}
	}

	void render() override {
		glClear(GL_COLOR_BUFFER_BIT);
		particles.render();
	}

	void renderUI() override {}

	struct ModelInfo {
		mat4 transform, uvtransform;
	};

	struct CameraInfo {
		mat4 proj, view;
	};

private:
	ResourceCache<TiledTexture> textures;
	ParticleSystem particles;

	opengl::UniformBuffer<ModelInfo> modelInfoUBO;
	opengl::UniformBuffer<CameraInfo> cameraInfoUBO;
};

int main(int argc, const char *argv[]) {
	std::vector<std::string> args(argv, argv + argc);

	glfw::init();
	atexit(glfw::cleanup);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
	glfwWindowHint(GLFW_DECORATED, 0);

	Game game;
	return game.run();
}