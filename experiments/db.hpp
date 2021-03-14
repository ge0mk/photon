#pragma once

#include <fstream>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include "chunk.hpp"
#include "entity.hpp"

//	world file:
//	section: 64 chunks
//		header[16384]:
//			pos[64] next section
//			({x[64], y[64]} : pos[64] of entitydata, chunkdata is directly behind header)
//			({x[64], y[64]} : pos[64] of chunkdata, pos[64] of entitydata) * 63
//		chunk[16384] * n | 0 <= n <= 64
//			tiledata for 1 chunk
//		entities[16384] * n | 0 <= n <= 64
//			offset[64] to next entitydata block
//			{x[64],y[64]} chunk coords of the relative position of all entities in this block
//			entity data for multiple chunks (each entity has its own coordinates)

//	sync:
//	when cache is an open file:
//		save changed chunks
//		unload chunks out of range
//		load chungs in range, that aren't loaded

class WorldDB {
public:
	WorldDB();
	WorldDB(const std::string &path);

	void open(const std::string &path);

	void sync();

private:
	std::fstream file; //std::fstream("world.dat", std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc)
};