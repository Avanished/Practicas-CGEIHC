// Práctica 3: Pyraminx (Pyraminx Rubik)

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
GLfloat     deltaTime = 0.0f;
GLfloat     lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

Camera         camera;
Window         mainWindow;
vector<Mesh*>  meshList;
vector<Shader> shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";

Sphere sp = Sphere(1.0, 20, 20);

// Base negra del Pyraminx
void CrearTetraedroRegular()
{
    const float yb = -0.204124f;
    const float yt = 0.612372f;
    const float R = 0.577350f;
    const float H = 0.500000f;

    unsigned int indices[] = { 1,2,3, 2,0,3, 0,1,3, 0,2,1 };
    GLfloat vertices[] = {
         0.0f, yb, -R,
        -H,    yb,  R / 2.0f,
         H,    yb,  R / 2.0f,
         0.0f, yt,  0.0f
    };
    Mesh* obj = new Mesh();
    obj->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj);  // índice 0
}

// meshList[1]: Triángulo PLANO equilátero centrado en origen
void CrearTrianguloPlano()
{
    const float h = 0.866025f;  // sqrt(3)/2
    const float y0 = -0.288675f;  // -h/3
    const float y2 = 0.577350f;  // +2h/3

    // Dos triángulos para doble cara (visible desde ambos lados)
    unsigned int indices[] = {
        0, 1, 2,   // cara frontal
        0, 2, 1    // cara trasera (normal invertida)
    };
    GLfloat vertices[] = {
        -0.5f, y0, 0.0f,   // V0: base izquierda
         0.5f, y0, 0.0f,   // V1: base derecha
         0.0f, y2, 0.0f    // V2: punta arriba
    };
    Mesh* obj = new Mesh();
    obj->CreateMesh(vertices, indices, 9, 6);
    meshList.push_back(obj);  // índice 1
}

void CreateShaders()
{
    Shader* s1 = new Shader();
    s1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*s1);
    Shader* s2 = new Shader();
    s2->CreateFromFiles(vShaderColor, fShader);
    shaderList.push_back(*s2);
}

// Dibujar 9 triángulos planos de color sobre una cara del tetraedro.
void DibujarCaraPyraminx(
    GLuint uniformModel,
    GLuint uniformColor,
    const glm::mat4& tGlobal,
    glm::vec3 A, glm::vec3 B, glm::vec3 C,
    glm::vec3 centroPiramide,
    glm::vec3 color,
    float gapFactor = 0.82f,
    float offN = 0.015f)
{
    // Ejes del plano de la cara
    glm::vec3 t1 = glm::normalize(B - A);
    glm::vec3 n = glm::normalize(glm::cross(B - A, C - A));
    if (glm::dot(n, (A + B + C) / 3.0f - centroPiramide) < 0.0f)
        n = -n;
    glm::vec3 t2 = glm::normalize(glm::cross(n, t1));

    // Orientación: X?t1, Y?t2, Z?n
    // El triángulo plano vive en Z=0, así que Z?n lo pega sobre la cara
    glm::mat4 rot = glm::mat4(1.0f);
    rot[0] = glm::vec4(t1, 0.0f);
    rot[1] = glm::vec4(t2, 0.0f);
    rot[2] = glm::vec4(n, 0.0f);

    // Escala uniforme: cada sub-triángulo ocupa gapFactor de su celda
    float escSub = (glm::length(B - A) / 3.0f) * gapFactor;

    // Rotar 180° en Z para triángulos DOWN (punta hacia abajo)
    glm::mat4 rotInv = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 0, 1));

    // Subdivisión paramétrica
    auto P = [&](int i, int j) -> glm::vec3 {
        return A + (float(i) / 3.0f) * (B - A) + (float(j) / 3.0f) * (C - A);
    };

    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 3 - j; i++)
        {
            // UP: centroide del sub-triángulo + offset sobre la cara
            glm::vec3 cUp = (P(i, j) + P(i + 1, j) + P(i, j + 1)) / 3.0f + offN * n;
            glm::mat4 mUp = glm::translate(glm::mat4(1.0f), cUp)
                * rot
                * glm::scale(glm::mat4(1.0f), glm::vec3(escSub));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(tGlobal * mUp));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            meshList[1]->RenderMesh();   // triángulo plano

            // DOWN: mismo pero rotado 180° en Z
            if (i < 2 - j)
            {
                glm::vec3 cDn = (P(i + 1, j) + P(i, j + 1) + P(i + 1, j + 1)) / 3.0f + offN * n;
                glm::mat4 mDn = glm::translate(glm::mat4(1.0f), cDn)
                    * rot
                    * rotInv
                    * glm::scale(glm::mat4(1.0f), glm::vec3(escSub));
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(tGlobal * mDn));
                glUniform3fv(uniformColor, 1, glm::value_ptr(color));
                meshList[1]->RenderMesh();   // triángulo plano
            }
        }
    }
}

int main()
{
    mainWindow = Window(800, 600);
    mainWindow.Initialise();

    CrearTetraedroRegular();  // meshList[0]: tetraedro negro
    CrearTrianguloPlano();    // meshList[1]: triángulo plano de color
    CreateShaders();

    camera = Camera(
        glm::vec3(0.0f, 0.6f, 3.5f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f, -10.0f, 0.3f, 0.3f);

    GLuint uniformProjection = 0, uniformModel = 0,
        uniformView = 0, uniformColor = 0;

    glm::mat4 projection = glm::perspective(
        glm::radians(60.0f),
        mainWindow.getBufferWidth() / mainWindow.getBufferHeight(),
        0.1f, 100.0f);

    sp.init(); sp.load();

    const float ESC = 2.2f;

    glm::vec3 V0 = glm::vec3(0.0f, -0.204124f, -0.577350f) * ESC;
    glm::vec3 V1 = glm::vec3(-0.500000f, -0.204124f, 0.288675f) * ESC;
    glm::vec3 V2 = glm::vec3(0.500000f, -0.204124f, 0.288675f) * ESC;
    glm::vec3 V3 = glm::vec3(0.0f, 0.612372f, 0.0f) * ESC;

    glm::vec3 colRojo = glm::vec3(0.85f, 0.08f, 0.08f);
    glm::vec3 colVerde = glm::vec3(0.05f, 0.72f, 0.12f);
    glm::vec3 colAzul = glm::vec3(0.10f, 0.22f, 0.90f);
    glm::vec3 colAmarillo = glm::vec3(0.95f, 0.88f, 0.04f);
    glm::vec3 colNegro = glm::vec3(0.03f, 0.03f, 0.03f);

    struct Cara { glm::vec3 A, B, C, color; };
    Cara caras[4] = {
        { V1, V2, V3, colRojo     },
        { V2, V0, V3, colVerde    },
        { V0, V1, V3, colAzul     },
        { V0, V2, V1, colAmarillo },
    };

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        glClearColor(0.18f, 0.18f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0].useShader();
        uniformModel = shaderList[0].getModelLocation();
        uniformProjection = shaderList[0].getProjectLocation();
        uniformView = shaderList[0].getViewLocation();
        uniformColor = shaderList[0].getColorLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

        // Rotación global (E=X, R=Y, T=Z) — todo rota junto
        glm::mat4 tGlobal = glm::mat4(1.0f);
        tGlobal = glm::rotate(tGlobal, glm::radians(mainWindow.getrotax()), glm::vec3(1, 0, 0));
        tGlobal = glm::rotate(tGlobal, glm::radians(mainWindow.getrotay()), glm::vec3(0, 1, 0));
        tGlobal = glm::rotate(tGlobal, glm::radians(mainWindow.getrotaz()), glm::vec3(0, 0, 1));

        // PASO 1: Tetraedro negro grande (fondo ? da los bordes entre piezas)
        {
            glm::mat4 mN = glm::scale(glm::mat4(1.0f), glm::vec3(ESC));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(tGlobal * mN));
            glUniform3fv(uniformColor, 1, glm::value_ptr(colNegro));
            meshList[0]->RenderMesh();
        }

        // PASO 2: 9 triángulos PLANOS de color por cara
        for (int i = 0; i < 4; i++)
        {
            DibujarCaraPyraminx(
                uniformModel, uniformColor,
                tGlobal,
                caras[i].A, caras[i].B, caras[i].C,
                glm::vec3(0.0f),
                caras[i].color,
                0.82f,   // gapFactor
                0.015f); // offN
        }

        glUseProgram(0);
        mainWindow.swapBuffers();
    }
    return 0;
}