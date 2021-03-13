#include <glfw/window.hpp>

namespace opengl {
	class Window : public glfw::Window {
	public:
		Window(math::ivec2 size, const std::string &title);
		~Window();

		virtual int exec();

	protected:
		virtual void update();
		virtual void render();
		virtual void onFramebufferResized(math::ivec2 size);
	};
}