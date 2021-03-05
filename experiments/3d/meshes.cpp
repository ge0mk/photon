#include <photon.hpp>

std::pair<std::vector<Engine::Vertex>, std::vector<unsigned>> Engine::generatePlane(int detail, math::vec4 color) {
	std::vector<Engine::Vertex> vertices = {
		{ math::vec3( 1.0f, 1.0f, 0.0f), color, math::vec3(), math::vec2(1,1) },
		{ math::vec3(-1.0f, 1.0f, 0.0f), color, math::vec3(), math::vec2(0,1) },
		{ math::vec3(-1.0f,-1.0f, 0.0f), color, math::vec3(), math::vec2(0,0) },
		{ math::vec3( 1.0f,-1.0f, 0.0f), color, math::vec3(), math::vec2(1,0) },
	};
	std::vector<unsigned> indices = {
		0, 1, 2, 2, 3, 0,
	};

	subdivide(detail, vertices, indices, false);
	return {vertices, indices};
}

std::pair<std::vector<Engine::Vertex>, std::vector<unsigned>> Engine::generateCube(int detail, math::vec4 color) {
	std::vector<Engine::Vertex> vertices = {
		{ math::vec3( 1.0f, 1.0f, 1.0f), color, math::vec3( 0, 0, 1), math::vec2(1,0) },
		{ math::vec3( 1.0f,-1.0f, 1.0f), color, math::vec3( 0, 0, 1), math::vec2(1,1) },
		{ math::vec3(-1.0f,-1.0f, 1.0f), color, math::vec3( 0, 0, 1), math::vec2(0,1) },
		{ math::vec3(-1.0f, 1.0f, 1.0f), color, math::vec3( 0, 0, 1), math::vec2(0,0) },
		{ math::vec3( 1.0f, 1.0f,-1.0f), color, math::vec3( 0, 0,-1), math::vec2(1,0) },
		{ math::vec3( 1.0f,-1.0f,-1.0f), color, math::vec3( 0, 0,-1), math::vec2(1,1) },
		{ math::vec3(-1.0f,-1.0f,-1.0f), color, math::vec3( 0, 0,-1), math::vec2(0,1) },
		{ math::vec3(-1.0f, 1.0f,-1.0f), color, math::vec3( 0, 0,-1), math::vec2(0,0) },

		{ math::vec3( 1.0f, 1.0f, 1.0f), color, math::vec3( 0, 1, 0), math::vec2(1,0) },
		{ math::vec3( 1.0f, 1.0f,-1.0f), color, math::vec3( 0, 1, 0), math::vec2(1,1) },
		{ math::vec3(-1.0f, 1.0f,-1.0f), color, math::vec3( 0, 1, 0), math::vec2(0,1) },
		{ math::vec3(-1.0f, 1.0f, 1.0f), color, math::vec3( 0, 1, 0), math::vec2(0,0) },
		{ math::vec3( 1.0f,-1.0f, 1.0f), color, math::vec3( 0,-1, 0), math::vec2(1,0) },
		{ math::vec3( 1.0f,-1.0f,-1.0f), color, math::vec3( 0,-1, 0), math::vec2(1,1) },
		{ math::vec3(-1.0f,-1.0f,-1.0f), color, math::vec3( 0,-1, 0), math::vec2(0,1) },
		{ math::vec3(-1.0f,-1.0f, 1.0f), color, math::vec3( 0,-1, 0), math::vec2(0,0) },

		{ math::vec3( 1.0f, 1.0f, 1.0f), color, math::vec3( 1, 0, 0), math::vec2(1,0) },
		{ math::vec3( 1.0f, 1.0f,-1.0f), color, math::vec3( 1, 0, 0), math::vec2(0,0) },
		{ math::vec3( 1.0f,-1.0f,-1.0f), color, math::vec3( 1, 0, 0), math::vec2(0,1) },
		{ math::vec3( 1.0f,-1.0f, 1.0f), color, math::vec3( 1, 0, 0), math::vec2(1,1) },
		{ math::vec3(-1.0f, 1.0f, 1.0f), color, math::vec3(-1, 0, 0), math::vec2(1,0) },
		{ math::vec3(-1.0f, 1.0f,-1.0f), color, math::vec3(-1, 0, 0), math::vec2(0,0) },
		{ math::vec3(-1.0f,-1.0f,-1.0f), color, math::vec3(-1, 0, 0), math::vec2(0,1) },
		{ math::vec3(-1.0f,-1.0f, 1.0f), color, math::vec3(-1, 0, 0), math::vec2(1,1) },
	};
	std::vector<unsigned> indices = {
		 0, 1, 2, 2, 3, 0,
		 6, 5, 4, 4, 7, 6,
		10, 9, 8, 8,11,10,
		12,13,14,14,15,12,
		16,17,18,18,19,16,
		22,21,20,20,23,22,
	};

	subdivide(detail, vertices, indices, false);
	return {vertices, indices};
}

std::pair<std::vector<Engine::Vertex>, std::vector<unsigned>> Engine::generateSphere(int detail, math::vec4 color) {
	const float f = (1.0 + sqrt(5.0)) / 2.0;

	std::vector<Vertex> vertices = {
		{ math::normalize(math::vec3(-1,  f,  0)), color, math::vec3(), math::vec2() },
		{ math::normalize(math::vec3( 1,  f,  0)), color, math::vec3(), math::vec2() },
		{ math::normalize(math::vec3(-1, -f,  0)), color, math::vec3(), math::vec2() },
		{ math::normalize(math::vec3( 1, -f,  0)), color, math::vec3(), math::vec2() },

		{ math::normalize(math::vec3( 0, -1,  f)), color, math::vec3(), math::vec2() },
		{ math::normalize(math::vec3( 0,  1,  f)), color, math::vec3(), math::vec2() },
		{ math::normalize(math::vec3( 0, -1, -f)), color, math::vec3(), math::vec2() },
		{ math::normalize(math::vec3( 0,  1, -f)), color, math::vec3(), math::vec2() },

		{ math::normalize(math::vec3( f,  0, -1)), color, math::vec3(), math::vec2() },
		{ math::normalize(math::vec3( f,  0,  1)), color, math::vec3(), math::vec2() },
		{ math::normalize(math::vec3(-f,  0, -1)), color, math::vec3(), math::vec2() },
		{ math::normalize(math::vec3(-f,  0,  1)), color, math::vec3(), math::vec2() },
	};

	std::vector<unsigned> indices = {
		 0,  5, 11,
		 0,  1,  5,
		 0,  7,  1,
		 0, 10,  7,
		 0, 11, 10,

		 1,  9,  5,
		 5,  4, 11,
		11,  2, 10,
		10,  6,  7,
		 7,  8,  1,

		 3,  4,  9,
		 3,  2,  4,
		 3,  6,  2,
		 3,  8,  6,
		 3,  9,  8,

		 4,  5,  9,
		 2, 11,  4,
		 6, 10,  2,
		 8,  7,  6,
		 9,  1,  8
	};

	subdivide(detail, vertices, indices, true);

	for(unsigned i = 0; i < vertices.size(); i++) {
		auto & [pos, color, normal, uv] = vertices[i]; // tmp;

		pos = math::vec4(math::normalize(pos), 1.0f);
		float u = 0.5 + atan2(pos.z, pos.x) / (2 * 3.141592654f);
		float v = 0.5 - asin(pos.y) / (3.141592654f);
		uv = math::vec2(1.0) - math::vec2(u, -v);
		normal = pos;	// works only on unit sphere
	}

	// uv correction
	std::vector<unsigned> wrapped;
	for (int i = 0; i < indices.size(); i += 3) {
		uint a = indices[i];
		uint b = indices[i + 1];
		uint c = indices[i + 2];

		math::vec3 texA = vertices[a].get<3>();
		math::vec3 texB = vertices[b].get<3>();
		math::vec3 texC = vertices[c].get<3>();
		math::vec3 texNormal = math::cross(texB - texA, texC - texA);
		if (texNormal.z < 0) {
			wrapped.push_back(i);
		}
	}

	std::unordered_map<unsigned, unsigned> visited;
	for (unsigned i : wrapped) {
		unsigned a = indices[i];
		unsigned b = indices[i + 1];
		unsigned c = indices[i + 2];
		Engine::Vertex A = vertices[a];
		Engine::Vertex B = vertices[b];
		Engine::Vertex C = vertices[c];
		if (A.get<3>().x < 0.25f) {
			unsigned tempA = a;
			if (!visited.count(a)) {
				A.get<3>().x += 1;
				visited[a] = vertices.size();
				tempA = vertices.size();
				vertices.push_back(A);
			}
			else {
				tempA = visited[a];
			}
			a = tempA;
		}
		if (B.get<3>().x < 0.25f) {
			unsigned tempB = b;
			if (!visited.count(b)) {
				B.get<3>().x += 1;
				visited[b] = vertices.size();
				tempB = vertices.size();
				vertices.push_back(B);
			}
			else {
				tempB = visited[b];
			}
			b = tempB;
		}
		if (C.get<3>().x < 0.25f) {
			unsigned tempC = c;
			if (!visited.count(c)) {
				C.get<3>().x += 1;
				visited[c] = vertices.size();
				tempC = vertices.size();
				vertices.push_back(C);
			}
			else {
				tempC = visited[c];
			}
			c = tempC;
		}
		indices[i] = a;
		indices[i + 1] = b;
		indices[i + 2] = c;
	}

	// poles
	for (int i = 0; i < indices.size(); i += 3) {
		math::vec3 auv = vertices[indices[i]].get<3>();
		math::vec3 buv = vertices[indices[i + 1]].get<3>();
		math::vec3 cuv = vertices[indices[i + 2]].get<3>();
		for(unsigned k = 0; k < 3; k++) {
			unsigned index = indices[i + k];
			Engine::Vertex vertex = vertices[indices[i + k]];
			if(abs(vertex.get<0>().y) == 1.0f) {
				math::vec2 uv = vertex.get<3>();
				if(k == 0) {
					uv.x = (buv.x + cuv.x) / 2;
				} else if(k == 1) {
					uv.x = (auv.x + cuv.x) / 2;
				} else if(k == 2) {
					uv.x = (auv.x + buv.x) / 2;
				}
				vertex.get<3>() = uv;
				indices[i + k] = vertices.size();
				vertices.push_back(vertex);
			}
		}
	}

	return {vertices, indices};
}

void Engine::subdivide(int detail, std::vector<Vertex> &vertices, std::vector<unsigned> &baseIndices, bool useSlerp) {
	typedef std::vector<unsigned> Edge;

	auto mix = [useSlerp](const Vertex &a, const Vertex &b, float t) -> Vertex {
		auto [pos_a, color_a, normal_a, uv_a] = a;
		auto [pos_b, color_b, normal_b, uv_b] = b;

		if(useSlerp) {
			math::vec4 pos = math::slerp(pos_a, pos_b, t);
			math::vec4 color = math::lerp(color_a, color_b, t);
			math::vec3 normal = math::slerp(normal_a, normal_b, t);
			math::vec2 uv = math::slerp(uv_a, uv_b, t);
			return {pos, color, normal, uv};
		} else {
			math::vec4 pos = math::lerp(pos_a, pos_b, t);
			math::vec4 color = math::lerp(color_a, color_b, t);
			math::vec3 normal = math::lerp(normal_a, normal_b, t);
			math::vec2 uv = math::lerp(uv_a, uv_b, t);
			return {pos, color, normal, uv};
		}
	};

	std::vector<unsigned> indices;

	int baseFaceCount = baseIndices.size() / 3;
	int baseVertexCount = vertices.size();
	int divisionCount = math::max(0, detail - 1);
	int verticesPerFace = ((divisionCount + 3) * (divisionCount + 3) - (divisionCount + 3)) / 2;
	int verticesCount = verticesPerFace * 8 - (divisionCount + 2) * baseVertexCount + baseVertexCount;
	int trianglesPerFace = (divisionCount + 1) * (divisionCount + 1);

	vertices.reserve(verticesCount);
	indices.reserve(trianglesPerFace * baseFaceCount * 3);

	std::vector<Edge> edges;

	for(unsigned i = 0; i < baseIndices.size(); i += 3) {
		for(unsigned k = 0; k < 3; k++) {
			Vertex start = vertices[baseIndices[i + k]];
			Vertex end = vertices[baseIndices[i + (k + 1) % 3]];

			std::vector<unsigned> edgeVertexIndices(divisionCount + 2);

			if(k == 2) {
				edgeVertexIndices[divisionCount + 1] = baseIndices[i + k];
				edgeVertexIndices[0] = baseIndices[i + (k + 1) % 3];
			}
			else {
				edgeVertexIndices[0] = baseIndices[i + k];
				edgeVertexIndices[divisionCount + 1] = baseIndices[i + (k + 1) % 3];
			}

			for(unsigned division = 0; division < divisionCount; division++) {
				float t = float(division + 1.0f) / float(divisionCount + 1.0f);
				if(k == 2) {
					t = 1.0 - t;
				}
				edgeVertexIndices[division + 1] = vertices.size();
				vertices.push_back(mix(start, end, t));
			}
			edges.push_back(edgeVertexIndices);
		}
	}

	for(unsigned i = 0; i < edges.size(); i += 3) {
		const Edge &sideA = edges[i];
		const Edge &sideB = edges[i + 1];
		const Edge &sideC = edges[i + 2];
		unsigned pointsPerEdge = sideA.size();

		std::vector<unsigned> vertexMap;
		vertexMap.reserve(verticesPerFace);
		vertexMap.push_back(sideA.back());

		for(unsigned i = 1; i < pointsPerEdge - 1; i++) {
			vertexMap.push_back(sideA[sideA.size() - i - 1]);

			Vertex sideAVertex = vertices[sideA[sideA.size() - i - 1]];
			Vertex sideBVertex = vertices[sideB[i]];
			unsigned innerPoints = i - 1;
			for(unsigned k = 0; k < innerPoints; k++) {
				float t = float(k + 1.0f) / float(innerPoints + 1.0f);
				vertexMap.push_back(vertices.size());
				vertices.push_back(mix(sideAVertex, sideBVertex, t));
			}

			vertexMap.push_back(sideB[i]);
		}

		for(unsigned i = 0; i < pointsPerEdge; i ++) {
			vertexMap.push_back(sideC[i]);
		}

		int rowCount = divisionCount + 1;
		for(unsigned row = 0; row < rowCount; row++) {
			int top = ((row + 1) * (row + 1) - (row + 1)) / 2;
			int bottom = ((row + 2) * (row + 2) - (row + 2)) / 2;
			int trianglesPerRow = row * 2 + 1;

			for(int column = 0; column < trianglesPerRow; column++) {
				int v0, v1, v2;
				if(column % 2 == 0) {
					v0 = top;
					v1 = bottom + 1;
					v2 = bottom;
					top++, bottom++;
				}
				else {
					v0 = top;
					v1 = bottom;
					v2 = top - 1;
				}

				indices.push_back(vertexMap[v0]);
				indices.push_back(vertexMap[v1]);
				indices.push_back(vertexMap[v2]);
			}
		}
	}

	baseIndices = indices;
}

std::pair<std::vector<Engine::Vertex>, std::vector<unsigned>> Engine::loadOBJFile(const std::string &filename) {
	return {};
}

std::pair<std::vector<Engine::Vertex>, std::vector<unsigned>> Engine::loadHeightmap(const Image &image) {
	return {};
}
