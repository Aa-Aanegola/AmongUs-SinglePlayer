#include "defs.hpp"

#ifndef PLAYER_H
#define PLAYER_H

class Player{
public:
    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;

    float health;

    glm::vec3 position;

    Player(){
        health = PLAYER_HEALTH;
        position = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    int init(float, float);

    int draw(unsigned int, GLFWwindow*);

    int move(int, float, Maze&);

};

int Player::init(float pos_x, float pos_y){
    float width = ((float)CELL_WIDTH/SCR_WIDTH) * 0.3;
    float height = ((float)CELL_WIDTH/SCR_HEIGHT) * 0.3;

    // rectangle
    for(int i = -1; i<=1; i+=2){
        for(int j = -1; j<=1; j+=2){
            vertices.insert(vertices.end(), {(pos_x+j*width/2), (pos_y+i*height/2), 0});
            vertices.insert(vertices.end(), {0.4196f, 0.7568f, 0.1373f});
        }
    }

    for(unsigned int i = 0; i<=1; i++){
        indices.insert(indices.end(), {i, i+1, i+2});
    }

    // visor
    for(int i = -1; i<=1; i+=2){
        for(int j = -1; j<=1; j+=2){
            vertices.insert(vertices.end(), {(pos_x+j*width/3), (height/2+pos_y+i*height/4), 0});
            vertices.insert(vertices.end(), {0.9000f, 0.9100f, 0.9800f});
        }
    }

    for(unsigned int i = 0; i<=1; i++){
        indices.insert(indices.end(), {4+i, 5+i, 6+i});
    }

    //semi-circle
    vertices.insert(vertices.end(), {0.0f, height/2, 0.0f});
    vertices.insert(vertices.end(), {0.4196f, 0.7568f, 0.1373f});

    float cur_angle = 0;
    float increment = 5.0;
    
    for(int i = 0; i<= 180.0/increment; i++){
        vertices.insert(vertices.end(), {(pos_x+(width/2)*(float)cos(cur_angle)), (pos_y+(height/2)*(1+(float)sin(cur_angle))), 0});
        vertices.insert(vertices.end(), {0.4196f, 0.7568f, 0.1373f});
        cur_angle += glm::radians(increment);
    }

    for(unsigned int i = 0; i<180.0/increment; i++){
        indices.insert(indices.end(), {8, (i+9), (i+10)});
    }

    return EXT_SUCC;
}

int Player::draw(unsigned int shaderProgram, GLFWwindow *window){
    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // first parameter is for number of buffer objects to create
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // make the camera look in the 'front' direction
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

int Player::move(int dir, float speed, Maze &maze){

    if(dir == NORTH){
        glm::vec3 temp_position = position + speed*glm::vec3(0.0f, 1.0f, 0.0f);

        int maze_pos = maze.can_move(vertices, temp_position, dir);

        if(maze_pos == EXT_FAIL)
            return EXT_FAIL;

        position = temp_position;
        return EXT_SUCC;
    }
    if(dir == SOUTH){
        glm::vec3 temp_position = position - speed*glm::vec3(0.0f, 1.0f, 0.0f);

        int maze_pos = maze.can_move(vertices, temp_position, dir);

        if(maze_pos == EXT_FAIL)   
            return EXT_FAIL;

        position = temp_position;
        return EXT_SUCC;
    }
    if(dir == EAST){
        glm::vec3 temp_position = position + speed*glm::vec3(1.0f, 0.0f, 0.0f);

        int maze_pos = maze.can_move(vertices, temp_position, dir);

        if(maze_pos == EXT_FAIL)
            return EXT_FAIL;

        position = temp_position;
        return EXT_SUCC;
    }
    if(dir == WEST){
        glm::vec3 temp_position = position - speed*glm::vec3(1.0f, 0.0f, 0.0f);

        int maze_pos = maze.can_move(vertices, temp_position, dir);

        if(maze_pos == EXT_FAIL)
            return EXT_FAIL;

        position = temp_position;
        return EXT_SUCC;
    }
    return EXT_FAIL;
}



#endif