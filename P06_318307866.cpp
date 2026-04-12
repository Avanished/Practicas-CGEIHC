/*
Práctica 6: Texturizado
*/
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture logofiTexture;
Texture CuboemoTexture;
Texture dado8Texture;

Model CuboEmociones_M;
Model Carrotex_M;
Model Cofretex_M;
Model Llantatexsupder_M;
Model Llantatexsupizq_M;
Model Llantatexinfder_M;
Model Llantatexinfizq_M;

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

static const char* vShader = "shaders/shader_texture.vert";
static const char* fShader = "shaders/shader_texture.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f,  1.0f,	0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -0.6f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,	0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	 0.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f, -10.0f,	10.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f,  10.0f,	 0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f,  10.0f,	10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f,  0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.0f,	1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.0f,	0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		 0.0f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  0.5f,  0.5f,	1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	unsigned int cubo_indices[] = {
		0,  1,  2,
		2,  3,  0,
		8,  9,  10,
		10, 11,  8,
		12, 13, 14,
		14, 15, 12,
		16, 17, 18,
		18, 19, 16,
		20, 21, 22,
		22, 23, 20,
		4,  5,  6,
		6,  7,  4,
	};

	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,   0.503f, 0.401f,   0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,   0.660f, 0.401f,   0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f,  0.5f,   0.660f, 0.626f,   0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f,  0.5f,   0.503f, 0.626f,   0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,   0.311f, 0.401f,  -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,   0.469f, 0.401f,  -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,   0.469f, 0.626f,  -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,   0.311f, 0.626f,  -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,   0.859f, 0.401f,   0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,   0.684f, 0.401f,   0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,   0.684f, 0.626f,   0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,   0.859f, 0.626f,   0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,   0.122f, 0.401f,   1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,   0.280f, 0.401f,   1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,   0.280f, 0.626f,   1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,   0.122f, 0.626f,   1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,   0.503f, 0.119f,   0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,   0.660f, 0.119f,   0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,   0.660f, 0.344f,   0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,   0.503f, 0.344f,   0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,   0.503f, 0.626f,   0.0f, -1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,   0.660f, 0.626f,   0.0f, -1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,   0.660f, 0.851f,   0.0f, -1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,   0.503f, 0.851f,   0.0f, -1.0f,  0.0f,
	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);
}

void CrearDado8()
{
	unsigned int oct_indices[] = {
		0,  1,  2,
		3,  4,  5,
		6,  7,  8,
		9,  10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
	};

	GLfloat oct_vertices[] = {
		 0.0f,  1.0f,  0.0f,  0.4920f, 0.7431f,  0.57f,  0.57f,  0.57f,
		 0.0f,  0.0f,  1.0f,  0.2580f, 0.4982f,  0.57f,  0.57f,  0.57f,
		 1.0f,  0.0f,  0.0f,  0.7240f, 0.5030f,  0.57f,  0.57f,  0.57f,
		  0.0f,  1.0f,  0.0f,  0.0180f, 0.2545f,  0.57f,  0.57f, -0.57f,
		  1.0f,  0.0f,  0.0f,  0.4820f, 0.2545f,  0.57f,  0.57f, -0.57f,
		  0.0f,  0.0f, -1.0f,  0.2540f, 0.4947f,  0.57f,  0.57f, -0.57f,
		   0.0f,  1.0f,  0.0f,  0.0220f, 0.7406f, -0.57f,  0.57f, -0.57f,
		   0.0f,  0.0f, -1.0f,  0.2520f, 0.4970f, -0.57f,  0.57f, -0.57f,
		  -1.0f,  0.0f,  0.0f,  0.4740f, 0.7406f, -0.57f,  0.57f, -0.57f,
		   0.0f,  1.0f,  0.0f,  0.4920f, 0.2581f, -0.57f,  0.57f,  0.57f,
		  -1.0f,  0.0f,  0.0f,  0.7140f, 0.4970f, -0.57f,  0.57f,  0.57f,
		   0.0f,  0.0f,  1.0f,  0.2640f, 0.4922f, -0.57f,  0.57f,  0.57f,
			0.0f, -1.0f,  0.0f,  0.7320f, 0.9808f,  0.57f, -0.57f,  0.57f,
			1.0f,  0.0f,  0.0f,  0.9520f, 0.7491f,  0.57f, -0.57f,  0.57f,
			0.0f,  0.0f,  1.0f,  0.5080f, 0.7467f,  0.57f, -0.57f,  0.57f,
			 0.0f, -1.0f,  0.0f,  0.9440f, 0.2581f,  0.57f, -0.57f, -0.57f,
			 0.0f,  0.0f, -1.0f,  0.5080f, 0.2545f,  0.57f, -0.57f, -0.57f,
			 1.0f,  0.0f,  0.0f,  0.7320f, 0.4850f,  0.57f, -0.57f, -0.57f,
			  0.0f, -1.0f,  0.0f,  0.9540f, 0.7370f, -0.57f, -0.57f, -0.57f,
			 -1.0f,  0.0f,  0.0f,  0.5120f, 0.7382f, -0.57f, -0.57f, -0.57f,
			  0.0f,  0.0f, -1.0f,  0.7320f, 0.5042f, -0.57f, -0.57f, -0.57f,
			   0.0f, -1.0f,  0.0f,  0.7320f, 0.0228f, -0.57f, -0.57f,  0.57f,
			   0.0f,  0.0f,  1.0f,  0.5140f, 0.2448f, -0.57f, -0.57f,  0.57f,
			  -1.0f,  0.0f,  0.0f,  0.9540f, 0.2448f, -0.57f, -0.57f,  0.57f,
	};

	Mesh* dado8 = new Mesh();
	dado8->CreateMesh(oct_vertices, oct_indices, 192, 24);
	meshList.push_back(dado8);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CrearDado8();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado-de-numeros.png");
	dadoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();
	CuboemoTexture = Texture("Textures/dadoemociones.jpg");
	CuboemoTexture.LoadTextureA();
	dado8Texture = Texture("Textures/dado8.jpg");
	dado8Texture.LoadTextureA();

	CuboEmociones_M = Model();
	CuboEmociones_M.LoadModel("Models/Cuboeociones.obj");

	Carrotex_M = Model();
	Carrotex_M.LoadModel("Models/Carrotex.obj");
	Cofretex_M = Model();
	Cofretex_M.LoadModel("Models/Cofretex.obj");

	Llantatexsupder_M = Model();
	Llantatexsupder_M.LoadModel("Models/Llantatexsupder.obj");
	Llantatexsupizq_M = Model();
	Llantatexsupizq_M.LoadModel("Models/Llantatexsupizq.obj");
	Llantatexinfder_M = Model();
	Llantatexinfder_M.LoadModel("Models/Llantatexinfder.obj");
	Llantatexinfizq_M = Model();
	Llantatexinfizq_M.LoadModel("Models/Llantatexinfizq.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);

		// Piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();

		// Dado OpenGL - meshList[4]
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshList[4]->RenderMesh();

		// Cubo emociones importado
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 3.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuboEmociones_M.RenderModel();

		// Dado 8 caras - meshList[5]
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dado8Texture.UseTexture();
		meshList[5]->RenderMesh();

		// --- Carro nuevo con movimiento ---
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), -0.5f, -3.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Cuerpo del carro
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carrotex_M.RenderModel();

		// Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.87f, -2.65f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofretex_M.RenderModel();

		// Llanta superior derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, -3.85f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantatexsupder_M.RenderModel();

		// Llanta superior izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, -3.85f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantatexsupizq_M.RenderModel();

		// Llanta inferior derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 2.50f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantatexinfder_M.RenderModel();

		// Llanta inferior izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 2.50f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantatexinfizq_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}

/*
//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		logofiTexture.UseTexture();
		FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		glDisable(GL_BLEND);
*/