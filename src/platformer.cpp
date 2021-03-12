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

	//initialising glfw -> User Input and  Window handling
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

	//glad -> dealing with graphics card
	if (!gladLoadGL()) //abweichung durch neuere OpenGL version!!!
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//glViewport(0, 0, 800, 600);

	//GL functions after Glad!!!!!!!!!
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //auto Viewport anpassung an Fenster Res

	//creating vertex shader for dealing with coordinates
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);//create vertex shader
	//vertexshadersource is code in opengl-language
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //vertexshader soll mit dem String compiliert werden
	glCompileShader(vertexShader); //compile  auftrag
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
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	//checking correct compilation
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}//out compile error of fragment shader


	//start shader programm to link created shaders, putting input and output (of each shader) together
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram(); //create shaderprogram

	//add vertex and fragment shader and link all shaders together in program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, vertexShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::Program::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//Smoother Attempt

	//start of vertex shader (position and transform of coordinates - vertices)
	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	//generating VBO and VAO
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO); //bind vao
	//now call vbo's and attr pointer -> unbind vao later

	//Bind VAO like VBO, just to use the functions for all VBO's stored in VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	//Loop contains rendercalls for every frame! -> put rendering triangle here!
	while (!glfwWindowShouldClose(window)) //Fenster zeigen, bis geschlossen wird, sonst nur für ein Frame geöffnet
	{
		//input
		processInput(window); //funktion checkt jeden frame, ob taste gedrückt wurde

		//commands for rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //new floats for standard glclear function
		glClear(GL_COLOR_BUFFER_BIT);

		//call here the VAO and what to do with it
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//for later drawing just bind the VAO with needed attributes
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//output and eventlistener
		glfwSwapBuffers(window); //output
		glfwPollEvents(); //checks inputs/events and generates new window state, calls needed funktions
	}

	//delete now all shaders, to free resources, at end of execution
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);


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