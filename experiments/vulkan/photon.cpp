#include "photon.hpp"
#include "graviton.hpp"

Photon::Photon(math::ivec2 size, std::string title) : win(size.x, size.y, title) {
	srand(unsigned(time(NULL)));
	initVulkan();
	initImgui();
	initAngelscript();

	loadScript("main.as", "main", true);
	cam.init(engine->GetModule("main"));
	gui.init(engine->GetModule("main"));

	initImguiFonts();

	for(int i = 0; i < GLFW_KEY_LAST; i++) keys[i] = false;
	keycontrols["forward"] = GLFW_KEY_W;
	keycontrols["backward"] = GLFW_KEY_S;
	keycontrols["left"] = GLFW_KEY_A;
	keycontrols["right"] = GLFW_KEY_D;
	keycontrols["up"] = GLFW_KEY_SPACE;
	keycontrols["down"] = GLFW_KEY_LEFT_SHIFT;
	keycontrols["aim"] = GLFW_KEY_LEFT_CONTROL;
}

Photon::~Photon() {
	cleanImgui();
	cam.deinit();
	gui.deinit();
	contextManager.AbortAll();
	engine->ShutDownAndRelease();
}

int Photon::run() {
	win.setEventCallback([this](Event *event){this->handle(event);});
	setMouseVisible(true);

	bool quit = false;
	std::thread scriptThread = std::thread([this, &quit]() {
		while(!quit) {
			contextManager.ExecuteScripts();
			std::this_thread::sleep_for(10ms);
		}
		asThreadCleanup();
	});
	while(!win.shouldClose()) {
		win.pollEvents();
		static float t = glfwGetTime();
		float dt = glfwGetTime() - t;
		t = glfwGetTime();
		cam.update(dt, dmouse);
		dmouse = 0, dscroll = 0;
		math::mat4 view = cam.getViewTransform();
		math::mat4 proj = cam.getProjectionMatrix();
		render({view, proj});
	}
	quit = true;
	scriptThread.join();
	device->waitIdle();
	return 0;
}

void Photon::handle(Event *event) {
	ImGuiIO &io = ImGui::GetIO();
	dispatch<ResizeEvent>(event, [this, &io](ResizeEvent *event) {
		recreateSwapchain();
		vk::Extent2D extent = swapchain->getExtent();
		io.DisplaySize = ImVec2(extent.width, extent.height);
		io.DisplayFramebufferScale = ImVec2(1, 1);
	});
	dispatch<KeyPressEvent>(event, [this, &io](KeyPressEvent *event){
		if(event->key == GLFW_KEY_F1) {
			setMouseVisible(!getMouseVisible());
		}
	});
	if(getMouseVisible()) {	// interact with the ui
		dispatch<MouseMoveEvent>(event, [this, &io](MouseMoveEvent *event){
			io.MousePos = ImVec2(event->pos.x, event->pos.y);
		});
		dispatch<MousePressEvent>(event, [this, &io](MousePressEvent *event){
			io.MouseDown[event->button] = true;
		});
		dispatch<MouseReleaseEvent>(event, [this, &io](MouseReleaseEvent *event){
			io.MouseDown[event->button] = false;
		});
		dispatch<ScrollEvent>(event, [this, &io](ScrollEvent *event){
			io.MouseWheel = event->dir.y;
			io.MouseWheelH = event->dir.x;
		});
		dispatch<KeyPressEvent>(event, [this, &io](KeyPressEvent *event){
			io.KeysDown[event->key] = true;
			io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
			io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
			io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
			io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
		});
		dispatch<KeyReleaseEvent>(event, [this, &io](KeyReleaseEvent *event){
			io.KeysDown[event->key] = false;
		});
		dispatch<KeyTypedEvent>(event, [this, &io](KeyTypedEvent *event){
			if(event->key > 0 && event->key < 0x10000){
				io.AddInputCharacter(event->key);
			}
		});
	}
	else {					// interact with scene
		dispatch<MouseMoveEvent>(event, [this](MouseMoveEvent *e){
			dmouse += e->dir;
		});
		dispatch<KeyPressEvent>(event, [this](KeyPressEvent *e){
			keys[e->key] = true;
		});
		dispatch<KeyReleaseEvent>(event, [this](KeyReleaseEvent *e){
			keys[e->key] = false;
		});
		dispatch<ScrollEvent>(event, [this](ScrollEvent *e){
			dscroll += e->dir;
		});
	}
}

float Photon::getInput(std::string name) {
	if(keycontrols.count(name)) {
		return keys[keycontrols[name]];
	}
	else if(name == "scrollx") {
		return dscroll.x;
	}
	else if(name == "scrolly") {
		return dscroll.y;
	}
	// controller input
	return 0.0f;
}

void Photon::renderUI() {
	gui.render();
	static ImNodes::CanvasState canvas;

	ImGui::Begin("ImNodes");
	ImNodes::BeginCanvas(&canvas);
    struct Node
    {
        ImVec2 pos{};
        bool selected{};
        ImNodes::Ez::SlotInfo inputs[1];
        ImNodes::Ez::SlotInfo outputs[1];
    };

    static Node nodes[3] = {
        {{50, 100}, false, {{"In", 1}}, {{"Out", 1}}},
        {{250, 50}, false, {{"In", 1}}, {{"Out", 1}}},
        {{250, 100}, false, {{"In", 1}}, {{"Out", 1}}},
    };

    for (Node& node : nodes)
    {
        if (ImNodes::Ez::BeginNode(&node, "Node Title", &node.pos, &node.selected))
        {
            ImNodes::Ez::InputSlots(node.inputs, 1);
        	ImGui::Text("Content");
            ImNodes::Ez::OutputSlots(node.outputs, 1);
            ImNodes::Ez::EndNode();
        }
    }

    ImNodes::Connection(&nodes[1], "In", &nodes[0], "Out");
    ImNodes::Connection(&nodes[2], "In", &nodes[0], "Out");
	ImNodes::EndCanvas();
	ImGui::End();
}

void Photon::setMouseVisible(bool visible) {
	win.setMouse(visible);
	mouseVisible = visible;
}
bool Photon::getMouseVisible() {
	return mouseVisible;
}

int main(int argc, char *argv[]) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	Photon engine(math::ivec2(1080, 720), "Vulkan");
	math::quaternion q(3.141592654, math::vec3(0, 1, 0));
	return engine.run();
}