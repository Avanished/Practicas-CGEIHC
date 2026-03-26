/*
Practica 5: Optimizacion y Carga de Modelos
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
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Model carro;
Model cofre;
Model llanta_infder;
Model llanta_infizq;
Model llanta_supder;
Model llanta_supizq;

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

//Rotaciones
// Llantas giro libre 360, cofre 0-45
const float LIMITE_LLANTA = 360.0f;
const float LIMITE_COFRE = 45.0f;

float angLlantaSupDer = 0.0f;
float angLlantaSupIzq = 0.0f;
float angLlantaInfDer = 0.0f;
float angLlantaInfIzq = 0.0f;
float angCofre = 0.0f;

// Traslacion global del carro (Z = adelante/atras)
float posCarroZ = 0.0f;

static const char* vShader = "shaders/shader_m.vert";
static const char* fShader = "shaders/shader_m.frag";

float clampf(float v, float lo, float hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
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
        -1.0f, -1.0f, -0.6f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         0.0f, -1.0f,  1.0f,   0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -0.6f,   1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
         0.0f,  1.0f,  0.0f,   0.5f, 1.0f,   0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.0f,    0.0f,  0.0f,   0.0f, -1.0f, 0.0f,
         10.0f, 0.0f, -10.0f,   10.0f,  0.0f,   0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f,  10.0f,    0.0f, 10.0f,   0.0f, -1.0f, 0.0f,
         10.0f, 0.0f,  10.0f,   10.0f, 10.0f,   0.0f, -1.0f, 0.0f
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
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

void handleModelKeys(bool* keys, float dt)
{
    float velLlanta = 90.0f * dt;
    float velCofre = 60.0f * dt;
    float velCarro = 5.0f * dt;

    // Avanzar/retroceder carro completo: Z avanza, X retrocede
    if (keys[GLFW_KEY_Z]) posCarroZ -= velCarro;
    if (keys[GLFW_KEY_X]) posCarroZ += velCarro;

    // Cofre: R abre, F cierra
    if (keys[GLFW_KEY_R]) angCofre += velCofre;
    if (keys[GLFW_KEY_F]) angCofre -= velCofre;
    angCofre = clampf(angCofre, 0.0f, LIMITE_COFRE);

    // Llanta superior derecha: T adelante, G atras
    if (keys[GLFW_KEY_T]) angLlantaSupDer += velLlanta;
    if (keys[GLFW_KEY_G]) angLlantaSupDer -= velLlanta;
    angLlantaSupDer = clampf(angLlantaSupDer, -LIMITE_LLANTA, LIMITE_LLANTA);

    // Llanta superior izquierda: Y adelante, H atras
    if (keys[GLFW_KEY_Y]) angLlantaSupIzq += velLlanta;
    if (keys[GLFW_KEY_H]) angLlantaSupIzq -= velLlanta;
    angLlantaSupIzq = clampf(angLlantaSupIzq, -LIMITE_LLANTA, LIMITE_LLANTA);

    // Llanta inferior derecha: U adelante, J atras
    if (keys[GLFW_KEY_U]) angLlantaInfDer += velLlanta;
    if (keys[GLFW_KEY_J]) angLlantaInfDer -= velLlanta;
    angLlantaInfDer = clampf(angLlantaInfDer, -LIMITE_LLANTA, LIMITE_LLANTA);

    // Llanta inferior izquierda: I adelante, K atras
    if (keys[GLFW_KEY_I]) angLlantaInfIzq += velLlanta;
    if (keys[GLFW_KEY_K]) angLlantaInfIzq -= velLlanta;
    angLlantaInfIzq = clampf(angLlantaInfIzq, -LIMITE_LLANTA, LIMITE_LLANTA);
}

int main()
{
    mainWindow = Window(1366, 768);
    mainWindow.Initialise();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -10.0f, 0.3f, 1.0f);

    carro.LoadModel("Models/Carro.obj");
    cofre.LoadModel("Models/Cofre.obj");
    llanta_infder.LoadModel("Models/Llantainfder.obj");
    llanta_infizq.LoadModel("Models/Llantainfizq.obj");
    llanta_supder.LoadModel("Models/Llantasupder.obj");
    llanta_supizq.LoadModel("Models/Llantasupizq.obj");

    std::vector<std::string> skyboxFaces;
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
    skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
    skybox = Skybox(skyboxFaces);

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0,
        uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
    GLuint uniformColor = 0;

    glm::mat4 projection = glm::perspective(45.0f,
        (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(),
        0.1f, 1000.0f);

    glm::mat4 model(1.0f);
    glm::mat4 modelaux(1.0f);
    glm::mat4 baseMat(1.0f);
    glm::vec3 color(1.0f);

    float escala = 5.0f;

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        handleModelKeys(mainWindow.getsKeys(), deltaTime);

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

        // Piso
        color = glm::vec3(0.0f, 1.0f, 0.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.2f, 0.0f));
        model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        // Matriz base jerarquica: todas las piezas heredan esta traslacion
        // Z cambia con teclas Z/X para mover todo el carro junto
        baseMat = glm::mat4(1.0f);
        baseMat = glm::translate(baseMat, glm::vec3(0.0f, 0.0f, posCarroZ));

        // CARRO (cuerpo principal)
        color = glm::vec3(0.6f, 0.6f, 0.6f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        modelaux = baseMat;
        modelaux = glm::translate(modelaux, glm::vec3(0.0f, 1.5f, 0.0f));
        modelaux = glm::scale(modelaux, glm::vec3(escala, escala, escala));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
        carro.RenderModel();

        // COFRE - R abre (hasta 45 grados), F cierra
        color = glm::vec3(1.0f, 0.5f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        modelaux = baseMat;
        modelaux = glm::translate(modelaux, glm::vec3(-0.62f, 1.85f, -5.40f));
        modelaux = glm::rotate(modelaux, angCofre * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        modelaux = glm::scale(modelaux, glm::vec3(escala, escala, escala));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
        cofre.RenderModel();

        color = glm::vec3(1.0f, 0.0f, 0.0f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));

        // LLANTA SUP DER - T adelante, G atras
        modelaux = baseMat;
        modelaux = glm::translate(modelaux, glm::vec3(4.3f, -1.1f, -7.70f));
        modelaux = glm::rotate(modelaux, angLlantaSupDer * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        modelaux = glm::scale(modelaux, glm::vec3(escala, escala, escala));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
        llanta_supder.RenderModel();

        // LLANTA SUP IZQ - Y adelante, H atras
        modelaux = baseMat;
        modelaux = glm::translate(modelaux, glm::vec3(-4.3f, -1.1f, -7.58f));
        modelaux = glm::rotate(modelaux, angLlantaSupIzq * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        modelaux = glm::scale(modelaux, glm::vec3(escala, escala, escala));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
        llanta_supizq.RenderModel();

        // LLANTA INF DER - U adelante, J atras
        modelaux = baseMat;
        modelaux = glm::translate(modelaux, glm::vec3(4.3f, -1.2f, 5.20f));
        modelaux = glm::rotate(modelaux, angLlantaInfDer * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        modelaux = glm::scale(modelaux, glm::vec3(escala, escala, escala));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
        llanta_infder.RenderModel();

        // LLANTA INF IZQ - I adelante, K atras
        modelaux = baseMat;
        modelaux = glm::translate(modelaux, glm::vec3(-4.3f, -1.2f, 5.20f));
        modelaux = glm::rotate(modelaux, angLlantaInfIzq * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        modelaux = glm::scale(modelaux, glm::vec3(escala, escala, escala));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
        llanta_infizq.RenderModel();

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}