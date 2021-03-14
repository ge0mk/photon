#include <array>
#include <iostream>

struct vec2 {float x,y;};

enum class TileType {
	null,
	stone,
	dirt,
};

struct StoneTile {
	static TileType type() { return TileType::stone; }
	static vec2 uv() { return {1, 1}; }
};

struct Tile {
	TileType type;

	Tile(const Tile &other) = default;
	Tile() : type{TileType::null} {}

	Tile(TileType type) : type(type) {}

	vec2 uv() {
		switch(type) {
			case TileType::null: return {0, 0};
			case TileType::stone: return StoneTile::uv();
			default: return {0, 0};
		}
	}
};

int main(int argc, const char *argv[]) {
	std::array<Tile, 32*32> chunk;
	chunk[0] = Tile(TileType::stone);
	std::cout<<chunk[0].uv().x<<"\n";
	return 0;
}