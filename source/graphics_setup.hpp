#include "defs.hpp"
#include "world.hpp"
#include "player.hpp"

#ifndef SHADERS_H
#define SHADERS_H
 

const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = view * model * vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";


// fragment shader
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

GLFWwindow *setup_graphics(unsigned int &shaderProgram, GLFWwindow *window){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Game", NULL, NULL);
    std::cout << window;
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
   

    // glad: load all OpenGL function pointers connect to opengl implementaion from driver
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return NULL;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return NULL;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return NULL;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, SCR_WIDTH/SCR_HEIGHT, GL_FALSE, glm::value_ptr(projection));

    return window;
}

void processInput(GLFWwindow *window, Maze &world, Player &player){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float x_speed = 5.0/SCR_WIDTH;
    float y_speed = 5.0/SCR_HEIGHT;

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        int ret = player.move(NORTH, y_speed, world);
        if(ret == EXT_SUCC)
            cameraPos += y_speed * cameraUp;
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        int ret = player.move(SOUTH, y_speed, world);
        if(ret == EXT_SUCC)
            cameraPos -= y_speed * cameraUp;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        int ret = player.move(EAST, x_speed, world);
        if(ret == EXT_SUCC)
            cameraPos += x_speed * cameraRight;
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        int ret = player.move(WEST, x_speed, world);
        if(ret == EXT_SUCC)
            cameraPos -= x_speed * cameraRight;
    }

    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
        world.lights_on();
    }
    if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
        world.lights_off();
    }
}

#endif