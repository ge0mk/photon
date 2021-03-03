#include <imgui/app.hpp>
#include <utils/json.hpp>
#include <opengl/shader.hpp>
#include <opengl/program.hpp>
#include <opengl/buffer.hpp>
#include <opengl/mesh.hpp>
#include <opengl/vao.hpp>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <math/math.hpp>
#include <math/quaternion.hpp>
#include <utils/image.hpp>
#include <entt/entt.hpp>

class Engine : public imgui::Application {
public:
	Engine();
	~Engine();

protected:
	void update() override;
	void render() override;
	void renderUI() override;

	void renderScene();
	void renderSkybox();

	void onKeyPressed(int key, int scancode, int modifier, bool repeat) override;
	void onKeyTyped(int key) override;
	void onMouseMoved(math::vec2 pos, math::vec2 dir) override;
	void onScrollEvent(math::vec2 dir) override;
	void onFramebufferResized(math::ivec2 size) override;
	void onWindowFocusChanged(bool focus) override;

	using Vertex = opengl::Vertex<math::vec3, math::vec4, math::vec3, math::vec2>;
	using Mesh = opengl::Mesh<math::vec3, math::vec4, math::vec3, math::vec2>;
	/*
	struct Vertex {
		math::vec4 pos = math::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		math::vec4 color = math::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		math::vec3 normal = math::vec3(0.0f, 0.0f, 1.0f);
		math::vec3 uv = math::vec3(0.5f, 0.5f);
	};
	*/

	struct CameraInfo {
		math::mat4 view, proj;
	};

	struct FrameInfo {
		math::ivec2 res;
		float time, dt;
		float gamma = 0.94f, ambient = 0.264f;
	} frame;

	struct LightInfo {
		math::vec4 pos;
		math::vec4 dir;
		math::vec4 color;
	};

	struct ModelInfo {
		math::mat4 transform = math::mat4();
		math::mat4 uvtransform = math::mat4();
	};

	struct Camera {
		math::vec3 pos = math::vec3(0, 0, -2);
		math::vec2 rot;
		math::vec2 res;
		float fov = 90, aspect = 1.0f;
		float znear = 0.1, zfar = 1000;

		CameraInfo ubo;
	} cam;

	template<typename T>
	struct ptr_component {
		T* operator->() { return ptr; }
		T *ptr;
	};

	template<typename T>
	struct value_component {
		T value;
	};

	typedef ptr_component<Mesh> MeshComponent;
	typedef ptr_component<opengl::Texture> TextureComponent;

	struct TransformComponent {
		math::vec3 pos = math::vec3(0, 0, 0);
		math::vec3 rot = math::vec3(0, 0, 0);
		math::vec3 scale = math::vec3(1, 1, 1);
	};

	struct UVTransformComponent {
		math::vec2 pos, scale;
		float rot;
	};

	std::pair<std::vector<Vertex>, std::vector<unsigned>> generatePlane(int detail = 1, math::vec4 color = math::vec4(1, 1, 1, 0));
	std::pair<std::vector<Vertex>, std::vector<unsigned>> generateCube(int detail = 1, math::vec4 color = math::vec4(1, 1, 1, 0));
	std::pair<std::vector<Vertex>, std::vector<unsigned>> generateSphere(int detail = 1, math::vec4 color = math::vec4(1, 1, 1, 0));
	void subdivide(int detail, std::vector<Vertex> &vertices, std::vector<unsigned> &indices, bool useSlerp = false);
	std::pair<std::vector<Vertex>, std::vector<unsigned>> loadOBJFile(const std::string &filename);
	std::pair<std::vector<Vertex>, std::vector<unsigned>> loadHeightmap(const Image &image);

private:
	bool guiVisible = false;
	opengl::Program prog, crosshair, skybox;
	opengl::Buffer<CameraInfo> cameraInfoUBO;
	opengl::Buffer<FrameInfo> frameInfoUBO;
	opengl::Buffer<LightInfo> lightInfoUBO;
	opengl::Buffer<ModelInfo> modelInfoUBO;

	opengl::Texture skyboxTexture, cubeTexture, sphereTexture;
	opengl::FrameBuffer fb;
	ImFont* pFont;
	std::vector<LightInfo> lights;
	bool wireframe = false;

	entt::registry registry;

	Mesh *cubeMesh, *planeMesh, *sphereMesh;

	opengl::Program loadProgram(const std::string &path);
};

int main(int argc, const char *argv[]);