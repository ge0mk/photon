#include <photon.hpp>
#include <iostream>
#include <fstream>

Engine::Engine()
	  : imgui::Application(1080, 720, "Application"),
		cameraInfoUBO(opengl::Buffer<CameraInfo>::Uniform),
		frameInfoUBO(opengl::Buffer<FrameInfo>::Uniform),
		lightInfoUBO(opengl::Buffer<LightInfo>::Uniform),
		modelInfoUBO(opengl::Buffer<ModelInfo>::Uniform),
		fb(math::ivec2(1080, 720)) {
	// setup opengl flags
	glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_STENCIL_TEST);

	pFont = io->Fonts->AddFontFromFileTTF("res/jetbrains-mono.ttf", 32);
	pFont->Scale = 0.75;

	lights = std::vector<LightInfo>({
		{math::vec4(5, -5, 5), math::vec4(), math::vec4(0.3)}
	});

	prog = loadProgram("res/main.glsl");
	crosshair = loadProgram("res/crosshair.glsl");
	skybox = loadProgram("res/skybox.glsl");

	cameraInfoUBO.initEmpty(opengl::Buffer<CameraInfo>::DynamicDraw);
	cameraInfoUBO.bindBase(0);

	modelInfoUBO.initEmpty(opengl::Buffer<ModelInfo>::DynamicDraw);
	modelInfoUBO.bindBase(1);

	frameInfoUBO.initEmpty(opengl::Buffer<FrameInfo>::DynamicDraw);
	frameInfoUBO.bindBase(2);

	lightInfoUBO.setData(lights, opengl::Buffer<LightInfo>::DynamicDraw);
	lightInfoUBO.bindBase(3);

	cubeMesh = new Mesh(generateCube(8));
	planeMesh = new Mesh(generatePlane());
	sphereMesh = new Mesh(generateSphere(32, math::vec4(1.0, 1.0, 1.0, 0.0)));

	HDRImage img("res/cape_hill_8k.hdr");
	skyboxTexture.load(img);
	skyboxTexture.bind();
	skyboxTexture.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP);

	Image img2("res/rough_block_wall_diff_2k.png");
	cubeTexture.load(img2);

	Image img3("res/2k_mars.jpg");
	sphereTexture.load(img3);

	fb.bind();
	fb.addAttachment(std::move(opengl::Texture(GL_RGBA, getFramebufferSize(), GL_UNSIGNED_BYTE)), GL_COLOR_ATTACHMENT0);
	fb.addAttachment(std::move(opengl::RenderBuffer(GL_DEPTH24_STENCIL8, getFramebufferSize())), GL_DEPTH_STENCIL_ATTACHMENT);
	if(fb.checkStatus() != GL_FRAMEBUFFER_COMPLETE) {
		spdlog::error("framebuffer incomplete!");
	}
	fb.unbind();

	onFramebufferResized(getFramebufferSize());

	auto sphere = registry.create();
	registry.emplace<MeshComponent>(sphere, sphereMesh);
	registry.emplace<TextureComponent>(sphere, &sphereTexture);
	registry.emplace<TransformComponent>(sphere, math::vec3(0, 1, 0), math::vec3(0, 0, 0), math::vec3(1, 1, 1));

	auto cube = registry.create();
	registry.emplace<MeshComponent>(cube, cubeMesh);
	registry.emplace<TextureComponent>(cube, &cubeTexture);
	registry.emplace<TransformComponent>(cube, math::vec3(0, -1, 0), math::vec3(0, 0, 0), math::vec3(1, 1, 1));
}

Engine::~Engine() {
	delete cubeMesh;
	delete planeMesh;
	delete sphereMesh;
}

void Engine::update() {
	frame.time = glfwGetTime();
	frame.dt = dt;

	// update player
	float dx = getKey(GLFW_KEY_D) - getKey(GLFW_KEY_A);
	float dy = getKey(GLFW_KEY_SPACE) - getKey(GLFW_KEY_LEFT_CONTROL);
	float dz = getKey(GLFW_KEY_W) - getKey(GLFW_KEY_S);
	math::vec3 dir = math::mat4().rotate(math::vec3(0, cam.rot.y, 0)) * math::vec4(dx, dy, dz, 0);
	if(math::length(dir) > 0.0f)
		cam.pos += math::normalize(dir) * dt * 3.0f;

	// update camera
	cam.ubo.view = math::mat4().translate(cam.pos).rotate(cam.rot).inverse();
	cam.ubo.proj = math::mat4::projection(math::d2r(cam.fov), cam.aspect, cam.znear, cam.zfar);
	cameraInfoUBO.update(cam.ubo);

	frameInfoUBO.update(frame);
}

void Engine::render() {
	fb.bind();
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderSkybox();

	cameraInfoUBO.update(cam.ubo);

	prog.use();
	float tmp1 = frame.gamma, tmp2 = frame.ambient;
	frame.gamma = -1;

	if(wireframe) {
		frame.ambient = 0;
		frameInfoUBO.update(frame);
		frame.ambient = tmp2;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		lightInfoUBO.setData(std::vector<LightInfo>{{}}, opengl::Buffer<LightInfo>::DynamicDraw);

		renderScene();

		lightInfoUBO.setData(lights, opengl::Buffer<LightInfo>::DynamicDraw);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	frameInfoUBO.update(frame);
	frame.gamma = tmp1;

	renderScene();

	fb.unbind();

	glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	crosshair.use();
	cameraInfoUBO.update({math::mat4(), math::mat4()});

	opengl::Texture &texture2 = fb.get<opengl::Texture>(GL_COLOR_ATTACHMENT0);
	texture2.activate();
	texture2.bind();

	planeMesh->drawElements();
}

void Engine::renderScene() {
	auto view = registry.view<MeshComponent>();
	for(auto entity : view) {
		math::mat4 transform, uvtransform;

		if(registry.has<TransformComponent>(entity)) {
			auto t = registry.get<TransformComponent>(entity);
			transform = math::mat4().translate(t.pos).rotate(t.rot).scale(t.scale);
		}

		if(registry.has<TextureComponent>(entity)) {
			auto texture = registry.get<TextureComponent>(entity);
			texture->bind();
			texture->activate();
		}
		modelInfoUBO.update({transform, uvtransform});

		auto mesh = registry.get<MeshComponent>(entity);
		mesh->drawElements();

		if(registry.has<TextureComponent>(entity)) {
			auto texture = registry.get<TextureComponent>(entity);
			texture->unbind();
		}
	}
}

void Engine::renderSkybox() {
	modelInfoUBO.update({math::mat4(), math::mat4()});
	cameraInfoUBO.update({
		math::mat4().rotate(cam.rot).inverse(),
		math::mat4::projection(math::d2r(cam.fov), cam.aspect, cam.znear, cam.zfar)
	});

	glCullFace(GL_FRONT);
	glDisable(GL_DEPTH_TEST);
	skybox.use();
	frameInfoUBO.update(frame);
	skyboxTexture.activate();
	skyboxTexture.bind();

	cubeMesh->drawElements();

	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

void Engine::renderUI() {
	// render HUD (no user input)
	ImGuiWindowFlags hudWindowFlags = ImGuiWindowFlags_NoDocking;
	hudWindowFlags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	hudWindowFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
	hudWindowFlags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar;
	hudWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::PushFont(pFont);
	if(ImGui::Begin("HUD-root", nullptr, hudWindowFlags)) {
		ImGui::Text("pos: %f, %f, %f", cam.pos.x, cam.pos.y, cam.pos.z);
		ImGui::Text("rot: %f, %f", cam.rot.x, cam.rot.y);
		ImGui::Text("FPS: %f, dt: %f", 1.0 / dt, dt);
		ImGui::Text("time: %f", glfwGetTime());
	} ImGui::End();
	ImGui::PopStyleVar(2);
	ImGui::PopFont();

	// render additional ui that requires input (inventory / debug stuff / ...)
	if(guiVisible) {
		ImGui::Begin("Settings");
			ImGui::Checkbox("wireframe", &wireframe);
			ImGui::SliderFloat("FOV", &cam.fov, 1.0f, 179.0f, "%1.0f");
			ImGui::SliderFloat("gamma", &frame.gamma, 0.0f, 10.0f);
			ImGui::SliderFloat("ambient light", &frame.ambient, 0.0f, 1.0f);
			//opengl::Texture &texture = fb.get<opengl::Texture>(GL_COLOR_ATTACHMENT0);
			//texture.bind();
			//ImGui::Image((void*)(intptr_t)texture.getHandle(), ImVec2(texture.size().x, texture.size().y));
		ImGui::End();
	}
}

// forward input events to imgui if the ui is used, else ignore them
void Engine::onKeyPressed(int key, int scancode, int modifier, bool repeat) {
	if(key == GLFW_KEY_F1 && repeat == false) {
		guiVisible = !guiVisible;

		Window::setInputMode(GLFW_CURSOR, guiVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
	else if(key == GLFW_KEY_F11 && repeat == false) {
		Window::toggleFullscreen();
	}
	else if(key == GLFW_KEY_F12 && repeat == false) {
		math::ivec2 fbsize = getFramebufferSize();
		std::vector<uint8_t> data(fbsize.x * fbsize.y * sizeof(float) * 3);
		glReadPixels(0, 0, fbsize.x, fbsize.y, GL_RGB, GL_UNSIGNED_BYTE, data.data());
		Image img(data, fbsize, 3);
		stbi_flip_vertically_on_write(true);
		img.save("screenshot.png");
	}
	else if(guiVisible) {
		imgui::Application::onKeyPressed(key, scancode, modifier, repeat);
	}
}

void Engine::onKeyTyped(int key) {
	if(guiVisible) {
		imgui::Application::onKeyTyped(key);
	}
}

void Engine::onMouseMoved(math::vec2 pos, math::vec2 dir) {
	if(guiVisible) {
		imgui::Application::onMouseMoved(pos, dir);
	}
	else {
		cam.rot -= math::vec2(dir.y, dir.x) * 0.0025;
		cam.rot.x = math::clamp(cam.rot.x, -3.141592654f / 2, 3.141592654f / 2);
	}
}

void Engine::onScrollEvent(math::vec2 dir) {
	cam.fov += dir.y;
	cam.fov = math::clamp(cam.fov, 1.0f, 179.0f);
}

void Engine::onFramebufferResized(math::ivec2 size) {
	glViewport(0, 0, size.x, size.y);
	cam.res = size;
	cam.aspect = cam.res.x / cam.res.y;
	imgui::Application::onFramebufferResized(size);
	fb.resize(size);
	frame.res = size;
}

void Engine::onWindowFocusChanged(bool focus) {
	if(focus && !guiVisible) {
		Window::setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		Window::setInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

opengl::Program Engine::loadProgram(const std::string &path) {
	std::ifstream src(path, std::ios::ate);
	std::string buffer(src.tellg(), '\0');
	src.seekg(src.beg);
	src.read(buffer.data(), buffer.size());

	std::cout<<"loading "<<path<<"\n";
	opengl::Program prog = opengl::Program::load(buffer, opengl::Shader::VertexStage | opengl::Shader::FragmentStage);
	prog.use();
	prog.setUniform("sampler", 0);

	return std::move(prog);
}
