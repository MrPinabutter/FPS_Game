#define GLEW_STATIC
#include <iostream>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>
#include "SOIL2/SOIL2.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Camera.h"

const GLint WIDTH = 800, HEIGHT = 600;
bool flash = true;
int SCREEN_WIDTH, SCREEN_HEIGHT;

void updateInput(GLFWwindow* window){   
  // Close window with esc press
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH/2.0f;
GLfloat lastY = WIDTH/2.0f;

bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

  // Suport to alpha (png)
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Build and compile shaders and lamp
  Shader lightShader("lighting.vs", "lighting.frag");
  // Shader lampShader("lamp.vs", "lamp.frag");

  // use with Perspective Projection
  GLfloat vertices[] =
  {
    // Positions            // Normals              // Texture Coords
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
    0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
    
    -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
    
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
    glm::vec3(  0.0f,   0.0f,   0.0f),
    glm::vec3(  2.0f,   5.0f,   -15.0f),
    glm::vec3(  -1.5f,  -2.2f,  -2.5f),
    glm::vec3(  -3.8f,  -2.0f,  -12.3f),
    glm::vec3(  2.4f,   -0.4f,  -3.5f),
    glm::vec3(  -1.7f,  3.0f,   -7.5f),
    glm::vec3(  1.3f,   -2.0f,  -2.5f),
    glm::vec3(  1.5f,   2.0f,   -2.5f),
    glm::vec3(  1.5f,   0.2f,   -1.5f),
    glm::vec3(  -1.3f,  1.0f,   -1.5f)
  };

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

/*
  GLuint lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(lightVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) 0 );
  glEnableVertexAttribArray(0); // Unbind VAO
*/

  glBindVertexArray(0); // Unbind boxVAO

  GLuint diffuseMap, specularMap, emissionMap;
  glGenTextures(1, &diffuseMap);
  glGenTextures(1, &specularMap);
  glGenTextures(1, &emissionMap);

  int textureWidth, textureHeight;
  unsigned char *image;
  
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

  glBindTexture(GL_TEXTURE_2D, 0);
  lightShader.Use();
  glUniform1i(glGetUniformLocation(lightShader.Program, "material.diffuse"), 0);
  glUniform1i(glGetUniformLocation(lightShader.Program, "material.specular"), 1);

  glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( GLfloat )SCREEN_WIDTH / ( GLfloat )SCREEN_HEIGHT, 0.1f, 100.0f );

  while(!glfwWindowShouldClose(window)){
    lightPos.x -= 0.005;
    lightPos.z -= 0.005;
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwPollEvents();
    DoMovement();
    updateInput(window);

    // RENDER
    // Clear
    // glClearColor(0.2f, 0.6f, 0.8f, 1.0f);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Objects
    lightShader.Use();
    GLint lightPosLoc = glGetUniformLocation(lightShader.Program, "light.position" );
    GLint lightSpotDirLoc  = glGetUniformLocation(lightShader.Program, "light.direction");
    GLint lightSpotCutOffLoc  = glGetUniformLocation(lightShader.Program, "light.cutOff");
    GLint lightSpotOuterCutOffLoc  = glGetUniformLocation(lightShader.Program, "light.outerCutOff");
    // GLint lightDirLoc = glGetUniformLocation(lightShader.Program, "light.direction");
    GLint viewPosLoc = glGetUniformLocation(lightShader.Program, "viewPos" );
    glUniform3f(lightSpotDirLoc, camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
    if(flash){
      glUniform1f(lightSpotCutOffLoc, glm::cos(glm::radians(12.5f)));
      glUniform1f(lightSpotOuterCutOffLoc, glm::cos(glm::radians(24.5f)));
    }else{
      glUniform1f(lightSpotCutOffLoc, glm::cos(glm::radians(0.0f)));
      glUniform1f(lightSpotOuterCutOffLoc, glm::cos(glm::radians(0.0f)));
    }
    glUniform3f(lightPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
    // glUniform3f(lightDirLoc, -0.2f, 1.0f, -0.3f);
    glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

    glUniform3f(glGetUniformLocation(lightShader.Program, "light.ambient"), 0.1f, 0.1f, 0.1f);
    glUniform3f(glGetUniformLocation(lightShader.Program, "light.diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(lightShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightShader.Program, "light.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightShader.Program, "light.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(lightShader.Program, "light.quadratic"), 0.032f);

    glUniform1f(glGetUniformLocation(lightShader.Program, "material.shininess"), 32.0f);

    glm::mat4 view(1.0f);
    view = camera.GetViewMatrix();

    GLint modelLoc = glGetUniformLocation(lightShader.Program, "model");
    GLint viewLoc = glGetUniformLocation(lightShader.Program, "view");
    GLint projectionLoc = glGetUniformLocation(lightShader.Program, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glm::mat4 model;
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

    // Lamp
    /*
    lampShader.Use();

    modelLoc = glGetUniformLocation(lampShader.Program, "model");
    viewLoc = glGetUniformLocation(lampShader.Program, "view");
    projectionLoc = glGetUniformLocation(lampShader.Program, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0); // Unbind

    */
    // Swap buffers
    glfwSwapBuffers(window);
  }
  glDeleteVertexArrays(1, &boxVAO);
  // glDeleteVertexArrays(1, &lightVAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();

  return EXIT_SUCCESS;
}

void DoMovement(){
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
