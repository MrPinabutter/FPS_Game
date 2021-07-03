#define GLEW_STATIC
#include <iostream>
#include <string>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>
#include "SOIL2/SOIL2.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Camera.h"
#include "Model.h"

const GLint WIDTH = 800, HEIGHT = 600;
bool flash = true;
// Bullets Props
bool shoot[4];
bool showBullet[4];
bool backBullets = false;
int actualBullet = -1; // Unlock weapon on first click
glm::vec3 bulletDirections[4];

int SCREEN_WIDTH, SCREEN_HEIGHT;

// Close window with esc press
void updateInput(GLFWwindow* window){   
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

// Callbacks
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void MouseClicksCallback(GLFWwindow* window, int button, int action, int mods);
void DoMovement();


// Colissions
bool colission(float x1, float y1 , float z1, float height, float width, float depth, float x2, float y2, float z2, float raio2);
bool colissionSphereSphere(float x1, float y1, float z1, float raio1, float x2, float y2, float z2, float raio2);

// Initial camera position
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH/2.0f;
GLfloat lastY = WIDTH/2.0f;

// Keyboard keys
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main() {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a window
  GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr );
  if(window == NULL)
  {
    std::cout << "Window is null" << "\n";
  }
    
  glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
  
  if ( nullptr == window )
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate( );  
    
    return EXIT_FAILURE;
  }
    
  glfwMakeContextCurrent( window );

  glfwSetKeyCallback(window, KeyCallback);
  glfwSetCursorPosCallback(window, MouseCallback);
  glfwSetMouseButtonCallback(window, MouseClicksCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
  glewExperimental = GL_TRUE;

  // Initialize GLEW to setup the OpenGL Function pointers
  if ( GLEW_OK != glewInit( ) )
  {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return EXIT_FAILURE;
  }
  
  // Define the viewport dimensions
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  glEnable(GL_DEPTH_TEST);

  // Compile shaders
  Shader shader( "res/shaders/modelLoading.vs", "res/shaders/modelLoading.frag" );
    
  // Load models
  Model gunModel( "res/models/Gun.obj" );
    

  // Suport to alpha (png)
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Build and compile shaders and lamp
  Shader lightShader("lighting.vs", "lighting.frag");
  Shader lampShader("lamp.vs", "lamp.frag");

  // use with Perspective Projection
  GLfloat vertices[] =
  {
    // Positions            // Normals              // Texture Coords
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
    0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
    0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
    
    -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
    
    -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
    
    0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
    0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
    
    -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
    0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
    
    -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
    0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
  };

  glm::vec3 cubePositions[] = {
    glm::vec3(  0.0f,   4.0f,   0.0f),
    glm::vec3(  2.0f,   5.0f,   15.0f),
    glm::vec3(  -15.0f,  -2.2f,  -2.5f),
    glm::vec3(  -5.0f,  -2.0f,  -12.3f),
    glm::vec3(  2.4f,   -0.4f,  -13.5f),
    glm::vec3(  -1.7f,  3.0f,   -7.5f),
    glm::vec3(  13.0f,   -2.0f,  -2.5f),
    glm::vec3(  1.5f,   2.0f,   -2.5f),
    glm::vec3(  1.5f,   0.2f,   -11.5f),
    glm::vec3(  -1.3f,  1.0f,   -9.0f)
  };

  // Positions of point lights
  glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f, -0.2f, 2.0f),
    glm::vec3(7.0f, -1.3f, -4.0f),
    glm::vec3(-4.0f, 0.0f, -12.0f),
    glm::vec3(-7.0f, 1.0f, -3.0f)
  };

  // Wall Positions
  glm::vec3 wallCubePositions[] = {
    glm::vec3(30.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, -30.0f),
    glm::vec3(-30.0f,0.0f, 0.0f),
    glm::vec3(0.0f,0.0f, 30.0f)
  };

  // Init Bullets
  for(int i = 0; i < 4; i++){
    shoot[i] = false;
  }

  GLuint VBO, boxVAO;
  glGenVertexArrays(1, &boxVAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(boxVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0 );
  glEnableVertexAttribArray(0); 

  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(1); 

  // Texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (6 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(2); 

  glBindVertexArray(0); // Unbind boxVAO

  GLuint lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(lightVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0 );
  glEnableVertexAttribArray(0); // Unbind VAO

  glBindVertexArray(0); // Unbind boxVAO

  GLuint diffuseMap, specularMap, diffuseMapFloor, specularMapFloor, diffuseMapWall, specularMapWall,emissionMap;
  glGenTextures(1, &diffuseMap);
  glGenTextures(1, &specularMap);
  glGenTextures(1, &diffuseMapFloor);
  glGenTextures(1, &specularMapFloor);
  glGenTextures(1, &diffuseMapWall);
  glGenTextures(1, &specularMapWall);
  glGenTextures(1, &emissionMap);

  int textureWidth, textureHeight;
  unsigned char *image;
  
  // Diamond blocks
  // diffuseMap
  image = SOIL_load_image("res/images/diamond.png", & textureWidth, & textureHeight, 0, SOIL_LOAD_RGB);
  glBindTexture(GL_TEXTURE_2D, diffuseMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  
  // specularMap
  image = SOIL_load_image("res/images/diamond_mask.png", & textureWidth, & textureHeight, 0, SOIL_LOAD_RGB);
  glBindTexture(GL_TEXTURE_2D, specularMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

  // GROUND TEXTURES
  // diffuseMap
  image = SOIL_load_image("res/images/snow_02_diff_2k.png", & textureWidth, & textureHeight, 0, SOIL_LOAD_RGB);
  glBindTexture(GL_TEXTURE_2D, diffuseMapFloor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  
  // specularMap
  image = SOIL_load_image("res/images/snow_02_spec_2k.png", & textureWidth, & textureHeight, 0, SOIL_LOAD_RGB);
  glBindTexture(GL_TEXTURE_2D, specularMapFloor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

  // WALL TEXTURES
  // diffuseMap
  image = SOIL_load_image("res/images/wall_dif.png", & textureWidth, & textureHeight, 0, SOIL_LOAD_RGB);
  glBindTexture(GL_TEXTURE_2D, diffuseMapWall);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  
  // specularMap
  image = SOIL_load_image("res/images/wall_spec.png", & textureWidth, & textureHeight, 0, SOIL_LOAD_RGB);
  glBindTexture(GL_TEXTURE_2D, specularMapWall);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

  glBindTexture(GL_TEXTURE_2D, 0);
  lightShader.Use();
  glUniform1i(glGetUniformLocation(lightShader.Program, "material.diffuse"), 0);
  glUniform1i(glGetUniformLocation(lightShader.Program, "material.specular"), 1);

  glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( GLfloat )SCREEN_WIDTH / ( GLfloat )SCREEN_HEIGHT, 0.1f, 100.0f );

  while(!glfwWindowShouldClose(window)){
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwPollEvents();
    DoMovement();
    updateInput(window);

    // RENDER
    // Clear
    glClearColor(0.08f, 0.14f, 0.2f, 1.0f);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Objects
    lightShader.Use();

    GLint viewPosLoc = glGetUniformLocation(lightShader.Program, "viewPos" );
    glUniform3f( viewPosLoc, camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z);
    glUniform1f(glGetUniformLocation(lightShader.Program, "material.shininess"), 32.0f);

    // Directional Light
    glUniform3f( glGetUniformLocation( lightShader.Program, "dirLight.direction" ), -0.2f, -1.0f, -0.3f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "dirLight.ambient" ), 0.05f, 0.05f, 0.05f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "dirLight.diffuse" ), 0.4f, 0.4f, 0.4f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "dirLight.specular" ), 0.5f, 0.5f, 0.5f );
    
    // Point light 1
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[0].position" ), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[0].ambient" ), 0.05f, 0.05f, 0.05f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[0].diffuse" ), 0.6f, 0.4f, 1.0f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[0].specular" ), 0.6f, 0.4f, 1.0f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[0].constant" ), 1.0f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[0].linear" ), 0.09f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[0].quadratic" ), 0.032f );
    
    // Point light 2
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[1].position" ), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[1].ambient" ), 0.05f, 0.05f, 0.05f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[1].diffuse" ), 0.6f, 0.4f, 1.0f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[1].specular" ), 0.6f, 0.4f, 1.0f  );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[1].constant" ), 1.0f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[1].linear" ), 0.09f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[1].quadratic" ), 0.032f );
    
    // Point light 3
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[2].position" ), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[2].ambient" ), 0.05f, 0.05f, 0.05f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[2].diffuse" ), 0.6f, 0.4f, 1.0f);
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[2].specular" ), 0.6f, 0.4f, 1.0f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[2].constant" ), 1.0f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[2].linear" ), 0.09f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[2].quadratic" ), 0.032f );
    
    // Point light 4
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[3].position" ), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[3].ambient" ), 0.05f, 0.05f, 0.05f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[3].diffuse" ), 0.6f, 0.4f, 1.0f);
    glUniform3f( glGetUniformLocation( lightShader.Program, "pointLights[3].specular" ), 0.6f, 0.4f, 1.0f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[3].constant" ), 1.0f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[3].linear" ), 0.09f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "pointLights[3].quadratic" ), 0.032f );
    
    // SpotLight
    glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.position" ), camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z );
    glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.direction" ), camera.GetFront( ).x, camera.GetFront( ).y, camera.GetFront( ).z );
    glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.ambient" ), 0.0f, 0.0f, 0.0f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.diffuse" ), 1.0f, 1.0f, 1.0f );
    glUniform3f( glGetUniformLocation( lightShader.Program, "spotLight.specular" ), 1.0f, 1.0f, 1.0f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "spotLight.constant" ), 1.0f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "spotLight.linear" ), 0.09f );
    glUniform1f( glGetUniformLocation( lightShader.Program, "spotLight.quadratic" ), 0.032f );
    if(flash){
      glUniform1f(glGetUniformLocation(lightShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
      glUniform1f(glGetUniformLocation(lightShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(20.0f)));
    }else{
      glUniform1f(glGetUniformLocation(lightShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(0.0f)));
      glUniform1f(glGetUniformLocation(lightShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(0.0f)));
    }

    glm::mat4 view(1.0f);
    view = camera.GetViewMatrix();

    GLint modelLoc = glGetUniformLocation(lightShader.Program, "model");
    GLint viewLoc = glGetUniformLocation(lightShader.Program, "view");
    GLint projectionLoc = glGetUniformLocation(lightShader.Program, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Floor Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMapFloor);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMapFloor);

    // Floor Model
    glm::mat4 model;
    glBindVertexArray(boxVAO);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
    model = glm::scale(model, glm::vec3(60.0f, 1.0f, 60.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Diamond Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    // Flying boxes
    glBindVertexArray(boxVAO);
    for(GLuint i = 0; i < 10; i++){
      model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      
      GLfloat angle = 20.0f * i;

      model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      glDrawArrays(GL_TRIANGLES, 0, 36);
    } 
    glBindVertexArray(0);

    // Wall textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMapWall);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMapWall);

    // Wall Models
    glBindVertexArray(boxVAO);
    for(GLuint i = 0; i < 4; i++){
      // cout << i << endl;
      model = glm::mat4(1.0f);
      model = glm::translate(model, wallCubePositions[i]);
      if(wallCubePositions[i].x != 0)
        model = glm::scale(model, glm::vec3(3.0f, 10.0f, 60.0f));
      if(wallCubePositions[i].z != 0)
        model = glm::scale(model, glm::vec3(60.0f, 10.0f, 3.0f));
      
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      glDrawArrays(GL_TRIANGLES, 0, 36);
    } 
    glBindVertexArray(0);

    // Load Models
    shader.Use();

    glUniformMatrix4fv( glGetUniformLocation( shader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
    glUniformMatrix4fv( glGetUniformLocation( shader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );

    // Draw the loaded model
    // translate to player
    model = glm::mat4(1.0f);
    model = glm::translate( model, glm::vec3( camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z ) ); 
    model = glm::translate( model, glm::vec3( camera.GetFront().x, camera.GetFront().y, camera.GetFront().z ) ); 
    model = glm::rotate( model, glm::radians(camera.GetRight()), glm::vec3(0.0f, -1.0f, 0.0f ));
    model = glm::rotate( model, 60.0f, glm::vec3(0.0f, 1.0f, 0.0f ) );
    model = glm::rotate( model, camera.GetFront().y, glm::vec3(0.0f, 0.0f, -1.0f ) );
    model = glm::scale( model, glm::vec3(0.9f));
    model = glm::translate( model, glm::vec3( 0.3, -0.3f, -0.4f ) ); 

    glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
    gunModel.Draw( shader );

    // Lamp
    lampShader.Use();

    modelLoc = glGetUniformLocation(lampShader.Program, "model");
    viewLoc = glGetUniformLocation(lampShader.Program, "view");
    projectionLoc = glGetUniformLocation(lampShader.Program, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(lightVAO);

    for(GLuint i = 0; i < 4; i++){
      int countBulletsOnAir = 0; //
      model = glm::mat4(1.0f);
      model = glm::translate(model, pointLightPositions[i]);

      // Handle back bullets to player
      if(backBullets){
        for(int i = 0; i < 4; i++){ 
          // Push only shooted bullets
          if(shoot[i]){ 
            bulletDirections[i].x = (camera.GetPosition().x - pointLightPositions[i].x)/10;
            bulletDirections[i].y = (camera.GetPosition().y - pointLightPositions[i].y)/10;
            bulletDirections[i].z = (camera.GetPosition().z - pointLightPositions[i].z)/10;
            actualBullet = -1;
          }else{
            countBulletsOnAir ++;
          }
        }
        // Catch bullets
        if(colissionSphereSphere(
          pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z, 0.3f, // bullet
          camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 1.0f)       // Player
        ){ 
          pointLightPositions[i] = glm::vec3(-999.0f);
          shoot[i] = false;
          if(countBulletsOnAir == 4){ 
            backBullets = false;
            countBulletsOnAir = 0;
          }
        }
      }

      // Handle shoot
      if(shoot[i]) { 
        pointLightPositions[i].x += bulletDirections[i].x/3;
        pointLightPositions[i].y += bulletDirections[i].y/3;
        pointLightPositions[i].z += bulletDirections[i].z/3;
        // Handle back bullets to player
      } else
      // Recharge bullet to trigger
      if(i == actualBullet) { 
        pointLightPositions[i].x = camera.GetPosition().x;
        pointLightPositions[i].y = camera.GetPosition().y - 0.2f;
        pointLightPositions[i].z = camera.GetPosition().z;
        model = glm::translate(model, camera.GetPosition()); 
        model = glm::translate(model, camera.GetFront()); 
        model = glm::rotate(model, glm::radians( camera.GetRight()), glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, 60.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, camera.GetFront().y, glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(0.05f));
        model = glm::translate( model, glm::vec3(0.3, -0.4f, -0.4f)); 
      } 
      model = glm::scale(model, glm::vec3(0.1f)); 
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0); // Unbind

    // Swap buffers
    glfwSwapBuffers(window);
  }
  glDeleteVertexArrays(1, &boxVAO);
  glDeleteVertexArrays(1, &lightVAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();

  return EXIT_SUCCESS;
}

void DoMovement(){
  camera.ProcessGravity(deltaTime, keys[GLFW_KEY_LEFT_CONTROL]);
  
  if((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT])){
    deltaTime = 2 * deltaTime;
  }

  if(keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]){
    camera.ProcessKeyboard(FORWARD, deltaTime);
  }
  if(keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]){
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  }
  if(keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]){
    camera.ProcessKeyboard(LEFT, deltaTime);
  }
  if(keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]){
    camera.ProcessKeyboard(RIGHT, deltaTime);
  }
  if(keys[GLFW_KEY_R]){
    backBullets = true;
    actualBullet = -1;
  }
  if(keys[GLFW_KEY_SPACE]) {
    camera.ProcessKeyboard(UP, deltaTime);
  }
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode){
  if(key >= 0 && key < 1024){
    if(GLFW_PRESS == action){
      keys[key] = true;
    }else if(GLFW_RELEASE == action){
      keys[key] = false;
    }
    if(key == 102 || key == 70){
      if(GLFW_PRESS == action){
        flash = !flash;
      } 
    }
    if(key == GLFW_KEY_SPACE) 
    {
      if(GLFW_PRESS == action)
      camera.ProcessJump(deltaTime, 4);
    }
  }
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos){
  if(firstMouse){
    lastX = xPos;
    lastY = yPos;
    firstMouse = false;
  }

  GLfloat xOffset = xPos - lastX;
  GLfloat yOffset = lastY - yPos;

  lastX = xPos;
  lastY = yPos;

  camera.ProcessMouseMovement(xOffset, yOffset);
}

void MouseClicksCallback(GLFWwindow* window, int button, int action, int mods){
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
    shoot[actualBullet] = true;
    bulletDirections[actualBullet].x = camera.GetFront().x;
    bulletDirections[actualBullet].y = camera.GetFront().y;
    bulletDirections[actualBullet].z = camera.GetFront().z;
    if(actualBullet < 4){
      actualBullet ++;
    }
  }
}

bool colissionSphereSphere(float x1, float y1, float z1, float raio1, float x2, float y2, float z2, float raio2) {
	float d = sqrt((- x1 + x2)  * (- x1 +  x2) + (-y1 + y2) * (-y1 + y2) +  ( -z1 + z2) * (-z1 + z2));
	if(d <= (raio1 + raio2))
		return true;
	else
		return false;
}

bool colission(float x1, float y1 , float z1, float height, float width, float depth, float x2, float y2, float z2, float raio2) {
	double sphereXDistance = fabs(x2 - x1);
	double sphereYDistance = fabs(y2 - y1);//profundidade no caso do jogo
	double sphereZDistance = fabs(z2 - z1);

	if (sphereXDistance > (width / 2 + raio2)) {
		return false;
	}
	if (sphereYDistance > (depth / 2 + raio2)) {
		return false;
	}
	if (sphereZDistance > (height / 2 + raio2)) {
		return false;
	}

	if (sphereXDistance <= (width / 2)) {
		return true;
	}
	if (sphereYDistance <= (depth / 2)) {
		return true;
	}
	if (sphereZDistance <= (height / 2)) {
		return true;
	}

	double cornerDistance_sq = ((sphereXDistance - width / 2) * (sphereXDistance - width / 2)) +
    ((sphereYDistance - depth / 2) * (sphereYDistance - depth / 2) +
    ((sphereZDistance - height / 2) * (sphereZDistance - height / 2)));

	return (cornerDistance_sq <= (raio2 * raio2));
}