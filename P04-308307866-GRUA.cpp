/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
F: articulacion1 — brazo 1
G: articulacion2 — brazo 2
H: articulacion3 — brazo 3
J: articulacion4 — canasta (giro en Y)
V: articulacion5 — llanta frente derecha
B: articulacion6 — llanta frente izquierda
N: articulacion7 — llanta atrás derecha
M: articulacion8 — llanta atrás izquierda
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
using std::vector;
const float toRadians = 3.14159265f / 180.0;
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20);

bool firstMouse = true;
float lastX = 400.0f;
float lastY = 300.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xChange = xpos - lastX;
	float yChange = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.mouseControl(xChange, yChange);
}

void CrearCubo()
{
	//el cubo unitario tiene su centro en el origen (0,0,0)
	//sus vértices van de -0.5 a 0.5 en los tres ejes
	//al escalarlo con glm::scale el centro sigue en el mismo punto
	//y los extremos se alejan proporcionalmente en cada eje
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		7, 6, 5, 5, 4, 7,
		4, 0, 3, 3, 7, 4,
		4, 5, 1, 1, 0, 4,
		3, 2, 6, 6, 7, 3
	};
	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

void CrearPiramideTriangular()
{
	//la pirámide triangular tiene su base centrada en Y=-0.5
	//y su vértice superior en Y=0.5, el centro geométrico queda en el origen
	unsigned int indices_piramide_triangular[] = {
		0,1,2, 1,3,2, 3,0,2, 1,0,3
	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.0f,  0.5f, -0.25f,
		 0.0f, -0.5f, -0.5f,
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);
}

void CrearCilindro(int res, float R) {
	//el cilindro se genera con su eje central sobre Y, va de Y=-0.5 a Y=0.5
	//los vértices de la circunferencia se calculan con cos y sin sobre el plano XZ
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	for (n = 0; n <= (res); n++) {
		if (n != res) { x = R * cos((n)*dt); z = R * sin((n)*dt); }
		else { x = R * cos(0); z = R * sin(0); }
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			case 3: vertices.push_back(x); break;
			case 4: vertices.push_back(0.5); break;
			case 5: vertices.push_back(z); break;
			}
		}
	}
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(-0.5f); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(0.5); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

void CrearCono(int res, float R) {
	//el cono tiene su punta en Y=0.5 y su base circular en Y=-0.5 sobre el plano XZ
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	vector<GLfloat> vertices;
	vector<unsigned int> indices;
	vertices.push_back(0.0); vertices.push_back(0.5); vertices.push_back(0.0);
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt); z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);
	for (i = 0; i < res + 2; i++) indices.push_back(i);
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

void CrearPiramideCuadrangular()
{
	//la base cuadrada está en Y=-0.5 centrada en el origen sobre XZ
	//el vértice superior está en Y=0.5 en el centro (0,0.5,0)
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4, 3,2,4, 2,1,4, 1,0,4, 0,1,2, 0,2,4
	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.0f,  0.5f,  0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CrearCubo();               //índice 0 en meshList
	CrearPiramideTriangular(); //índice 1 en meshList
	CrearCilindro(36, 1.0f);  //índice 2 en meshList — 36 segmentos para verse redondo como llanta
	CrearCono(25, 2.0f);      //índice 3 en meshList
	CrearPiramideCuadrangular();//índice 4 en meshList
	CreateShaders();

	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.2f);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	sp.init();
	sp.load();

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelBase(1.0);
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

	//cursor bloqueado al centro de la ventana
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
		// el mouse ya lo maneja mouse_callback, no se llama mouseControl aquí

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		//CREANDO LA CABINA — nodo raíz de la jerarquía
		//el cubo unitario tiene centro en origen, translate lo lleva a (0,7,-4)
		//escalado (7,4,5): ocupa X de -3.5 a +3.5, Y de +5 a +9, Z de -6.5 a -1.5
		//modelaux guarda el pivot de la cabina sin escala — nodo padre de base y brazos
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 7.0f, -4.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(7.0f, 4.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(0.0f, 0.8f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//base — nodo hijo de la cabina, hereda de modelaux (pivot de la cabina)
		//la punta de la pirámide (Y=+0.5 local) queda en el origen de la cabina (0,7,-4)
		//translate -1.5 en Y baja el centro para que la punta coincida con ese origen
		//escala (9,3,7): 2 unidades más ancha que la cabina en X y Z
		//modelBase guarda el pivot de la base sin escala — nodo padre de las llantas
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		modelBase = model;
		model = glm::scale(model, glm::vec3(9.0f, 3.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.9f, 0.0f); // amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();

		//llantas — nodos hijos de la base, heredan de modelBase
		//el centro de modelBase está en (0, 5.5, -4) en espacio mundo
		//los translates son relativos a ese centro para llegar a cada esquina
		//X=±4.5 (mitad del ancho 9), Y=-1.5 (bajar a la base), Z=±3.5 (mitad del largo 7)
		//cada llanta rota en Y sobre su propio pivote con teclas V, B, N, M
		//rotate 90° en X voltea el cilindro para que su cara circular quede de lado

		//llanta frente derecha — articulacion5 tecla V
		//pivot en esquina frente derecha de la base (+4.5, -1.5, +3.5) relativo a modelBase
		model = modelBase;
		model = glm::translate(model, glm::vec3(4.5f, -1.5f, 3.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f); // blanco
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//llanta frente izquierda — articulacion6 tecla B
		//pivot en esquina frente izquierda de la base (-4.5, -1.5, +3.5) relativo a modelBase
		model = modelBase;
		model = glm::translate(model, glm::vec3(-4.5f, -1.5f, 3.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//llanta atrás derecha — articulacion7 tecla N
		//pivot en esquina atrás derecha de la base (+4.5, -1.5, -3.5) relativo a modelBase
		model = modelBase;
		model = glm::translate(model, glm::vec3(4.5f, -1.5f, -3.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//llanta atrás izquierda — articulacion8 tecla M
		//pivot en esquina atrás izquierda de la base (-4.5, -1.5, -3.5) relativo a modelBase
		model = modelBase;
		model = glm::translate(model, glm::vec3(-4.5f, -1.5f, -3.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry();

		//articulación 1: pivote en el centro de la cabina (0,7,-4)
		//la rotación en Z gira el brazo como manecilla de reloj sobre ese punto
		//135° fijos orientan el brazo hacia arriba-izquierda en posición inicial
		//getarticulacion1 suma grados adicionales con tecla F
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//primer brazo que conecta con la cabina
		//el cubo tiene centro en origen, translate 2.5 en X lo aleja del pivote
		//así el extremo izquierdo del cubo queda en el pivote y el derecho apunta hacia afuera
		//scale (5,1,1) estira el cubo 5 unidades sobre su eje X local (ya rotado 135°)
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//articulación 2: pivote en el extremo libre del brazo 1 (translate 2.5 en X local)
		//la rotación en Z permite doblar el brazo 2 desde ese punto con tecla G
		//la esfera tiene centro en el origen, scale 0.5 la reduce y queda centrada en el pivote
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		//segundo brazo: cuelga hacia abajo desde la articulación 2
		//translate -2.5 en Y local baja el centro del cubo, dejando su extremo superior en el pivote
		//scale (1,5,1) estira el cubo 5 unidades sobre su eje Y local
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		model = modelaux;

		//articulación 3: pivote en el extremo inferior del brazo 2 (translate -2.5 en Y local)
		//la rotación en Z permite doblar el brazo 3 desde ese punto con tecla H
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		//dibujar una pequeña esfera centrada en el pivote de la articulación 3
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		//tercer brazo: misma lógica que el primero, translate 2.5 en X local aleja el centro del pivote
		//la orientación inicial es 0° adicionales sobre la rotación heredada de la articulación 3
		//scale (5,1,1) estira el cubo sobre su eje X local
		model = modelaux;
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//articulación 4: pivote en el extremo libre del brazo 3 (translate 2.5 en X local)
		//la rotación es en Y, eso gira la canasta de izquierda a derecha con tecla J
		//la esfera queda centrada en ese pivote con scale 0.5
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;

		//dibujar una pequeña esfera centrada en el pivote de la articulación 4
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		//canasta: hereda la rotación en Y de la articulación 4
		//translate -1 en Y la baja del pivote, el cubo tiene centro en origen
		//scale (3.5,2,2.5) es la mitad de la cabina (7,4,5)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(3.5f, 2.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}