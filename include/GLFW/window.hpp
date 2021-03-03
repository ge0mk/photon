#pragma once

#include <string>
#include <functional>
#include <cassert>

#include <glad/glad.h>
#include <math/vector.hpp>
#include <spdlog/spdlog.h>
#include <photonvk/instance.hpp>
#include "glfw3.h"
#include "monitor.hpp"

namespace glfw {
	void init();
	void cleanup();

	class Window {
	public:
		static void defaultWindowHints();
		static void setWindowHint(int hint, int value);
		static void setWindowHintString(int hint, const std::string &value);
		static void pollEvents();
		static void waitEvents(double timeout = -1);
		static void postEmptyEvent();


		Window(int width, int height, std::string title);
		~Window();
		GLFWwindow* getHandle();

		void makeContextCurrent();
		void swapBuffers();

		bool windowShouldClose();
		void setWindowShouldClose(bool value = true);
		void setWindowTitle(const std::string &title);
		math::ivec2 getWindowPos();
		void setWindowPos(math::ivec2 pos);
		math::ivec2 getWindowSize();
		void setWindowSizeLimits(math::ivec2 min, math::ivec2 max);
		void setWindowSize(math::ivec2 size);
		float getAspectRatio();
		math::ivec2 getFramebufferSize();
		math::ivec4 getWindowFrameSize();
		math::vec2 getWindowContentScale();
		float getWindowOpacity();
		void setWindowOpacity(float opacity);
		void minimizeWindow();
		void restoreWindow();
		void maximizeWindow();
		void showWindow();
		void hideWindow();
		void focusWindow();
		void requestWindowAttention();
		Monitor getWindowMonitor();
		void setWindowMonitor(const Monitor &monitor);
		int getWindowAttrib(int attrib);
		void setWindowAttrib(int attrib, int value);

		void setMouse(bool visible);
		bool isFullscreen();
		void setFullscreen(bool fullscreen);
		void toggleFullscreen();
		void setClipboardText(const char *data);
		const char* getClipboardText();
		int getInputMode(int mode);
		void setInputMode(int mode, int value);
		int getKey(int key);
		math::vec2 getCursorPos();
		int getMouseButton(int button);

		std::vector<const char*> getRequiredExtensions();
		photonvk::Surface createSurface(const photonvk::Instance &instance);

	protected:
		virtual void onKeyTyped(int key);
		virtual void onKeyChanged(int key, int scancode, int modifier, int action);
		virtual void onKeyPressed(int key, int scancode, int modifier, bool repeat);
		virtual void onKeyReleased(int key, int scancode, int modifier);
		virtual void onMouseMoved(math::vec2 pos, math::vec2 dir);
		virtual void onMousePressed(int button, int modifier);
		virtual void onMouseReleased(int button, int modifier);
		virtual void onScrollEvent(math::vec2 dir);
		virtual void onWindowMoved(math::ivec2 pos);
		virtual void onWindowContentScaleChanged(math::vec2 scale);
		virtual void onWindowResized(math::ivec2 size);
		virtual void onWindowMinimized(bool minimized);
		virtual void onWindowMaximized(bool maximized);
		virtual void onWindowFocusChanged(bool focussed);
		virtual void onWindowNeedsRefresh();
		virtual void onWindowClosed();
		virtual void onFramebufferResized(math::ivec2 size);
		virtual void onMonitorUpdated(GLFWmonitor *monitor, int event);

		GLFWwindow *handle = nullptr;

	private:
		void initCallbacks();
		// fullscreen
		std::array<Monitor, 2> windowModes;
		bool fullscreen = false;
		math::vec2 cursor;
	};
}