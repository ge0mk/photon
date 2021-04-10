#include "platformer.hpp"
#include <math/noise.hpp>

#include <chrono>

using namespace std::chrono_literals;

TileCursor::TileCursor(std::shared_ptr<TiledTexture> sprites) : Entity(sprites) {}

void TileCursor::update([[maybe_unused]] float time, [[maybe_unused]] float dt, [[maybe_unused]] WorldContainer &world) {
	transform = mat4().translate(pos + 0.5 * Tile::resolution).scale(Tile::resolution + 1.0f);
}

Game::Game() : opengl::Window({1080, 720}, "Game"), gui(freetype::Font("assets/jetbrains-mono.ttf")) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	auto playerSprite = textures.load("assets/player.png", ivec2(16, 13));
	player = std::shared_ptr<Player>(new Player(&cam, playerSprite));
	world.setMainEntity(player);

	auto palette = textures.load("assets/palette.png", 1);

	auto tileset = textures.load("assets/tileset.png", ivec2(32));
	world.initRenderer(std::ref(cam), player, tileset);
	world.initGenerator(tileset->scale());
	world.initParticleSystem(palette);
	world.initTextRenderer(freetype::Font("assets/jetbrains-mono.ttf"));

	for(int i = 5; i < 128; i++) {
		world[ivec2(i + 4, i)] = Tile::stone;
		world[ivec2(i + 4, i - 1)] = Tile::stone;
	}
	for(int i = 0; i < 16; i++) {
		world[ivec2(-i - 5, 8)] = Tile::stone;
	}
	world[ivec2(-5, 0)] = Tile::stone;
	world[ivec2(-5, 1)] = Tile::stone;
	world[ivec2(-5, 2)] = Tile::stone;
	world[ivec2(-5, 3)] = Tile::stone;

	auto cursorSprite = textures.load("assets/crosshair.png", 1);
	cursor = world.createEntity<TileCursor>(cursorSprite);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	onFramebufferResized(getFramebufferSize());
	glfwSwapInterval(1);
}

#if defined(MULTITHREADING)
	int Game::exec() {
		std::thread updateThread([this](){
			while(!windowShouldClose()) {
				update();
			}
		});
		while(!windowShouldClose()) {
			pollEvents();
			updateInputs();
			render();
			swapBuffers();
		}
		updateThread.join();
		return 0;
	}
#else
	int Game::exec() {
		while(!windowShouldClose()) {
			pollEvents();
			updateInputs();
			update();
			render();
			swapBuffers();
		}
		return 0;
	}
#endif

void Game::update() {
	double current_time = glfwGetTime();
	dt = time > 0.0 ? (current_time - time) : (1.0f / 60.0f);
	time = current_time;

	world.update(glfwGetTime(), dt);
	cam.update();
}

void Game::updateInputs() {
	player->setInput(Player::move, getKey(GLFW_KEY_D) - getKey(GLFW_KEY_A));
	if(getKey(GLFW_KEY_SPACE))
		player->setInput(Player::jump, 1.0f);
	if(getKey(GLFW_KEY_LEFT_ALT) || getKey(GLFW_KEY_RIGHT_ALT))
		player->setInput(Player::dash, 1.0f);
	if(getKey(GLFW_KEY_LEFT_SHIFT))
		player->setInput(Player::walk, 1.0f);

	if(getKey(GLFW_KEY_S)) {
		player->gravity.y = abs(player->gravity.y);
	}
	else {
		player->gravity.y = -abs(player->gravity.y);
	}

	lvec2 cursorTileIndex = world.getTileIndex(screenToWorldSpace(getCursorPos()));
	cursor->pos = cursorTileIndex * Tile::resolution;
	if(getMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)) {
		//world.createBloodParticles(screenToWorldSpace(getCursorPos()) - 0.5);
	}

	updateUI();

	if(!gui.usesMouse()) {
		if (getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			world[cursorTileIndex] = Tile::stone;
		}
		else if (getMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
			world[cursorTileIndex] = Tile::null;
		}
	}
}

void Game::updateUI() {
	static float t = 0;
	static int frames = 0;
	static float fps = 0;
	gui.beginFrame(glfwGetTime(), dt);
		t += dt;
		frames += 1;
		if(t >= 0.5f) {
			fps = float(frames) / t;
			t = 0, frames = 0;
		}

		gui.text("FPS: {}", vec2(8.0f, 32.0f), vec4(1.0f), vec2(0.5f), 0.0f, round(fps * 10.0f) / 10.0f);
		gui.text("Δdt: {}ms", vec2(8.0f, 64.0f), vec4(1.0f), vec2(0.5f), 0.0f, 1000.0f / fps);
		gui.rect(vec2(8.0f, 70.0f), vec2(200.0f, 68.0f), vec4(1.0f));
		gui.text("chunk: {} {}", vec2(8.0f, 96.0f), vec4(1.0f), vec2(0.5f), 0.0f, world.offset().x, world.offset().y);
		gui.text("pos: {} {}", vec2(8.0f, 128.0f), vec4(1.0f), vec2(0.5f), 0.0f, round(player->pos.x), round(player->pos.y));
		gui.text("speed: {} {}", vec2(8.0f, 160.0f), vec4(1.0f), vec2(0.5f), 0.0f, round(player->speed.x), round(player->speed.y));
		gui.rect(vec2(8.0f, 166.0f), vec2(200.0f, 164.0f), vec4(1.0f));

		if(gui.button("Respawn!", vec2(getFramebufferSize().x - 310.0f, 0.0f), vec4(0.2f, 0.2f, 0.2f, 1.0f))) {
			player->pos = vec2(0);
			player->speed = vec2(0);
			world.shift(world.offset());
		}
	gui.endFrame();
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	world.render();
	gui.render();
}

vec2 Game::screenToWorldSpace(vec2 screenpos) {
	mat4 view = mat4().translate(vec3(0, 0, cam.pos.z)).inverse();
	mat4 proj = cam.proj();

	vec2 normalizedpos = screenpos / getFramebufferSize();
	vec2 glpos = (normalizedpos * 2 - 1) * vec2(1, -1);
	vec4 tmp = (proj * view * vec4(0, 0, 0, 1)) + glpos;
	vec2 worldpos = proj.inverse() * view.inverse() * tmp;

	return worldpos * tmp.w + cam.pos.xy;
}

vec2 Game::worldToScreenSpace(vec2 worldpos) {
	mat4 view = mat4().translate(vec3(0, 0, cam.pos.z)).inverse();
	mat4 proj = cam.proj();

	vec2 tmp = worldpos - cam.pos.xy;
	vec4 glpos = proj * view * vec4(tmp, 0, 1);
	vec2 normalizedpos = (glpos + 1) / vec2(2, -2);
	vec2 screenpos = normalizedpos * getFramebufferSize();

	return screenpos;
}

void Game::onKeyTyped(int key) {}

void Game::onKeyChanged(int key, int scancode, int modifier, int action) {}

void Game::onKeyPressed(int key, int scancode, int modifier, bool repeat) {}

void Game::onKeyReleased(int key, int scancode, int modifier) {}

void Game::onMouseMoved(vec2 pos, vec2 dir) {
	gui.onMouseMoved(pos, dir);
}

void Game::onMousePressed(int button, int modifier) {
	gui.onMousePressed(button, modifier);
}

void Game::onMouseReleased(int button, int modifier) {
	gui.onMouseReleased(button, modifier);
}

void Game::onScrollEvent(vec2 dir) {}

void Game::onWindowContentScaleChanged(vec2 scale) {}

void Game::onWindowFocusChanged(bool focussed) {}

void Game::onFramebufferResized(ivec2 size) {
	cam.res = size;
	opengl::Window::onFramebufferResized(size);
	gui.onFramebufferResized(size);
}

int main(int argc, const char *argv[]) {
	stbi_flip_vertically_on_write(true);
	std::vector<std::string> args(argv, argv + argc);

	glfw::init();
	freetype::init();

	atexit([](){
		glfw::cleanup();
		freetype::cleanup();
	});

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	Game game;
	return game.run();
}
