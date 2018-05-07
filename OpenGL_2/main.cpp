#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "SOIL2/SOIL2.h"

#include "BULLET/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BULLET/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BULLET/btBulletCollisionCommon.h"


// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"


// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera( glm::vec3( 0.0f, 0.0f, 3.0f ) );
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos( 1.2f, 1.0f, 2.0f );

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame


int main( )
{
    // Init GLFW
    glfwInit( );
    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "SendNudesYaBish", nullptr, nullptr );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    
    // GLFW Options
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // OpenGL options
    glEnable( GL_DEPTH_TEST );
    
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    
    // Set up the collision configuration and dispatcher
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    // The actual physics solver
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    
    // The world.
    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0,-9.81f,0));
    
    // Setup and compile our shaders
    Shader lightingShader( "resources/shaders/lighting.vs", "resources/shaders/lighting.frag" );
    
    
    Shader skyboxShader( "resources/shaders/skybox.vs", "resources/shaders/skybox.frag" );
    
    Model ourModel2( "resources/models/nanosuit.obj" );
    Model bridgeModel( "resources/models/bridge/bridge.obj" );

    
    GLfloat skyboxVertices[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3(  0.7f,  0.2f,  2.0f      ),
        glm::vec3(  2.3f, -3.3f, -4.0f      ),
        glm::vec3(  -4.0f,  2.0f, -12.0f    ),
        glm::vec3(  0.0f,  0.0f, -3.0f      )
    };

    
    // Setup skybox VAO
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays( 1, &skyboxVAO );
    glGenBuffers( 1, &skyboxVBO );
    glBindVertexArray( skyboxVAO );
    glBindBuffer( GL_ARRAY_BUFFER, skyboxVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), ( GLvoid * ) 0 );
    glBindVertexArray(0);
    
    // Load textures
    GLuint cubeTexture = TextureLoading::LoadTexture( "resources/images/container2.png" );
    
    // Cubemap (Skybox)
    vector<const GLchar*> faces;
    faces.push_back( "resources/images/skybox/right.png" );
    faces.push_back( "resources/images/skybox/left.png" );
    faces.push_back( "resources/images/skybox/top.png" );
    faces.push_back( "resources/images/skybox/bottom.png" );
    faces.push_back( "resources/images/skybox/back.png" );
    faces.push_back( "resources/images/skybox/front.png" );
    GLuint cubemapTexture = TextureLoading::LoadCubemap( faces );
    
    
    glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( float )SCREEN_WIDTH/( float )SCREEN_HEIGHT, 0.1f, 1000.0f );
    
    // Game loop
    while( !glfwWindowShouldClose( window ) )
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check and call events
        glfwPollEvents( );
        DoMovement( );
        
        // Clear the colorbuffer
        glClearColor( 0.05f, 0.05f, 0.05f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        
        glm::mat4 model;
        glm::mat4 view ;
        
        
        
        lightingShader.Use( );
        GLint viewPosLoc = glGetUniformLocation( lightingShader.Program, "viewPos" );
        glUniform3f( viewPosLoc, camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z);
        
        
        // Directional light
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirLight.direction" ), -0.2f, -1.0f, -0.3f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirLight.ambient" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirLight.diffuse" ), 0.4f, 0.4f, 0.4f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirLight.specular" ), 0.5f, 0.5f, 0.5f );
        
        // Point light 1
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].position" ), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].diffuse" ), 0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[0].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[0].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[0].quadratic" ), 0.032f );
        
        // Point light 2
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].position" ), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].diffuse" ), 0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[1].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[1].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[1].quadratic" ), 0.032f );
        
        // Point light 3
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].position" ), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].diffuse" ), 0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[2].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[2].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[2].quadratic" ), 0.032f );
        
        // Point light 4
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].position" ), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].diffuse" ), 0.8f, 0.8f, 0.8f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[3].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[3].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[3].quadratic" ), 0.032f );
        
        // SpotLight
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.position" ), camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.direction" ), camera.GetFront( ).x, camera.GetFront( ).y, camera.GetFront( ).z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.ambient" ), 0.0f, 0.0f, 0.0f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.diffuse" ), 1.0f, 1.0f, 1.0f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.quadratic" ), 0.032f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.cutOff" ), glm::cos( glm::radians( 12.5f ) ) );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.outerCutOff" ), glm::cos( glm::radians( 15.0f ) ) );
        
        
        view = camera.GetViewMatrix( );
        
        
       
        model = glm::translate(model, glm::vec3(1.2f, 3.0f, 5.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        
        
        GLint modelLoc = glGetUniformLocation( lightingShader.Program, "model" );
        GLint viewLoc = glGetUniformLocation( lightingShader.Program, "view" );
        GLint projLoc = glGetUniformLocation( lightingShader.Program, "projection" );
        
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        ourModel2.Draw( lightingShader );
        
        model = glm::mat4(1.0);
        
        model = glm::scale(model, glm::vec3(5.0f,8.0f,5.0f));
        
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        bridgeModel.Draw( lightingShader );

        
        
        
        // Draw skybox as last
        glDepthFunc( GL_LEQUAL );  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use( );
        view = glm::mat4( glm::mat3( camera.GetViewMatrix( ) ) );    // Remove any translation component of the view matrix
        
        glUniformMatrix4fv( glGetUniformLocation( skyboxShader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( glGetUniformLocation( skyboxShader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
        
        // skybox cube
        glBindVertexArray( skyboxVAO );
        glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        glDepthFunc( GL_LESS ); // Set depth function back to default
        
        
        
        // Swap the buffers
        glfwSwapBuffers( window );
    }
    
    glfwTerminate( );
    return 0;
}


// Moves/alters the camera positions based on user input
void DoMovement( )
{
    // Camera controls
    if ( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }
    
    if ( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if ( GLFW_KEY_ESCAPE == key && GLFW_PRESS == action )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if ( key >= 0 && key < 1024 )
    {
        if ( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if ( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if ( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}
