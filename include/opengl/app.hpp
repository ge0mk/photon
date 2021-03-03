#include <GLFW/app.hpp>

namespace opengl {
	class Application : public glfw::Application {
	public:
		Application(int width, int height, std::string title);
		~Application();

		virtual int exec();

	protected:
		virtual void update();
		virtual void render();
		virtual void onFramebufferResized(math::ivec2 size);
	};
}