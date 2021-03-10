#include <glfw/window.hpp>

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