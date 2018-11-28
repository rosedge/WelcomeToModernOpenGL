#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Application.h"


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;


	float positions[] = {
	//	----pos----		--uv--
		100.0f, 100.0f,	0.0f, 0.0f,	//0
		200.0f, 100.0f,	1.0f, 0.0f,	//1
		200.0f, 200.0f,	1.0f, 1.0f,	//2
		100.0f, 200.0f,	1.0f, 0.0f	//3
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};


	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f,	-0.5f,	0.0f,
		 0.5f,	-0.5f,	0.0f,
		-0.5f,	 0.5f,	0.0f,
		 0.5f,	 0.5f,  0.0f,
	};

	struct Particle {
		glm::vec3 pos;
		glm::vec3 speed;
		unsigned char r, g, b, a;
		float size;
		float angle;
		float weight;
		float life;
		float camera_distance;

		bool operator< (const Particle& that) const {
			//Sort in reverse order : far particles draw first.
			return this->camera_distance > that.camera_distance;
		}
	};

	const unsigned int MaxParticles = 10000;
	Particle ParticlesContainer[MaxParticles];
	unsigned int LastUsedParticle{ 0 };

	auto FindUnusedParticles([&]() {
		for (auto i = LastUsedParticle; i < MaxParticles; i++) {
			if (ParticlesContainer[i].life < 0.0f) {
				LastUsedParticle = i;
				return i;
			}
		}

		for (auto i = 0u; i < LastUsedParticle; i++) {
			if (ParticlesContainer[i].life < 0.0f) {
				LastUsedParticle = i;
				return i;
			}
		}

		return 0u;
	});

	auto SortParticles = [&]() {
		std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
	};

	VertexBuffer vb_billboard(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
	VertexBufferLayout vbl_billboard;
	vbl_billboard.Push<float>(2);
	VertexArray va_billboard;
	va_billboard.AddBuffer(vb_billboard, vbl_billboard);

	VertexBuffer vb_particles(nullptr, MaxParticles * 4 * sizeof GLfloat);
	VertexBufferLayout vbl_particles;
	vbl_particles.Push<float>(2);
	VertexArray va_particles;
	va_particles.AddBuffer(vb_particles, vbl_particles);

	VertexBuffer vb_particles_color(nullptr, MaxParticles * 4 * sizeof GLfloat);
	VertexBufferLayout vbl_particles_color;
	vbl_particles_color.Push<float>(2);
	VertexArray va_particles_color;
	va_particles_color.AddBuffer(vb_particles_color, vbl_particles_color);

	auto lastTime = glfwGetTime();


	VertexArray va;
	VertexBuffer vb(positions, 4 * 4 * sizeof(float)); //Already vb.Bind();
	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(indices, 6);
	

	glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));

	glm::mat4 mvp = proj * view * model;

	Shader shader("Shaders/Base.shader");
	shader.Bind();
	shader.SetUniformMat4f("u_MVP", mvp);

	Texture texture("Res/wuxia.jpg");
	texture.Bind();
	shader.SetUniform1i("u_Texture", 0);

	float r = 0.0f;
	float increment = 0.05f;

	va.Unbind();
	shader.Unbind();
	vb.Unbind();
	ib.Unbind();

	Renderer renderer;



	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderer.Clear();

		auto currentTime = glfwGetTime();
		auto deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		

		va.Bind();
		ib.Bind();
		shader.Bind();

		renderer.Draw(va, ib, shader);

		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;

		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}