#include <entt/entt.hpp>

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

#include <soloud/soloud.h>

#include <spdlog/spdlog.h>

#include <utils/image.hpp>
#include <utils/json.hpp>

#include <string>
#include <vector>

using namespace entt;
using namespace math;

class Engine : public imgui::Application {
public:
	Engine() : imgui::Application(1080, 720, "Hello World!"),
			cameraInfoUBO(opengl::Buffer<CameraInfo>::Uniform),
			frameInfoUBO(opengl::Buffer<FrameInfo>::Uniform),
			modelInfoUBO(opengl::Buffer<ModelInfo>::Uniform) {
		// ImFileDialog requires you to set the CreateTexture and DeleteTexture
		ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
			GLuint tex;

			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			return (void*)tex;
		};
		ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
			GLuint texID = (GLuint)((uintptr_t)tex);
			glDeleteTextures(1, &texID);
		};

		main = loadProgram("res/platformer.glsl");

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		mainfb.bind();
		mainfb.addAttachment(std::move(opengl::Texture(GL_RGBA, getFramebufferSize(), GL_UNSIGNED_BYTE)), GL_COLOR_ATTACHMENT0);
		mainfb.addAttachment(std::move(opengl::RenderBuffer(GL_DEPTH24_STENCIL8, getFramebufferSize())), GL_DEPTH_STENCIL_ATTACHMENT);
		mainfb.resize(getFramebufferSize());

		tilefb.bind();
		tilefb.addAttachment(std::move(opengl::Texture(GL_RGBA, tilefb.size(), GL_UNSIGNED_BYTE)), GL_COLOR_ATTACHMENT0);
		tilefb.resize(ivec2(64, 64));

		cameraInfoUBO.initEmpty(opengl::Buffer<CameraInfo>::DynamicDraw);
		cameraInfoUBO.bindBase(0);

		modelInfoUBO.initEmpty(opengl::Buffer<ModelInfo>::DynamicDraw);
		modelInfoUBO.bindBase(1);

		frameInfoUBO.initEmpty(opengl::Buffer<FrameInfo>::DynamicDraw);
		frameInfoUBO.bindBase(2);

		unitplane = Mesh({
			{ math::vec3( 1.0f, 1.0f, 0.0f), math::vec2(1,0) },
			{ math::vec3(-1.0f, 1.0f, 0.0f), math::vec2(0,0) },
			{ math::vec3(-1.0f,-1.0f, 0.0f), math::vec2(0,1) },
			{ math::vec3( 1.0f,-1.0f, 0.0f), math::vec2(1,1) },
		}, {
			0, 1, 2, 2, 3, 0,
		});

		root = ecs.create();
		ecs.emplace<ParentComponent>(root);
		textureCache.load<TextureLoader>(entt::hashed_string{"crosshair"}, "res/crosshair.png");

		loadLevel("res/level.json");
	}

	~Engine() {}

protected:
	// structures
	enum class State {
		null,
		editor,
		game
	} state = State::editor;

	using Texture = opengl::Texture;
	using Program = opengl::Program;
	using FrameBuffer = opengl::FrameBuffer;
	using Vertex = opengl::Vertex<vec3, vec2>;
	using Mesh = opengl::Mesh<vec3, vec2>;

	struct Tile {
		std::string name;
		vec2 uv, scale;
		bool solid;
	};

	struct CameraInfo {
		math::mat4 view, proj;
	};

	struct FrameInfo {
		math::ivec2 res;
		float time, dt;
	} frame;

	struct ModelInfo {
		math::mat4 transform;
		math::mat4 uvtransform;
	};


	// Components

	typedef Mesh MeshComponent;
	typedef entt::resource_handle<Texture> TextureComponent;

	struct TransformComponent {
		vec2 pos, scale;
	};

	struct ParentComponent {
		std::vector<entity> children;
		std::vector<entity>* operator->() { return &children; }
	};

	struct ChildComponent {
		entity parent;
		operator entity() { return parent; }
	};

	struct ChunkComponent {
		std::array<Tile*, 32*32> tiles;
	};

	struct TextureLoader final : resource_loader<TextureLoader, Texture> {
		std::shared_ptr<Texture> load(const std::string &filename) const {
			Texture *texture = new Texture();
			Image img(filename);
			texture->load(img);
			texture->bind();
			texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			texture->setParameter(GL_TEXTURE_WRAP_R, GL_REPEAT);
			texture->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
			texture->unbind();
			return std::shared_ptr<Texture>(texture);
		}
	};

	struct TileLoader final : resource_loader<TileLoader, Tile> {
		std::shared_ptr<Tile> load(const std::string name, const json &data) const {
			Tile *tile = new Tile{
				name,
				vec2(
					data.at("uv").at(0).toFloat(),
					data.at("uv").at(1).toFloat()
				),
				vec2(
					1.0f / data.at("scale").at(0).toFloat(),
					1.0f / data.at("scale").at(1).toFloat()
				),
				data.at("solid").toBool()
			};
			return std::shared_ptr<Tile>(tile);
		}
		std::shared_ptr<Tile> load(const std::string name, const ivec2 &uv, vec2 scale, bool solid) const {
			return std::shared_ptr<Tile>(new Tile{name, uv, scale, solid});
		}
	};

	// functionality

	void update() override {
		if(!io->WantCaptureKeyboard) {
			float dx = getKey(GLFW_KEY_D) - getKey(GLFW_KEY_A);
			float dy = getKey(GLFW_KEY_W) - getKey(GLFW_KEY_S);

			campos += vec2(dx, dy) * dt * 5;
		}
	}

	void render() override {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		renderScene();
	}

	void renderScene() {
		glClearColor(0.0f, 0.02f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		main.use();

		cameraInfoUBO.update({
			math::mat4().translate(campos).inverse(),
			math::mat4::projection(math::d2r(90), getAspectRatio(), 0.1, 64)
		});

		auto texture = getCurrentTilesetTexture();
		texture->activate();
		texture->bind();

		// render chunks
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		auto view = ecs.view<ChunkComponent, MeshComponent>();
		for(auto entity : view) {
			math::mat4 transform;

			if(ecs.has<TransformComponent>(entity)) {
				auto t = ecs.get<TransformComponent>(entity);
				transform = math::mat4().translate(t.pos).scale(vec3(t.scale, 1.0f));
			}

			modelInfoUBO.update({transform, mat4()});

			Mesh &mesh = ecs.get<MeshComponent>(entity);
			mesh.drawElements();
		}

		if(wireframe) {
			texture->unbind();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for(auto entity : view) {
				math::mat4 transform;

				if(ecs.has<TransformComponent>(entity)) {
					auto t = ecs.get<TransformComponent>(entity);
					transform = math::mat4().translate(t.pos).scale(vec3(t.scale, 1.0f));
				}

				modelInfoUBO.update({transform, mat4()});

				Mesh &mesh = ecs.get<MeshComponent>(entity);
				mesh.drawElements();
			}
		}
		// render other entities, e.g. player

		// render worldcursor
		if(state == State::editor) {
			Texture &crosshair = textureCache.handle(entt::hashed_string{"crosshair"}).get();
			crosshair.activate();
			crosshair.bind();

			modelInfoUBO.update({
				math::mat4().translate(vec3(worldcursor, 0)).scale(0.5).translate(vec3(1,1,0)).scale(1.05),
				mat4()
			});
			unitplane.drawElements();
		}
	}

	void renderUI() override {
		if(state == State::editor) {
			ImGuiWindowFlags rootWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			rootWindowFlags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
			rootWindowFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
			rootWindowFlags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration;
			rootWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(vec2(viewport->Size) - vec2(viewport->WorkSize));
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

			if(ImGui::Begin("root", nullptr, rootWindowFlags)) {
				if(ImGui::BeginMenuBar()) {
					if(ImGui::BeginMenu("File")) {
						if(ImGui::MenuItem("Load tileset")) {
							loadTileset("res/tileset.json");
						}
						if(ImGui::MenuItem("Load level")) {
							loadLevel("res/level.json");
						}
						if(ImGui::MenuItem("Save tileset")) {
							saveTileset("res/tileset.json");
						}
						if(ImGui::MenuItem("Save level")) {
							saveLevel("res/level.json");
						}
						if(ImGui::MenuItem("Quit")) {
							setWindowShouldClose();
						}
						ImGui::EndMenu();
					}
					if(ImGui::BeginMenu("View")) {
						if(ImGui::MenuItem("Toggle fullscreen")) {
							Window::toggleFullscreen();
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}
			} ImGui::End();
			ImGui::PopStyleVar(3);


			if(ImGui::Begin("Tileset", nullptr)) {
				ImVec2 size = ImGui::GetWindowContentRegionMax();
				resource_handle<Texture> texture = getCurrentTilesetTexture();

				static const int width = 4;

				// render all tiles in a grid
				unsigned row = 0, col = 0;
				ImGui::BeginGroup();
				tileCache.each([&](const entt::id_type id, entt::resource_handle<Tile> tile) {
					vec2 uvpos = tile->uv * tile->scale;
					vec2 uvpos2 = uvpos + tile->scale;

					if(selected == &tile.get()) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.5, 0.5, 1));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6, 0.6, 0.6, 1));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5, 0.5, 0.5, 1));
					}
					else {
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 1));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3, 0.3, 0.3, 1));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3, 0.3, 0.3, 1));
					}
					ImGui::PushID(row * width + col % width);
					if(ImGui::ImageButton((void*)(intptr_t)texture->getHandle(), ImVec2(64, 64), ImVec2(uvpos.x, uvpos.y), ImVec2(uvpos2.x, uvpos2.y), -1, ImVec4(0, 0, 0, 1))) {
						selected = &tile.get();
					}
					ImGui::PopID();
					ImGui::PopStyleColor(3);

					col++;
					if(col % width == 0) {
						row++;
						ImGui::EndGroup();
						ImGui::Spacing();
						ImGui::BeginGroup();
					}
					else {
						ImGui::SameLine();
					}
				});
				ImGui::EndGroup();

				if(ImGui::Button("New Tile")) {
					tileCache.load<TileLoader>(tileCache.size(), "", vec2(0, 0), vec2(1.0) / vec2(16, 16), true);
				}
			} ImGui::End();

			if(ImGui::Begin("Tile Properties", nullptr)) {
				resource_handle<Texture> texture = getCurrentTilesetTexture();
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				vec2 size = ImGui::GetContentRegionAvail();
				float d = min(size.x, size.y);
				ImGui::Image((void*)(intptr_t)texture->getHandle(), vec2(d));

				if(selected) {
					bool changed = false;
					vec2 scale = selected->scale;
					ivec2 size = vec2(1.0f) / scale;
					vec2 buttonSize = vec2(d) / size;

					ImGui::SetCursorPos(ImGui::GetCursorStartPos());
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, vec2(0, 0));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, vec2(0, 1));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, vec2(0, 0));
					ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0);
					ImGui::PushStyleColor(ImGuiCol_Button, vec4(vec3(0),0.0));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, vec4(vec3(1),0.3333));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, vec4(vec3(1),0.6666));
					ImGui::BeginGroup();
					for(unsigned y = 0; y < size.y; y++) {
						ImGui::PushID(y);
						ImGui::BeginGroup();
						for(unsigned x = 0; x < size.x; x++) {
							ImGui::PushID(x);
							if(ImGui::Button("##", buttonSize)) {
								selected->uv = vec2(x, y);
								changed = true;
							}
							if(x < size.x - 1) {
								ImGui::SameLine();
							}
							ImGui::PopID();
						}
						ImGui::EndGroup();
						ImGui::PopID();
					}
					ImGui::EndGroup();
					ImGui::PopStyleVar(5);
					ImGui::PopStyleColor(3);

					changed |= ImGui::InputText("id", &selected->name);
					changed |= ImGui::SliderFloat2("uv position", &selected->uv.x, 0.0f, length(vec2(1.0f) / selected->scale), "%1.1f");
					vec2 tmpscale = selected->scale * texture->size();
					changed |= ImGui::SliderFloat2("uv scale", &tmpscale.x, 1.0f, min(texture->size().x, texture->size().y), "%1.0f");
					selected->scale = tmpscale / texture->size();
					changed |= ImGui::Checkbox("solid", &selected->solid);
					if(changed) {
						rebuildChunks();
					}
				}
			} ImGui::End();

			if(ImGui::Begin("Console", nullptr)) {
				ImGui::SliderFloat3("campos", &campos.x, -32.0f, 32.0f, "%1.1f");
				ImGui::Text("Cursor: (%f, %f)", worldcursor.x, worldcursor.y);
				ImGui::Checkbox("wireframe", &wireframe);
				if(ImGui::Button("test")) {
					ifd::FileDialog::Instance().Open("ShaderOpenDialog", "Open a shader", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
				}
			} ImGui::End();

			// file dialogs
			if (ifd::FileDialog::Instance().IsDone("ShaderOpenDialog")) {
				if (ifd::FileDialog::Instance().HasResult()) {
					const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
					for (const auto& r : res) { // ShaderOpenDialog supports multiselection
						printf("OPEN[%s]\n", r.string().c_str());
					}
				}
				ifd::FileDialog::Instance().Close();
			}

			// block placement
			if(!io->WantCaptureMouse) {
				vec2 screenpos = getCursorPos();
				vec2 normalizedpos = screenpos / getFramebufferSize();
				vec2 glpos = normalizedpos * vec2(2, 2) - vec2(1);
				glpos *= vec2(1, -1);	// flip y axis

				mat4 view = math::mat4().translate(vec3(0,0,campos.z)).inverse();
				mat4 proj = math::mat4::projection(math::d2r(90), getAspectRatio(), 0.1, 64);

				vec4 tmp = vec4(proj * view * vec4(0, 0, 0, 1));
				tmp += glpos;

				// scene.proj * scene.view * obj.transform * pos
				vec2 worldpos = proj.inverse() * view.inverse() * tmp;
				ivec2 tilepos = ivec2(worldpos * tmp.w + fract(campos.xy));
				tilepos -= ivec2(glpos.x < 0 ? 1 : 0, glpos.y < 0 ? 1 : 0);

				worldcursor = tilepos + ivec2(campos.xy);

				if(getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
					placeTile(worldcursor, selected);
				}
				else if(getMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
					placeTile(worldcursor, nullptr);
				}
			}
		}
	}

	Program loadProgram(const std::string &path) {
		std::ifstream src(path, std::ios::ate);
		std::string buffer(src.tellg(), '\0');
		src.seekg(src.beg);
		src.read(buffer.data(), buffer.size());

		Program prog = Program::load(buffer, opengl::Shader::VertexStage | opengl::Shader::FragmentStage);
		prog.use();
		prog.setUniform("sampler", 0);

		return std::move(prog);
	}

	resource_handle<Texture> loadTexture(const std::string &path) {
		return textureCache.load<TextureLoader>(hashed_string(path.c_str()), path);
	}

	resource_handle<Texture> getCurrentTilesetTexture() {
		return ecs.get<TextureComponent>(root);
	}

	void loadTileset(const std::string &path) {
		std::ifstream src(path, std::ios::ate);
		std::string buffer(src.tellg(), '\0');
		src.seekg(src.beg);
		src.read(buffer.data(), buffer.size());

		json data = json::parse(buffer);
		json textureinfo = data["texture"];
		auto texture = loadTexture(textureinfo["path"].toString());

		json tiles = data["tiles"];
		for(const auto &[name, tile] : tiles.getObject()) {
			tileCache.load<TileLoader>(hashed_string(name.c_str()), name, tile);
		}

		ecs.emplace_or_replace<TextureComponent>(root, texture);
	}

	void saveTileset(const std::string &path) {
		json data = json::object{
			{"texture", json::object{
				{"path", "res/tileset.png"}
			}},
			{"tiles", [this]() -> json::object {
				json::object object;
				tileCache.each([&object](const entt::id_type id, entt::resource_handle<Tile> tile){
					object[tile->name] = json::object{
						{"uv", json::array{tile->uv.x, tile->uv.y}},
						{"scale", json::array{1.0f / tile->scale.x, 1.0f / tile->scale.y}},
						{"solid", tile->solid}
					};
				});
				return object;
			} ()}
		};

		std::ofstream out(path);
		out<<data.print(json::minified);
	}

	Tile* getTileByName(const std::string &name) const {
		if(tileCache.contains(entt::hashed_string(name.c_str()))) {
			return &tileCache.handle(entt::hashed_string(name.c_str())).get();
		}
		return nullptr;
	}

	void buildChunk(entity chunkid) {
		std::array<Tile*, 32*32> data = ecs.get<ChunkComponent>(chunkid).tiles;

		std::vector<Vertex> vertices;
		std::vector<unsigned> indices;

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

		for(unsigned y = 0; y < 32; y++) {
			for(unsigned x = 0; x < 32; x++) {
				Tile *current = data[y * 32 + x];
				if(current) {
					vec2 bl = vec2(x, y);
					vec2 tr = bl + vec2(1);
					vec2 tl = vec2(bl.x, tr.y);
					vec2 br = vec2(tr.x, bl.y);

					vec2 uvtl = current->uv * current->scale;
					vec2 uvbr = uvtl + current->scale;
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

		Mesh &mesh = ecs.get<MeshComponent>(chunkid);
		mesh.setVertexData(vertices);
		mesh.setIndexData(indices);
	}

	void rebuildChunks() {
		auto view = ecs.view<ChunkComponent, MeshComponent>();
		for(auto chunk : view) {
			buildChunk(chunk);
		}
	}

	void placeTile(ivec2 pos, Tile *tile) {
		int cx = pos.x / 32 - (pos.x < 0 ? 1 : 0);
		int cy = pos.y / 32 - (pos.y < 0 ? 1 : 0);

		int tx = (pos.x - 32 * cx) % 32;
		int ty = (pos.y - 32 * cy) % 32;

		ivec2 chunkpos(cx, cy);
		ivec2 tileoffset(tx, ty);

		entity chunk = entt::null;
		auto view = ecs.view<ChunkComponent, TransformComponent>();
		for(auto entity : view) {
			TransformComponent &transform = ecs.get<TransformComponent>(entity);
			ChunkComponent &data = ecs.get<ChunkComponent>(entity);
			if(ivec2(transform.pos) == chunkpos * 32) {
				data.tiles[ty * 32 + tx] = tile;
				chunk = entity;
				break;
			}
		}

		if(chunk == entt::null && tile != nullptr) {
			chunk = ecs.create();
			ecs.emplace<MeshComponent>(chunk);
			ecs.emplace<TransformComponent>(chunk, vec2(cx, cy) * 32, vec2(1));
			auto data = ecs.emplace<ChunkComponent>(chunk);
			data.tiles[ty * 32 + tx] = tile;
		}

		if(chunk != entt::null) {
			buildChunk(chunk);
		}
	}

	void loadLevel(const std::string &path) {
		std::ifstream src(path, std::ios::ate);
		std::string buffer(src.tellg(), '\0');
		src.seekg(src.beg);
		src.read(buffer.data(), buffer.size());

		json data = json::parse(buffer);
		json tileset = data["tileset"];
		loadTileset(tileset.at("path").toString());

		std::map<int, Tile*> tilemap;
		for(const auto &[tile, index] : tileset["tiles"].getObject()) {
			tilemap[index.toInt()] = getTileByName(tile);
		}

		json chunks = data["chunks"];
		for(const json &current : chunks.getArray()) {
			const json &rawpos = current.at("pos");
			vec2 pos = vec2(rawpos.at(0).toFloat(), rawpos.at(1).toFloat());

			const json::array &rawdata = current.at("data").getArray();
			std::array<Tile*, 32*32> data;
			for(auto &t : data) { t = nullptr; }
			std::transform(rawdata.begin(), rawdata.end(), data.begin(), [&tilemap](const json& in) -> Tile* {
				return tilemap.at(in.toInt());
			});

			entity chunk = ecs.create();
			ecs.emplace<ChunkComponent>(chunk, data);
			ecs.emplace<TransformComponent>(chunk, pos * 32, vec2(1));
			ecs.emplace<MeshComponent>(chunk);

			buildChunk(chunk);	// can be multithreaded
		}
	}

	void saveLevel(const std::string &path) {
		std::map<Tile*, int> tilemap;
		tilemap[nullptr] = 0;
		int i = 1;
		tileCache.each([&](const entt::id_type id, entt::resource_handle<Tile> tile) {
			tilemap[&tile.get()] = i++;
		});

		json data = json::object{
			{"tileset", json::object{
				{"path", "res/tileset.json"},
				{"tiles", [&]() -> json::object {
					json::object tiles;
					for(const auto &[tile, index] : tilemap) {
						if(tile) {
							tiles[tile->name] = index;
						}
						else {
							tiles["null"] = index;
						}
					}
					return tiles;
				}()}
			}},
			{"chunks", [&]() ->json::array {
				auto view = ecs.view<ChunkComponent, TransformComponent>();
				json::array chunks;
				for(const auto &[entity, chunkdata, transform] : view.each()) {
					ivec2 pos = transform.pos / 32;
					chunks.push_back(json::object{
						{"pos", json::array{pos.x, pos.y}},
						{"data", [&](std::array<Tile*, 32*32> tiles) -> json::array {
							json::array data;
							for(Tile* t : tiles) {
								data.push_back(tilemap[t]);
							}
							return data;
						}(chunkdata.tiles)}
					});
				}
				return chunks;
			}()}
		};

		std::ofstream out(path);
		out<<data.print(json::minified);
	}

	void onKeyPressed(int key, int scancode, int modifier, bool repeat) override {
		if(key == GLFW_KEY_F1 && repeat == false) {
			if(state == State::editor) {
				state = State::game;
			}
			else if(state == State::game) {
				state = State::editor;
			}
		}
		else if(key == GLFW_KEY_F11 && repeat == false) {
			Window::toggleFullscreen();
		}
		else if(state == State::editor) {
			imgui::Application::onKeyPressed(key, scancode, modifier, repeat);
		}
	}

	void onKeyTyped(int key) override {
		if(state == State::editor) {
			imgui::Application::onKeyTyped(key);
		}
	}

	void onMouseMoved(vec2 pos, vec2 dir) override {
		if(state == State::editor) {
			imgui::Application::onMouseMoved(pos, dir);
		}
	}

private:
	FrameBuffer mainfb = FrameBuffer(ivec2(0));
	FrameBuffer tilefb = FrameBuffer(ivec2(512, 512));
	Program main;
	Mesh unitplane;
	opengl::Buffer<CameraInfo> cameraInfoUBO;
	opengl::Buffer<FrameInfo> frameInfoUBO;
	opengl::Buffer<ModelInfo> modelInfoUBO;

	registry ecs;
	entity root;

	resource_cache<Texture> textureCache;
	resource_cache<Tile> tileCache;

	Tile *selected = nullptr;
	vec2 worldcursor = ivec2(0);

	bool wireframe = false;
	vec3 campos = vec3(0, 0, -5);
};


int main(int argc, const char *argv[]) {
	std::vector<std::string> args(argv, argv + argc);

	glfw::init();
	atexit(glfw::cleanup);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	Engine game;
	return game.run();
}