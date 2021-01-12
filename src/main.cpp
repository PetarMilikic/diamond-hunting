#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(-4.0f,1.7f,10.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
glm::vec3 lightPos(glm::vec3(6.5f,3.0f,0.0f));

float delta=0.4;
vector<bool> diamondOutIscrtan(3);
vector<bool> cubeIscrtan(4);
int brojSakupljenih=0;
int brojStarih=-1;
glm::vec3 transMatrica(-2.0f,-0.5f,5.0f);

struct Light {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    glm::vec3 position;//pozicija svetla
    glm::vec3 direction;//smer padanja svetla
    float cutOff;//unutrasnji cut ugao
    float outerCutOff;//spoljasnji cut ugao

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

float LightPower=1.0;

int main()
{
    fill(diamondOutIscrtan.begin(),diamondOutIscrtan.end(),false);
    fill(cubeIscrtan.begin(),cubeIscrtan.end(),false);
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "diamond-hunting", NULL, NULL);
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

    glEnable(GL_DEPTH_TEST);//ukljucujemo testiranje za dubinu

    // build and compile shaders
    // -------------------------
    Shader shader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader secondShader("resources/shaders/2.model_lighting.vs", "resources/shaders/drugaKocka.fs");
    Shader banderaShader("resources/shaders/bandera.vs", "resources/shaders/bandera.fs");
    Shader banderaLightShader("resources/shaders/banderaLight.vs", "resources/shaders/banderaLight.fs");
    Shader mesecShader("resources/shaders/mesecLight.vs", "resources/shaders/mesecLight.fs");
    Shader floorShader("resources/shaders/floor.vs", "resources/shaders/floor.fs");
    Shader draguljShader("resources/shaders/dragulj.vs","resources/shaders/dragulj.fs");
    Shader modelShader("resources/shaders/modelShader.vs","resources/shaders/modelShader.fs");
    Shader treeShader("resources/shaders/tree.vs","resources/shaders/tree.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
            // positions          // texture Coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    float planeVertices[] = {
            // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
            7.0f, -0.5f,  7.0f,  0.0f,  1.0f,  0.0f, 2.0f, 0.0f,
            -7.0f, -0.5f,  7.0f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
            -7.0f, -0.5f, -7.0f,  0.0f,  1.0f,  0.0f, 0.0f, 2.0f,


            7.0f, -0.5f,  7.0f, 0.0f,  1.0f,  0.0f, 2.0f, 0.0f,
            -7.0f, -0.5f, -7.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,
            7.0f, -0.5f, -7.0f, 0.0f,  1.0f,  0.0f,  2.0f, 2.0f
    };

    float grassVertices[] = {
            // positions   //texture coords    //Normals=const
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    };


    //pozicije za zgrade
    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  0.0f, -1.0f),
            glm::vec3(-1.5f, 0.0f, -2.5f),
            glm::vec3(-3.8f, 0.0f, 3.3f),
            glm::vec3( 2.4f, 0.0f, -3.5f),

            glm::vec3(-1.7f,  0.0f, 1.5f),
            glm::vec3( 3.3f, 0.0f, -2.0f),
            glm::vec3( 2.0f,  0.0f, 2.7f),
            glm::vec3( 4.0f,  0.0f, 4.0f),
            glm::vec3(-4.0f,  0.0f, -4.5f)
    };
    glm::vec3 banderaPositions[] = {
            glm::vec3( -1.1, 1.0, 4.5),
            glm::vec3( -1.1f,  1.0f, -1.0f),//2
            glm::vec3(-2.8f, 1.0f, 1.0f),//3.3
            glm::vec3(2.8f, 1.0f, 3.5f),//2.8
            glm::vec3( 2.8, 1.0f, 0.0f),//-2.0
            glm::vec3( 4.0f, 1.0f, 1.5f)


    };
    glm::vec3 diamondOutPositions[]={
            glm::vec3( -1.3, 0.3, 4.0),
            glm::vec3( -1.3f,  0.3f, -1.1f),//2
            glm::vec3(-5.0f, 0.02f, 1.0f),//3.3
            glm::vec3(2.5f, 0.02f, 3.7f),//2.8
            glm::vec3( 2.1, 0.02f, 0.0f),//
    };
    glm::vec3 banderaLightsPositions[] = {
            glm::vec3( -1.1, 2.6, 4.5),
            glm::vec3( -1.1f,  2.6f, -1.0f),//2
            glm::vec3(-2.8f, 2.6f, 1.0f),//3.3
            glm::vec3(2.8f, 2.6f, 3.5f),//2.8
            glm::vec3( 2.8, 2.6f, 0.0f),//-2.0
            glm::vec3( 4.0f, 2.6f, 1.5f)


    };

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    //drugi model kocke
    unsigned int cubeVAO2, cubeVBO2;
    glGenVertexArrays(1, &cubeVAO2);
    glGenBuffers(1, &cubeVBO2);
    glBindVertexArray(cubeVAO2);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    //postavke za benderu:
    unsigned int banderaVAO, banderaVBO;
    glGenVertexArrays(1, &banderaVAO);
    glGenBuffers(1, &banderaVBO);
    glBindVertexArray(banderaVAO);
    glBindBuffer(GL_ARRAY_BUFFER, banderaVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    //postavke za svetlo na banderi
    unsigned int banderaLightVAO, banderaLightVBO;
    glGenVertexArrays(1, &banderaLightVAO);
    glGenBuffers(1, &banderaLightVBO);
    glBindVertexArray(banderaLightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, banderaLightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    //POSTAVKE ZA MESEC
    unsigned int mesecVAO, mesecVBO;
    glGenVertexArrays(1, &mesecVAO);
    glGenBuffers(1, &mesecVBO);
    glBindVertexArray(mesecVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesecVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    //postavke za dragulje:
    unsigned int draguljVAO, draguljVBO;
    glGenVertexArrays(1, &draguljVAO);
    glGenBuffers(1, &draguljVBO);
    glBindVertexArray(draguljVAO);
    glBindBuffer(GL_ARRAY_BUFFER, draguljVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    //POSTAVKE ZA TRAVU:
    unsigned int treeVAO, treeVBO;
    glGenVertexArrays(1, &treeVAO);
    glGenBuffers(1, &treeVBO);
    glBindVertexArray(treeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), &grassVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------

    unsigned int floorTextureDif = loadTexture(FileSystem::getPath("resources/textures/fff.jpg").c_str());
    unsigned int floorTextureSpe = loadTexture(FileSystem::getPath("resources/textures/fff.jpg").c_str());

    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/z8.jpg").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/z8.jpg").c_str());

    unsigned int diffuseMap2 = loadTexture(FileSystem::getPath("resources/textures/z3.jpg").c_str());
    unsigned int specularMap2 = loadTexture(FileSystem::getPath("resources/textures/z3.jpg").c_str());

    unsigned int treeTexture = loadTexture(FileSystem::getPath("resources/textures/tree.png").c_str());

    shader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);

    floorShader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);

    secondShader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);

    treeShader.use();
    treeShader.setInt("material.diffuse",0);
    treeShader.setInt("material.specular",1);

    //model postavka
    Model orao(FileSystem::getPath("resources/objects/orao/5.obj"));
    Model annies(FileSystem::getPath("resources/objects/annies/annies-ghost.obj"));
    Model moon(FileSystem::getPath("resources/objects/mesecic/moon.obj"));

    orao.SetShaderTextureNamePrefix("material.");
    annies.SetShaderTextureNamePrefix("material.");
    moon.SetShaderTextureNamePrefix("material.");

    //svetlo za modele
    Light lightOrao;
    lightOrao.direction = glm::vec3(0.0f,1.0f,0.0f);
    lightOrao.ambient = glm::vec3(0.2f,0.2f,0.2f);
    lightOrao.diffuse = glm::vec3(0.05f,0.05f,0.05f);
    lightOrao.specular = glm::vec3(1.0f,1.0f,1.0f);


    PointLight pointLightOrao;
    pointLightOrao.position = glm::vec3(4.0,4.0,0.0);
    pointLightOrao.ambient = glm::vec3(0.4,0.4,0.2);
    pointLightOrao.diffuse = glm::vec3(0.6,0.5,0.6);
    pointLightOrao.specular = glm::vec3(1.0,1.0,1.0);
    pointLightOrao.constant = 1.0f;
    pointLightOrao.linear = 0.09f;
    pointLightOrao.quadratic = 0.032f;

    SpotLight spotLightOrao;
    spotLightOrao.position = glm::vec3(camera.Position);
    spotLightOrao.direction = glm::vec3(camera.Front);
    spotLightOrao.ambient = glm::vec3(0.0f,0.0f,0.0f);
    spotLightOrao.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLightOrao.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    spotLightOrao.constant = 1.0f;
    spotLightOrao.linear = 0.09f;
    spotLightOrao.quadratic = 0.032;
    spotLightOrao.cutOff = abs(glm::cos(glm::radians(12.5*LightPower*0.2)));
    spotLightOrao.outerCutOff = abs(glm::cos(glm::radians(15.0*LightPower*0.2)));

    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if(currentFrame >= 90.0f) {
            if (brojSakupljenih >= 7){
                //ukoliko vreme istekne, a sakupljeno je vise od 7 dijamanata, korisnik moze ostati da razgleda,
                //u suprotnom, igra se prekida
            }
            else {
                glfwSetWindowShouldClose(window, true);
                cout << "GAME OVER! YOU FAILED!" << endl;
            }
        }
        else {

            if (brojSakupljenih != brojStarih) {
                cout << "Number of diamonds collected: " << brojSakupljenih << endl;
                brojStarih = brojSakupljenih;
                if (brojSakupljenih >= 7)
                    cout << "CONGRADS!\n";
                brojStarih = brojSakupljenih;
            }


        }
        // input
        // -----
        processInput(window);

        // render
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // don't forget to clear the stencil buffer!

        // set uniforms
        floorShader.use();
        floorShader.setVec3("viewPos", camera.Position);

        // light properties
        floorShader.setVec3("light.direction", 0.0f, 1.0f, 0.0f);
        floorShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        floorShader.setVec3("light.diffuse", 0.05f, 0.05f, 0.05f);
        floorShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        floorShader.setVec3("pointLight.position", lightPos);
        floorShader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        floorShader.setVec3("pointLight.diffuse", 0.6f,0.328f,0.181f);
        floorShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        floorShader.setFloat("pointLight.constant", 1.0f);
        floorShader.setFloat("pointLight.linear", 0.09);
        floorShader.setFloat("pointLight.quadratic", 0.032);

        // material properties
        floorShader.setFloat("material.shininess", 32.0f);

        //spot svetlo
        floorShader.setVec3("spotLight.position", camera.Position);
        floorShader.setVec3("spotLight.direction", camera.Front);
        floorShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        floorShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        floorShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        floorShader.setFloat("spotLight.constant", 1.0f);
        floorShader.setFloat("spotLight.linear", 0.09);
        floorShader.setFloat("spotLight.quadratic", 0.032);
        floorShader.setFloat("spotLight.cutOff", abs(glm::cos(glm::radians(12.5*LightPower*0.2))));
        floorShader.setFloat("spotLight.outerCutOff", abs(glm::cos(glm::radians(15.0*LightPower*0.2))));

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        floorShader.setMat4("view", view);
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("model", model);

        //CRTANJE PODA

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTextureDif);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floorTextureSpe);
        glBindVertexArray(planeVAO);
        floorShader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 1st. render pass, draw objects as normal, writing to the stencil buffer
        // --------------------------------------------------------------------
        //glStencilFunc(GL_ALWAYS, 1, 0xFF);
        //glStencilMask(0xFF);

        //ZGRADE
        shader.use();
        // light properties
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        shader.setVec3("light.diffuse", 0.05f, 0.05f, 0.05f);
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);

        //point svetlo
        shader.setVec3("pointLight.position", lightPos);
        shader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLight.diffuse", 0.6f,0.328f,0.181f);
        shader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLight.constant", 1.0f);
        shader.setFloat("pointLight.linear", 0.09);
        shader.setFloat("pointLight.quadratic", 0.032);

        // material properties
        shader.setFloat("material.shininess", 32.0f);

        //spot svetlo
        shader.setVec3("spotLight.position", camera.Position);
        shader.setVec3("spotLight.direction", camera.Front);
        shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("spotLight.constant", 1.0f);
        shader.setFloat("spotLight.linear", 0.09);
        shader.setFloat("spotLight.quadratic", 0.032);
        shader.setFloat("spotLight.cutOff", abs(glm::cos(glm::radians(12.5*LightPower*0.2))));
        shader.setFloat("spotLight.outerCutOff", abs(glm::cos(glm::radians(15.0*LightPower*0.2))));

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        model = glm::mat4(1.0f);
        // shader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glBindVertexArray(cubeVAO);


        //prvi set zgrada:
        for (int i = 0; i < 4; i++) {

            model = glm::translate(model,cubePositions[i]);
            model=glm::translate(model,glm::vec3(0,0.4,0));
            model = glm::scale(model, glm::vec3(1.0f,2.0f,1.0f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            model = glm::mat4(1.0f);
        }

        model=glm::translate(model,glm::vec3(0.0f,0.4f,0.0f));
        model = glm::scale(model, glm::vec3(1.0f,2.0f,1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        glBindVertexArray(0);


        //druga tekstura za zgradu
        secondShader.use();
        // light properties
        secondShader.setVec3("viewPos", camera.Position);
        secondShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        secondShader.setVec3("light.diffuse", 0.05f, 0.05f, 0.05f);
        secondShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        secondShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);

        //point svetlo
        secondShader.setVec3("pointLight.position", lightPos);
        secondShader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        secondShader.setVec3("pointLight.diffuse", 0.6f,0.328f,0.181f);
        secondShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        secondShader.setFloat("pointLight.constant", 1.0f);
        secondShader.setFloat("pointLight.linear", 0.09);
        secondShader.setFloat("pointLight.quadratic", 0.032);

        // material properties
        secondShader.setFloat("material.shininess", 32.0f);

        //spot svetlo
        secondShader.setVec3("spotLight.position", camera.Position);
        secondShader.setVec3("spotLight.direction", camera.Front);
        secondShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        secondShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        secondShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        secondShader.setFloat("spotLight.constant", 1.0f);
        secondShader.setFloat("spotLight.linear", 0.09);
        secondShader.setFloat("spotLight.quadratic", 0.032);
        secondShader.setFloat("spotLight.cutOff", abs(glm::cos(glm::radians(12.5*LightPower*0.2))));
        secondShader.setFloat("spotLight.outerCutOff", abs(glm::cos(glm::radians(15.0*LightPower*0.2))));

        secondShader.setMat4("view", view);
        secondShader.setMat4("projection", projection);
        model = glm::mat4(1.0f);
        // shader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap2);
        glBindVertexArray(cubeVAO2);

        //drugi set zgrada:
        for (int i = 4; i < 10; i++) {

            model = glm::translate(model,cubePositions[i]);
            model=glm::translate(model,glm::vec3(0,0.3,0));
            model = glm::scale(model, glm::vec3(1.1f,1.5f,1.0f));
            secondShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            model = glm::mat4(1.0f);
        }

        glActiveTexture(0);

        // begin BANDERA:

        //glBindTexture();
        //glActiveTexture(0);
        //glDisable(GL_TEXTURE_2D);
        //model = glm::translate(model, glm::vec3(-1.1, 1.0, 3));
        //model = glm::scale(model, glm::vec3(0.15, 3, 0.15));

        banderaShader.use();
        banderaShader.setVec3("viewPos", camera.Position);
        banderaShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        banderaShader.setVec3("light.diffuse", 0.05f, 0.05f, 0.05f);
        banderaShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        banderaShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);

        //point svetlo
        banderaShader.setVec3("pointLight.position", lightPos);
        banderaShader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        banderaShader.setVec3("pointLight.diffuse", 0.6f,0.328f,0.181f);
        banderaShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        banderaShader.setFloat("pointLight.constant", 1.0f);
        banderaShader.setFloat("pointLight.linear", 0.09);
        banderaShader.setFloat("pointLight.quadratic", 0.032);

        // material properties
        banderaShader.setVec3("material.ambient", 0.1,0.1,0.1);
        banderaShader.setVec3("material.diffuse", 0.5f, 0.5f, 0.5f);
        banderaShader.setVec3("material.specular", 0.50196078f, 0.50196078f, 0.50196078f);
        banderaShader.setFloat("material.shininess", 32.0f);

        //spot svetlo
        banderaShader.setVec3("spotLight.position", camera.Position);
        banderaShader.setVec3("spotLight.direction", camera.Front);
        banderaShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        banderaShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        banderaShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        banderaShader.setFloat("spotLight.constant", 1.0f);
        banderaShader.setFloat("spotLight.linear", 0.09);
        banderaShader.setFloat("spotLight.quadratic", 0.032);
        banderaShader.setFloat("spotLight.cutOff", abs(glm::cos(glm::radians(12.5*LightPower*0.2))));
        banderaShader.setFloat("spotLight.outerCutOff", abs(glm::cos(glm::radians(15.0*LightPower*0.2))));


        model = glm::mat4(1.0f);
        banderaShader.setMat4("view", view);
        banderaShader.setMat4("projection", projection);
        banderaShader.setMat4("model", model);
        glBindVertexArray(banderaVAO);

        for(int i=0;i<6;i++) {
            //glBindTexture(GL_TEXTURE_2D, 0);
            model=glm::translate(model,banderaPositions[i]);
            model = glm::scale(model, glm::vec3(0.15, 3, 0.15));
            banderaShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            model = glm::mat4(1.0f);

        }
        glBindVertexArray(0);

        banderaLightShader.use();
        banderaLightShader.setMat4("view", view);
        banderaLightShader.setMat4("projection", projection);
        banderaLightShader.setMat4("model", model);
        glBindVertexArray(banderaVAO);

        glBindVertexArray(banderaLightVAO);
        for(int i=0;i<6;i++) {
            //glBindTexture(GL_TEXTURE_2D, 0);
            model=glm::translate(model,banderaLightsPositions[i]);
            model = glm::scale(model, glm::vec3(0.15, 0.15, 0.15));
            banderaLightShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            model = glm::mat4(1.0f);

        }

        glBindVertexArray(0);

        //end BANDERA

        modelShader.use();
        pointLightOrao.position = glm::vec3(camera.Position);
        modelShader.setVec3("pointLight.position", pointLightOrao.position);
        modelShader.setVec3("pointLight.ambient", pointLightOrao.ambient);
        modelShader.setVec3("pointLight.diffuse", pointLightOrao.diffuse);
        modelShader.setVec3("pointLight.specular", pointLightOrao.specular);
        modelShader.setFloat("pointLight.constant", pointLightOrao.constant);
        modelShader.setFloat("pointLight.linear", pointLightOrao.linear);
        modelShader.setFloat("pointLight.quadratic", pointLightOrao.quadratic);
        modelShader.setFloat("material.shininess", 32.0f);
        modelShader.setVec3("viewPosition", camera.Position);

        spotLightOrao.position = glm::vec3(camera.Position);
        spotLightOrao.direction = glm::vec3(camera.Front);
        modelShader.setVec3("spotLight.direction", spotLightOrao.direction);
        modelShader.setFloat("spotLight.cutOff", abs(glm::cos(glm::radians(12.5*LightPower*0.2))));
        modelShader.setVec3("spotLight.position", spotLightOrao.position);
        modelShader.setFloat("spotLight.outerCutOff", abs(glm::cos(glm::radians(15.0*LightPower*0.2))));
        modelShader.setVec3("spotLight.ambient", spotLightOrao.ambient);
        modelShader.setVec3("spotLight.diffuse", spotLightOrao.diffuse);
        modelShader.setVec3("spotLight.specular", spotLightOrao.specular);
        modelShader.setFloat("spotLight.constant", spotLightOrao.constant);
        modelShader.setFloat("spotLight.linear", spotLightOrao.linear);
        modelShader.setFloat("spotLight.quadratic", spotLightOrao.quadratic);

        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        //crtanje orla
        float angle = -glfwGetTime();
        model = glm::mat4(1.0f);
        model = glm::rotate(model, 0.8f*(float)angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(2.0f, 4.5f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.01f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);
        orao.Draw(modelShader);

        //crtanje uklete mlade
        model = glm::mat4(1.0f);
        model = glm::rotate(model, -0.2f*(float)angle, glm::vec3(0.0f, 3.0f, 0.0f));
        model = glm::translate(model, transMatrica); // translate it down so it's at the center of the scene
        model = glm::rotate(model, 45.0f, glm::vec3(0.0f,1.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.3f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);
        glm::vec4 matricaPom = glm::vec4(model * glm::vec4(1.0f));
        annies.Draw(modelShader);

        //crtanje velikog meseca
        mesecShader.use();
        mesecShader.setMat4("projection", projection);
        mesecShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPos)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.01f));	// it's a bit too big for our scene, so scale it down
        mesecShader.setMat4("model", model);
        //crtanje modela
        moon.Draw(mesecShader);


        if(matricaPom.x-1.5*delta<=camera.Position.x&&matricaPom.x+1.5*delta>=camera.Position.x
           &&matricaPom.z-1.5*delta<=camera.Position.z&&matricaPom.z+1.5*delta>=camera.Position.z) {
            glfwSetWindowShouldClose(window, true);
            std::cout << "GAME OVER! HAUNTED BRIDE KILLED YOU!" << std::endl;
        }

        //dragulj begin:
        draguljShader.use();
        draguljShader.setMat4("projection", projection);
        draguljShader.setMat4("view", view);
        model = glm::mat4(1.0f);

        glBindVertexArray(draguljVAO);
        //cout<<"["<<camera.Position.x<<","<<camera.Position.y<<","<<camera.Position.z<<"], broj sakupljenih dijamanata: "<<brojSakupljenih<<"\n";
        for(int i=2;i<10;i++) {

            //glBindTexture(GL_TEXTURE_2D, 0);
            //(0.8,0.2,0.35,1.0)
            // draguljShader.setVec4("faktor",0.8,0.2,i/10.0,1.0);
            if(i>5) {
                int j=i-6;
                //glm::vec3(-5.0f, 0.02f, 1.0f),//3.3
                if(cubePositions[i].x-delta<=camera.Position.x&&cubePositions[i].x+delta>=camera.Position.x
                   &&cubePositions[i].y-delta<=camera.Position.y&&cubePositions[i].y+delta>=camera.Position.y
                   &&cubePositions[i].z-delta<=camera.Position.z&&cubePositions[i].z+delta>=camera.Position.z&&!cubeIscrtan[j])

                {
                    cubeIscrtan[j]=true;
                    brojSakupljenih++;
                    continue;
                }
                if(!cubeIscrtan[j]) {
                    model = glm::translate(model, cubePositions[i]);
                    model = glm::scale(model, glm::vec3(0.20, 0.20, 0.20));
                    model = glm::rotate(model, glm::radians((float) glfwGetTime() * 10), cubePositions[i]);
                    draguljShader.setMat4("model", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    model = glm::mat4(1.0f);
                }
            }
            else
            {

                int j=i-2;
                //glm::vec3(-5.0f, 0.02f, 1.0f),//3.3
                if(diamondOutPositions[i].x-delta<=camera.Position.x&&diamondOutPositions[i].x+delta>=camera.Position.x
                   &&diamondOutPositions[i].y-delta<=camera.Position.y&&diamondOutPositions[i].y+delta>=camera.Position.y
                   &&diamondOutPositions[i].z-delta<=camera.Position.z&&diamondOutPositions[i].z+delta>=camera.Position.z&&!diamondOutIscrtan[j])

                {
                    diamondOutIscrtan[j]=true;
                    brojSakupljenih++;
                    continue;
                }
                if(!diamondOutIscrtan[j]) {
                    model = glm::translate(model, diamondOutPositions[i]);
                    model = glm::scale(model, glm::vec3(0.20, 0.20, 0.20));
                    model = glm::rotate(model, glm::radians((float) glfwGetTime() * 10), cubePositions[i]);
                    draguljShader.setMat4("model", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    model = glm::mat4(1.0f);
                }
            }


        }

        //Iscrtavanje DRVECA:
        treeShader.use();
        treeShader.setVec3("viewPos", camera.Position);

        // light properties
        treeShader.setVec3("light.direction", 0.0f, 1.0f, 0.0f);
        treeShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        treeShader.setVec3("light.diffuse", 0.05f, 0.05f, 0.05f);
        treeShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        treeShader.setVec3("pointLight.position", lightPos);
        treeShader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        treeShader.setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
        treeShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        treeShader.setFloat("pointLight.constant", 1.0f);
        treeShader.setFloat("pointLight.linear", 0.09);
        treeShader.setFloat("pointLight.quadratic", 0.032);

        // material properties
        treeShader.setFloat("material.shininess", 32.0f);

        //spot svetlo
        treeShader.setVec3("spotLight.position", camera.Position);
        treeShader.setVec3("spotLight.direction", camera.Front);
        treeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        treeShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        treeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        treeShader.setFloat("spotLight.constant", 1.0f);
        treeShader.setFloat("spotLight.linear", 0.09);
        treeShader.setFloat("spotLight.quadratic", 0.032);
        treeShader.setFloat("spotLight.cutOff", abs(glm::cos(glm::radians(12.5*LightPower*0.2))));
        treeShader.setFloat("spotLight.outerCutOff", abs(glm::cos(glm::radians(15.0*LightPower*0.2))));
        //DOVDE



        model = glm::mat4(1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, treeTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, treeTexture);
        model=glm::translate(model,glm::vec3(-4,3.8,-7.0));
        model=glm::scale(model,glm::vec3(9,9,9));
        treeShader.setMat4("model",model);
        treeShader.setMat4("view",view);
        treeShader.setMat4("projection",projection);
        glBindVertexArray(treeVAO);
        glDrawArrays(GL_TRIANGLES,0,6);

        model=glm::scale(model,glm::vec3(0.7,0.7,0.7));
        model=glm::translate(model,glm::vec3(1.0,-0.2,0));
        treeShader.setMat4("model",model);
        // glBindVertexArray(grassVAO);
        glDrawArrays(GL_TRIANGLES,0,6);

        model=glm::scale(model,glm::vec3(0.8,0.8,0.8));
        model=glm::translate(model,glm::vec3(-1.5,-0.12,0));
        treeShader.setMat4("model",model);
        // glBindVertexArray(grassVAO);
        glDrawArrays(GL_TRIANGLES,0,6);





        /*
        model = glm::mat4(1.0f);
       // model = glm::rotate(model, 0.8f*(float)angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
        shader.setMat4("model", model);
        //crtanje modela
        annies.Draw(shader);
        */

        /*model = glm::translate(model, banderaLightsPositions[0]);
         model = glm::scale(model, glm::vec3(0.2f));
         model=glm::rotate(model,glm::radians((float)80.0f),banderaLightsPositions[0]);




         draguljShader.setMat4("model", model);

         glBindVertexArray(draguljVAO);
         glDrawArrays(GL_TRIANGLES, 0, 36);
         */
        //dragulj end

        //glStencilMask(0xFF);
        //glStencilFunc(GL_ALWAYS, 0, 0xFF);
        //glEnable(GL_DEPTH_TEST);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteVertexArrays(1, &banderaVAO);
    glDeleteBuffers(1, &banderaVBO);
    glDeleteVertexArrays(1, &treeVAO);
    glDeleteBuffers(1, &treeVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);

    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        LightPower += 1.0 * 0.2;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        LightPower-=1.0*0.2;
    }


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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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
