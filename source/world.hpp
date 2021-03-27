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

    bool lights;
    int tasks;

    std::vector<GLfloat> wall_vertices;
    std::vector<unsigned int> wall_indices;

    std::vector<GLfloat> end_vertices;
    std::vector<unsigned int> end_indices; 

    std::vector<GLfloat> bot_kill_vertices;
    std::vector<unsigned int> bot_kill_indices;

    std::vector<GLfloat> powerup_vertices;
    std::vector<unsigned int> powerup_indices;

    std::vector<GLfloat> powerups_vertices;
    std::vector<unsigned int> powerups_indices;

    std::pair<int, int> end;

    std::pair<int, int> bot_kill;

    std::pair<int, int> powerup;

    bool powerup_activated;

    std::vector<std::pair<std::pair<int, int>, int>> powerup_pos;

    Maze(int r, int c){
        rows = r;
        columns = c;
        
        maze = std::vector<std::vector<Node>>(rows, std::vector<Node>(columns));

        lights = true;
        tasks = 2;

        powerup_activated = false;
    }

    // Function to add a path in the maze
    int path(int, int, int);
    // Function to generate a maze procedurally
    int init();

    int draw(unsigned int, GLFWwindow*);

    int can_move(std::vector<GLfloat>, glm::vec3, int);

    std::pair<std::pair<int, int>, std::pair<int, int>> get_bounds(std::vector<float>, glm::vec3);

    int update_lights(std::vector<float>, glm::vec3);

    int lights_on();
    int lights_off();

    int shortest_path(std::vector<float>, glm::vec3, std::vector<float>, glm::vec3);

    void activate_powerups();
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
    st.push(std::make_pair(rand() % rows, rand() % columns));

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
            st.push(std::make_pair(cur.ff-1, cur.ss));
        if(sel == SOUTH)
            st.push(std::make_pair(cur.ff+1, cur.ss));
        if(sel == WEST)
            st.push(std::make_pair(cur.ff, cur.ss-1));
        if(sel == EAST)
            st.push(std::make_pair(cur.ff, cur.ss+1));


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

    end = std::make_pair(rand() % MAZE_WIDTH, rand() % MAZE_HEIGHT);

    x = width * (end.ff - MAZE_WIDTH/2);
    y = height * (MAZE_HEIGHT/2 - end.ss);

    for(int i = -1; i<=1; i+=2){
        for(int j = -1; j<=1; j+=2){
            end_vertices.insert(end_vertices.end(), {x + j*width/3, y + i*height/3, 0});
            end_vertices.insert(end_vertices.end(), {0.19, 0.90, 0.37});
        }
    }
    for(unsigned int i = 0; i<2; i++)
        end_indices.insert(end_indices.end(), {i, i+1, i+2});

    bot_kill = std::make_pair(rand() % MAZE_WIDTH, rand() % MAZE_HEIGHT);

    while(bot_kill.ff == end.ff && bot_kill.ss == end.ss){
        bot_kill = std::make_pair(rand() % MAZE_WIDTH, rand() % MAZE_HEIGHT);
    }

    x = width * (bot_kill.ff - MAZE_WIDTH/2);
    y = height * (MAZE_HEIGHT/2 - bot_kill.ss);

    for(int i = -1; i<=1; i+=2){
        for(int j = -1; j<=1; j+=2){
            bot_kill_vertices.insert(bot_kill_vertices.end(), {x + j*width/3, y + i*height/3, 0});
            bot_kill_vertices.insert(bot_kill_vertices.end(), {0.90, 0.00, 0.30});
        }
    }
    for(unsigned int i = 0; i<2; i++)
        bot_kill_indices.insert(bot_kill_indices.end(), {i, i+1, i+2});

    powerup = std::make_pair(rand() % MAZE_WIDTH, rand() % MAZE_HEIGHT);

    while((powerup.ff == end.ff && powerup.ss == end.ss) || (powerup.ff == bot_kill.ff && powerup.ss == bot_kill.ss)){
        powerup = std::make_pair(rand() % MAZE_WIDTH, rand() % MAZE_HEIGHT);
    }

    x = width * (powerup.ff - MAZE_WIDTH/2);
    y = height * (MAZE_HEIGHT/2 - powerup.ss);

    for(int i = -1; i<=1; i+=2){
        for(int j = -1; j<=1; j+=2){
            powerup_vertices.insert(powerup_vertices.end(), {x + j*width/3, y + i*height/3, 0});
            powerup_vertices.insert(powerup_vertices.end(), {0.90, 0.90, 0.00});
        }
    }
    for(unsigned int i = 0; i<2; i++)
        powerup_indices.insert(powerup_indices.end(), {i, i+1, i+2});

    return EXT_SUCC;
}

int Maze::draw(unsigned int shaderProgram, GLFWwindow *window){
    unsigned int VBO, VAO, EBO;

    glUseProgram(shaderProgram);

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


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, end_vertices.size()*sizeof(GLfloat), &end_vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, end_indices.size()*sizeof(unsigned int), &end_indices[0], GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, end_indices.size(), GL_UNSIGNED_INT, 0);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, bot_kill_vertices.size()*sizeof(GLfloat), &bot_kill_vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bot_kill_indices.size()*sizeof(unsigned int), &bot_kill_indices[0], GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, bot_kill_indices.size(), GL_UNSIGNED_INT, 0);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, powerup_vertices.size()*sizeof(GLfloat), &powerup_vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, powerup_indices.size()*sizeof(unsigned int), &powerup_indices[0], GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, powerup_indices.size(), GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, powerups_vertices.size()*sizeof(GLfloat), &powerups_vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, powerups_indices.size()*sizeof(unsigned int), &powerups_indices[0], GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, powerups_indices.size(), GL_UNSIGNED_INT, 0);
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
}

std::pair<std::pair<int, int>, std::pair<int, int>> Maze::get_bounds(std::vector<float> vertices, glm::vec3 pos){
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
    
    std::pair<int, int> x, y;

    x.ff = (pos.x + ends_x.ff - zero.ff)/width;
    x.ss = (pos.x + ends_x.ss - zero.ff)/width;

    y.ff = (zero.ss - pos.y - ends_y.ff)/height;
    y.ss = (zero.ss - pos.y - ends_y.ss)/height;

    std::pair<std::pair<int, int>, std::pair<int, int>> ret = std::make_pair(x, y);

    return ret;
}

int Maze::lights_off(){
    lights = false;
    return EXT_SUCC;
}

int Maze::lights_on(){
    lights = true;
    return EXT_SUCC;
}

int Maze::update_lights(std::vector<float> vertices, glm::vec3 pos){

    if(lights == true){
        for(int i = 0; i<wall_vertices.size(); i+=6){
            wall_vertices[i+3] = 0.0f;
            wall_vertices[i+4] = 1.0f;
            wall_vertices[i+5] = 1.0f;
        }
        for(int i = 0; i<end_vertices.size(); i+=6){
            end_vertices[i+3] = 0.19;
            end_vertices[i+4] = 0.90;
            end_vertices[i+5] = 0.37;
        }   

        for(int i = 0; i<bot_kill_vertices.size(); i+=6){
            bot_kill_vertices[i+3] = 0.90;
            bot_kill_vertices[i+4] = 0.00;
            bot_kill_vertices[i+5] = 0.30;
        }  

        for(int i = 0; i<bot_kill_vertices.size(); i+=6){
            powerup_vertices[i+3] = 0.90;
            powerup_vertices[i+4] = 0.90;
            powerup_vertices[i+5] = 0.00;
        }  

        for(int i = 0; i<powerups_vertices.size(); i+=6){
            if(powerup_pos[i/24].ss == 0){
                powerups_vertices[i+3] = 0.00;
                powerups_vertices[i+4] = 0.50;
                powerups_vertices[i+5] = 0.50;
            }
            if(powerup_pos[i/24].ss == 1){
                powerups_vertices[i+3] = 1.00;
                powerups_vertices[i+4] = 0.20;
                powerups_vertices[i+5] = 0.20;
            }
        }

        return EXT_SUCC;
    }

    std::pair<std::pair<int, int>, std::pair<int, int>> bounds = get_bounds(vertices, pos);

    std::vector<std::vector<int>> dist(rows, std::vector<int>(columns, -1));

    std::queue<std::pair<int, int>> q;

    // std::cout << bounds.ff.ff << " " << bounds.ff.ss << " " << bounds.ss.ff << " " << bounds.ss.ss << "\n";

    if(bounds.ff.ff != bounds.ff.ss && bounds.ss.ff != bounds.ss.ss){
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ff));
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ss));
        q.push(std::make_pair(bounds.ff.ss, bounds.ss.ff));
        q.push(std::make_pair(bounds.ff.ss, bounds.ss.ss));
        dist[bounds.ss.ff][bounds.ff.ff] = 0;
        dist[bounds.ss.ss][bounds.ff.ff] = 0;
        dist[bounds.ss.ff][bounds.ff.ss] = 0;
        dist[bounds.ss.ss][bounds.ff.ss] = 0;
    }
    else if(bounds.ff.ff != bounds.ff.ss){
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ff));
        q.push(std::make_pair(bounds.ff.ss, bounds.ss.ff));
        dist[bounds.ss.ff][bounds.ff.ff] = 0;
        dist[bounds.ss.ff][bounds.ff.ss] = 0;
    }
    else if(bounds.ss.ff != bounds.ss.ss){
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ff));
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ss));
        dist[bounds.ss.ff][bounds.ff.ff] = 0;
        dist[bounds.ss.ss][bounds.ff.ff] = 0;
    }
    else{
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ff));
        dist[bounds.ss.ff][bounds.ff.ff] = 0;
    }

    // BFS
    while(!q.empty()){
        std::pair<int, int> cur = q.front();
        q.pop();
        if(maze[cur.ss][cur.ff].north == PATH && dist[cur.ss-1][cur.ff] == -1){
            dist[cur.ss-1][cur.ff] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff, cur.ss-1));
        }

        if(maze[cur.ss][cur.ff].south == PATH && dist[cur.ss+1][cur.ff] == -1){
            dist[cur.ss+1][cur.ff] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff, cur.ss+1));
        }

        if(maze[cur.ss][cur.ff].east == PATH && dist[cur.ss][cur.ff+1] == -1){
            dist[cur.ss][cur.ff+1] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff+1, cur.ss));
        }

        if(maze[cur.ss][cur.ff].west == PATH && dist[cur.ss][cur.ff-1] == -1){
            dist[cur.ss][cur.ff-1] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff-1, cur.ss));
        }
    }

    int vert;
    float scale;

    for(int i = 0; i<wall_vertices.size(); i+=6){
        vert = i/6;
        
        scale = 1000;

        if(vert%(MAZE_WIDTH+1)-1 >= 0 && vert/(MAZE_WIDTH+1)-1 >= 0)
            scale = min(scale, dist[vert/(MAZE_WIDTH+1)-1][vert%(MAZE_WIDTH+1)-1]);
        
        if(vert%(MAZE_WIDTH+1)-1 >= 0 && vert/(MAZE_WIDTH+1) < MAZE_HEIGHT)
            scale = min(scale, dist[vert/(MAZE_WIDTH+1)][vert%(MAZE_WIDTH+1)-1]);

        if(vert%(MAZE_WIDTH+1) < MAZE_WIDTH && vert/(MAZE_WIDTH+1)-1 >= 0)
            scale = min(scale, dist[vert/(MAZE_WIDTH+1)-1][vert%(MAZE_WIDTH+1)]);
        
        if(vert%(MAZE_WIDTH+1) < MAZE_WIDTH && vert/(MAZE_WIDTH+1) < MAZE_HEIGHT)
            scale = min(scale, dist[vert/(MAZE_WIDTH+1)][vert%(MAZE_WIDTH+1)]);
        
        wall_vertices[i+4] = max(0.0, 1.0 - GRADIENT*scale);
        wall_vertices[i+5] = max(0.0, 1.0 - GRADIENT*scale);
    }

    for(int i = 0; i<end_vertices.size(); i+=6){
        end_vertices[i+3] = max(0.0, 0.19*(1 - dist[end.ss][end.ff]*GRADIENT));
        end_vertices[i+4] = max(0.0, 0.90*(1 - dist[end.ss][end.ff]*GRADIENT));
        end_vertices[i+5] = max(0.0, 0.37*(1 - dist[end.ss][end.ff]*GRADIENT));
    }

    for(int i = 0; i<bot_kill_vertices.size(); i+=6){
        bot_kill_vertices[i+3] = max(0.0, 0.90*(1 - dist[bot_kill.ss][bot_kill.ff]*GRADIENT));
        bot_kill_vertices[i+4] = max(0.0, 0.00*(1 - dist[bot_kill.ss][bot_kill.ff]*GRADIENT));
        bot_kill_vertices[i+5] = max(0.0, 0.30*(1 - dist[bot_kill.ss][bot_kill.ff]*GRADIENT));
    }

    for(int i = 0; i<powerup_vertices.size(); i+=6){
        powerup_vertices[i+3] = max(0.0, 0.90*(1 - dist[powerup.ss][powerup.ff]*GRADIENT));
        powerup_vertices[i+4] = max(0.0, 0.90*(1 - dist[powerup.ss][powerup.ff]*GRADIENT));
        powerup_vertices[i+5] = max(0.0, 0.00*(1 - dist[powerup.ss][powerup.ff]*GRADIENT));
    }

    for(int i = 0; i<powerups_vertices.size(); i+=6){
        if(powerup_pos[i/24].ss == 0){
            powerups_vertices[i+3] = max(0.0f, 0.00*(1-dist[powerup_pos[i/24].ff.ss][powerup_pos[i/24].ff.ff]*GRADIENT));
            powerups_vertices[i+4] = max(0.0f, 0.50*(1-dist[powerup_pos[i/24].ff.ss][powerup_pos[i/24].ff.ff]*GRADIENT));
            powerups_vertices[i+5] = max(0.0f, 0.50*(1-dist[powerup_pos[i/24].ff.ss][powerup_pos[i/24].ff.ff]*GRADIENT));
        }
        if(powerup_pos[i/24].ss == 1){
            powerups_vertices[i+3] = max(0.0f, 1.00*(1-dist[powerup_pos[i/24].ff.ss][powerup_pos[i/24].ff.ff]*GRADIENT));
            powerups_vertices[i+4] = max(0.0f, 0.20*(1-dist[powerup_pos[i/24].ff.ss][powerup_pos[i/24].ff.ff]*GRADIENT));
            powerups_vertices[i+5] = max(0.0f, 0.20*(1-dist[powerup_pos[i/24].ff.ss][powerup_pos[i/24].ff.ff]*GRADIENT));
        }
    }

    return EXT_SUCC;
}

int Maze::shortest_path(std::vector<float> src_vertices, glm::vec3 src_pos, std::vector<float> dest_vertices, glm::vec3 dest_pos){
    std::pair<std::pair<int, int>, std::pair<int, int>> bounds = get_bounds(dest_vertices, dest_pos);

    std::vector<std::vector<int>> dist(rows, std::vector<int>(columns, -1));

    std::queue<std::pair<int, int>> q;

    // std::cout << bounds.ff.ff << " " << bounds.ff.ss << " " << bounds.ss.ff << " " << bounds.ss.ss << "\n";

    if(bounds.ff.ff != bounds.ff.ss && bounds.ss.ff != bounds.ss.ss){
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ff));
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ss));
        q.push(std::make_pair(bounds.ff.ss, bounds.ss.ff));
        q.push(std::make_pair(bounds.ff.ss, bounds.ss.ss));
        dist[bounds.ss.ff][bounds.ff.ff] = 0;
        dist[bounds.ss.ss][bounds.ff.ff] = 0;
        dist[bounds.ss.ff][bounds.ff.ss] = 0;
        dist[bounds.ss.ss][bounds.ff.ss] = 0;
    }
    else if(bounds.ff.ff != bounds.ff.ss){
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ff));
        q.push(std::make_pair(bounds.ff.ss, bounds.ss.ff));
        dist[bounds.ss.ff][bounds.ff.ff] = 0;
        dist[bounds.ss.ff][bounds.ff.ss] = 0;
    }
    else if(bounds.ss.ff != bounds.ss.ss){
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ff));
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ss));
        dist[bounds.ss.ff][bounds.ff.ff] = 0;
        dist[bounds.ss.ss][bounds.ff.ff] = 0;
    }
    else{
        q.push(std::make_pair(bounds.ff.ff, bounds.ss.ff));
        dist[bounds.ss.ff][bounds.ff.ff] = 0;
    }

    // BFS
    while(!q.empty()){
        std::pair<int, int> cur = q.front();
        q.pop();
        if(maze[cur.ss][cur.ff].north == PATH && dist[cur.ss-1][cur.ff] == -1){
            dist[cur.ss-1][cur.ff] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff, cur.ss-1));
        }

        if(maze[cur.ss][cur.ff].south == PATH && dist[cur.ss+1][cur.ff] == -1){
            dist[cur.ss+1][cur.ff] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff, cur.ss+1));
        }

        if(maze[cur.ss][cur.ff].east == PATH && dist[cur.ss][cur.ff+1] == -1){
            dist[cur.ss][cur.ff+1] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff+1, cur.ss));
        }

        if(maze[cur.ss][cur.ff].west == PATH && dist[cur.ss][cur.ff-1] == -1){
            dist[cur.ss][cur.ff-1] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff-1, cur.ss));
        }
    }

    std::pair<std::pair<int, int>, std::pair<int, int>> src_bounds = get_bounds(src_vertices, src_pos);

    
    int dir = 0;
    int m = INF;

    if(src_bounds.ff.ff != src_bounds.ff.ss && src_bounds.ss.ff != src_bounds.ss.ss){
        if(dist[src_bounds.ss.ss][src_bounds.ff.ff] < dist[src_bounds.ss.ff][src_bounds.ff.ff]){
            dir = NORTH;
        }
        if(dist[src_bounds.ss.ss][src_bounds.ff.ss] < dist[src_bounds.ss.ff][src_bounds.ff.ff]){
            dir = NORTH;
        }
        else{
            dir = SOUTH;
        }
    }
    else if(src_bounds.ff.ff != src_bounds.ff.ss){
        if(dist[src_bounds.ss.ff][src_bounds.ff.ff] < dist[src_bounds.ss.ff][src_bounds.ff.ss]){
            dir = WEST;
        }
        else{
            dir = EAST;
        }
    }
    else if(src_bounds.ss.ff != src_bounds.ss.ss){
        if(dist[src_bounds.ss.ff][src_bounds.ff.ff] < dist[src_bounds.ss.ss][src_bounds.ff.ff]){
            dir = SOUTH;
        }
        else{
            dir = NORTH;
        }
    }
    else{
        if(maze[src_bounds.ss.ff][src_bounds.ff.ff].north == PATH){
            if(dist[src_bounds.ss.ff-1][src_bounds.ff.ff] < m){
                dir = NORTH;
                m = dist[src_bounds.ss.ff-1][src_bounds.ff.ff];
            }
        }
        if(maze[src_bounds.ss.ff][src_bounds.ff.ff].south == PATH){
            if(dist[src_bounds.ss.ff+1][src_bounds.ff.ff] < m){
                dir = SOUTH;
                m = dist[src_bounds.ss.ff+1][src_bounds.ff.ff];
            }
        }
        if(maze[src_bounds.ss.ff][src_bounds.ff.ff].east == PATH){
            if(dist[src_bounds.ss.ff][src_bounds.ff.ff+1] < m){
                dir = EAST;
                m = dist[src_bounds.ss.ff][src_bounds.ff.ff+1];
            }
        }
        if(maze[src_bounds.ss.ff][src_bounds.ff.ff].west == PATH){
            if(dist[src_bounds.ss.ff][src_bounds.ff.ff-1] < m){
                dir = WEST;
                m = dist[src_bounds.ss.ff][src_bounds.ff.ff-1];
            }
        }
    }

    return dir;
}

void Maze::activate_powerups(){
    powerup_activated = true;
    float height = (float)CELL_WIDTH/SCR_HEIGHT;
    float width = (float)CELL_WIDTH/SCR_WIDTH;
    std::pair<float, float> zero = {-width*(columns/2), height*(rows/2)};

    for(int i = 0; i<NUM_POWERUP; i++){
        powerup_pos.push_back(std::make_pair(std::make_pair(rand() % MAZE_WIDTH, rand() % MAZE_HEIGHT), rand() % 2));
        auto cur = powerup_pos.back();
        for(int j = -1; j<=1; j+=2){
            for(int k = -1; k<=1; k+=2){
                powerups_vertices.insert(powerups_vertices.end(), {zero.ff+cur.ff.ff*width+j*(width/4), zero.ss-cur.ff.ss*height+k*(height/4), 0});
                if(cur.ss == 0){
                    powerups_vertices.insert(powerups_vertices.end(), {0.0f, 0.5f, 0.5f});
                }
                else{
                    powerups_vertices.insert(powerups_vertices.end(), {1.0f, 0.2f, 0.2f});
                }
            }
        }
        for(unsigned int j = 0; j<=1; j++){
            powerups_indices.insert(powerups_indices.end(), {(unsigned int)4*i+j, (unsigned int)4*i+1+j, (unsigned int)4*i+2+j});
        }
    }
}

#endif