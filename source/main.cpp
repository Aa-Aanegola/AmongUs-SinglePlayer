#include "defs.hpp"
#include "world.hpp"
#include "player.hpp"
#include "graphics_setup.hpp"
#include "joint.hpp"

using namespace std;

int main(){
    window = setup_graphics(shaderProgram, window);
    if(window == NULL){
        cout << "sid is bond sir";
        return 0;
    }

    float width = (float)CELL_WIDTH/SCR_WIDTH;
    float height = (float)CELL_WIDTH/SCR_HEIGHT;
    
    std::pair<float, float> zero = {-width*((float)MAZE_WIDTH/2-0.5), height*((float)MAZE_HEIGHT/2-0.5)};

    Maze world(MAZE_HEIGHT, MAZE_WIDTH);
    Player player;
    Player bot;

    world.init();
    player.init(0.0, 0.0);
    bot.init(zero.ff, zero.ss);

    glUseProgram(shaderProgram);

    while(!glfwWindowShouldClose(window)){

        processInput(window, world, player, bot);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        world.draw(shaderProgram, window);
        player.draw(shaderProgram, window);
        bot.draw(shaderProgram, window);
        world.update_lights(player.vertices, player.position);
        update_bot_visibility(player, bot, world);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXT_SUCC;
}