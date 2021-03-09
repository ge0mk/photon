#include "platformer.hpp"

TileCursor::TileCursor(std::shared_ptr<TiledTexture> sprites) : Entity(sprites) {}

void TileCursor::update(float time, float dt, World *world) {
	transform = mat4().translate(pos).scale(0.5).translate(vec3(1,1,0)).scale(1.05);
}

Game::Game() : imgui::Application(1080, 720, "Game"), world("res/platformer.glsl", &cam) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	auto tileset = textures.load("res/tileset.png", ivec2(16, 16));

	world.setTexturePtr(tileset);
	world.createChunk(ivec2(0, -1))->fill(Tile::stone);
	world.createChunk(ivec2(-1, -1))->fill(Tile::stone);
	for(int x = -31; x < 32; x++) {
		//world[ivec2(x, -1)] = Tile::grass;
	}
	world.setAutoGrow(true);
	for(int i = 0; i < 128; i++) {
		world[ivec2(i)] = Tile::stone;
	}
	for(int i = 0; i < 8; i++) {
		world[ivec2(-i - 3, 5)] = Tile::stone;
	}
	world[ivec2(-3,0)] = Tile::stone;
	world[ivec2(-3,1)] = Tile::stone;
	world[ivec2(-3,2)] = Tile::stone;

	auto playerSprite = textures.load("res/player.png", 1);
	player = world.createEntity<Player>(&cam, playerSprite);

	auto cursorSprite = textures.load("res/crosshair.png", 1);
	cursor = world.createEntity<TileCursor>(cursorSprite);

	rain = world.createEntity<ParticleSystem>(cursorSprite);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::update() {
	if(!io->WantCaptureKeyboard) {
		float dx = getKey(GLFW_KEY_D) - getKey(GLFW_KEY_A);
		float dy = getKey(GLFW_KEY_W) - getKey(GLFW_KEY_S);

		player->applyForce(vec2(dx, dy) * vec2(100, 200));
	}
	if(!io->WantCaptureMouse) {
		cursor->pos = snapToGrid(screenToWorldSpace(getCursorPos()));
	}

	world.update(glfwGetTime(), dt);
	cam.update(glfwGetTime(), dt);

	rain->update(glfwGetTime(), dt, nullptr);
	if(rain->size() < 10000) {
		rain->spawn({vec2(float(rand()) / float(RAND_MAX) * 50 - 25, 20), 0, 0, 0, vec2(0, -0.5), 1.5, 0, vec2(0.03, 0.1)});
	}
	for(Particle &p : *rain) {
		if(p.pos.y < 0) {
			p.pos = vec2(float(rand()) / float(RAND_MAX) * 50 - 25, 20);
			p.speed = 0;
		}
	}
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	world.render();
	world.renderCollisions(player);
}

void Game::renderUI() {
	if(!io->WantCaptureMouse) {
		if(getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			world[cursor->pos] = Tile::stone;
		}
		else if(getMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
			world[cursor->pos] = Tile::null;
		}
	}
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

vec2 Game::snapToGrid(vec2 worldpos) {
	return ivec2(worldpos) - ivec2(worldpos.x < 0 ? 1 : 0, worldpos.y < 0 ? 1 : 0);
}

void Game::onFramebufferResized(ivec2 size) {
	cam.res = size;
	imgui::Application::onFramebufferResized(size);
}

int main(int argc, const char *argv[]) {
	std::vector<std::string> args(argv, argv + argc);

	glfw::init();
	atexit(glfw::cleanup);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	Game game;
	return game.run();
}