#include <chunk.hpp>

Chunk::Chunk(const WorldContainer &container, lvec2 pos, vec2 tileScale) : container(container), pos(pos), tileScale(tileScale) {}

void Chunk::fill(uint64_t type) {
	for(Tile &tile : tiles) {
		tile = Tile(type);
	}
	rebuild = true;
}

void Chunk::build() {
	std::lock_guard<std::mutex> lock(meshMutex);

	vertices.clear();
	indices.clear();

	auto addQuad = [&](std::array<Vertex, 4> quad) {
		unsigned indexBase = vertices.size();
		vertices.push_back(quad[0]);
		vertices.push_back(quad[1]);
		vertices.push_back(quad[2]);
		vertices.push_back(quad[3]);

		indices.push_back(indexBase + 0);
		indices.push_back(indexBase + 1);
		indices.push_back(indexBase + 2);
		indices.push_back(indexBase + 2);
		indices.push_back(indexBase + 3);
		indices.push_back(indexBase + 0);
	};

	for(unsigned y = 0; y < size; y++) {
		for(unsigned x = 0; x < size; x++) {
			const Tile &current = tiles[y * size + x];
			if(current.render()) {
				vec2 bl = vec2(x, y) * Tile::resolution;
				vec2 tr = bl + vec2(1) * Tile::resolution;
				vec2 tl = vec2(bl.x, tr.y);
				vec2 br = vec2(tr.x, bl.y);

				vec2 uvtl = current.texture(svec2(x, y)) * tileScale;
				vec2 uvbr = uvtl + tileScale;
				uvtl += vec2(0.000001), uvbr -= vec2(0.000001);
				vec2 uvbl = vec2(uvtl.x, uvbr.y);
				vec2 uvtr = vec2(uvbr.x, uvtl.y);

				addQuad({
					Vertex { vec3(bl), uvbl },
					Vertex { vec3(br), uvbr },
					Vertex { vec3(tr), uvtr },
					Vertex { vec3(tl), uvtl },
				});
			}
		}
	}
}

void Chunk::update(float time, float dt) {
	for(unsigned y = 0; y < size; y++) {
		for(unsigned x = 0; x < size; x++) {
			tiles[y * size + x].update(time, dt, ivec2(x, y), *this);
		}
	}
	if(rebuild) {
		sync = true;
		build();
		rebuild = false;
	}
}

void Chunk::render() {
	if(!mesh) {
		mesh = std::unique_ptr<Mesh>(new Mesh());
	}
	if(sync) {
		std::lock_guard<std::mutex> lock(meshMutex);
		mesh->setVertexData(vertices);
		mesh->setIndexData(indices);
		sync = false;
	}
	mesh->drawElements();
}

lvec2 Chunk::getPos() {
	return pos;
}

Tile& Chunk::operator[](ivec2 pos) {
	if(pos.x >= 0 && pos.y >= 0 && pos.x < size && pos.y < size) {
		rebuild = true;
		return tiles[pos.y * size + pos.x];
	}
	throw std::runtime_error(std::string("error: tile (") + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ") is not in this chunk!");
}

const Tile& Chunk::operator[](ivec2 pos) const {
	if(pos.x >= 0 && pos.y >= 0 && pos.x < size && pos.y < size) {
		return tiles[pos.y * size + pos.x];
	}
	throw std::runtime_error(std::string("error: tile (") + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ") is not in this chunk!");
}

Tile& Chunk::at(ivec2 pos) {
	return this->operator[](pos);
}

const Tile& Chunk::at(ivec2 pos) const {
	return this->operator[](pos);
}