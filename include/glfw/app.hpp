#include "window.hpp"

namespace glfw {
	// Window + event loop
	class Application : public Window {
	public:
		Application(int width, int height, std::string title);
		~Application();

		virtual int exec();
		inline int run() { return this->exec(); }
	};
}