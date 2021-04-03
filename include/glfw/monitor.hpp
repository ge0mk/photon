#pragma once

#include <vector>

#include <math/vector.hpp>

#include "glfw3.h"

namespace glfw {
	class Monitor {
	public:
		static std::vector<Monitor> getMonitors();
		static Monitor getPrimaryMonitor();
		static void onMonitorUpdated();
		static Monitor createFullscreenMonitor();

		Monitor(GLFWmonitor *monitor = nullptr);
		Monitor(const Monitor &other);

		Monitor& operator=(const Monitor &other);

		GLFWmonitor* getHandle() const;

		math::ivec2 getMonitorPos();
		math::ivec4 getMonitorWorkArea();
		math::vec2 getMonitorPhysicalSize();
		math::vec2 getMonitorContentScale();
		std::string getMonitorName();
		void setGamma(float gamma);

		void setWindowSizeCache(math::ivec2 size);
		math::ivec2 getWindowSizeCache() const;
		void setWindowPosCache(math::ivec2 pos);
		math::ivec2 getWindowPosCache() const;
		void setMonitorRefreshRate(int refreshRate);
		int getMonitorRefreshRate() const;

	private:
		GLFWmonitor *handle;
		math::ivec2 windowSizeCache, windowPosCache;
		int refreshRate = 0;
	};
}