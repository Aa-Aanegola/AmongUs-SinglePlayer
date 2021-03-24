#include "defs.hpp"
#include "world.hpp"
#include "player.hpp"
#include "graphics_setup.hpp"

using namespace std;

int main(){
    window = setup_graphics(shaderProgram, window);
    if(window == NULL){
        cout << "sid is bond sir";
        return 0;
    }

    Maze world(MAZE_HEIGHT, MAZE_WIDTH);
    Player player;

    world.init();
    player.init(0.0, 0.0);

    glUseProgram(shaderProgram);

    while(!glfwWindowShouldClose(window)){

        processInput(window, world, player);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        world.draw(shaderProgram, window);
        player.draw(shaderProgram, window);
        world.update_lights(player.vertices, player.position);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXT_SUCC;
}