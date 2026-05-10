// Animacion por Keyframes
// Ejercicio: la nave recorre un movimiento senoidal hacia la derecha,
// gira 180 grados al llegar al extremo, regresa por el mismo camino y
// vuelve a girar para terminar en su orientacion original.
// Las alas aletean y las helices giran de forma continua durante todo el recorrido.

// Para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <fstream>


#ifdef _WIN32
#include <windows.h>
#endif

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

// Para iluminacion
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

// Si es true se cargan los 23 keyframes ya definidos en codigo.
// Si es false se inicia con un solo keyframe y los demas se crean por teclado.
const bool USAR_KEYFRAMES_PRECARGADOS = false;

// Variables para animacion del coche
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;

// Variables para animacion de texturas dinamicas
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0f;
float angulovaria = 0.0f;

// Animacion propia de la nave: helices y alas
float naveEscala = 1.0f;
float naveVelHelice = 7800.0f;
float naveVelAla = 20.5f;
float naveAlaFrente = 60.0f;
float naveAlaAtras = -5.0f;

float naveRotHelice = 0.0f;
float naveOscAla = 0.0f;
float naveAngAla = 0.0f;

// Posicion relativa de cada pieza con respecto al cuerpo de la nave
glm::vec3 naveOffsetCuerpo = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 naveOffsetAlaIzq = glm::vec3(0.2f, 0.0f, 0.4f);
glm::vec3 naveOffsetAlaDer = glm::vec3(0.2f, 0.0f, -0.4f);
glm::vec3 naveOffsetHeliceIzq = glm::vec3(0.42f, -0.28f, 0.37f);
glm::vec3 naveOffsetHeliceDer = glm::vec3(0.42f, -0.28f, -0.37f);

// Rotacion base de cada pieza
glm::vec3 naveRotAlaIzq = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 naveRotAlaDer = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 naveRotHeliceIzq = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 naveRotHeliceDer = glm::vec3(0.0f, 0.0f, 0.0f);

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

Model Kitt_M;
Model Llanta_M;
Model Pista_M;

// Modelos de la nave: cuerpo, ala izquierda, ala derecha, helice izquierda, helice derecha
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
static double limitFPS = 1.0 / 60.0;

// Declaracion adelantada de funciones de keyframes
void inputKeyframes(bool* keys);
void configurarConsolaUTF8();
void cargarKeyframesPrecargados();

// Luz direccional, solo 1 y siempre debe de existir
DirectionalLight mainLight;
// Para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

// Calculo del promedio de las normales para sombreado de Phong
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

// Devuelve un valor que oscila suavemente entre minimo y maximo
// usando una funcion seno parametrizada por el tiempo acumulado.
// Sirve para el aleteo de las alas (sube y baja sin cortes).
float OscilarEntre(float minimo, float maximo, float tiempo)
{
	float t = (sin(tiempo) + 1.0f) * 0.5f;
	return minimo + (maximo - minimo) * t;
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
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4, 5, 6,
	   4, 6, 7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,	0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,	0.0f, -1.0f, 0.0f,
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

// Habilita la salida UTF-8 en la consola de Windows para que los acentos
// y la letra n con virgulilla se muestren correctamente.
void configurarConsolaUTF8()
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif
}


// KEYFRAMES

bool animacion = false;

// NEW Keyframes
// Posicion base de la nave en el mundo
float posXavion = 2.0f, posYavion = 2.0f, posZavion = 0.0f;

// Desplazamientos relativos acumulados por teclado o por animacion
float movAvion_x = 0.0f, movAvion_y = 0.0f;

// Angulo de giro acumulado en grados sobre el eje Y
float giroAvion = 0.0f;

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 6;

// Estructura que representa un cuadro clave de la animacion.
// Cada cuadro guarda un estado (posicion y giro) y los incrementos
// que se usaran para interpolar hasta el siguiente cuadro.
typedef struct _frame
{
	// Variables para GUARDAR Key Frames
	float movAvion_x;		// Variable para PosicionX
	float movAvion_y;		// Variable para PosicionY
	float movAvion_xInc;	// Variable para IncrementoX
	float movAvion_yInc;	// Variable para IncrementoY
	float giroAvion;
	float giroAvionInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 1;
bool play = false;
int playIndex = 0;

// Guarda el estado actual de la nave como un nuevo keyframe (tecla L).
// No volatil, agregar una forma de escribir a un archivo para guardar los frames
void saveFrame(void)
{
	if (FrameIndex >= MAX_FRAMES)
	{
		printf("No hay espacio para mas frames\n");
		return;
	}

	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion = giroAvion;

	printf("Frame %d guardado: x=%.2f, y=%.2f, giro=%.2f\n",
		FrameIndex, movAvion_x, movAvion_y, giroAvion);

	// Guardar el frame tambien en un archivo de texto
	std::ofstream archivo("keyframes.txt", std::ios::app);
	if (archivo.is_open())
	{
		archivo << "Frame " << FrameIndex
			<< " | x=" << movAvion_x
			<< " | y=" << movAvion_y
			<< " | giro=" << giroAvion << "\n";
		archivo.close();
	}

	FrameIndex++;
}

// Restablece la nave al estado del primer keyframe (tecla 0 / R).
void resetElements(void)
{
	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
}

// Calcula los incrementos por paso para interpolar linealmente entre el
// keyframe actual (playIndex) y el siguiente (playIndex + 1).
void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc =
		(KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc =
		(KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc =
		(KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;
}

// Avanza la animacion un paso por frame mientras play sea true.
// Cuando i_curr_steps llega a i_max_steps, salta al siguiente tramo
// y recalcula la interpolacion. Al terminar todos los keyframes,
// detiene la animacion y restablece la nave al estado inicial.
void animate(void)
{
	// Movimiento del objeto - barra espaciadora
	if (play)
	{
		// Fin del tramo entre dos keyframes
		if (i_curr_steps >= i_max_steps)
		{
			playIndex++;

			// Fin de la animacion completa
			if (playIndex > FrameIndex - 2)
			{
				playIndex = 0;
				play = false;
				resetElements();
				printf("Animacion terminada\n");
			}
			else
			{
				// Siguiente tramo de interpolacion
				i_curr_steps = 0;
				interpolation();
			}
		}
		else
		{
			// Aplica el incremento de este paso
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}
	}
}

// Carga la secuencia completa de 23 keyframes que describen el recorrido:
// frames 0 a 8  -> onda senoidal de ida con giro 0 grados
// frames 9 a 11 -> giro de 180 grados en el extremo derecho
// frames 12 a 19 -> onda senoidal de regreso con giro 180 grados
// frames 20 a 22 -> giro final de 180 grados para cerrar el ciclo
void cargarKeyframesPrecargados()
{
	KeyFrame[0].movAvion_x = 0.0f;   KeyFrame[0].movAvion_y = 0.0f;   KeyFrame[0].giroAvion = 0.0f;
	KeyFrame[1].movAvion_x = 1.0f;   KeyFrame[1].movAvion_y = 2.0f;   KeyFrame[1].giroAvion = 0.0f;
	KeyFrame[2].movAvion_x = 2.0f;   KeyFrame[2].movAvion_y = 0.0f;   KeyFrame[2].giroAvion = 0.0f;
	KeyFrame[3].movAvion_x = 3.0f;   KeyFrame[3].movAvion_y = -2.0f;  KeyFrame[3].giroAvion = 0.0f;
	KeyFrame[4].movAvion_x = 4.0f;   KeyFrame[4].movAvion_y = 0.0f;   KeyFrame[4].giroAvion = 0.0f;
	KeyFrame[5].movAvion_x = 5.0f;   KeyFrame[5].movAvion_y = 2.0f;   KeyFrame[5].giroAvion = 0.0f;
	KeyFrame[6].movAvion_x = 6.0f;   KeyFrame[6].movAvion_y = 0.0f;   KeyFrame[6].giroAvion = 0.0f;
	KeyFrame[7].movAvion_x = 7.0f;   KeyFrame[7].movAvion_y = -2.0f;  KeyFrame[7].giroAvion = 0.0f;
	KeyFrame[8].movAvion_x = 8.0f;   KeyFrame[8].movAvion_y = 0.0f;   KeyFrame[8].giroAvion = 0.0f;
	KeyFrame[9].movAvion_x = 8.0f;   KeyFrame[9].movAvion_y = 0.0f;   KeyFrame[9].giroAvion = 60.0f;
	KeyFrame[10].movAvion_x = 8.0f;  KeyFrame[10].movAvion_y = 0.0f;  KeyFrame[10].giroAvion = 120.0f;
	KeyFrame[11].movAvion_x = 8.0f;  KeyFrame[11].movAvion_y = 0.0f;  KeyFrame[11].giroAvion = 180.0f;
	KeyFrame[12].movAvion_x = 7.0f;  KeyFrame[12].movAvion_y = -2.0f; KeyFrame[12].giroAvion = 180.0f;
	KeyFrame[13].movAvion_x = 6.0f;  KeyFrame[13].movAvion_y = 0.0f;  KeyFrame[13].giroAvion = 180.0f;
	KeyFrame[14].movAvion_x = 5.0f;  KeyFrame[14].movAvion_y = 2.0f;  KeyFrame[14].giroAvion = 180.0f;
	KeyFrame[15].movAvion_x = 4.0f;  KeyFrame[15].movAvion_y = 0.0f;  KeyFrame[15].giroAvion = 180.0f;
	KeyFrame[16].movAvion_x = 3.0f;  KeyFrame[16].movAvion_y = -2.0f; KeyFrame[16].giroAvion = 180.0f;
	KeyFrame[17].movAvion_x = 2.0f;  KeyFrame[17].movAvion_y = 0.0f;  KeyFrame[17].giroAvion = 180.0f;
	KeyFrame[18].movAvion_x = 1.0f;  KeyFrame[18].movAvion_y = 2.0f;  KeyFrame[18].giroAvion = 180.0f;
	KeyFrame[19].movAvion_x = 0.0f;  KeyFrame[19].movAvion_y = 0.0f;  KeyFrame[19].giroAvion = 180.0f;
	KeyFrame[20].movAvion_x = 0.0f;  KeyFrame[20].movAvion_y = 0.0f;  KeyFrame[20].giroAvion = 240.0f;
	KeyFrame[21].movAvion_x = 0.0f;  KeyFrame[21].movAvion_y = 0.0f;  KeyFrame[21].giroAvion = 300.0f;
	KeyFrame[22].movAvion_x = 0.0f;  KeyFrame[22].movAvion_y = 0.0f;  KeyFrame[22].giroAvion = 360.0f;

	FrameIndex = 23;
}

// FIN KEYFRAMES


int main()
{
	configurarConsolaUTF8();

	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

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

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");

	// Carga de la nave por piezas: cuerpo, ala izquierda, ala derecha,
	// helice izquierda y helice derecha. Cada pieza se rendea por separado
	// con sus propios offsets y rotaciones para permitir el aleteo y el giro.
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

	// Luz direccional, solo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	// Contador de luces puntuales
	unsigned int pointLightCount = 0;
	// Declaracion de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	// Linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	// Luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset(0.0f, 0.0f);

	// Keyframes
	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);

	// Inicializacion del arreglo de keyframes
	if (USAR_KEYFRAMES_PRECARGADOS)
	{
		cargarKeyframesPrecargados();
		printf("Modo: keyframes precargados (23 frames listos para reproducir)\n");
	}
	else
	{
		KeyFrame[0].movAvion_x = 0.0f;
		KeyFrame[0].movAvion_y = 0.0f;
		KeyFrame[0].giroAvion = 0.0f;
		FrameIndex = 1;
		printf("Modo: crear keyframes por teclado\n");
	}

	printf("\nTeclas de la animacion:\n");
	printf("ESPACIO : reproducir / pausar animacion\n");
	printf("0       : habilitar reproducir de nuevo\n");
	printf("L       : guardar frame actual\n");
	printf("1       : mover X +1\n");
	printf("2       : mover X -1\n");
	printf("3       : mover Y +2\n");
	printf("4       : mover Y -2\n");
	printf("5       : girar +60 grados\n");
	printf("R       : reiniciar a la posicion inicial\n\n");



	// Borrar el archivo de keyframes anterior al iniciar
	std::ofstream limpiar("keyframes.txt", std::ios::trunc);
	limpiar.close();

	lastTime = glfwGetTime();

	// Loop principal mientras no se cierre la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;

		// Como haces para que el coche no se salga del piso?
		movCoche -= movOffset * deltaTime;
		rotllanta += rotllantaOffset * deltaTime;

		// Actualiza la rotacion continua de las helices y el aleteo de las alas.
		// Las helices giran a una velocidad alta y constante; las alas oscilan
		// suavemente entre el angulo trasero y el frontal con una funcion seno.
		naveRotHelice += naveVelHelice * deltaTime;
		if (naveRotHelice >= 360.0f)
			naveRotHelice -= 360.0f;

		naveOscAla += naveVelAla * deltaTime;
		naveAngAla = OscilarEntre(naveAlaAtras, naveAlaFrente, naveOscAla);

		// Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Limpieza de la ventana
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

		// Informacion en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// Luz ligada a la camara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		// Informacion al shader de fuentes de iluminacion
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// Reiniciar variables antes de que sean enviadas al shader
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		// Pista
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.1f, 2.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Pista_M.RenderModel();

		// Instancia del coche
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche - 50.0f, 0.5f, -2.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		// Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f); // Llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		// Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		// Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		// Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		// Aqui va la nave con jerarquia de modelos, completar
		// La matriz modelaux conserva la posicion y orientacion globales de la
		// nave que se controlan con los keyframes (movAvion_x, movAvion_y,
		// giroAvion). A partir de ella se rendea cada pieza con sus offsets
		// locales y, en el caso de las alas y las helices, con sus rotaciones
		// de animacion propia.
		model = glm::mat4(1.0);
		posblackhawk = glm::vec3(posXavion + movAvion_x, posYavion + movAvion_y, posZavion);
		model = glm::translate(model, posblackhawk);
		model = glm::rotate(model, (giroAvion)*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Cuerpo de la nave
		model = modelaux;
		model = glm::translate(model, naveOffsetCuerpo);
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();

		// Ala izquierda con aleteo
		model = modelaux;
		model = glm::translate(model, naveOffsetAlaIzq);
		model = glm::rotate(model, naveRotAlaIzq.x * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (naveRotAlaIzq.y + naveAngAla) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, naveRotAlaIzq.z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_M.RenderModel();

		// Ala derecha con aleteo opuesto al de la izquierda
		model = modelaux;
		model = glm::translate(model, naveOffsetAlaDer);
		model = glm::rotate(model, naveRotAlaDer.x * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (naveRotAlaDer.y - naveAngAla) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, naveRotAlaDer.z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala2_M.RenderModel();

		// Helice izquierda con rotacion continua sobre el eje X
		model = modelaux;
		model = glm::translate(model, naveOffsetHeliceIzq);
		model = glm::rotate(model, (naveRotHeliceIzq.x + naveRotHelice) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, naveRotHeliceIzq.y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, naveRotHeliceIzq.z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HeliceIzq_M.RenderModel();

		// Helice derecha con rotacion continua sobre el eje X
		model = modelaux;
		model = glm::translate(model, naveOffsetHeliceDer);
		model = glm::rotate(model, (naveRotHeliceDer.x + naveRotHelice) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, naveRotHeliceDer.y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, naveRotHeliceDer.z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(naveEscala, naveEscala, naveEscala));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HeliceDer_M.RenderModel();

		// Aeolipile
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, -0.5f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_base_M.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_M.RenderModel();

		// Modelos con blending al final para que no afecten a los demas objetos,
		// aunque tambien se pueden renderizar al inicio pero con blending
		// activado y desactivado.

		// Agave - que sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		// Blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		// Textura con movimiento
		// Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001f;
		toffsetflechav = 0.000f;
		// Para que no se desborde la variable
		if (toffsetflechau > 1.0f)
			toffsetflechau = 0.0f;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		// Pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
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

		// Plano con todos los numeros
		toffsetnumerou = 0.0f;
		toffsetnumerov = 0.0f;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
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

		// Numero 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		// Numeros 2 a 4
		for (int i = 1; i < 4; i++)
		{
			toffsetnumerou += 0.25f;
			toffsetnumerov = 0.0f;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
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

		// Numeros 5 a 8
		for (int j = 1; j < 5; j++)
		{
			toffsetnumerou += 0.25f;
			toffsetnumerov = -0.33f;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
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

		// Numero cambiante
		// Como hacer para que sea a una velocidad visible?
		toffsetnumerocambiau += 0.25f;
		if (toffsetnumerocambiau > 1.0f)
			toffsetnumerocambiau = 0.0f;
		toffsetnumerov = 0.0f;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = glm::mat4(1.0);
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

		// Cambiar automaticamente entre textura numero 1 y numero 2
		toffsetnumerou = 0.0f;
		toffsetnumerov = 0.0f;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Numero1Texture.UseTexture();
		//if
		//Numero1Texture.UseTexture();
		//Numero2Texture.UseTexture();

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

// Procesa el input de teclado para la animacion por keyframes.
// Aplica deteccion de flanco (curr && !prev) para que cada pulsacion
// se registre una sola vez aunque la tecla se mantenga presionada,
// evitando saturar la consola y los incrementos multiples por frame.
void inputKeyframes(bool* keys)
{
	// Estado anterior de cada tecla (persistente entre llamadas)
	static bool prevSpace = false;
	static bool prev0 = false;
	static bool prevL = false;
	static bool prev1 = false;
	static bool prev2 = false;
	static bool prev3 = false;
	static bool prev4 = false;
	static bool prev5 = false;
	static bool prevR = false;

	// Estado actual de cada tecla
	bool currSpace = keys[GLFW_KEY_SPACE];
	bool curr0 = keys[GLFW_KEY_0];
	bool currL = keys[GLFW_KEY_L];
	bool curr1 = keys[GLFW_KEY_1];
	bool curr2 = keys[GLFW_KEY_2];
	bool curr3 = keys[GLFW_KEY_3];
	bool curr4 = keys[GLFW_KEY_4];
	bool curr5 = keys[GLFW_KEY_5];
	bool currR = keys[GLFW_KEY_R];

	// ESPACIO - reproducir o pausar la animacion
	if (currSpace && !prevSpace)
	{
		if (!play && FrameIndex > 1)
		{
			resetElements();
			interpolation();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
			printf("Reproduciendo animacion con %d frames\n", FrameIndex);
		}
		else if (play)
		{
			play = false;
			printf("Animacion pausada\n");
		}
		else
		{
			printf("No hay frames suficientes para reproducir. Crea mas con la tecla L\n");
		}
	}

	// 0 - habilitar reproducir de nuevo
	if (curr0 && !prev0)
	{
		printf("Listo para reproducir de nuevo con ESPACIO\n");
	}

	// L - guardar el estado actual como un nuevo keyframe
	if (currL && !prevL)
	{
		saveFrame();
	}

	// 1 - mover en X
	if (curr1 && !prev1)
	{
		movAvion_x += 1.0f;
		printf("X = %.2f\n", movAvion_x);
	}

	// 2 - mover en X negativo
	if (curr2 && !prev2)
	{
		movAvion_x -= 1.0f;
		printf("X = %.2f\n", movAvion_x);
	}

	// 3 - mover en Y
	if (curr3 && !prev3)
	{
		movAvion_y += 2.0f;
		printf("Y = %.2f\n", movAvion_y);
	}

	// 4 - mover en Y negativo
	if (curr4 && !prev4)
	{
		movAvion_y -= 2.0f;
		printf("Y = %.2f\n", movAvion_y);
	}

	// 5 - acumular giro de 60 grados
	if (curr5 && !prev5)
	{
		giroAvion += 60.0f;
		printf("Giro = %.2f grados\n", giroAvion);
	}

	// R - reinicio manual al estado del primer keyframe
	if (currR && !prevR)
	{
		resetElements();
		play = false;
		printf("Reiniciado a la posicion inicial\n");
	}

	// Actualiza el estado anterior para la siguiente iteracion
	prevSpace = currSpace;
	prev0 = curr0;
	prevL = currL;
	prev1 = curr1;
	prev2 = curr2;
	prev3 = curr3;
	prev4 = curr4;
	prev5 = curr5;
	prevR = currR;
}