#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\random.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"
using std::vector;

const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

Camera camera;
Window mainWindow;
vector<Mesh*>  meshList;
vector<Shader> shaderList;
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20);

bool  firstMouse = true;
float lastX = 400.0f;
float lastY = 300.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
	float xChange = xpos - lastX;
	float yChange = lastY - ypos;
	lastX = xpos; lastY = ypos;
	camera.mouseControl(xChange, yChange);
}

// ??? Geometrías ???????????????????????????????????????????????????????????????

void CrearCubo()
{
	unsigned int indices[] = {
		0,1,2, 2,3,0, 1,5,6, 6,2,1,
		7,6,5, 5,4,7, 4,0,3, 3,7,4,
		4,5,1, 1,0,4, 3,2,6, 6,7,3
	};
	GLfloat vertices[] = {
		-0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,
		 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
		-0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
		 0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f
	};
	Mesh* obj = new Mesh();
	obj->CreateMesh(vertices, indices, 24, 36);
	meshList.push_back(obj); // [0]
}

void CrearCilindro(int res, float R)
{
	// eje en Y, rango [-0.5, +0.5], tapas cerradas con abanico de triángulos
	int n, i;
	GLfloat dt = 2 * PI / res, x, z;
	vector<GLfloat>      verts;
	vector<unsigned int> idx;

	for (n = 0; n <= res; n++) {
		x = (n != res) ? R * cos(n * dt) : R;
		z = (n != res) ? R * sin(n * dt) : 0.0f;
		verts.push_back(x); verts.push_back(-0.5f); verts.push_back(z);
		verts.push_back(x); verts.push_back(0.5f); verts.push_back(z);
	}
	verts.push_back(0.0f); verts.push_back(-0.5f); verts.push_back(0.0f);
	for (n = 0; n <= res; n++) {
		x = R * cos(n * dt); z = R * sin(n * dt);
		verts.push_back(x); verts.push_back(-0.5f); verts.push_back(z);
	}
	verts.push_back(0.0f); verts.push_back(0.5f); verts.push_back(0.0f);
	for (n = 0; n <= res; n++) {
		x = R * cos(n * dt); z = R * sin(n * dt);
		verts.push_back(x); verts.push_back(0.5f); verts.push_back(z);
	}

	for (i = 0; i < (int)verts.size(); i++) idx.push_back(i);
	Mesh* obj = new Mesh();
	obj->CreateMeshGeometry(verts, idx, verts.size(), idx.size());
	meshList.push_back(obj); // [1]
}

void CrearCono(int res, float R)
{
	// punta en Y=+0.5, base circular en Y=-0.5, tapa base cerrada
	int n;
	GLfloat dt = 2 * PI / res, x, z;
	vector<GLfloat>      verts;
	vector<unsigned int> idx;

	verts.push_back(0.0f); verts.push_back(0.5f); verts.push_back(0.0f);
	for (n = 0; n <= res; n++) {
		x = R * cos(n * dt); z = R * sin(n * dt);
		verts.push_back(x); verts.push_back(-0.5f); verts.push_back(z);
	}
	verts.push_back(0.0f); verts.push_back(-0.5f); verts.push_back(0.0f);
	for (n = 0; n <= res; n++) {
		x = R * cos(n * dt); z = R * sin(n * dt);
		verts.push_back(x); verts.push_back(-0.5f); verts.push_back(z);
	}

	for (n = 0; n < (int)verts.size(); n++) idx.push_back(n);
	Mesh* obj = new Mesh();
	obj->CreateMeshGeometry(verts, idx, verts.size(), idx.size());
	meshList.push_back(obj); // [2]
}

void CreateShaders()
{
	Shader* s = new Shader();
	s->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*s);
}

// ??? Main ?????????????????????????????????????????????????????????????????????

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CrearCubo();
	CrearCilindro(36, 1.0f);
	CrearCono(36, 1.0f);
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 5.0f, 22.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.2f, 0.2f);

	GLuint uniformModel = 0;
	GLuint uniformProjection = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;

	glm::mat4 projection = glm::perspective(glm::radians(60.0f),
		mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	sp.init();
	sp.load();

	glm::mat4 model(1.0f);
	glm::mat4 modelTorso(1.0f);
	glm::mat4 modelCuello(1.0f);
	glm::mat4 modelCabeza(1.0f);
	glm::mat4 modelCola(1.0f);
	glm::mat4 modelaux(1.0f);
	glm::mat4 modelCaderaDD(1.0f);
	glm::mat4 modelCaderaDI(1.0f);
	glm::mat4 modelCaderaTD(1.0f);
	glm::mat4 modelCaderaTI(1.0f);

	glm::vec3 cafe = glm::vec3(0.55f, 0.35f, 0.15f);
	glm::vec3 cafeClaro = glm::vec3(0.70f, 0.45f, 0.20f);
	glm::vec3 amarillo = glm::vec3(1.0f, 0.9f, 0.0f);
	glm::vec3 blanco = glm::vec3(1.0f, 1.0f, 1.0f);

	glfwSetInputMode(mainWindow.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(mainWindow.getGLFWwindow(), mouse_callback);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);

		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// ?? Torso ?????????????????????????????????????????????????????????????
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
		modelTorso = model;
		model = glm::scale(model, glm::vec3(10.0f, 4.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafe));
		meshList[0]->RenderMesh();

		// ?? Cuello ????????????????????????????????????????????????????????????
		model = modelTorso;
		model = glm::translate(model, glm::vec3(5.0f, 1.5f, 0.0f));
		modelCuello = model;
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafe));
		meshList[0]->RenderMesh();

		// ?? Cabeza ????????????????????????????????????????????????????????????
		model = modelCuello;
		model = glm::translate(model, glm::vec3(1.2f, 2.8f, 0.0f));
		modelCabeza = model;
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafeClaro));
		sp.render();

		// ?? Oreja izquierda (F) ???????????????????????????????????????????????
		model = modelCabeza;
		model = glm::translate(model, glm::vec3(0.0f, 2.1f, 2.2f));
		model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.2f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafe));
		meshList[2]->RenderMeshGeometry();

		// ?? Oreja derecha (G) ?????????????????????????????????????????????????
		model = modelCabeza;
		model = glm::translate(model, glm::vec3(0.0f, 2.1f, -2.2f));
		model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.2f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafe));
		meshList[2]->RenderMeshGeometry();

		// ?? Cola seg1 (K) ?????????????????????????????????????????????????????
		model = modelTorso;
		model = glm::translate(model, glm::vec3(-5.0f, 0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
		modelCola = model;
		model = glm::scale(model, glm::vec3(4.0f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafe));
		meshList[0]->RenderMesh();

		// esfera articulación cola
		model = modelCola;
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(blanco));
		sp.render();

		// Cola seg2 (L)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafeClaro));
		meshList[0]->RenderMesh();

		// ?? Pata delantera derecha  Z=muslo  X=canilla ????????????????????????
		model = modelTorso;
		model = glm::translate(model, glm::vec3(3.5f, -2.0f, 3.0f));
		modelCaderaDD = model;
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.5f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(amarillo));
		meshList[1]->RenderMeshGeometry();

		model = modelCaderaDD;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 3.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafe));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(blanco));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 3.0f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafeClaro));
		meshList[0]->RenderMesh();

		// ?? Pata delantera izquierda  C=muslo  V=canilla ??????????????????????
		model = modelTorso;
		model = glm::translate(model, glm::vec3(3.5f, -2.0f, -3.0f));
		modelCaderaDI = model;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(150.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.5f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(amarillo));
		meshList[1]->RenderMeshGeometry();

		model = modelCaderaDI;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 3.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafe));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(blanco));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 3.0f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafeClaro));
		meshList[0]->RenderMesh();

		// ?? Pata trasera derecha  B=muslo  N=canilla ??????????????????????????
		model = modelTorso;
		model = glm::translate(model, glm::vec3(-3.5f, -2.0f, 3.0f));
		modelCaderaTD = model;
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.5f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(amarillo));
		meshList[1]->RenderMeshGeometry();

		model = modelCaderaTD;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 3.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafe));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion10()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(blanco));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 3.0f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafeClaro));
		meshList[0]->RenderMesh();

		// ?? Pata trasera izquierda  M=muslo  ,=canilla ????????????????????????
		model = modelTorso;
		model = glm::translate(model, glm::vec3(-3.5f, -2.0f, -3.0f));
		modelCaderaTI = model;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(150.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.5f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(amarillo));
		meshList[1]->RenderMeshGeometry();

		model = modelCaderaTI;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion11()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 3.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafe));
		meshList[0]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion12()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(blanco));
		sp.render();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 3.0f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(cafeClaro));
		meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}