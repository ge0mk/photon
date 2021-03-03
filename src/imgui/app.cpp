#include <imgui/app.hpp>
#include <spdlog/spdlog.h>

namespace imgui {
	Application::Application(int width, int height, std::string title)
	 : opengl::Application(width, height, title) {
		initImgui();
	}

	Application::~Application() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	int Application::exec() {
		while(!windowShouldClose()) {
			pollEvents();

			double current_time = glfwGetTime();
			dt = io->DeltaTime = time > 0.0 ? (float)(current_time - time) : (float)(1.0f / 60.0f);
			time = current_time;

			update();
			render();

			newFrame();

			renderUI();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			if(io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			swapBuffers();
		}
		return 0;
	}

	void Application::renderUI() {
		ImGui::ShowDemoWindow();
	}

	void Application::onKeyTyped(int key) {
		io->AddInputCharacter(key);
	}

	void Application::onKeyChanged(int key, int scancode, int modifier, int action) {
		if(action == GLFW_PRESS)
			io->KeysDown[key] = true;
		else if(action == GLFW_RELEASE)
			io->KeysDown[key] = false;

		// Modifiers are not reliable across systems
		io->KeyCtrl = 	io->KeysDown[GLFW_KEY_LEFT_CONTROL] || io->KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io->KeyShift = 	io->KeysDown[GLFW_KEY_LEFT_SHIFT] 	|| io->KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io->KeyAlt = 	io->KeysDown[GLFW_KEY_LEFT_ALT] 	|| io->KeysDown[GLFW_KEY_RIGHT_ALT];
		io->KeySuper = 	io->KeysDown[GLFW_KEY_LEFT_SUPER] 	|| io->KeysDown[GLFW_KEY_RIGHT_SUPER];
	}

	void Application::onMouseMoved(math::vec2 pos, math::vec2 dir) {
		io->MousePos = ImVec2(pos.x, pos.y);
	}

	void Application::onMousePressed(int button, int modifier) {
		if(button < IM_ARRAYSIZE(io->MouseDown))
			io->MouseDown[button] = true;
	}

	void Application::onMouseReleased(int button, int modifier) {
		if(button < IM_ARRAYSIZE(io->MouseDown))
			io->MouseDown[button] = false;
	}

	void Application::onScrollEvent(math::vec2 dir) {
		io->MouseWheelH += dir.x;
		io->MouseWheel += dir.y;
	}

	void Application::onFramebufferResized(math::ivec2 size) {
		math::ivec2 windowSize = getWindowSize();
		math::ivec2 frameBufferSize = getFramebufferSize();
		math::vec2 displayScale = math::vec2(frameBufferSize) / math::vec2(windowSize);
		io->DisplaySize = ImVec2(windowSize.x, windowSize.y);
		if(windowSize.x > 0 && windowSize.y > 0) {
			io->DisplayFramebufferScale = ImVec2(displayScale.x, displayScale.y);
		}

		opengl::Application::onFramebufferResized(size);
	}

	void Application::initImgui() {
		ImGui::CreateContext();
		io = &ImGui::GetIO();
		style = &ImGui::GetStyle();
		main_viewport = ImGui::GetMainViewport();
		(void)(*io);
		io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();
		if(io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style->WindowRounding = 0.0f;
			style->Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL(getHandle(), false);
		ImGui_ImplOpenGL3_Init("#version 430 core");
		onFramebufferResized(getFramebufferSize());


		io->BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io->BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
		io->BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
#if GLFW_HAS_MOUSE_PASSTHROUGH || (GLFW_HAS_WINDOW_HOVERED && defined(_WIN32))
		io->BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can set io.MouseHoveredViewport correctly (optional, not easy)
#endif
		io->BackendPlatformName = "imgui_impl_glfw_object_oriented";

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
		io->KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io->KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io->KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io->KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io->KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io->KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io->KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io->KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io->KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io->KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io->KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io->KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io->KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io->KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io->KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io->KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
		io->KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io->KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io->KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io->KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io->KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io->KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		io->SetClipboardTextFn = [](void *that, const char *data){ reinterpret_cast<Application*>(that)->setClipboardText(data); };
		io->GetClipboardTextFn = [](void *that) -> const char * { return reinterpret_cast<Application*>(that)->getClipboardText(); };
		io->ClipboardUserData = this;

		g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
		g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
		g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
	}

	void Application::newFrame() {
		ImGui_ImplOpenGL3_NewFrame();

		// update cursor
		if (!(io->ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) && getInputMode(GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
			ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
			ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
			for (int n = 0; n < platform_io.Viewports.Size; n++) {
				GLFWwindow* window = (GLFWwindow*)platform_io.Viewports[n]->PlatformHandle;
				if (imgui_cursor == ImGuiMouseCursor_None || io->MouseDrawCursor) {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				}
				else {
					glfwSetCursor(window, g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
			}
		}

		ImGui::NewFrame();
	}
}