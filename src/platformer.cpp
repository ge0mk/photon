#include "platformer.hpp"

TileCursor::TileCursor(std::shared_ptr<TiledTexture> sprites) : Entity(sprites) {}

void TileCursor::update(float time, float dt, World *world) {
	transform = mat4().translate(pos + 0.5 * Tile::resolution).scale(Tile::resolution + 1.0f);
}

Game::Game() : opengl::Window({1080, 720}, "Game"), world("res/platformer.glsl", &cam), gui(freetype::Font("res/jetbrains-mono.ttf")) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	auto tileset = textures.load("res/tileset.png", ivec2(32));
	world.setTexturePtr(tileset);

	world.setAutoGrow(true);
	world.createChunk(ivec2(0, -1))->fill(Tile::stone);
	world.createChunk(ivec2(-1, -1))->fill(Tile::stone);
	for(int x = -33; x < 32; x++) {
		world[ivec2(x, -1)] = Tile::grass;
		world[ivec2(x, -2)] = Tile::grass;
		world[ivec2(x, -3)] = Tile::dirt;
		world[ivec2(x, -4)] = Tile::dirt;
	}
	for(int i = 5; i < 128; i++) {
		world[ivec2(i + 4, i)] = Tile::stone;
		world[ivec2(i + 4, i - 1)] = Tile::stone;
	}
	for(int i = 0; i < 16; i++) {
		world[ivec2(-i - 5, 8)] = Tile::stone;
	}
	world[ivec2(-5,0)] = Tile::stone;
	world[ivec2(-5,1)] = Tile::stone;
	world[ivec2(-5,2)] = Tile::stone;
	world[ivec2(-5,3)] = Tile::stone;

	auto playerSprite = textures.load("res/player.png", ivec2(16, 13));
	player = world.createEntity<Player>(&cam, playerSprite);

	auto cursorSprite = textures.load("res/crosshair.png", 1);
	cursor = world.createEntity<TileCursor>(cursorSprite);

	auto palette = textures.load("res/palette.png", 1);
	world.setParticleTexturePtr(palette);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	onFramebufferResized(getFramebufferSize());
	glfwSwapInterval(1);
}

void Game::update() {
	double current_time = glfwGetTime();
	dt = time > 0.0 ? (current_time - time) : (1.0f / 60.0f);
	time = current_time;

	player->setInput(Player::move, getKey(GLFW_KEY_D) - getKey(GLFW_KEY_A));
	if(getKey(GLFW_KEY_SPACE))
		player->setInput(Player::jump, 1.0f);
	if(getKey(GLFW_KEY_S))
		player->setInput(Player::dash, 1.0f);
	if(getKey(GLFW_KEY_LEFT_SHIFT))
		player->setInput(Player::walk, 1.0f);

	cursor->pos = world.getTileIndex(screenToWorldSpace(getCursorPos())) * Tile::resolution;
	if(getMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)) {
		world.createBloodParticles(screenToWorldSpace(getCursorPos()) - 0.5);
	}

	if (getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
		world[cursor->pos / Tile::resolution] = Tile::stone;
	}
	else if (getMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
		world[cursor->pos / Tile::resolution] = Tile::null;
	}

	world.update(glfwGetTime(), dt);
	cam.update(glfwGetTime(), dt);

	auto to_string = [](float v, int precision = 3) -> std::string {
		std::stringstream stream;
		stream<<std::fixed<<std::setprecision(precision)<<v;
		return stream.str();
	};

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

		gui.rect(vec2(0, 0), vec2(420.0f, 60.0f), vec4(0.2f, 0.2f, 0.2f, 1.0f));
		gui.text("Hello World", vec2(0.0f, 50.0f), vec4(0.0f, 0.9f, 0.1f, 1.0f));
		gui.circle(vec2(600.0f, 100.0f), 70.0f, 0.0f, vec4(0.05, 0.5f, 0.1f, 1.0f));
		gui.circleSegment(vec2(600.0f, 100.0f), 100.0f, 80.0f, pi/2.0f, pi*2.0f, vec4(0.5, 0.05f, 0.1f, 1.0f), 96);
		gui.circleSegment(vec2(600.0f, 100.0f), 100.0f, 80.0f, 0.0f, pi/2.0f, vec4(0.1f, 0.2f, 0.8f, 1.0f), 32);
		gui.text(to_string(round(fps * 10.0f) / 10.0f) + " fps", vec2(getFramebufferSize().x - 450.0f, 50.0f), vec4(1.0f));
		if(gui.button("Click me!", vec2(10, 100), vec4(0.2f, 0.2f, 0.2f, 1.0f))) {
			std::cout<<"click!\n";
		}
	gui.endFrame();
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	world.render();
	world.renderCollisions(player->collidingTiles, textures.get(2));
	gui.render();
}

vec2 Game::screenToWorldSpace(vec2 screenpos) {
	mat4 view = mat4().translate(vec3(0, 0, cam.pos.z)).inverse();
	mat4 proj = cam.proj;

	vec2 normalizedpos = screenpos / getFramebufferSize();
	vec2 glpos = (normalizedpos * 2 - 1) * vec2(1, -1);
	vec4 tmp = (proj * view * vec4(0, 0, 0, 1)) + glpos;
	vec2 worldpos = proj.inverse() * view.inverse() * tmp;

	return worldpos * tmp.w + cam.pos.xy;
}

vec2 Game::worldToScreenSpace(vec2 worldpos) {
	mat4 view = mat4().translate(vec3(0, 0, cam.pos.z)).inverse();
	mat4 proj = cam.proj;

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