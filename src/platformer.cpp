#include <glfw/window.hpp>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = R"(#version 330 core
out vec4 FragColor;
void main()
{
	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
})"; //ganzer String, ohne lästiges zeug

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);



int main(int argc, const char *argv[]) {	

	glfw::init();
	atexit(glfw::cleanup);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	//Fenster vor Gladloadgl erstellen!!!

	GLFWwindow* window = glfwCreateWindow(800, 600, "Hello_openGL_Window", NULL, NULL); //Null Null ist Monitor und anderes Fenster im Zusammenhang (im opengl kontext)
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	if (!gladLoadGL()) //abweichung durch neuere OpenGL version!!!
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//glViewport(0, 0, 800, 600);

	//void framebuffer_size_callback(GLFWwindow * window, int width, int height);


	//GL functions nach Glad!!!!!!!!!

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //auto Viewport anpassung an Fenster Res

	while (!glfwWindowShouldClose(window)) //Fenster zeigen, bis geschlossen wird, sonst nur für ein Frame geöffnet
	{
		//input
		processInput(window); //funktion checkt jeden frame, ob taste gedrückt wurde

		//commands for rendering
		glClearColor(0.5f, 0.7f, 0.3f, 1.0f); //new floats for standard glclear function
		glClear(GL_COLOR_BUFFER_BIT);


		//output and eventlistener
		glfwSwapBuffers(window); //output
		glfwPollEvents(); //checks inputs/events and generates new window state, calls needed funktions
	}


	//start of vertex shader (position and transform of coordinates - vertices)

	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	unsigned int VBO; //VBO stores many Vertices in GPU Memory (Vertex Buffer Object)
	//unsigned für  null bis 2^64; signed von -2^32 bis +2^32!!!
	glGenBuffers(1, &VBO); //generates VBO with one Buffer

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //VBO and GL_ARRAY_BUFFER connection
	//changes/calls to gl_array_buffer are made for bound buffer (vbo)!

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //writes vertex data in bound buffer 
	//Buffer to save in, size of data, type of data, telling what to do with data

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER); //verstexshader erstellen

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //vertexshader soll mit dem String compiliert werden
	glCompileShader(vertexShader); //comppile  auftrag

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //check if compilation was successfull

	if (!success)//gibt fehlermeldung für nichtkompilierten shader aus
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//compiling of vertex shader finished




	//start of fragment shader (color)

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//int  success2;
	//char infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}//out compile error of fragment shader

	//start shader programm to link created shaders, putting input and output (of each shader) together

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram(); //create shaderprogram

	//add vertex and fragment shader and link all shaders together in program
	glAttachShader(vertexShader, shaderProgram);
	glAttachShader(fragmentShader, shaderProgram);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::Program::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//linking program is ready

	//run program
	glUseProgram(shaderProgram);
	//shaders after this call are ignored
	//delete now all shaders, to free resources

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*// 0. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 1. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 2. use our shader program when we want to render an object
	glUseProgram(shaderProgram);
	// 3. now draw the object 
	//someOpenGLFunctionThatDrawsOurTriangle();*/

	glfwTerminate(); //freigabe genutzter ressourcen
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { //automatischer Viewport an Fenster angepasst
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)  //checkt input einer Taste.
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}