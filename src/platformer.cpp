#include "platformer.hpp"

TileCursor::TileCursor(std::shared_ptr<TiledTexture> sprites) : Entity(sprites) {}

void TileCursor::update(float time, float dt, World *world) {
	transform = mat4().translate(pos + 0.5 * Tile::resolution).scale(Tile::resolution + 1.0f);
}

Game::Game() : opengl::Window({1080, 720}, "Game"), world("res/platformer.glsl", &cam) {
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
	player = std::shared_ptr<Player>(new Player(&cam, playerSprite));
	world.setCameraHostPtr(player);

	auto cursorSprite = textures.load("res/crosshair.png", 1);
	cursor = world.createEntity<TileCursor>(cursorSprite);

	auto palette = textures.load("res/palette.png", 1);
	world.setParticleTexturePtr(palette);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

	std::cout<<1.0f / dt<<" physics updates per second\n";
	std::cout<<player->pos<<"\n";

	world.update(glfwGetTime(), dt);
	cam.update(glfwGetTime(), dt);
}

void Game::updateInputs() {
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
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	world.render();
	world.renderCollisions(player->collidingTiles, textures.get(2));
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

void Game::onFramebufferResized(ivec2 size) {
	cam.res = size;
	opengl::Window::onFramebufferResized(size);
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