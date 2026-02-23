//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
//clases para dar orden y limpieza al código
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;

//Vertex Shader original (para cubos y pirámides sin color en VAO)
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";

//Vertex Shader con color en VAO (letras originales del profesor)
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";

// ===== SHADERS DE COLOR FIJO (uno por color) =====
// Cada .vert hardcodea su color, todos comparten shader.frag
static const char* vShaderrojo = "shaders/shaderrojo.vert";      // rojo   (1, 0, 0)
static const char* vShaderverde = "shaders/shaderverde.vert";     // verde  (0, 1, 0)
static const char* vShaderazul = "shaders/shaderazul.vert";      // azul   (0, 0, 1)
static const char* vShadercafe = "shaders/shadercafe.vert";      // café   (0.478, 0.255, 0.067)
static const char* vShaderverdeoscuro = "shaders/shaderverdoscuro.vert"; // verde oscuro (0, 0.39, 0)

float angulo = 0.0f;

// ===================================================
//  ÍNDICES EN shaderList:
//  0 → shader original (vShader + fShader)        para cubo/pirámide sin color
//  1 → shaderColor (vShaderColor + fShaderColor)  para MeshColor con color en VAO
//  2 → shaderrojo
//  3 → shaderverde
//  4 → shaderazul
//  5 → shadercafe
//  6 → shaderverdoscuro
// ===================================================

// ===================================================
//  ÍNDICES EN meshList:
//  0 → Pirámide
//  1 → Cubo
// ===================================================

//Pirámide triangular regular
void CreaPiramide()
{
	unsigned int indices[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
	};
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,  //0
		 0.5f, -0.5f, 0.0f,  //1
		 0.0f,  0.5f,-0.25f, //2
		 0.0f, -0.5f,-0.5f,  //3
	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1); // índice 0
}

//Vértices de un cubo
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};
	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo); // índice 1
}

// MeshColor solo se deja por compatibilidad con la plantilla del profesor
// Para este ejercicio NO se usan (dibujamos con cubos/pirámides + shaders de color)
void CrearLetrasyFiguras()
{
	GLfloat vertices_letras[] = {
		//X		Y		Z		R		G		B
		-1.0f,	-1.0f,	0.5f,	0.0f,	0.0f,	1.0f,
		 1.0f,	-1.0f,	0.5f,	0.0f,	0.0f,	1.0f,
		 1.0f,	 1.0f,	0.5f,	0.0f,	0.0f,	1.0f,
	};
	MeshColor* letras = new MeshColor();
	letras->CreateMeshColor(vertices_letras, 18);
	meshColorList.push_back(letras); // índice 0 (no se usa en este ejercicio)

	GLfloat vertices_triangulorojo[] = {
		//X		Y		Z		R		G		B
		-1.0f,	-1.0f,	0.5f,	1.0f,	0.0f,	0.0f,
		 1.0f,	-1.0f,	0.5f,	1.0f,	0.0f,	0.0f,
		 0.0f,	 1.0f,	0.5f,	1.0f,	0.0f,	0.0f,
	};
	MeshColor* triangulorojo = new MeshColor();
	triangulorojo->CreateMeshColor(vertices_triangulorojo, 18);
	meshColorList.push_back(triangulorojo); // índice 1 (no se usa en este ejercicio)

	GLfloat vertices_cuadradoverde[] = {
		//X		Y		Z		R		G		B
		-0.5f,	-0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		 0.5f,	-0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		 0.5f,	 0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		 0.5f,	 0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
		-0.5f,	 0.5f,	0.5f,	0.0f,	1.0f,	0.0f,
	};
	MeshColor* cuadradoverde = new MeshColor();
	cuadradoverde->CreateMeshColor(vertices_cuadradoverde, 36);
	meshColorList.push_back(cuadradoverde); // índice 2 (no se usa en este ejercicio)

	GLfloat vertices_cuadradorojo[] = {
		//X		Y		Z		R		G		B
		-0.5f,	-0.5f,	0.5f,	1.0f,	0.0f,	0.0f,
		 0.5f,	-0.5f,	0.5f,	1.0f,	0.0f,	0.0f,
		 0.5f,	 0.5f,	0.5f,	1.0f,	0.0f,	0.0f,
		-0.5f,	-0.5f,	0.5f,	1.0f,	0.0f,	0.0f,
		 0.5f,	 0.5f,	0.5f,	1.0f,	0.0f,	0.0f,
		-0.5f,	 0.5f,	0.5f,	1.0f,	0.0f,	0.0f,
	};
	MeshColor* cuadradorojo = new MeshColor();
	cuadradorojo->CreateMeshColor(vertices_cuadradorojo, 36);
	meshColorList.push_back(cuadradorojo); // índice 3 (no se usa en este ejercicio)
}

void CreateShaders()
{
	Shader* shader1 = new Shader(); // shader original sin color en VAO (cubo/pirámide)
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1); // índice 0

	Shader* shader2 = new Shader(); // shader con color en VAO (MeshColor)
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2); // índice 1

	// ---- Shaders de color fijo ----
	Shader* shader3 = new Shader(); // rojo
	shader3->CreateFromFiles(vShaderrojo, fShader);
	shaderList.push_back(*shader3); // índice 2

	Shader* shader4 = new Shader(); // verde
	shader4->CreateFromFiles(vShaderverde, fShader);
	shaderList.push_back(*shader4); // índice 3

	Shader* shader5 = new Shader(); // azul
	shader5->CreateFromFiles(vShaderazul, fShader);
	shaderList.push_back(*shader5); // índice 4

	Shader* shader6 = new Shader(); // café
	shader6->CreateFromFiles(vShadercafe, fShader);
	shaderList.push_back(*shader6); // índice 5

	Shader* shader7 = new Shader(); // verde oscuro
	shader7->CreateFromFiles(vShaderverdeoscuro, fShader);
	shaderList.push_back(*shader7); // índice 6
}

// ===========================================================
// Función auxiliar: activa un shader de color fijo y devuelve
// las ubicaciones de model y projection ya configuradas.
// shaderIndex: índice en shaderList
// ===========================================================
void UsarShaderColor(int shaderIndex, GLuint& uModel, GLuint& uProj, const glm::mat4& projection)
{
	shaderList[shaderIndex].useShader();
	uModel = shaderList[shaderIndex].getModelLocation();
	uProj = shaderList[shaderIndex].getProjectLocation();
	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projection));
}

int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	CreaPiramide();          // índice 0 en meshList
	CrearCubo();             // índice 1 en meshList
	CrearLetrasyFiguras();   // meshColorList (compatibilidad con plantilla, no se renderizan)
	CreateShaders();

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;

	// Proyección ORTOGONAL: coordenadas normalizadas -1 a 1
	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	//glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Matriz de modelo (se reinicializa en cada objeto)
	glm::mat4 model(1.0);

	// Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		// Recibir eventos del usuario
		glfwPollEvents();
		// Limpiar la ventana (fondo blanco)
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ================================================================
		// LETRAS ARC  (formadas con cubos instanciados)
		// Zona superior de la pantalla: y entre 0.55 y 0.95 aprox.
		// Cada letra usa un color diferente:
		//   A → rojo   (shaderList[2])
		//   R → verde  (shaderList[3])
		//   C → azul   (shaderList[4])
		// ================================================================

		// -------  LETRA  A  (rojo)  -------
		// Formada por: barra izquierda, barra derecha, travesaño central y techo (pirámide/cubo)
		UsarShaderColor(2, uniformModel, uniformProjection, projection); // rojo

		// Pata izquierda de la A
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.82f, 0.68f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f, 0.28f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh(); // cubo

		// Pata derecha de la A
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.68f, 0.68f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f, 0.28f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Travesaño central de la A
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.75f, 0.72f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Techo de la A (pirámide invertida simulada con pirámide normal escalada)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.75f, 0.87f, 0.0f));
		model = glm::scale(model, glm::vec3(0.22f, 0.12f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); // pirámide como punta de la A

		// -------  LETRA  R  (verde)  -------
		UsarShaderColor(3, uniformModel, uniformProjection, projection); // verde

		// Palo vertical de la R
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.07f, 0.72f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f, 0.34f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Barriga superior de la R (parte horizontal superior)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.04f, 0.86f, 0.0f));
		model = glm::scale(model, glm::vec3(0.16f, 0.05f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Barriga inferior de la R (parte horizontal media)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.04f, 0.72f, 0.0f));
		model = glm::scale(model, glm::vec3(0.16f, 0.05f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Arco derecho de la R (barra vertical derecha de la barriga)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.10f, 0.79f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f, 0.17f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Pata diagonal de la R (barra inclinada abajo-derecha)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.09f, 0.63f, 0.0f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.18f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// -------  LETRA  C  (azul)  -------
		UsarShaderColor(4, uniformModel, uniformProjection, projection); // azul

		// Barra horizontal superior de la C
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.69f, 0.88f, 0.0f));
		model = glm::scale(model, glm::vec3(0.22f, 0.05f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Barra vertical izquierda de la C
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.59f, 0.74f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f, 0.32f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// Barra horizontal inferior de la C
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.69f, 0.59f, 0.0f));
		model = glm::scale(model, glm::vec3(0.22f, 0.05f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// ================================================================
		// CONSTRUCCIÓN DE LA CASA (cubos y pirámides con shaders de color)
		// Zona central-inferior: y entre -0.6 y 0.4
		// ================================================================

		// ---- Pared principal (cubo ROJO) ----
		UsarShaderColor(2, uniformModel, uniformProjection, projection); // rojo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh(); // cubo como pared

		// ---- Ventana izquierda (cubo VERDE) ----
		UsarShaderColor(3, uniformModel, uniformProjection, projection); // verde
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.15f, -0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// ---- Ventana derecha (cubo VERDE) ----
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.15f, -0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// ---- Puerta (cubo VERDE, más alto que ancho) ----
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.39f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.22f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// ---- Techo (pirámide AZUL) ----
		UsarShaderColor(4, uniformModel, uniformProjection, projection); // azul
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.17f, 0.0f));
		model = glm::scale(model, glm::vec3(0.67f, 0.35f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); // pirámide como techo

		// ================================================================
		// ÁRBOL IZQUIERDO
		// ================================================================

		// Copa (pirámide VERDE OSCURO)
		UsarShaderColor(6, uniformModel, uniformProjection, projection); // verde oscuro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.72f, -0.15f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.4f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); // pirámide como copa

		// Tronco izq (cubo CAFÉ)
		UsarShaderColor(5, uniformModel, uniformProjection, projection); // café
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.72f, -0.45f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f, 0.2f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh(); // cubo como tronco

		// ================================================================
		// ÁRBOL DERECHO
		// ================================================================

		// Copa (pirámide VERDE OSCURO)
		UsarShaderColor(6, uniformModel, uniformProjection, projection); // verde oscuro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.72f, -0.15f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.4f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Tronco der(cubo CAFÉ)
		UsarShaderColor(5, uniformModel, uniformProjection, projection); // café
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.72f, -0.45f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f, 0.2f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		////Para el cubo y la pirámide se usa el primer set de shaders con índice 0 en ShaderList
		//shaderList[0].useShader(); 
		//uniformModel = shaderList[0].getModelLocation();
		//uniformProjection = shaderList[0].getProjectLocation();
		//angulo += 0.01;
		////Inicializar matriz de dimensión 4x4 que servirá como matriz de modelo para almacenar las transformaciones geométricas
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		//model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//meshList[1]->RenderMesh();
		//
		////piramide
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-1.25f, 1.25f, -4.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		//meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}

// inicializar matriz: glm::mat4 model(1.0);
// reestablecer matriz: model = glm::mat4(1.0);
//Traslación
//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
//////////////// ROTACIÓN //////////////////
//model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
////////////////  ESCALA ////////////////
//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
///////////////////// T+R////////////////
/*model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
*/
/////////////R+T//////////
/*model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
*/