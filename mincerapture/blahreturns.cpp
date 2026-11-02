#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include "stb_image.h"
#include "hsvtorgb.h" //his vetor gubuh
#include "attenuationinputs.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// call me back
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void iconify_callback(GLFWwindow* window, int iconified);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path);

// Screen Samping (Size)
int SW = 800;
int SH = 600;

// Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Camping
Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
bool firstMouse = true;

bool mouseHeld;
bool oldMouseHeld;
bool sprinting;
bool flashlight = true;

float flashoffTime = 5.0f;
float flashoffTimer = flashoffTime;

// Lamping
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SW, SH, "pop22", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window WTF ARGH ARGH ARGH" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowIconifyCallback(window, iconify_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader lightingShader("shader.vs", "shader.fsh");

	Shader lightCubeShader("light_cube.vs", "light_cube.fsh");

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	// positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};

	glm::vec3 pointLightColors[] = {
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.3f, 0.1f, 0.1f)
	};


	#pragma region "this this shit"
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

#pragma endregion

	#pragma region "texture shit"
	unsigned int diffuseMap = loadTexture("container2.png");
	unsigned int specularMap = loadTexture("container2_specular.png");
	stbi_set_flip_vertically_on_load(true);
	unsigned int emissionMap = loadTexture("wegato.png");

	#pragma endregion

	glClearColor(0.0, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		float timeValue = glfwGetTime();
		deltaTime = timeValue - lastFrame;
		lastFrame = timeValue;
		sprinting = false;

		if (glfwGetKey(window, GLFW_ICONIFIED) == GLFW_FALSE)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			processInput(window);

			if (!oldMouseHeld && mouseHeld) flashlight = !flashlight;

			flashoffTimer -= deltaTime;
			if (flashoffTimer <= 0)
			{
				flashlight = false;
				flashoffTimer += flashoffTime;
			}

			lightingShader.use();
			lightingShader.setVec3("viewPos", camera.Position);

			lightingShader.setInt("material.diffuse", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			lightingShader.setInt("material.specular", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap);
			lightingShader.setInt("material.emission", 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, emissionMap);
			lightingShader.setFloat("material.shininess", 64.0f);

			double pi = 3.14159265359;
			float dur = 3.0f;
			glm::vec3 rainbowColor = glm::normalize(hsv_to_rgb(glm::vec3(abs(sin(timeValue/dur))*360.0f, 1.0f, 1.0f)));

			glm::vec3 dirLight = glm::vec3(0.2f, 0.2f, 0.2f);
			lightingShader.setVec3("dirLight.direction", glm::vec3(0.1f, -1.0f, -0.3f));
			lightingShader.setVec3("dirLight.ambient", dirLight * 0.0f);
			lightingShader.setVec3("dirLight.diffuse", dirLight * 0.25f);
			lightingShader.setVec3("dirLight.specular", dirLight * 1.0f);

			float lightRange = 20.0f;
			glm::vec2 attenValues = get_attenuation_inputs(lightRange);
			glm::vec3 pointLightColor = glm::vec3(245.0f / 255.0f, 203.0f / 255.0f, 66.0f / 255.0f);
			lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", attenValues.x);
			lightingShader.setFloat("pointLights[0].quadratic", attenValues.y);
			lightingShader.setVec3("pointLights[0].ambient", pointLightColors[0] * glm::vec3(0.1f));
			lightingShader.setVec3("pointLights[0].diffuse", pointLightColors[0] * glm::vec3(1.0f));
			lightingShader.setVec3("pointLights[0].specular", pointLightColors[0] * glm::vec3(1.0f));

			lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
			lightingShader.setFloat("pointLights[1].constant", 1.0f);
			lightingShader.setFloat("pointLights[1].linear", attenValues.x);
			lightingShader.setFloat("pointLights[1].quadratic", attenValues.y);
			lightingShader.setVec3("pointLights[1].ambient", pointLightColors[1] * glm::vec3(0.1f));
			lightingShader.setVec3("pointLights[1].diffuse", pointLightColors[1] * glm::vec3(1.0f));
			lightingShader.setVec3("pointLights[1].specular", pointLightColors[1] * glm::vec3(1.0f));

			pointLightColor = glm::vec3(245.0f / 255.0f, 66.0f / 255.0f, 66.0f / 255.0f);
			lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
			lightingShader.setFloat("pointLights[2].constant", 1.0f);
			lightingShader.setFloat("pointLights[2].linear", attenValues.x);
			lightingShader.setFloat("pointLights[2].quadratic", attenValues.y);
			lightingShader.setVec3("pointLights[2].ambient", pointLightColors[2] * glm::vec3(0.1f));
			lightingShader.setVec3("pointLights[2].diffuse", pointLightColors[2] * glm::vec3(1.0f));
			lightingShader.setVec3("pointLights[2].specular", pointLightColors[2] * glm::vec3(1.0f));

			lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
			lightingShader.setFloat("pointLights[3].constant", 1.0f);
			lightingShader.setFloat("pointLights[3].linear", attenValues.x);
			lightingShader.setFloat("pointLights[3].quadratic", attenValues.y);
			lightingShader.setVec3("pointLights[3].ambient", pointLightColors[3] * glm::vec3(0.1f));
			lightingShader.setVec3("pointLights[3].diffuse", pointLightColors[3] * glm::vec3(1.0f));
			lightingShader.setVec3("pointLights[3].specular", pointLightColors[3] * glm::vec3(1.0f));

			lightingShader.setVec3("spotLight.position", camera.Position);
			lightingShader.setVec3("spotLight.direction", camera.Front);
			lightingShader.setFloat("spotLight.cutOffStart", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("spotLight.cutOffEnd", glm::cos(glm::radians(17.5f)));
			lightingShader.setFloat("spotLight.constant", 1.0f);
			attenValues = get_attenuation_inputs(50.0f);
			lightingShader.setFloat("spotLight.linear", attenValues.x);
			lightingShader.setFloat("spotLight.quadratic", attenValues.y);
			glm::vec3 spotLightColor = flashlight ? glm::vec3(0) : glm::vec3(1.0f);
			lightingShader.setVec3("spotLight.ambient", glm::vec3(0.0f));
			lightingShader.setVec3("spotLight.diffuse", spotLightColor * glm::vec3(1.0f));
			lightingShader.setVec3("spotLight.specular", spotLightColor * glm::vec3(1.0f));

			glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)SW / (float)SH, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			lightingShader.setMat4("projection", proj);
			lightingShader.setMat4("view", view);
			glBindVertexArray(VAO);
			glm::mat4 model = glm::mat4(1.0f);
			for (unsigned int i = 0; i < 10; i++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePositions[i]);
				float angle = 20.0f * i;
				model = glm::rotate(model, glm::radians(angle),
					glm::vec3(1.0f, 0.3f, 0.5f));
				lightingShader.setMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			//LAMP
			lightCubeShader.use();
			lightCubeShader.setMat4("projection", proj);
			lightCubeShader.setMat4("view", view);

			glBindVertexArray(lightVAO);
			for (int i = 0; i < 4; i++)
			{
				lightCubeShader.setVec3("lightColor", pointLightColors[i]);
				model = glm::mat4(1.0f);
				model = glm::translate(model, pointLightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
				lightCubeShader.setMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}


			glfwSwapBuffers(window);
		}
		oldMouseHeld = mouseHeld;
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SW = width;
	SH = height;
	glViewport(0, 0, SW, SH);
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	sprinting = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) or sprinting;

	float speedDelta = deltaTime * fmax(3.0f*sprinting, 1.0f);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, speedDelta);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, speedDelta);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, speedDelta);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, speedDelta);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, speedDelta);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, speedDelta);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		lightPos = camera.Position;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // initially set to true
	{
		lastMouseX = xpos;
		lastMouseY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastMouseX;
	float yoffset = lastMouseY - ypos;
	lastMouseX = xpos;
	lastMouseY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT and action == GLFW_PRESS)
	{
		mouseHeld = true;
	}
	else {
		mouseHeld = false;
	}
}

void iconify_callback(GLFWwindow* window, int iconified)
{
	if (!(iconified == GLFW_TRUE))
		glfwMakeContextCurrent(window);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
