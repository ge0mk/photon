#include <glfw/window.hpp>
#include <stdexcept>

namespace glfw {
	void init() {
		assert(glfwInit());
		glfwSetErrorCallback([](int error, const char* description){
			if(error != 65537){
				std::cerr<<"("<<error<<")"<<description<<std::endl;
			}
		});
	}

	void cleanup() {
		glfwTerminate();
	}

	void Window::defaultWindowHints() {
		glfwDefaultWindowHints();
	}

	void Window::setWindowHint(int hint, int value) {
		glfwWindowHint(hint, value);
	}

	void Window::setWindowHintString(int hint, const std::string &value) {
		glfwWindowHintString(hint, value.c_str());
	}

	void Window::pollEvents() {
		glfwPollEvents();
	}

	void Window::waitEvents(double timeout) {
		if(timeout >= 0) {
			glfwWaitEventsTimeout(timeout);
		}
		else {
			glfwWaitEvents();
		}
	}

	void Window::postEmptyEvent() {
		glfwPostEmptyEvent();
	}

	Window::Window(math::ivec2 size, const std::string &title) {
		handle = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
		if(handle == nullptr) {
			throw std::runtime_error("couldn't create window!");
		}
		glfwSetWindowUserPointer(handle, this);
		pollEvents();
		cursor = getCursorPos();
		windowModes = {getWindowMonitor(), Monitor::createFullscreenMonitor()};
		initCallbacks();
	}

	Window::~Window() {
		glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwDestroyWindow(handle);
	}

	int Window::exec() {
		while(!windowShouldClose()) {
			pollEvents();
		}
		return 0;
	}

	GLFWwindow* Window::getHandle() {
		return handle;
	}

	void Window::makeContextCurrent() {
		std::scoped_lock<std::mutex> lock(mutex);
		glfwMakeContextCurrent(handle);
	}

	void Window::swapBuffers() {
		std::scoped_lock<std::mutex> lock(mutex);
		glfwSwapBuffers(handle);
	}

	bool Window::windowShouldClose() {
		std::scoped_lock<std::mutex> lock(mutex);
		return glfwWindowShouldClose(handle);
	}

	void Window::setWindowShouldClose(bool value) {
		std::scoped_lock<std::mutex> lock(mutex);
		glfwSetWindowShouldClose(handle, value);
	}

	void Window::setWindowTitle(const std::string &title) {
		glfwSetWindowTitle(handle, title.c_str());
	}

	void Window::setWindowIcon(const Image &icon) {
		GLFWimage img = icon.getGLFWImage();
		glfwSetWindowIcon(handle, 1, &img);
	}

	void Window::resetWindowIcon() {
		glfwSetWindowIcon(handle, 0, nullptr);
	}

	math::ivec2 Window::getWindowPos() {
		math::ivec2 pos;
		glfwGetWindowPos(handle, &pos.x, &pos.y);
		return pos;
	}

	void Window::setWindowPos(math::ivec2 pos) {
		glfwSetWindowPos(handle, pos.x, pos.y);
	}

	math::ivec2 Window::getWindowSize() {
		math::ivec2 size;
		glfwGetWindowSize(handle, &size.x, &size.y);
		return size;
	}

	void Window::setWindowSizeLimits(math::ivec2 min, math::ivec2 max) {
		glfwSetWindowSizeLimits(handle, min.x, min.y, max.x, max.y);
	}

	void Window::setWindowSize(math::ivec2 size) {
		glfwSetWindowSize(handle, size.x, size.y);
	}

	float Window::getAspectRatio() {
		math::ivec2 size = getFramebufferSize();
		return float(size.x) / float(size.y);
	}

	math::ivec2 Window::getFramebufferSize() {
		math::ivec2 size;
		glfwGetFramebufferSize(handle, &size.x, &size.y);
		return size;
	}

	math::ivec4 Window::getWindowFrameSize() {
		math::ivec4 size;
		glfwGetWindowFrameSize(handle, &size.x, &size.y, &size.z, &size.w);
		return size;
	}

	math::vec2 Window::getWindowContentScale() {
		math::vec2 scale;
		glfwGetWindowContentScale(handle, &scale.x, &scale.y);
		return scale;
	}

	float Window::getWindowOpacity() {
		return glfwGetWindowOpacity(handle);
	}

	void Window::setWindowOpacity(float opacity) {
		glfwSetWindowOpacity(handle, opacity);
	}

	void Window::minimizeWindow() {
		glfwIconifyWindow(handle);
	}

	void Window::restoreWindow() {
		glfwRestoreWindow(handle);
	}

	void Window::maximizeWindow() {
		glfwMaximizeWindow(handle);
	}

	void Window::showWindow() {
		glfwShowWindow(handle);
	}

	void Window::hideWindow() {
		glfwHideWindow(handle);
	}

	void Window::focusWindow() {
		glfwFocusWindow(handle);
	}

	void Window::requestWindowAttention() {
		glfwRequestWindowAttention(handle);
	}

	Monitor Window::getWindowMonitor() {
		Monitor m = glfwGetWindowMonitor(handle);
		m.setWindowSizeCache(getWindowSize());
		m.setWindowPosCache(getWindowPos());
		return m;
	}

	void Window::setWindowMonitor(const Monitor &monitor) {
		math::ivec2 size = monitor.getWindowSizeCache();
		math::ivec2 pos = monitor.getWindowPosCache();
		spdlog::info("refresh rate: {}", monitor.getMonitorRefreshRate());
		glfwSetWindowMonitor(handle, monitor.getHandle(), pos.x, pos.y, size.x, size.y, monitor.getMonitorRefreshRate());
	}

	void Window::setWindowAttrib(int attrib, int value) {
		glfwSetWindowAttrib(handle, attrib, value);
	}

	int Window::getWindowAttrib(int attrib) {
		return glfwGetWindowAttrib(handle, attrib);
	}

	void Window::setMouse(bool visible) {
		glfwSetInputMode(handle, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

	bool Window::isFullscreen() {
		return getWindowMonitor().getHandle() != nullptr;
	}

	void Window::setFullscreen(bool fullscreen) {
		this->fullscreen = fullscreen;
		if(fullscreen) {
			windowModes[0] = getWindowMonitor();
		}
		setWindowMonitor(windowModes[fullscreen]);
	}

	void Window::toggleFullscreen() {
		fullscreen = !fullscreen;
		if(fullscreen) {
			windowModes[0] = getWindowMonitor();
		}
		setWindowMonitor(windowModes[fullscreen]);
	}

	void Window::setClipboardText(const char *data) {
		glfwSetClipboardString(handle, data);
	}

	const char* Window::getClipboardText() {
		return glfwGetClipboardString(handle);
	}

	int Window::getInputMode(int mode) {
		return glfwGetInputMode(handle, mode);
	}

	void Window::setInputMode(int mode, int value) {
		glfwSetInputMode(handle, mode, value);
	}

	int Window::getKey(int key) {
		return glfwGetKey(handle, key);
	}

	math::vec2 Window::getCursorPos() {
		double x, y;
		glfwGetCursorPos(handle, &x, &y);
		return math::vec2(x, y);
	}

	int Window::getMouseButton(int button) {
		return glfwGetMouseButton(handle, button);
	}

	void Window::onKeyTyped([[maybe_unused]] int key) {}
	void Window::onKeyChanged([[maybe_unused]] int key, [[maybe_unused]] int scancode, [[maybe_unused]] int modifier, [[maybe_unused]] int action) {}
	void Window::onKeyPressed([[maybe_unused]] int key, [[maybe_unused]] int scancode, [[maybe_unused]] int modifier, [[maybe_unused]] bool repeat) {}
	void Window::onKeyReleased([[maybe_unused]] int key, [[maybe_unused]] int scancode, [[maybe_unused]] int modifier) {}
	void Window::onMouseMoved([[maybe_unused]] math::vec2 pos, [[maybe_unused]] math::vec2 dir) {}
	void Window::onMousePressed([[maybe_unused]] int button, [[maybe_unused]] int modifier) {}
	void Window::onMouseReleased([[maybe_unused]] int button, [[maybe_unused]] int modifier) {}
	void Window::onScrollEvent([[maybe_unused]] math::vec2 dir) {}
	void Window::onWindowMoved([[maybe_unused]] math::ivec2 pos) {}
	void Window::onWindowContentScaleChanged([[maybe_unused]] math::vec2 scale) {}
	void Window::onWindowResized([[maybe_unused]] math::ivec2 size) {}
	void Window::onWindowMinimized([[maybe_unused]] bool minimized) {}
	void Window::onWindowMaximized([[maybe_unused]] bool maximized) {}
	void Window::onWindowFocusChanged([[maybe_unused]] bool focussed) {}
	void Window::onWindowNeedsRefresh() {}
	void Window::onWindowClosed() {}
	void Window::onFramebufferResized([[maybe_unused]] math::ivec2 size) {}
	void Window::onMonitorUpdated([[maybe_unused]] GLFWmonitor *monitor, [[maybe_unused]] int event) {}

	void Window::initCallbacks() {
		glfwSetKeyCallback(handle, [](GLFWwindow *handle, int key, int scancode, int action, int modifiers) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onKeyChanged(key, scancode, modifiers, action);
			switch (action)	{
				case GLFW_PRESS: {
					obj->onKeyPressed(key, scancode, modifiers, false);
				} break;
				case GLFW_RELEASE: {
					obj->onKeyReleased(key, scancode, modifiers);
				} break;
				case GLFW_REPEAT: {
					obj->onKeyPressed(key, scancode, modifiers, true);
				} break;
			}
		});
		glfwSetCharCallback(handle, [](GLFWwindow *handle, unsigned int c) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onKeyTyped(c);
		});
		glfwSetCursorPosCallback(handle, [](GLFWwindow *handle, double x, double y) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			math::vec2 pos = math::vec2(x, y);
			math::vec2 dir = obj->cursor - pos;
			obj->cursor = pos;
			obj->onMouseMoved(pos, dir);
		});
		glfwSetMouseButtonCallback(handle, [](GLFWwindow *handle, int button, int action, int modifier) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			switch (action)	{
				case GLFW_PRESS: {
					obj->onMousePressed(button, modifier);
				} break;
				case GLFW_RELEASE: {
					obj->onMouseReleased(button, modifier);
				} break;
			}
		});
		glfwSetScrollCallback(handle, [](GLFWwindow *handle, double dx, double dy) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onScrollEvent(math::vec2(dx, dy));
		});
		glfwSetWindowPosCallback(handle, [](GLFWwindow *handle, int x, int y) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onWindowMoved(math::ivec2(x, y));
		});
		glfwSetWindowContentScaleCallback(handle, [](GLFWwindow *handle, float sx, float sy) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onWindowContentScaleChanged(math::vec2(sx, sy));
		});
		glfwSetWindowSizeCallback(handle, [](GLFWwindow *handle, int width, int height) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onWindowResized(math::ivec2(width, height));
		});
		glfwSetWindowIconifyCallback(handle, [](GLFWwindow *handle, int minimized) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onWindowMinimized(minimized);
		});
		glfwSetWindowMaximizeCallback(handle, [](GLFWwindow *handle, int maximized) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onWindowMaximized(maximized);
		});
		glfwSetWindowFocusCallback(handle, [](GLFWwindow *handle, int focussed) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onWindowFocusChanged(focussed);
		});
		glfwSetWindowRefreshCallback(handle, [](GLFWwindow *handle) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onWindowNeedsRefresh();
		});
		glfwSetWindowCloseCallback(handle, [](GLFWwindow *handle) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onWindowClosed();
		});
		glfwSetFramebufferSizeCallback(handle, [](GLFWwindow *handle, int width, int height) {
			Window* obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
			obj->onFramebufferResized(math::ivec2(width, height));
		});
	}
}