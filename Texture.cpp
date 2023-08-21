
//Joe Arena
//CS 330
// 7-1 Project

#include <glad/glad.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"

#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "camera.h"
#include "filesystem.h"




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(4.0f, 2.0f, -4.0f);
 
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("6.multiple_lights.vs", "6.multiple_lights.fs");
    Shader lightCubeShader("6.light_cube.vs", "6.light_cube.fs");
    Shader lightingShaders("colors.vs", "colors.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // normals           // texture coords
        -0.25f, -0.25f, -0.25f,  0.0f,  0.0f, -0.5f,  0.0f,  0.0f,
         0.25f, -0.25f, -0.25f,  0.0f,  0.0f, -0.5f,  1.0f,  0.0f,
         0.25f,  0.25f, -0.25f,  0.0f,  0.0f, -0.5f,  1.0f,  1.0f,
         0.25f,  0.25f, -0.25f,  0.0f,  0.0f, -0.5f,  1.0f,  1.0f,
        -0.25f,  0.25f, -0.25f,  0.0f,  0.0f, -0.5f,  0.0f,  1.0f,
        -0.25f, -0.25f, -0.25f,  0.0f,  0.0f, -0.5f,  0.0f,  0.0f,

        -0.25f, -0.25f,  0.25f,  0.0f,  0.0f,  0.5f,  0.0f,  0.0f,
         0.25f, -0.25f,  0.25f,  0.0f,  0.0f,  0.5f,  1.0f,  0.0f,
         0.25f,  0.25f,  0.25f,  0.0f,  0.0f,  0.5f,  1.0f,  1.0f,
         0.25f,  0.25f,  0.25f,  0.0f,  0.0f,  0.5f,  1.0f,  1.0f,
        -0.25f,  0.25f,  0.25f,  0.0f,  0.0f,  0.5f,  0.0f,  1.0f,
        -0.25f, -0.25f,  0.25f,  0.0f,  0.0f,  0.5f,  0.0f,  0.0f,

        -0.25f,  0.25f,  0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.25f,  0.25f, -0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.25f, -0.25f, -0.25f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.25f, -0.25f, -0.25f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.25f, -0.25f,  0.25f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.25f,  0.25f,  0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.25f,  0.25f,  0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.25f,  0.25f, -0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.25f, -0.25f, -0.25f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.25f, -0.25f, -0.25f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.25f, -0.25f,  0.25f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.25f,  0.25f,  0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.25f, -0.25f, -0.25f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.25f, -0.25f, -0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.25f, -0.25f,  0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.25f, -0.25f,  0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.25f, -0.25f,  0.25f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.25f, -0.25f, -0.25f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.25f,  0.25f, -0.25f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.25f,  0.25f, -0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.25f,  0.25f,  0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.25f,  0.25f,  0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.25f,  0.25f,  0.25f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.25f,  0.25f, -0.25f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f

    };


    float planeVertices[] = {
        // positions          // texture Coords 

         3.0f, -0.5f,  5.0f,  0.0f,  0.5f,   0.5f,  2.0f, 0.0f,
        -3.0f, -0.5f,  5.0f,  0.0f,  0.0f,   0.5f,  0.0f, 0.0f,
        -3.0f, -0.5f, -5.0f,  0.0f,  0.0f,   0.5f,  0.0f, 2.0f,

         3.0f, -0.5f,  5.0f,  0.0f,  0.5f,   0.5f,  2.0f, 0.0f,
        -3.0f, -0.5f, -5.0f,  0.0f,  0.5f,   0.5f,  0.0f, 2.0f,
         3.0f, -0.5f, -5.0f,  0.0f,  0.5f,   0.5f,  2.0f, 2.0f
    };
    float screenVertices[]{
          1.0f,   1.0f,  -2.0f, 1.0f, 0.0f,
          1.0f,  -1.0f,  -2.0f, 0.0f, 0.0f,
         -1.0f,  -1.0f,   1.0f, 0.0f, 0.0f,
         -1.0f,   1.0f,   1.0f, 0.0f, 0.0f
    };
    //stand for monitor 1
    float standVertices[]{
          -0.2f,   0.25f,   0.15f, 1.0f, 0.0f,
          -0.2f,  -1.0f,    0.15f, 0.0f, 0.0f,
           0.2f,  -1.0f,   -0.3f,  0.0f, 0.0f,
           0.2f,   0.25f,  -0.3f,  0.0f, 0.0f
    };
    //first screen
    float screenVertices2[]{
          -1.0f,   1.0f,  -2.0f,     1.0f, 0.0f,
          -1.0f,  -1.0f,  -2.0f,     0.0f, 0.0f,
           1.0f,  -1.0f,   0.5f,     0.0f, 0.0f,
           1.0f,   1.0f,   0.5f,     0.0f, 0.0f

    };
    //stand for monitor 2
    float standVertices2[]{
           -0.15f,  0.25f, -0.25f,     1.0f, 0.0f,
          -0.15f,  -1.0f,  -0.25f,     0.0f, 0.0f,
           0.15f,  -1.0f,   0.15f,      0.0f, 0.0f,
           0.15f,  0.25f,   0.15f,      0.0f, 0.0f

    };

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    //keyboard
    float keyVetrices[]{
        -0.25f,-0.05f,  -1.0f,  0.0f,  0.0f, -0.5f,  0.0f,  0.0f,
         0.5f, -0.05f,  -1.0f,  0.0f,  0.0f, -0.5f,  0.0f,  0.0f,
         0.5f,  0.05f,  -1.0f,  0.0f,  0.0f, -0.5f,  0.0f,  0.0f,
         0.5f,  0.05f,  -1.0f,  0.0f,  0.0f, -0.5f,  0.0f,  0.0f,
        -0.25f, 0.05f,  -1.0f,  0.0f,  0.0f, -0.5f,  0.0f,  0.0f,
        -0.25f,-0.05f,  -1.0f,  0.0f,  0.0f, -0.5f,  0.0f,  0.0f,

        -0.25f,-0.05f,  1.0f,   0.0f,  0.0f,  0.5f,  0.0f,  0.0f,
         0.5f, -0.05f,  1.0f,   0.0f,  0.0f,  0.5f,  0.0f,  0.0f,
         0.5f,  0.05f,  1.0f,   0.0f,  0.0f,  0.5f,  0.0f,  0.0f,
         0.5f,  0.05f,  1.0f,   0.0f,  0.0f,  0.5f,  0.0f,  0.0f,
        -0.25f, 0.05f,  1.0f,   0.0f,  0.0f,  0.5f,  0.0f,  0.0f,
        -0.25f,-0.05f,  1.0f,   0.0f,  0.0f,  0.5f,  0.0f,  0.0f,

        -0.25f,  0.05f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.25f,  0.05f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.25f, -0.05f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.25f, -0.05f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.25f, -0.05f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.25f,  0.05f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

         0.5f,  0.05f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.05f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -0.05f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -0.05f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -0.05f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.05f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

        -0.25f,-0.05f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -0.05f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -0.05f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -0.05f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.25f,-0.05f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.25f,-0.05f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

        -0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,   0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,   0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,   0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f

    };
    

    float sideVertices[]{
        4.0f, -0.5f, 0.0f,  0.0f, 0.0f,-0.5f,  0.0, 1.0f,  // top left 
        4.0f, -4.0f, 0.0f,  0.5f, 0.0f,-0.5f,  0.5f,1.0f, // bottom left
       -2.0f, -4.0f, 0.5f,  0.5f, 0.0f, 0.5f,  1.0f,0.0f, // bottom right
       -2.0f, -0.5f, 0.5f,  0.5f, 0.0f, 0.5f,  0.0f,0.0f, // top right

    };

    float floorVertices[] = {
         // positions          //normals          // texture Coords 
         20.0f,  -4.0f, 20.0f,  1.0f,  0.0f,  0.0f,  2.0f, 0.0f,
        -20.0f,  -4.0f, 20.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -20.0f,  -4.0f,-20.0f,  1.0f,  0.0f,  0.0f,  0.0f, 2.0f,

         20.0f,  -4.0f,  20.0f, 1.0f,  0.0f,  0.0f,  2.0f, 0.0f,
        -20.0f,  -4.0f, -20.0f, 1.0f,  0.0f,  0.0f,  0.0f, 2.0f,
         20.0f,  -4.0f, -20.0f, 1.0f,  0.0f,  0.0f,  2.0f, 2.0f

    };

    GLfloat pyramidVert[]{
    -0.5f, -0.5f, 0.0f, 0.0,  1.0f, 	
     0.0f,  0.5f,-0.5f, 0.5f, 1.0f, 	
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 		
    -0.5f, -0.5f,-1.0f, 0.0f, 0.0f, 	
     0.5f, -0.5f,-1.0f, 1.0f, 1.0f,

    };
    GLuint indices[] = {

            0, 1, 2,	// Front Triangle
            0, 3, 1,	// Right Side triangle
            4, 3, 1,	// Back triangle
            4, 2, 1,	// Left back side triangle
            0, 3, 4,	// Bottom Triangle
            0, 4, 2		// Bottom triangle

    };
    //position for lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f,   0.0f,  0.0f),
        glm::vec3(10.0f,   2.0f,  -5.0f),
        glm::vec3(-10.5f, 2.2f, -2.5f),
        glm::vec3( -10.8f,  2.0f,  -12.3f),
        glm::vec3(2.4f,   2.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f,  10.5f),
        glm::vec3(1.3f, 2.0f,  2.5f),
        glm::vec3(1.5f,   2.0f,  2.5f),
         
    };

    // Rubik's cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
  
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    
    //pyramid
    unsigned int pyrVAO, pyrVBO, pyrEBO{};

    glGenVertexArrays(1, &pyrVAO);
    glGenBuffers(1, &pyrVBO);
    glGenBuffers(1, &pyrEBO);

    glBindVertexArray(pyrVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyrVBO);


    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVert), &pyramidVert, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyrEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
        GL_STATIC_DRAW);

   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // desk VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    //side of desk
    unsigned int sideVAO, sideVBO;
    glGenVertexArrays(1, &sideVAO);
    glGenBuffers(1, &sideVBO);
    glBindVertexArray(sideVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sideVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sideVertices), &sideVertices, GL_STATIC_DRAW);
   
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //screenVAO
    unsigned int screenVAO, screenVBO;
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
   
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //standVAO
    unsigned int standVAO, standVBO;
    glGenVertexArrays(1, &standVAO);
    glGenBuffers(1, &standVBO);
    glBindVertexArray(standVAO);
    glBindBuffer(GL_ARRAY_BUFFER, standVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(standVertices), &standVertices, GL_STATIC_DRAW);
   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //screen2VAO
    unsigned int screen2VAO, screen2VBO;
    glGenVertexArrays(1, &screen2VAO);
    glGenBuffers(1, &screen2VBO);
    glBindVertexArray(screen2VAO);
    glBindBuffer(GL_ARRAY_BUFFER, screen2VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices2), &screenVertices2, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //stand2VAO
    unsigned int stand2VAO, stand2VBO;
    glGenVertexArrays(1, &stand2VAO);
    glGenBuffers(1, &stand2VBO);
    glBindVertexArray(stand2VAO);
    glBindBuffer(GL_ARRAY_BUFFER, stand2VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(standVertices2), &standVertices2, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //floorVAO
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //keyboard
    unsigned int keyVAO, keyVBO;
    glGenVertexArrays(1, &keyVAO);
    glGenBuffers(1, &keyVBO);
    glBindVertexArray(keyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, keyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(keyVetrices), &keyVetrices, GL_STATIC_DRAW);
   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    
    // load textures
    // -------------
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("3x3 grid.png").c_str());
    unsigned int  pyrTexture = loadTexture(FileSystem::getPath("brick.png").c_str());
    unsigned int floorTexture = loadTexture(FileSystem::getPath("floor texture.png").c_str());
    
    unsigned int monitorTexture = loadTexture(FileSystem::getPath("glossy.png").c_str());
    unsigned int deskTexture = loadTexture(FileSystem::getPath("desk.png").c_str());
    unsigned int screenText = loadTexture(FileSystem::getPath("screen.png").c_str());
    unsigned int keyboard = loadTexture(FileSystem::getPath("keyboard.png").c_str());
    unsigned int steelTexture = loadTexture(FileSystem::getPath("steel.png").c_str());
   

    // shader configuration
   // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

 

    // render loop
   // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
       
        lightingShader.setVec3("viewPos", camera.Position);
         
      
        // material properties
        lightingShader.setFloat("material.shininess", 32.0f);


        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f,  -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient",    0.05f,  0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse",    0.4f,   0.4f,  0.4f);
        lightingShader.setVec3("dirLight.specular",   0.5f,   0.5f,  0.5f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient",   0.05f, 0.05f,  0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.0f, 1.0f, 1.0f);
        lightingShader.setVec3("pointLights[0].specular",  1.0f,  0.0f,   1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear",   0.09f);
        lightingShader.setFloat("pointLights[0].quadratic",0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient",  0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f,  1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant",1.0f);
        lightingShader.setFloat("pointLights[1].linear",  0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient",  0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("pointLights[2].specular",  1.0f, 1.0f,  1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear",  0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);

        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
        // point light 5
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[5]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 6
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[6]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 7
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[7]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        lightingShader.use();
        lightingShaders.setVec3("light.ambient", 1.0f, 0.5f, 0.0f);
     
        // view/projection transformations
        

           
        
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

            glm::mat4 view = camera.GetViewMatrix();
            lightingShader.setMat4("projection", projection);


            lightingShader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            lightingShader.setMat4("model", model);

            // bind diffuse map
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            glBindVertexArray(cubeVAO);


            // calculate the model matrix for each object 

            model = glm::translate(model, glm::vec3(1.0f, -0.25f, -3.0f));
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);



            // pyramid
            lightingShader.use();
            lightCubeShader.setMat4("projection", projection);
            lightCubeShader.setMat4("view", view);
            model = glm::mat4(1.0f);
            lightingShader.setMat4("model", model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pyrTexture);
            glBindVertexArray(pyrVAO);
            model = glm::translate(model, glm::vec3(0.5f, 0.0f, -1.0f));
            lightingShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

            // desk plane
            lightingShader.use();
            lightCubeShader.setMat4("projection", projection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, deskTexture);
            glBindVertexArray(planeVAO);
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 10);



            //side  

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, deskTexture);
            glBindVertexArray(sideVAO);
            model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -5.0f));
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_QUADS, 0, 5);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-0.55f, 0.0f, 4.0f));
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_QUADS, 0, 5);
            glBindVertexArray(0);

            //screen
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, screenText);
            glBindVertexArray(screen2VAO);
            lightingShader.setFloat("material.shininess", 62.0f);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.55f, 1.0f, 2.5f));
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_QUADS, 0, 5);


            //stand
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, steelTexture);
            glBindVertexArray(standVAO);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -3.0f));
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_QUADS, 0, 5);


            //screen2
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, screenText);
            glBindVertexArray(screenVAO);
            lightingShader.setFloat("material.shininess", 62.0f);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.5f, 1.0f, -2.5f));
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_QUADS, 0, 5);

            //stand2
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, steelTexture);
            glBindVertexArray(stand2VAO);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.5f, 0.5f, 2.0f));
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_QUADS, 0, 5);
            glBindVertexArray(0);


            //keyboard
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, keyboard);
            glBindVertexArray(keyVAO);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.5f, -0.48f, -0.5f));
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 42);
            glBindVertexArray(0);

            //floor plane
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            glBindVertexArray(floorVAO);
            model = glm::translate(model, glm::vec3(20.0f, 20.0f, 0.0f));
            glDrawArrays(GL_TRIANGLES, 0, 10);
            glBindVertexArray(0);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &screenVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteVertexArrays(1, &sideVAO);
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteVertexArrays(1, &screen2VAO);
    glDeleteVertexArrays(1, &keyVAO);
    glDeleteVertexArrays(1, &standVAO);
    glDeleteVertexArrays(1, &stand2VAO);
    glDeleteVertexArrays(1, &pyrVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);

    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &screenVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &sideVBO);

    glDeleteVertexArrays(1, &floorVBO);
    glDeleteVertexArrays(1, &screen2VBO);
    glDeleteVertexArrays(1, &keyVBO);
    glDeleteVertexArrays(1, &standVBO);
    glDeleteVertexArrays(1, &stand2VBO);
    glDeleteVertexArrays(1, &pyrVBO);
    glDeleteBuffers(1, &pyrEBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
   glm::mat4 projection =  glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}