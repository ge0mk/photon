#include "platformer.hpp"

TileCursor::TileCursor(std::shared_ptr<TiledTexture> sprites) : Entity(sprites) {}

void TileCursor::update(float time, float dt, World *world) {
	transform = mat4().translate(pos).scale(0.5).translate(vec3(1,1,0)).scale(1.05);
}

Game::Game() : opengl::Window(1080, 720, "Game"), world("res/platformer.glsl", &cam) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	auto tileset = textures.load("res/tileset.png", ivec2(16, 16));
	world.setTexturePtr(tileset);

	world.setAutoGrow(true);
	world.createChunk(ivec2(0, -1))->fill(Tile::stone);
	world.createChunk(ivec2(-1, -1))->fill(Tile::stone);
	for(int x = -33; x < 32; x++) {
		world[ivec2(x, -1)] = Tile::grass;
	}
	for(int i = 0; i < 128; i++) {
		world[ivec2(i)] = Tile::stone;
	}
	for(int i = 0; i < 8; i++) {
		world[ivec2(-i - 4, 5)] = Tile::stone;
	}
	world[ivec2(-4,0)] = Tile::stone;
	world[ivec2(-4,1)] = Tile::stone;
	world[ivec2(-4,2)] = Tile::stone;

	auto playerSprite = textures.load("res/player.png", 1);
	player = world.createEntity<Player>(&cam, playerSprite);

	auto cursorSprite = textures.load("res/crosshair.png", 1);
	cursor = world.createEntity<TileCursor>(cursorSprite);

	auto palette = textures.load("res/palette.png", 1);
	world.setParticleTexturePtr(palette);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::update() {
	double current_time = glfwGetTime();
	dt = time > 0.0 ? (current_time - time) : (1.0f / 60.0f);
	time = current_time;

	if(getKey(GLFW_KEY_A))
		player->moveLeft();
	if(getKey(GLFW_KEY_D))
		player->moveRight();
	if(getKey(GLFW_KEY_SPACE))
		player->jump();
	if(getKey(GLFW_KEY_LEFT_ALT))
		player->dash();

	cursor->pos = snapToGrid(screenToWorldSpace(getCursorPos()));
	if(getMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)) {
		world.createBloodParticles(screenToWorldSpace(getCursorPos()) - 0.5);
	}

	world.update(glfwGetTime(), dt);
	cam.update(glfwGetTime(), dt);
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	world.render();
	world.renderCollisions(player, textures.get(2));
}
/*
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
*/
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