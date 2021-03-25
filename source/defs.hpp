#ifndef DEFS_H
#define DEFS_H

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define EXT_FAIL           -1
#define EXT_SUCC            1

#define WALL                1
#define PATH                0

#define NORTH               2
#define SOUTH               3
#define WEST                4
#define EAST                5

#define ff              first
#define ss             second

#define SCR_WIDTH        1920
#define SCR_HEIGHT       1080

#define CELL_WIDTH        200
#define MAZE_HEIGHT        25
#define MAZE_WIDTH         25

#define NUM_POWERUP        10

#define INF               1e9
#define GRADIENT          0.2

#define min(a, b)   (a<b?a:b)
#define max(a, b)   (a>b?a:b)


// Global variables
unsigned int shaderProgram;
unsigned int text_shader;
GLFWwindow *window;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 1.0);
glm::vec3 cameraFront = glm::vec3(0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraRight = glm::vec3(1.0, 0.0, 0.0);
float width = (float)CELL_WIDTH/SCR_WIDTH;
float height = (float)CELL_WIDTH/SCR_HEIGHT;
#endif