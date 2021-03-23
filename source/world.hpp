#include "defs.hpp"


#ifndef WORLD_H
#define WORLD_H

class Node{
public:
    int north;
    int south;
    int east;
    int west;

    // Constructor to set up the node, by default all the walls are set
    Node(){
        north = WALL;
        south = WALL;
        east = WALL;
        west = WALL;
    }

    int path(int);
};

// Adds a pathway in the specified direction
int Node::path(int dir){
    if(dir == NORTH)
        north = PATH;
    else if(dir == SOUTH)
        south = PATH;
    else if(dir == EAST)
        east = PATH;
    else if(dir == WEST)
        west = PATH;
    else 
        return EXT_FAIL;
    return EXT_SUCC; 
}

class Maze{
public:
    std::vector<std::vector<Node>> maze;
    int rows;
    int columns;

    std::vector<GLfloat> wall_vertices;
    std::vector<unsigned int> wall_indices;

    Maze(int r, int c){
        rows = r;
        columns = c;
        // Set the size of the maze
        maze = std::vector<std::vector<Node>>(rows, std::vector<Node>(columns));
    }

    // Function to add a path in the maze
    int path(int, int, int);
    // Function to generate a maze procedurally
    int init();

    int draw(unsigned int, GLFWwindow*);

    int can_move(std::vector<GLfloat>, glm::vec3, int);

};

int Maze::path(int r, int c, int dir){
    if(dir == NORTH){
        if(r == 0)
            return EXT_FAIL;
        
        maze[r][c].path(NORTH);
        maze[r-1][c].path(SOUTH);
    }
    else if(dir == SOUTH){
        if(r == rows-1)
            return EXT_FAIL;
        
        maze[r][c].path(SOUTH);
        maze[r+1][c].path(NORTH);
    }
    else if(dir == WEST){
        if(c == 0)
            return EXT_FAIL;
        
        maze[r][c].path(WEST);
        maze[r][c-1].path(EAST);
    }
    else if(dir == EAST){
        if(c == columns-1)
            return EXT_FAIL;
    
        maze[r][c].path(EAST);
        maze[r][c+1].path(WEST);
    }
    else
        return EXT_FAIL;
    
    return EXT_SUCC;
}

int Maze::init(){
    // Visited array for maze generation
    std::vector<std::vector<bool>> vis(rows, std::vector<bool>(columns, false));
    // Stack to memorize path
    std::stack<std::pair<int, int>> st;

    // Set seed for random number generator
    srand(time(0));

    // Start at random location
    st.push(std::mp(rand() % rows, rand() % columns));

    // Generate a perfect maze
    while(!st.empty()) {
        std::vector<int> dir;

        std::pair<int, int> cur = st.top();
        
        
        vis[cur.ff][cur.ss] = true;

        if(cur.ff != 0 && vis[cur.ff-1][cur.ss] == false)
            dir.push_back(NORTH);
        if(cur.ff != rows-1 && vis[cur.ff+1][cur.ss] == false)
            dir.push_back(SOUTH);
        if(cur.ss != 0 && vis[cur.ff][cur.ss-1] == false)
            dir.push_back(WEST);
        if(cur.ss != columns-1 && vis[cur.ff][cur.ss+1] == false)
            dir.push_back(EAST);
        
        if(dir.size() == 0){
            st.pop();
            continue;
        }

        int sel = dir[rand() % dir.size()];
        path(cur.ff, cur.ss, sel);
      
        if(sel == NORTH)
            st.push(std::mp(cur.ff-1, cur.ss));
        if(sel == SOUTH)
            st.push(std::mp(cur.ff+1, cur.ss));
        if(sel == WEST)
            st.push(std::mp(cur.ff, cur.ss-1));
        if(sel == EAST)
            st.push(std::mp(cur.ff, cur.ss+1));


        dir.clear();
    } 

    // Arbitrarily carve pathways
    // Each wall has 0.2 probability of being removed
    for(int i = 0; i<rows; i++){
        for(int j = 0; j<columns; j++){
            if(i != 0){
                if(rand()%9 < 1)
                    path(i, j, NORTH);
            }
            if(i != rows-1){
                if(rand()%9 < 1)
                    path(i, j, SOUTH);
            }
            if(j != 0){
                if(rand()%9 < 1)
                    path(i, j, WEST);
            }
            if(j != columns-1){
                if(rand()%9 < 1)
                    path(i, j, EAST);
            }
        }
    }

    /*
        The maze's centre is at 0, 0
        The cell width is given in terms of pixels and is scaled accordingly
    */

    float height = (float)CELL_WIDTH/SCR_HEIGHT;
    float width = (float)CELL_WIDTH/SCR_WIDTH;

    float y = height*columns/2;
    float x = -width*rows/2;

    // Adding the wall terminal points positions
    for(int i = 0; i<=rows; i++){
        for(int j = 0; j<=columns; j++){
            wall_vertices.insert(wall_vertices.end(), {(float)(x + j*width), y, 0});
            wall_vertices.insert(wall_vertices.end(), {0.0f, 1.0f, 1.0f});
        }
        y -= height;
    }

    // Adding the walls to the buffer
    for(unsigned int i = 0; i<rows; i++){
        for(unsigned int j = 0; j<columns; j++){
            if(maze[i][j].north == WALL)
                wall_indices.insert(wall_indices.end(), {((columns+1)*i+j), ((columns+1)*i+j+1)});
            
            if(maze[i][j].west == WALL)
                wall_indices.insert(wall_indices.end(), {((columns+1)*i+j), ((columns+1)*i+j+columns+1)}); 
            
            if(j == columns-1 && maze[i][j].east == WALL)
                wall_indices.insert(wall_indices.end(), {((columns+1)*i+j+1), ((columns+1)*i+j+1+columns+1)});
            
            if(i == rows-1 && maze[i][j].south == WALL)
                wall_indices.insert(wall_indices.end(), {((columns+1)*i+j+columns+1), ((columns+1)*i+j+1+columns+1)});
        }
    }

    return EXT_SUCC;
}

int Maze::draw(unsigned int shaderProgram, GLFWwindow *window){
    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // first parameter is for number of buffer objects to create
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, wall_vertices.size()*sizeof(GLfloat), &wall_vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, wall_indices.size()*sizeof(unsigned int), &wall_indices[0], GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // the model and view matrices, 
    glm::mat4 model = glm::mat4(1.0f);

    // make the camera look in the 'front' direction
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // assign the uniform values for model and view matrices
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


    glDrawElements(GL_LINES, wall_indices.size(), GL_UNSIGNED_INT, 0);
}

int Maze::can_move(std::vector<GLfloat> vertices, glm::vec3 pos, int dir){
    
    float width = (float)CELL_WIDTH/SCR_WIDTH;
    float height = (float)CELL_WIDTH/SCR_HEIGHT;
    
    std::pair<float, float> zero = {-width*columns/2, height*rows/2};
    std::pair<float, float> ends_x = {INF, -INF};
    std::pair<float, float> ends_y = {INF, -INF};

    for(int i = 0; i<vertices.size(); i+=6){
        if(vertices[i] > ends_x.ss)
            ends_x.ss = vertices[i];
        if(vertices[i] < ends_x.ff)
            ends_x.ff = vertices[i];
        if(vertices[i+1] > ends_y.ss)
            ends_y.ss = vertices[i+1];
        if(vertices[i+1] < ends_y.ff)
            ends_y.ff = vertices[i+1];
    }

    if((pos.x + ends_x.ff - zero.ff)/width < 0)
        return EXT_FAIL;
    if((pos.x + ends_x.ss - zero.ff)/width > columns)
        return EXT_FAIL;
    if((zero.ss - pos.y - ends_y.ff)/height > rows)
        return EXT_FAIL;
    if((zero.ss - pos.y - ends_y.ss)/height < 0)
        return EXT_FAIL;
    
    int x_low = (pos.x + ends_x.ff - zero.ff)/width;
    int x_high = (pos.x + ends_x.ss - zero.ff)/width;

    int y_low = (zero.ss - pos.y - ends_y.ff)/height;
    int y_high = (zero.ss - pos.y - ends_y.ss)/height;

    if(x_low != x_high && y_low != y_high){
        if(x_low == -1 || x_high == columns || y_low == -1 || y_high == rows)
            return EXT_FAIL;
        if(maze[y_high][x_low].south == WALL || maze[y_high][x_low].east == WALL)
            return EXT_FAIL;
        if(maze[y_high][x_high].south == WALL || maze[y_high][x_high].west == WALL)
            return EXT_FAIL;
        if(maze[y_low][x_low].north == WALL || maze[y_low][x_low].east == WALL)
            return EXT_FAIL;
        if(maze[y_low][x_high].north == WALL || maze[y_low][x_high].west == WALL)
            return EXT_FAIL;
        return EXT_SUCC;
    }

    if(x_low != x_high){
        if(x_low == -1 || x_high == columns || y_low == -1 || y_high == rows)
            return EXT_FAIL;
        if(maze[y_low][x_low].east == WALL)
            return EXT_FAIL;
        if(maze[y_high][x_low].east == WALL)
            return EXT_FAIL;
        return EXT_SUCC;
    }
    if(y_low != y_high){
        if(y_low == -1 || y_high == rows || x_low == -1 || x_high == columns)
            return EXT_FAIL;
        if(maze[y_low][x_low].north == WALL)
            return EXT_FAIL;
        if(maze[y_low][x_high].north == WALL)
            return EXT_FAIL;
        return EXT_SUCC;
    }

    // std::cout << ret.ss << " " << ret.ff << "\n";

    // for(int i = 0; i<vertices.size(); i+=6){
    //     float x = pos.x + vertices[i];
    //     float y = pos.y + vertices[i+1];

    //     // std::cout << x << " " << y << " " << width << " " << height << "\n";

    //     int col = (int)((x - zero.ff)/width);
    //     int row = (int)((zero.ss - y)/height);

    //     if(ret.ff != col || ret.ss != row){
    //             bool improper = false;
    //             if(dir == NORTH && maze[ret.ss][ret.ff].north == WALL)
    //                 improper = true;
    //             if(dir == SOUTH && maze[ret.ss][ret.ff].south == WALL)
    //                 improper = true;
    //             if(dir == EAST && maze[ret.ss][ret.ff].east == WALL)
    //                 improper = true;
    //             if(dir == WEST && maze[ret.ss][ret.ff].west == WALL)
    //                 improper = true;
    //             if(improper){
    //                 std::cout << ret.ff << " " << ret.ss << "\n";
    //                 ret.ff = -1;
    //                 ret.ss = -1;
    //                 return ret;
    //             }
    //         }
    //     // std::cout << col << " " << row << "\n";
    // }
    // return ret;
}

#endif