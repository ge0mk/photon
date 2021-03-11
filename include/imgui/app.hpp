#include <opengl/window.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

namespace imgui {
	class Application : public opengl::Window {
	public:
		Application(int width, int height, std::string title);
		~Application();

		virtual int exec();

	protected:
		virtual void renderUI();

		virtual void onKeyTyped(int key);
		virtual void onKeyChanged(int key, int scancode, int modifier, int action);
		virtual void onMouseMoved(math::vec2 pos, math::vec2 dir);
		virtual void onMousePressed(int button, int modifier);
		virtual void onMouseReleased(int button, int modifier);
		virtual void onScrollEvent(math::vec2 dir);
		virtual void onFramebufferResized(math::ivec2 size);

		ImGuiIO *io;
		ImGuiStyle *style;
		ImGuiViewport* main_viewport;

		double dt = 0;

	private:
		void initImgui();
		void newFrame();

		double time = 0;
		GLFWcursor *g_MouseCursors[ImGuiMouseCursor_COUNT] = {};
	};
}