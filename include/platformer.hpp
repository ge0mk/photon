#pragma once

#include <imgui/app.hpp>
#include <imgui/ImFileDialog.h>

#include <math/math.hpp>
#include <math/quaternion.hpp>

#include <opengl/buffer.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/mesh.hpp>
#include <opengl/program.hpp>
#include <opengl/texture.hpp>
#include <opengl/vao.hpp>

#include "tile.hpp"
#include "chunk.hpp"
#include "world.hpp"
#include "resources.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "particles.hpp"

class TileCursor : public Entity {
public:
	TileCursor(std::shared_ptr<TiledTexture> sprites);

	void update(float time, float dt, World *world) override;

	vec2 pos;
};

class Game : public imgui::Application {
public:
	using Texture = opengl::Texture;
	using Program = opengl::Program;
	using FrameBuffer = opengl::FrameBuffer;
	using Vertex = opengl::Vertex<vec3, vec2>;
	using Mesh = opengl::Mesh<vec3, vec2>;


	Game();

	// gameloop
	void update() override;
	void render() override;
	void renderUI() override;

	// mechanics
	vec2 screenToWorldSpace(vec2 cursorpos);
	vec2 worldToScreenSpace(vec2 worldpos);
	vec2 snapToGrid(vec2 worldpos);

	// events
	void onFramebufferResized(ivec2 size) override;

private:
	Camera cam = Camera(vec3(0, 0, -5), vec3(), vec2(1080, 720), 90);

	World world;
	TileCursor *cursor;
	Player* player;
	ResourceCache<TiledTexture> textures;
	ParticleSystem *rain;
};

int main(int argc, const char *argv[]);