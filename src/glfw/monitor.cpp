#include <glfw/monitor.hpp>

namespace glfw {
	std::vector<Monitor> Monitor::getMonitors() {
		int count = 0;
		GLFWmonitor **list = glfwGetMonitors(&count);
		return std::vector<Monitor>(list, list + count);
	}

	Monitor Monitor::getPrimaryMonitor() {
		GLFWmonitor *primary = glfwGetPrimaryMonitor();
		const GLFWvidmode *vidmode = glfwGetVideoMode(primary);
		Monitor monitor(primary);
		monitor.setMonitorRefreshRate(vidmode->refreshRate);
		return monitor;
	}

	void Monitor::onMonitorUpdated() {}

	Monitor Monitor::createFullscreenMonitor() {
		Monitor monitor = getPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(monitor.handle);
		monitor.setWindowSizeCache(math::ivec2(mode->width, mode->height));
		monitor.setWindowPosCache(math::ivec2(0, 0));
		monitor.setMonitorRefreshRate(144);
		return monitor;
	}

	Monitor::Monitor(GLFWmonitor *monitor) : handle(monitor) {}

	Monitor::Monitor(const Monitor &other)
	 : handle(other.handle), windowSizeCache(other.windowSizeCache), windowPosCache(other.windowPosCache), refreshRate(other.refreshRate) {}

	Monitor& Monitor::operator=(const Monitor &other) {
		handle = other.handle;
		windowSizeCache = other.windowSizeCache;
		windowPosCache = other.windowPosCache;
		refreshRate = other.refreshRate;
		return *this;
	}

	GLFWmonitor* Monitor::getHandle() const {
		return handle;
	}

	math::ivec2 Monitor::getMonitorPos() {
		math::ivec2 pos;
		glfwGetMonitorPos(handle, &pos.x, &pos.y);
		return pos;
	}

	math::ivec4 Monitor::getMonitorWorkArea() {
		math::ivec4 workArea;
		glfwGetMonitorWorkarea(handle, &workArea.x, &workArea.y, &workArea.z, &workArea.w);
		return workArea;
	}

	math::vec2 Monitor::getMonitorPhysicalSize() {
		math::ivec2 sizemm;
		glfwGetMonitorPhysicalSize(handle, &sizemm.x, &sizemm.y);
		return math::vec2(sizemm) / 1000;
	}

	math::vec2 Monitor::getMonitorContentScale() {
		math::vec2 scale;
		glfwGetMonitorContentScale(handle, &scale.x, &scale.y);
		return scale;
	}

	std::string Monitor::getMonitorName() {
		return glfwGetMonitorName(handle);
	}

	void Monitor::setGamma(float gamma) {
		glfwSetGamma(handle, gamma);
	}

	void Monitor::setWindowSizeCache(math::ivec2 size) {
		windowSizeCache = size;
	}

	math::ivec2 Monitor::getWindowSizeCache() const {
		return windowSizeCache;
	}

	void Monitor::setWindowPosCache(math::ivec2 pos) {
		windowPosCache = pos;
	}

	math::ivec2 Monitor::getWindowPosCache() const {
		return windowPosCache;
	}

	void Monitor::setMonitorRefreshRate(int refreshRate) {
		this->refreshRate = refreshRate;
	}

	int Monitor::getMonitorRefreshRate() const {
		return refreshRate;
	}
}