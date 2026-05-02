/*
Animacion:
- Simple o basica: Por banderas y condicionales
- Compleja: Por medio de funciones y algoritmos
- Textura Animada
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
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

// Ajustes generales

// Camara
float camVelocidad = 12.0f;
float camSensibilidad = 0.45f;

// Carro
float carEscala = 2.5f;
float carVelAvance = 8.0f;
float carVelLlantas = 520.0f;
float carVelGiro = 5.0f;
float carVelInclinacion = 4.0f;
float carYOffset = 0.15f;

// Nave
float naveEscala = 2.0f;
float naveAlturaVuelo = 20.0f;
float naveAlturaAterrizada = 1.5f;
float naveVelAvance = 8.0f;
float naveVelGiro = 5.0f;
float naveVelInclinacion = 4.0f;

// Alas y helices
float naveVelHelice = 7800.0f;
float naveVelAla = 20.5f;
float naveAlaFrente = 60.0f;
float naveAlaAtras = -5.0f;

// Texturas animadas
float flechaVelU = 1.20f;
float numeroCambioIntervalo = 0.10f;

// Variables de apoyo
float movCoche;
float movOffset;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0f;
float angulovaria = 0.0f;

// Numero animado
float numeroTimer = 0.0f;
int frameNumero = 0;

// Ruta de la pista
std::vector<glm::vec3> pistaWaypoints = {
	glm::vec3(3.99f,   -0.84f,   3.39f),
	glm::vec3(-19.69f, -0.73f,   3.96f),
	glm::vec3(-34.04f, -0.90f,   6.39f),
	glm::vec3(-45.99f, -0.46f,  10.77f),
	glm::vec3(-50.64f,  0.10f,  14.77f),
	glm::vec3(-57.34f,  1.09f,  10.58f),
	glm::vec3(-67.11f,  3.76f,  -1.02f),
	glm::vec3(-72.92f,  5.78f,  -7.91f),
	glm::vec3(-86.85f, 11.91f,  -7.08f),
	glm::vec3(-100.20f, 17.63f, -3.11f),
	glm::vec3(-116.15f, 23.23f, -2.65f),
	glm::vec3(-138.54f, 27.28f, -6.83f),
	glm::vec3(-163.63f, 26.34f, -15.36f),
	glm::vec3(-179.15f, 24.40f, -20.92f)
};

// Estado del carro
int   carWpActual = 0;
float carT = 0.0f;
glm::vec3 carPos = pistaWaypoints[0];
float carYaw = 0.0f;
float carYawObjetivo = 0.0f;
bool  carDetenido = false;
float rotllanta = 0.0f;
glm::vec3 carDir = glm::vec3(0.0f, 0.0f, 1.0f);

// Ajuste visual del carro
float carPitch = 0.0f;
float carRoll = 0.0f;
float carPitchObjetivo = 0.0f;
float carRollObjetivo = 0.0f;
float carYawVisualOffset = 180.0f;

// Estado de la nave
int   naveWpActual = 0;
float naveT = 0.0f;
glm::vec3 navePos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 naveDir = glm::vec3(0.0f, 0.0f, -1.0f);
float naveYaw = 0.0f;
float naveYawObjetivo = 0.0f;
float navePitch = 0.0f;
float naveRoll = 0.0f;
float navePitchObjetivo = 0.0f;
float naveRollObjetivo = 0.0f;
bool  naveAterrizando = false;
bool  naveAterrizada = false;
float naveAterrizajeT = 0.0f;

// Animacion de la nave
float naveRotHelice = 0.0f;
float naveOscAla = 0.0f;
float naveAngAla = 35.0f;

// Posicion de piezas
glm::vec3 naveOffsetCuerpo = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 naveOffsetAlaIzq = glm::vec3(0.2f, 0.0f, 0.4f);
glm::vec3 naveOffsetAlaDer = glm::vec3(0.2f, 0.0f, -0.4f);
glm::vec3 naveOffsetHeliceIzq = glm::vec3(0.8f, -0.6f, 0.72f);
glm::vec3 naveOffsetHeliceDer = glm::vec3(0.8f, -0.6f, -0.69f);

// Rotacion base de piezas
glm::vec3 naveRotAlaIzq = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 naveRotAlaDer = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 naveRotHeliceIzq = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 naveRotHeliceDer = glm::vec3(0.0f, 0.0f, 0.0f);

// Consola
float debugTimer = 0.0f;

// Teclas
bool teclaRAnterior = false;
bool teclaCAnterior = false;
bool teclaPAnterior = false;
bool teclaLAnterior = false;

// Captura de waypoints
bool modoCaptura = false;
int  capturaContador = 0;
std::vector<glm::vec3> waypointsCapturados;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;

// Modelos del carro
Model Carrotex_M;
Model Cofretex_M;
Model Llantatexsupder_M;
Model Llantatexsupizq_M;
Model Llantatexinfder_M;
Model Llantatexinfizq_M;

// Modelos de la escena
Model Pista_M;
Model Nave_M;
Model Ala_M;
Model Ala2_M;
Model HeliceIzq_M;
Model HeliceDer_M;
Model Aeolipile_base_M;
Model Aeolipile_M;

Skybox skybox;

// Materiales
Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Luces
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Shaders
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;

		glm::vec3 v1(vertices[in1] - vertices[in0],
			vertices[in1 + 1] - vertices[in0 + 1],
			vertices[in1 + 2] - vertices[in0 + 2]);

		glm::vec3 v2(vertices[in2] - vertices[in0],
			vertices[in2 + 1] - vertices[in0 + 1],
			vertices[in2 + 2] - vertices[in0 + 2]);

		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;

		vertices[in0] += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;

		vertices[in1] += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;

		vertices[in2] += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x;
		vertices[nOffset + 1] = vec.y;
		vertices[nOffset + 2] = vec.z;
	}
}

float InterpolarSuave(float actual, float objetivo, float velocidad, float dt)
{
	float t = velocidad * dt;
	if (t > 1.0f) t = 1.0f;
	return actual + (objetivo - actual) * t;
}

float CalcularYaw(glm::vec3 desde, glm::vec3 hacia)
{
	glm::vec3 seg = hacia - desde;
	return atan2(seg.x, seg.z) / toRadians;
}

float InterpolarYaw(float actual, float objetivo, float velocidad, float dt)
{
	float diff = objetivo - actual;

	while (diff > 180.0f) diff -= 360.0f;
	while (diff < -180.0f) diff += 360.0f;

	actual += diff * velocidad * dt;

	while (actual > 180.0f) actual -= 360.0f;
	while (actual < -180.0f) actual += 360.0f;

	return actual;
}

float OscilarEntre(float minimo, float maximo, float tiempo)
{
	float t = (sin(tiempo) + 1.0f) * 0.5f;
	return minimo + (maximo - minimo) * t;
}

void ResetCarro()
{
	carWpActual = 0;
	carT = 0.0f;
	carPos = pistaWaypoints[0];
	carDetenido = false;
	rotllanta = 0.0f;
	carPitch = 0.0f;
	carRoll = 0.0f;
	carPitchObjetivo = 0.0f;
	carRollObjetivo = 0.0f;
	carDir = glm::vec3(0.0f, 0.0f, 1.0f);

	if (pistaWaypoints.size() >= 2)
	{
		carYawObjetivo = CalcularYaw(pistaWaypoints[0], pistaWaypoints[1]);
		carYaw = carYawObjetivo;
	}
}

void InicializarNave()
{
	naveVelAvance = carVelAvance;
	naveVelGiro = carVelGiro;
	naveVelInclinacion = carVelInclinacion;

	naveWpActual = (int)pistaWaypoints.size() - 1;
	naveT = 0.0f;
	naveAterrizando = false;
	naveAterrizada = false;
	naveAterrizajeT = 0.0f;
	navePitch = 0.0f;
	naveRoll = 0.0f;
	navePitchObjetivo = 0.0f;
	naveRollObjetivo = 0.0f;

	if (pistaWaypoints.size() >= 1)
	{
		navePos = pistaWaypoints[naveWpActual] + glm::vec3(0.0f, naveAlturaVuelo, 0.0f);
	}

	if (pistaWaypoints.size() >= 2)
	{
		glm::vec3 seg = pistaWaypoints[naveWpActual - 1] - pistaWaypoints[naveWpActual];
		if (glm::length(seg) > 0.001f)
			naveDir = glm::normalize(seg);

		naveYawObjetivo = CalcularYaw(pistaWaypoints[naveWpActual], pistaWaypoints[naveWpActual - 1]);
		naveYaw = naveYawObjetivo;
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
		-10.0f, 0.0f, -10.0f,	0.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f,  10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
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

	unsigned int flechaIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f,  0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 0.5f, 0.0f,  0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	unsigned int scoreIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f,  0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 0.5f, 0.0f,  0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		 0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	unsigned int numeroIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f,  0.5f,		0.0f, 0.67f,	0.0f, -1.0f, 0.0f,
		 0.5f, 0.0f,  0.5f,		0.25f, 0.67f,	0.0f, -1.0f, 0.0f,
		 0.5f, 0.0f, -0.5f,		0.25f, 1.0f,	0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-60.0f,
		0.0f,
		camVelocidad,
		camSensibilidad
	);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();

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

	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");
	Ala_M = Model();
	Ala_M.LoadModel("Models/ala.obj");
	Ala2_M = Model();
	Ala2_M.LoadModel("Models/ala2.obj");
	HeliceIzq_M = Model();
	HeliceIzq_M.LoadModel("Models/heliceizq.obj");
	HeliceDer_M = Model();
	HeliceDer_M.LoadModel("Models/heliceder.obj");
	Aeolipile_base_M = Model();
	Aeolipile_base_M.LoadModel("Models/Aeolipile_base.obj");
	Aeolipile_M = Model();
	Aeolipile_M.LoadModel("Models/Aeolipile.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	mainLight = DirectionalLight(
		1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		0.0f, -1.0f, -1.0f
	);

	unsigned int pointLightCount = 0;
	pointLights[0] = PointLight(
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f
	);
	pointLightCount++;

	unsigned int spotLightCount = 0;

	// Luz de la camara
	spotLights[0] = SpotLight(
		1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f
	);
	spotLightCount++;

	// Luz azul fija
	spotLights[1] = SpotLight(
		0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f
	);
	spotLightCount++;

	// Faro del carro
	spotLights[2] = SpotLight(
		1.0f, 1.0f, 0.7f,
		0.5f, 3.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -0.3f, -1.0f,
		1.0f, 0.05f, 0.01f,
		20.0f
	);
	spotLightCount++;

	// Faro de la nave
	spotLights[3] = SpotLight(
		0.2f, 1.0f, 0.2f,
		1.2f, 4.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.003f,
		18.0f
	);
	spotLightCount++;

	if (pistaWaypoints.size() >= 2)
	{
		carYawObjetivo = CalcularYaw(pistaWaypoints[0], pistaWaypoints[1]);
		carYaw = carYawObjetivo;
	}

	InicializarNave();

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;

	glm::mat4 projection = glm::perspective(
		45.0f,
		(GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(),
		0.1f,
		1000.0f
	);

	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllantaOffset = 10.0f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);
	glm::mat4 model(1.0f);
	glm::mat4 modelaux(1.0f);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	lastTime = glfwGetTime();

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		if (deltaTime > 0.1f)
			deltaTime = 0.1f;

		angulovaria += 0.5f * deltaTime;

		// Reinicia el carro
		bool teclaRActual = mainWindow.getsKeys()[GLFW_KEY_R];
		if (teclaRActual && !teclaRAnterior)
		{
			ResetCarro();
		}
		teclaRAnterior = teclaRActual;

		// Activa o desactiva la captura
		bool teclaCActual = mainWindow.getsKeys()[GLFW_KEY_C];
		if (teclaCActual && !teclaCAnterior)
		{
			modoCaptura = !modoCaptura;

			if (modoCaptura)
			{
				printf("\nMODO CAPTURA ACTIVADO\n");
				printf("Mueve la camara con WASD sobre la pista. P=capturar punto. L=limpiar. C=salir.\n");
				printf("Lista de waypoints\n");
				capturaContador = 0;
				waypointsCapturados.clear();
			}
			else
			{
				printf("\nMODO CAPTURA DESACTIVADO\n");
				printf("Total de puntos capturados: %d\n", capturaContador);
				printf("Copia las lineas de arriba y reemplazalas en pistaWaypoints[]\n");
			}
		}
		teclaCAnterior = teclaCActual;

		// Guarda un punto de la ruta
		bool teclaPActual = mainWindow.getsKeys()[GLFW_KEY_P];
		if (teclaPActual && !teclaPAnterior && modoCaptura)
		{
			glm::vec3 cp = camera.getCameraPosition();
			float yPiso = cp.y - 1.5f;
			glm::vec3 punto(cp.x, yPiso, cp.z);
			waypointsCapturados.push_back(punto);

			printf("\tglm::vec3(%.2ff, %.2ff, %.2ff),\n",
				punto.x, punto.y, punto.z);
			fflush(stdout);

			capturaContador++;
		}
		teclaPAnterior = teclaPActual;

		// Limpia la ruta capturada
		bool teclaLActual = mainWindow.getsKeys()[GLFW_KEY_L];
		if (teclaLActual && !teclaLAnterior && modoCaptura)
		{
			printf("\nLista limpiada, empezando de nuevo\n");
			capturaContador = 0;
			waypointsCapturados.clear();
		}
		teclaLAnterior = teclaLActual;

		// Movimiento del carro
		if (modoCaptura)
		{
			glm::vec3 cp = camera.getCameraPosition();
			carPos = glm::vec3(cp.x, cp.y - 1.5f, cp.z);

			glm::vec3 dirCam = camera.getCameraDirection();
			carYaw = atan2(dirCam.x, dirCam.z) / toRadians + 180.0f;
			if (glm::length(dirCam) > 0.001f)
				carDir = glm::normalize(dirCam);

			carPitch = 0.0f;
			carRoll = 0.0f;
		}
		else if (!carDetenido && pistaWaypoints.size() >= 2)
		{
			glm::vec3 wpDesde = pistaWaypoints[carWpActual];
			glm::vec3 wpHacia = pistaWaypoints[carWpActual + 1];
			glm::vec3 segmento = wpHacia - wpDesde;
			float longitudSeg = glm::length(segmento);

			if (longitudSeg < 0.001f)
			{
				carT = 0.0f;
				carWpActual++;

				if (carWpActual >= (int)pistaWaypoints.size() - 1)
				{
					carPos = pistaWaypoints.back();
					carDetenido = true;
					carWpActual = (int)pistaWaypoints.size() - 2;
				}
			}
			else
			{
				carT += (carVelAvance * deltaTime) / longitudSeg;

				if (carT >= 1.0f)
				{
					carT = 0.0f;
					carWpActual++;

					if (carWpActual >= (int)pistaWaypoints.size() - 1)
					{
						carPos = pistaWaypoints.back();
						carDetenido = true;
						carWpActual = (int)pistaWaypoints.size() - 2;
					}
					else
					{
						wpDesde = pistaWaypoints[carWpActual];
						wpHacia = pistaWaypoints[carWpActual + 1];
						segmento = wpHacia - wpDesde;
						longitudSeg = glm::length(segmento);
					}
				}

				if (!carDetenido && longitudSeg >= 0.001f)
				{
					carPos = wpDesde + segmento * carT;

					glm::vec3 dirSeg = glm::normalize(segmento);
					carDir = dirSeg;

					carYawObjetivo = atan2(dirSeg.x, dirSeg.z) / toRadians;
					carYaw = InterpolarYaw(carYaw, carYawObjetivo, carVelGiro, deltaTime);

					rotllanta += carVelLlantas * deltaTime;
					if (rotllanta >= 360.0f) rotllanta -= 360.0f;

					float horizontal = sqrt((dirSeg.x * dirSeg.x) + (dirSeg.z * dirSeg.z));
					carPitchObjetivo = atan2(dirSeg.y, horizontal) / toRadians;
					carRollObjetivo = 0.0f;

					carPitch = InterpolarSuave(carPitch, carPitchObjetivo, carVelInclinacion, deltaTime);
					carRoll = InterpolarSuave(carRoll, carRollObjetivo, carVelInclinacion, deltaTime);
				}
			}
		}

		// Movimiento de la nave
		if (!naveAterrizada && pistaWaypoints.size() >= 2)
		{
			if (!naveAterrizando)
			{
				glm::vec3 wpDesde = pistaWaypoints[naveWpActual];
				glm::vec3 wpHacia = pistaWaypoints[naveWpActual - 1];
				glm::vec3 segmento = wpHacia - wpDesde;
				float longitudSeg = glm::length(segmento);

				if (longitudSeg < 0.001f)
				{
					naveT = 0.0f;
					naveWpActual--;

					if (naveWpActual <= 0)
					{
						naveAterrizando = true;
						naveWpActual = 1;
						navePos = pistaWaypoints[0] + glm::vec3(0.0f, naveAlturaVuelo, 0.0f);
					}
				}
				else
				{
					naveT += (naveVelAvance * deltaTime) / longitudSeg;

					if (naveT >= 1.0f)
					{
						naveT = 0.0f;
						naveWpActual--;

						if (naveWpActual <= 0)
						{
							naveAterrizando = true;
							naveWpActual = 1;
							navePos = pistaWaypoints[0] + glm::vec3(0.0f, naveAlturaVuelo, 0.0f);
						}
						else
						{
							wpDesde = pistaWaypoints[naveWpActual];
							wpHacia = pistaWaypoints[naveWpActual - 1];
							segmento = wpHacia - wpDesde;
							longitudSeg = glm::length(segmento);
						}
					}

					if (!naveAterrizando && longitudSeg >= 0.001f)
					{
						glm::vec3 basePos = wpDesde + segmento * naveT;
						navePos = basePos + glm::vec3(0.0f, naveAlturaVuelo, 0.0f);

						glm::vec3 dirSeg = glm::normalize(segmento);
						naveDir = dirSeg;

						naveYawObjetivo = atan2(dirSeg.x, dirSeg.z) / toRadians;
						naveYaw = InterpolarYaw(naveYaw, naveYawObjetivo, naveVelGiro, deltaTime);

						float horizontal = sqrt((dirSeg.x * dirSeg.x) + (dirSeg.z * dirSeg.z));
						navePitchObjetivo = -atan2(dirSeg.y, horizontal) / toRadians;
						naveRollObjetivo = 0.0f;

						navePitch = InterpolarSuave(navePitch, navePitchObjetivo, naveVelInclinacion, deltaTime);
						naveRoll = InterpolarSuave(naveRoll, naveRollObjetivo, naveVelInclinacion, deltaTime);
					}
				}
			}
			else
			{
				naveAterrizajeT += deltaTime * 0.45f;
				if (naveAterrizajeT >= 1.0f)
				{
					naveAterrizajeT = 1.0f;
					naveAterrizada = true;
				}

				glm::vec3 inicio = pistaWaypoints[0];
				float yInicial = inicio.y + naveAlturaVuelo;
				float yFinal = inicio.y + naveAlturaAterrizada;

				navePos = glm::vec3(
					inicio.x,
					yInicial + (yFinal - yInicial) * naveAterrizajeT,
					inicio.z
				);

				navePitch = InterpolarSuave(navePitch, 0.0f, naveVelInclinacion, deltaTime);
				naveRoll = InterpolarSuave(naveRoll, 0.0f, naveVelInclinacion, deltaTime);
			}
		}

		// Animacion propia de la nave
		naveRotHelice += naveVelHelice * deltaTime;
		if (naveRotHelice >= 360.0f)
			naveRotHelice -= 360.0f;

		naveOscAla += naveVelAla * deltaTime;
		naveAngAla = OscilarEntre(naveAlaAtras, naveAlaFrente, naveOscAla);

		// Actualiza las luces
		glm::vec3 dirCarroHorizontal = glm::vec3(carDir.x, 0.0f, carDir.z);
		if (glm::length(dirCarroHorizontal) > 0.001f)
			dirCarroHorizontal = glm::normalize(dirCarroHorizontal);
		else
			dirCarroHorizontal = glm::vec3(0.0f, 0.0f, 1.0f);

		glm::vec3 posFaroCarro = carPos + glm::vec3(0.0f, carYOffset + 1.65f, 0.0f) + dirCarroHorizontal * 2.3f;
		glm::vec3 dirFaroCarro = glm::normalize(dirCarroHorizontal + glm::vec3(0.0f, -0.25f, 0.0f));
		spotLights[2].SetFlash(posFaroCarro, dirFaroCarro);

		glm::vec3 dirNaveHorizontal = glm::vec3(naveDir.x, 0.0f, naveDir.z);
		if (glm::length(dirNaveHorizontal) > 0.001f)
			dirNaveHorizontal = glm::normalize(dirNaveHorizontal);
		else
			dirNaveHorizontal = glm::vec3(0.0f, 0.0f, -1.0f);

		glm::vec3 posFaroNave = navePos + dirNaveHorizontal * 2.2f + glm::vec3(0.0f, -0.2f, 0.0f);
		glm::vec3 dirFaroNave = glm::normalize(dirNaveHorizontal * 0.55f + glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[3].SetFlash(posFaroNave, dirFaroNave);

		debugTimer += deltaTime;
		if (debugTimer >= 0.1f && !modoCaptura)
		{
			debugTimer = 0.0f;
			glm::vec3 camPos = camera.getCameraPosition();
			printf("\rCAM(%.1f,%.1f,%.1f) | CAR(%.1f,%.1f,%.1f) wp:%d %s | NAVE(%.1f,%.1f,%.1f) wp:%d %s     ",
				camPos.x, camPos.y, camPos.z,
				carPos.x, carPos.y, carPos.z, carWpActual, carDetenido ? "[STOP]" : "[GO]",
				navePos.x, navePos.y, navePos.z, naveWpActual,
				naveAterrizada ? "[LANDED]" : (naveAterrizando ? "[LANDING]" : "[FLY]"));
			fflush(stdout);
		}

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
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition,
			camera.getCameraPosition().x,
			camera.getCameraPosition().y,
			camera.getCameraPosition().z);

		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		model = glm::mat4(1.0f);
		modelaux = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		// Piso
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		// Pista
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.1f, 2.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pista_M.RenderModel();

		// Dibujo del carro
		model = glm::mat4(1.0f);
		model = glm::translate(model, carPos + glm::vec3(0.0f, carYOffset, 0.0f));
		model = glm::rotate(model, (carYaw + carYawVisualOffset) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, carPitch * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, carRoll * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Cuerpo del carro
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(carEscala, carEscala, carEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Carrotex_M.RenderModel();

		// Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.87f, -2.65f));
		model = glm::scale(model, glm::vec3(carEscala, carEscala, carEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofretex_M.RenderModel();

		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		// Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, -3.85f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(carEscala, carEscala, carEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantatexsupder_M.RenderModel();

		// Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, -3.85f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(carEscala, carEscala, carEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantatexsupizq_M.RenderModel();

		// Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 2.50f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(carEscala, carEscala, carEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantatexinfder_M.RenderModel();

		// Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 2.50f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(carEscala, carEscala, carEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantatexinfizq_M.RenderModel();

		// Dibujo de la nave
		model = glm::mat4(1.0f);
		model = glm::translate(model, navePos);
		model = glm::rotate(model, naveYaw * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, navePitch * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, naveRoll * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 360.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;

		// Cuerpo de la nave
		model = modelaux;
		model = glm::translate(model, naveOffsetCuerpo);
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();

		// Ala izquierda
		model = modelaux;
		model = glm::translate(model, naveOffsetAlaIzq);
		model = glm::rotate(model, naveRotAlaIzq.x * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (naveRotAlaIzq.y + naveAngAla) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, naveRotAlaIzq.z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_M.RenderModel();

		// Ala derecha
		model = modelaux;
		model = glm::translate(model, naveOffsetAlaDer);
		model = glm::rotate(model, naveRotAlaDer.x * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (naveRotAlaDer.y - naveAngAla) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, naveRotAlaDer.z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala2_M.RenderModel();

		// Helice izquierda
		model = modelaux;
		model = glm::translate(model, naveOffsetHeliceIzq);
		model = glm::rotate(model, (naveRotHeliceIzq.x + naveRotHelice) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, naveRotHeliceIzq.y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, naveRotHeliceIzq.z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HeliceIzq_M.RenderModel();

		// Helice derecha
		model = modelaux;
		model = glm::translate(model, naveOffsetHeliceDer);
		model = glm::rotate(model, (naveRotHeliceDer.x + naveRotHelice) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, naveRotHeliceDer.y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, naveRotHeliceDer.z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HeliceDer_M.RenderModel();

		// Objeto decorativo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_base_M.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_M.RenderModel();

		// Planta
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		// Flecha animada
		toffsetflechau += flechaVelU * deltaTime;
		toffsetflechav = 0.0f;
		if (toffsetflechau > 1.0f)
			toffsetflechau -= 1.0f;

		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		// Panel de numeros
		toffsetnumerou = 0.0f;
		toffsetnumerov = 0.0f;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		// Numero fijo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f, 0.0f)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		for (int i = 1; i < 4; i++)
		{
			toffsetnumerou = i * 0.25f;
			toffsetnumerov = 0.0f;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0f), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();
		}

		for (int j = 1; j < 5; j++)
		{
			toffsetnumerou = (j - 1) * 0.25f;
			toffsetnumerov = -0.33f;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0f), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();
		}

		// Numero animado
		numeroTimer += deltaTime;
		if (numeroTimer >= numeroCambioIntervalo)
		{
			numeroTimer -= numeroCambioIntervalo;
			frameNumero = (frameNumero + 1) % 4;
		}

		toffsetnumerocambiau = frameNumero * 0.25f;
		toffsetnumerov = 0.0f;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		glDisable(GL_BLEND);
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}