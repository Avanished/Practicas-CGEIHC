/*
Práctica 8: Iluminación 2
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
#include <gtc\\matrix_transform.hpp>
#include <gtc\\type_ptr.hpp>

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

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dado8Texture;
Texture AguaTexture;
Texture VidrioTexture;

Model Carrotex_M;
Model Cofretex_M;
Model Llantatexsupder_M;
Model Llantatexsupizq_M;
Model Llantatexinfder_M;
Model Llantatexinfizq_M;
Model Nave_M;
Model Lamparatex_M;

Model PezCuerpo_M;
Model PezFoco_M;
Model PezAntena_M;

Skybox skybox;

Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

bool lamparaEncendida = true;
bool teclaLamparaPresionada = false;

bool luzBulboPezEncendida = true;
bool teclaBulboPezPresionada = false;

int colorFaro = 0;
bool teclaFaroPresionada = false;

glm::vec3 coloresFaro[] = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 0.0f, 1.0f)
};

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

const int MESH_PECERA_VIDRIO = 5;
const int MESH_PECERA_AGUA = 6;

// ===== PECERA =====
glm::vec3 posPecera = glm::vec3(10.0f, 1.2f, -14.0f);
const GLfloat peceraEscala = 2.10f;
const GLfloat peceraRotY = -90.0f;

const GLfloat peceraHX = 1.70f * peceraEscala;
const GLfloat peceraHY = 1.70f * peceraEscala;
const GLfloat peceraHZ = 2.80f * peceraEscala;

// ===== PEZ =====
glm::vec3 pezPosLocal = glm::vec3(0.0f, -0.8f, 0.0f);
GLfloat pezScale = 0.08f;
GLfloat pezRotX = 0.0f;
GLfloat pezRotY = 0.0f;
GLfloat pezRotZ = 0.0f;

glm::vec3 pezOffsetCuerpo = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 pezOffsetFoco = glm::vec3(0.08f, 1.205f, 0.34f);
glm::vec3 pezOffsetAntena = glm::vec3(0.10f, 0.5f, -0.19f);

GLfloat pezCuerpoRotX = 0.0f;
GLfloat pezCuerpoRotY = 0.0f;
GLfloat pezCuerpoRotZ = 0.0f;

GLfloat pezFocoRotX = 0.0f;
GLfloat pezFocoRotY = 0.0f;
GLfloat pezFocoRotZ = 0.0f;

GLfloat pezAntenaRotX = 0.0f;
GLfloat pezAntenaRotY = 0.0f;
GLfloat pezAntenaRotZ = 0.0f;

GLfloat pezCuerpoScale = 1.0f;
GLfloat pezFocoScale = 1.0f;
GLfloat pezAntenaScale = 1.0f;

GLfloat velocidadPez = 0.18f;

GLfloat spotPezYaw = 0.0f;
GLfloat spotPezPitch = 0.0f;
GLfloat velocidadSpotPez = 8.0f;

// ===== AGAVE =====
glm::vec3 posAgave = glm::vec3(0.0f, 1.0f, -4.0f);
GLfloat agaveScale = 4.0f;

// ===== POSICIÓN DE LA LÁMPARA =====
glm::vec3 posLampara = glm::vec3(-6.0f, -0.5f, -8.0f);

GLfloat clampf(GLfloat v, GLfloat minV, GLfloat maxV)
{
	if (v < minV) return minV;
	if (v > maxV) return maxV;
	return v;
}

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;

		glm::vec3 v1(
			vertices[in1] - vertices[in0],
			vertices[in1 + 1] - vertices[in0 + 1],
			vertices[in1 + 2] - vertices[in0 + 2]
		);

		glm::vec3 v2(
			vertices[in2] - vertices[in0],
			vertices[in2 + 1] - vertices[in0 + 1],
			vertices[in2 + 2] - vertices[in0 + 2]
		);

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

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	static GLfloat vertices[] = {
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f,  1.0f,	0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -0.6f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,	0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
	0, 2, 1,
	1, 2, 3
	};

	// Plano principal del piso
	static GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		 10.0f, 0.0f, -10.0f,  10.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		-10.0f, 0.0f,  10.0f,   0.0f,10.0f,   0.0f, 1.0f, 0.0f,
		 10.0f, 0.0f,  10.0f,  10.0f,10.0f,   0.0f, 1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7
	};

	// Cruz de planos para vegetación
	static GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f,  0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.0f,	1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.0f,	0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		 0.0f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  0.5f,  0.5f,	1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f, 0.0f, 0.0f
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

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado8()
{
	unsigned int oct_indices[] = {
		0,  2,  1,
		3,  5,  4,
		6,  8,  7,
		9,  11, 10,
		12, 14, 13,
		15, 17, 16,
		18, 20, 19,
		21, 23, 22,
	};

	static GLfloat oct_vertices[] = {
		 0.0f,  1.0f,  0.0f,  0.4920f, 0.7431f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,  0.2580f, 0.4982f,  0.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,  0.7240f, 0.5030f,  0.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,  0.0180f, 0.2545f,  0.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,  0.4820f, 0.2545f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,  0.2540f, 0.4947f,  0.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,  0.0220f, 0.7406f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,  0.2520f, 0.4970f,  0.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,  0.4740f, 0.7406f,  0.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,  0.4920f, 0.2581f,  0.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,  0.7140f, 0.4970f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,  0.2640f, 0.4922f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.7280f, 0.9808f,  0.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,  0.5080f, 0.7491f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,  0.9520f, 0.7467f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.9440f, 0.2581f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,  0.5080f, 0.2545f,  0.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,  0.7320f, 0.4850f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.9540f, 0.7370f,  0.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,  0.5120f, 0.7382f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,  0.7320f, 0.5042f,  0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,  0.7320f, 0.0228f,  0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,  0.5140f, 0.2448f,  0.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,  0.9540f, 0.2448f,  0.0f,  0.0f,  0.0f,
	};

	calcAverageNormals(oct_indices, 24, oct_vertices, 192, 8, 5);

	Mesh* dado8 = new Mesh();
	dado8->CreateMesh(oct_vertices, oct_indices, 192, 24);
	meshList.push_back(dado8);
}

void CrearPecera()
{
	const GLfloat hx = peceraHX;
	const GLfloat hy = peceraHY;
	const GLfloat hz = peceraHZ;

	const GLfloat hxAgua = hx - 0.05f;
	const GLfloat hzAgua = hz - 0.05f;
	const GLfloat yAgua = hy - 0.08f;

	const GLfloat guL = 35.0f / 576.0f;
	const GLfloat guR = 548.0f / 576.0f;
	const GLfloat gvT = 1.0f - (60.0f / 576.0f);
	const GLfloat gvB = 1.0f - (496.0f / 576.0f);

	const GLfloat auL = 58.0f / 576.0f;
	const GLfloat auR = 519.0f / 576.0f;
	const GLfloat avT = 1.0f - (63.0f / 576.0f);
	const GLfloat avB = 1.0f - (478.0f / 576.0f);

	unsigned int vidrioIndices[] = {
		0, 1, 2,   0, 2, 3,
		4, 5, 6,   4, 6, 7,
		8, 9, 10,  8, 10, 11,
		12, 13, 14, 12, 14, 15
	};

	// Vidrio de la pecera:
	// cara frontal, cara trasera, pared izquierda y pared derecha
	GLfloat vidrioVertices[] = {
		-hx, -hy,  hz,   guL, gvB,   0.0f,  0.0f, -1.0f,
		 hx, -hy,  hz,   guR, gvB,   0.0f,  0.0f, -1.0f,
		 hx,  hy,  hz,   guR, gvT,   0.0f,  0.0f, -1.0f,
		-hx,  hy,  hz,   guL, gvT,   0.0f,  0.0f, -1.0f,

		 hx, -hy, -hz,   guL, gvB,   0.0f,  0.0f,  1.0f,
		-hx, -hy, -hz,   guR, gvB,   0.0f,  0.0f,  1.0f,
		-hx,  hy, -hz,   guR, gvT,   0.0f,  0.0f,  1.0f,
		 hx,  hy, -hz,   guL, gvT,   0.0f,  0.0f,  1.0f,

		-hx, -hy, -hz,   guL, gvB,   1.0f,  0.0f,  0.0f,
		-hx, -hy,  hz,   guR, gvB,   1.0f,  0.0f,  0.0f,
		-hx,  hy,  hz,   guR, gvT,   1.0f,  0.0f,  0.0f,
		-hx,  hy, -hz,   guL, gvT,   1.0f,  0.0f,  0.0f,

		 hx, -hy,  hz,   guL, gvB,  -1.0f,  0.0f,  0.0f,
		 hx, -hy, -hz,   guR, gvB,  -1.0f,  0.0f,  0.0f,
		 hx,  hy, -hz,   guR, gvT,  -1.0f,  0.0f,  0.0f,
		 hx,  hy,  hz,   guL, gvT,  -1.0f,  0.0f,  0.0f
	};

	unsigned int aguaIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	// Plano superior del agua dentro de la pecera
	GLfloat aguaVertices[] = {
		-hxAgua, yAgua, -hzAgua,   auL, avB,   0.0f, 1.0f, 0.0f,
		 hxAgua, yAgua, -hzAgua,   auR, avB,   0.0f, 1.0f, 0.0f,
		 hxAgua, yAgua,  hzAgua,   auR, avT,   0.0f, 1.0f, 0.0f,
		-hxAgua, yAgua,  hzAgua,   auL, avT,   0.0f, 1.0f, 0.0f
	};

	Mesh* peceraVidrio = new Mesh();
	peceraVidrio->CreateMesh(vidrioVertices, vidrioIndices, 16 * 8, 24);
	meshList.push_back(peceraVidrio);

	Mesh* peceraAgua = new Mesh();
	peceraAgua->CreateMesh(aguaVertices, aguaIndices, 4 * 8, 6);
	meshList.push_back(peceraAgua);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CrearDado8();
	CrearPecera();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-60.0f, 0.0f, 0.3f, 0.5f);

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
	dado8Texture = Texture("Textures/dado8.jpg");
	dado8Texture.LoadTextureA();
	AguaTexture = Texture("Textures/Agua.tga");
	AguaTexture.LoadTextureA();
	VidrioTexture = Texture("Textures/vidrio.tga");
	VidrioTexture.LoadTextureA();

	// Modelos del carro
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

	// Modelo de la nave
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");

	// Modelo de la lámpara
	Lamparatex_M = Model();
	Lamparatex_M.LoadModel("Models/Lamparatext.obj");

	// Modelos del pez: cuerpo, bulbo y antena
	PezCuerpo_M = Model();
	PezCuerpo_M.LoadModel("Models/cuerpo_pez.obj");
	PezFoco_M = Model();
	PezFoco_M.LoadModel("Models/foco_pez.obj");
	PezAntena_M = Model();
	PezAntena_M.LoadModel("Models/antena_pez.obj");

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

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	unsigned int pointLightCount = 0;

	// Luz puntual roja fija en la escena
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	// Luz puntual de la lámpara
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.2f, 3.0f,
		posLampara.x, posLampara.y + 1.0f, posLampara.z,
		1.0f, 0.7f, 1.8f);
	pointLightCount++;

	// Luz puntual azul ligada al bulbo del pez
	pointLights[2] = PointLight(0.0f, 0.0f, 1.0f,
		0.1f, 3.0f,
		posPecera.x, posPecera.y, posPecera.z,
		1.0f, 0.7f, 1.8f);
	pointLightCount++;

	unsigned int spotLightCount = 0;

	// Spotlight de la cámara
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	// Spotlight verde fijo
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	// Faro del carro
	spotLights[2] = SpotLight(1.0f, 0.0f, 0.0f,
		0.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -0.05f, -1.0f,
		1.0f, 0.0f, 0.0f,
		22.0f);
	spotLightCount++;

	// Spotlight frontal de la nave
	spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, -100.0f, 6.0f,
		1.0f, -1.5f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	// Spotlight trasero de la nave
	spotLights[4] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, -100.0f, 6.0f,
		-1.0f, -1.5f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	// Spotlight del pez
	spotLights[5] = SpotLight(1.0f, 1.0f, 0.0f,
		0.0f, 2.0f,
		posPecera.x, posPecera.y, posPecera.z,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		18.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;

	glm::mat4 projection = glm::perspective(
		45.0f,
		(GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(),
		0.1f, 1000.0f
	);

	GLfloat now = 0.0f;

	bool avanza = false, retrocede = false, sube = false, baja = false;
	GLfloat deltaZ = 0.0f, deltaY = 0.0f;

	GLfloat yawRad = 0.0f, pitchRad = 0.0f;
	glm::vec3 dirSpotLocal = glm::vec3(0.0f);
	glm::vec3 dirSpotWorld = glm::vec3(0.0f);

	PointLight pointLightsActivas[MAX_POINT_LIGHTS];
	unsigned int pLightCount = 0;

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 modelaux = glm::mat4(1.0f);
	glm::mat4 modelLampara = glm::mat4(1.0f);
	glm::mat4 modelNave = glm::mat4(1.0f);
	glm::mat4 peceraBase = glm::mat4(1.0f);
	glm::mat4 pezBaseNoScale = glm::mat4(1.0f);
	glm::mat4 modelCuerpo = glm::mat4(1.0f);
	glm::mat4 modelFoco = glm::mat4(1.0f);
	glm::mat4 modelAntena = glm::mat4(1.0f);

	glm::vec3 bulboWorld = glm::vec3(0.0f);
	glm::vec3 lowerLight = glm::vec3(0.0f);
	glm::vec3 posFaroCoche = glm::vec3(0.0f);
	glm::vec3 dirFaroCoche = glm::vec3(0.0f);
	glm::vec3 lamparaLightWorld = glm::vec3(0.0f);
	glm::vec3 naveSpotPosFrontal = glm::vec3(0.0f);
	glm::vec3 naveSpotPosTrasero = glm::vec3(0.0f);
	glm::vec3 naveSpotPosOff = glm::vec3(0.0f);
	GLfloat naveX = 0.0f;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec4 localPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 localDir = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	while (!mainWindow.getShouldClose())
	{
		now = (GLfloat)glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (GLfloat)((now - lastTime) / limitFPS);
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Encendido independiente de la lámpara
		if (mainWindow.getsKeys()[GLFW_KEY_1])
		{
			if (!teclaLamparaPresionada)
			{
				lamparaEncendida = !lamparaEncendida;
				teclaLamparaPresionada = true;
			}
		}
		else
		{
			teclaLamparaPresionada = false;
		}

		// Encendido independiente de la luz azul del pez
		if (mainWindow.getsKeys()[GLFW_KEY_2])
		{
			if (!teclaBulboPezPresionada)
			{
				luzBulboPezEncendida = !luzBulboPezEncendida;
				teclaBulboPezPresionada = true;
			}
		}
		else
		{
			teclaBulboPezPresionada = false;
		}

		// Cambio de color del faro del carro
		if (mainWindow.getsKeys()[GLFW_KEY_F])
		{
			if (!teclaFaroPresionada)
			{
				colorFaro = (colorFaro + 1) % 6;
				teclaFaroPresionada = true;
			}
		}
		else
		{
			teclaFaroPresionada = false;
		}

		// Movimiento del pez dentro de la pecera
		avanza = mainWindow.getsKeys()[GLFW_KEY_I];
		retrocede = mainWindow.getsKeys()[GLFW_KEY_K];
		sube = mainWindow.getsKeys()[GLFW_KEY_U];
		baja = mainWindow.getsKeys()[GLFW_KEY_O];

		deltaZ = 0.0f;
		deltaY = 0.0f;

		if (sube)
		{
			deltaY += velocidadPez * deltaTime;
			deltaZ -= velocidadPez * deltaTime;
		}
		else if (baja)
		{
			deltaY -= velocidadPez * deltaTime;
			deltaZ -= velocidadPez * deltaTime;
		}
		else
		{
			if (avanza)
				deltaZ -= velocidadPez * deltaTime;
			if (retrocede)
				deltaZ += velocidadPez * deltaTime;
		}

		pezPosLocal.z += deltaZ;
		pezPosLocal.y += deltaY;

		pezPosLocal.z = clampf(pezPosLocal.z, -peceraHZ + 0.9f, peceraHZ - 0.9f);
		pezPosLocal.y = clampf(pezPosLocal.y, -peceraHY + 0.8f, peceraHY - 1.0f);

		// Dirección del spotlight del pez
		if (mainWindow.getsKeys()[GLFW_KEY_LEFT])  spotPezYaw -= velocidadSpotPez * deltaTime;
		if (mainWindow.getsKeys()[GLFW_KEY_RIGHT]) spotPezYaw += velocidadSpotPez * deltaTime;
		if (mainWindow.getsKeys()[GLFW_KEY_UP])    spotPezPitch += velocidadSpotPez * deltaTime;
		if (mainWindow.getsKeys()[GLFW_KEY_DOWN])  spotPezPitch -= velocidadSpotPez * deltaTime;

		spotPezPitch = clampf(spotPezPitch, -80.0f, 80.0f);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();

		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition,
			camera.getCameraPosition().x,
			camera.getCameraPosition().y,
			camera.getCameraPosition().z);

		// Base de la pecera en la escena
		peceraBase = glm::mat4(1.0f);
		peceraBase = glm::translate(peceraBase, posPecera);
		peceraBase = glm::rotate(peceraBase, peceraRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		// Base del pez dentro de la pecera
		pezBaseNoScale = peceraBase;
		pezBaseNoScale = glm::translate(pezBaseNoScale, pezPosLocal);
		pezBaseNoScale = glm::rotate(pezBaseNoScale, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		pezBaseNoScale = glm::rotate(pezBaseNoScale, pezRotX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		pezBaseNoScale = glm::rotate(pezBaseNoScale, pezRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		pezBaseNoScale = glm::rotate(pezBaseNoScale, pezRotZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		// Posición global del bulbo del pez
		bulboWorld = glm::vec3(pezBaseNoScale * glm::vec4(pezOffsetFoco, 1.0f));

		// Spotlight ligado a la cámara
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		// Base del carro en la escena
		modelaux = glm::mat4(1.0f);
		modelaux = glm::translate(modelaux, glm::vec3(-2.0f + mainWindow.getmuevex(), -0.2f, 1.0f));
		modelaux = glm::rotate(modelaux, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		// Posición y dirección del faro del carro
		localPos = glm::vec4(-1.5f, 0.7f, -4.0f, 1.0f);
		posFaroCoche = glm::vec3(modelaux * localPos);

		localDir = glm::vec4(0.0f, -0.05f, -1.0f, 0.0f);
		dirFaroCoche = glm::normalize(glm::vec3(modelaux * localDir));

		spotLights[2].SetFlash(posFaroCoche, dirFaroCoche);
		spotLights[2].SetColor(coloresFaro[colorFaro].r, coloresFaro[colorFaro].g, coloresFaro[colorFaro].b);

		// Transformación de la nave
		naveX = 0.0f + mainWindow.getmuevehelicoptero();

		glm::mat4 modelNaveRender = glm::mat4(1.0f);
		modelNaveRender = glm::translate(modelNaveRender, glm::vec3(naveX, 5.0f, 6.0f));
		modelNaveRender = glm::scale(modelNaveRender, glm::vec3(3.0f, 3.0f, 3.0f));
		modelNaveRender = glm::rotate(modelNaveRender, -180.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelNaveRender = glm::rotate(modelNaveRender, -180.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		modelNave = modelNaveRender;

		// Posiciones frontal y trasera de la nave para sus spotlights
		naveSpotPosFrontal = glm::vec3(modelNaveRender * glm::vec4(0.7f, 0.0f, 0.0f, 1.0f));
		naveSpotPosTrasero = glm::vec3(modelNaveRender * glm::vec4(-0.7f, 0.0f, 0.0f, 1.0f));
		naveSpotPosOff = glm::vec3(modelNaveRender * glm::vec4(0.0f, -35.0f, 0.0f, 1.0f));

		glm::vec3 dirFrenteLocal = glm::normalize(glm::vec3(1.0f, -1.5f, 0.0f));
		glm::vec3 dirAtrasLocal = glm::normalize(glm::vec3(-1.0f, -1.5f, 0.0f));

		glm::vec3 dirFrenteWorld = glm::normalize(glm::mat3(modelNaveRender) * dirFrenteLocal);
		glm::vec3 dirAtrasWorld = glm::normalize(glm::mat3(modelNaveRender) * dirAtrasLocal);

		// Activación de los spotlights de la nave
		if (mainWindow.getsKeys()[GLFW_KEY_H])
		{
			spotLights[3].SetFlash(naveSpotPosOff, dirFrenteWorld);
			spotLights[4].SetFlash(naveSpotPosTrasero, dirAtrasWorld);
		}
		else if (mainWindow.getsKeys()[GLFW_KEY_J])
		{
			spotLights[3].SetFlash(naveSpotPosFrontal, dirFrenteWorld);
			spotLights[4].SetFlash(naveSpotPosOff, dirAtrasWorld);
		}
		else
		{
			spotLights[3].SetFlash(naveSpotPosOff, dirFrenteWorld);
			spotLights[4].SetFlash(naveSpotPosOff, dirAtrasWorld);
		}

		// Base de la lámpara
		modelLampara = glm::mat4(1.0f);
		modelLampara = glm::translate(modelLampara, posLampara);

		// Actualización de la luz de la lámpara
		lamparaLightWorld = glm::vec3(modelLampara * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		pointLights[1].SetPos(lamparaLightWorld.x, lamparaLightWorld.y, lamparaLightWorld.z);

		// Actualización de la luz azul del pez
		pointLights[2].SetPos(bulboWorld.x, bulboWorld.y, bulboWorld.z);

		// Dirección del spotlight del pez a partir de yaw y pitch
		yawRad = spotPezYaw * toRadians;
		pitchRad = spotPezPitch * toRadians;

		dirSpotLocal.x = sin(yawRad) * cos(pitchRad);
		dirSpotLocal.y = sin(pitchRad);
		dirSpotLocal.z = cos(yawRad) * cos(pitchRad);
		dirSpotLocal = glm::normalize(dirSpotLocal);

		dirSpotWorld = glm::normalize(glm::mat3(pezBaseNoScale) * dirSpotLocal);
		spotLights[5].SetFlash(bulboWorld, dirSpotWorld);

		// Arreglo de luces puntuales activas
		pLightCount = 0;
		pointLightsActivas[pLightCount++] = pointLights[0];

		if (lamparaEncendida)
			pointLightsActivas[pLightCount++] = pointLights[1];

		if (luzBulboPezEncendida)
			pointLightsActivas[pLightCount++] = pointLights[2];

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLightsActivas, pLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// Piso principal
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Cuerpo principal del carro
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Carrotex_M.RenderModel();

		// Cofre del carro
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
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
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

		// Nave
		model = modelNave;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();

		// Dado de la escena
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dado8Texture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		// Lámpara
		model = modelLampara;
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lamparatex_M.RenderModel();

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		// Cuerpo del pez
		modelCuerpo = pezBaseNoScale;
		modelCuerpo = glm::translate(modelCuerpo, pezOffsetCuerpo);
		modelCuerpo = glm::rotate(modelCuerpo, pezCuerpoRotX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelCuerpo = glm::rotate(modelCuerpo, pezCuerpoRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelCuerpo = glm::rotate(modelCuerpo, pezCuerpoRotZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelCuerpo = glm::scale(modelCuerpo, glm::vec3(pezScale * pezCuerpoScale));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCuerpo));
		PezCuerpo_M.RenderModel();

		// Bulbo del pez
		modelFoco = pezBaseNoScale;
		modelFoco = glm::translate(modelFoco, pezOffsetFoco);
		modelFoco = glm::rotate(modelFoco, pezFocoRotX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelFoco = glm::rotate(modelFoco, pezFocoRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelFoco = glm::rotate(modelFoco, pezFocoRotZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelFoco = glm::scale(modelFoco, glm::vec3(pezScale * pezFocoScale));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelFoco));
		PezFoco_M.RenderModel();

		// Antena del pez
		modelAntena = pezBaseNoScale;
		modelAntena = glm::translate(modelAntena, pezOffsetAntena);
		modelAntena = glm::rotate(modelAntena, pezAntenaRotX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelAntena = glm::rotate(modelAntena, pezAntenaRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelAntena = glm::rotate(modelAntena, pezAntenaRotZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelAntena = glm::scale(modelAntena, glm::vec3(pezScale * pezAntenaScale));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelAntena));
		PezAntena_M.RenderModel();

		// Agave
		model = glm::mat4(1.0f);
		model = glm::translate(model, posAgave);
		model = glm::scale(model, glm::vec3(agaveScale, agaveScale, agaveScale));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		// Pecera: agua y vidrio
		model = glm::mat4(1.0f);
		model = glm::translate(model, posPecera);
		model = glm::rotate(model, peceraRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);

		// Plano superior del agua
		AguaTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[MESH_PECERA_AGUA]->RenderMesh();

		// Vidrio de la pecera
		glEnable(GL_CULL_FACE);
		VidrioTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		glCullFace(GL_FRONT);
		meshList[MESH_PECERA_VIDRIO]->RenderMesh();

		glCullFace(GL_BACK);
		meshList[MESH_PECERA_VIDRIO]->RenderMesh();

		glDisable(GL_CULL_FACE);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}