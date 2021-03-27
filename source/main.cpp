#include "defs.hpp"
#include "world.hpp"
#include "player.hpp"
#include "graphics_setup.hpp"
#include "joint.hpp"

using namespace std;

int main(){
    srand(0);
    window = setup_graphics(shaderProgram, window);

    if(window == NULL){
        cout << "sid is bond sir";
        return 0;
    }

    int x = rand() % MAZE_WIDTH;
    int y = rand() % MAZE_HEIGHT;
    while(x == MAZE_WIDTH/2){
        x = rand() % MAZE_WIDTH;
    }

    std::pair<float, float> pos = {-width*((float)x-(float)MAZE_WIDTH/2-0.5), height*((float)y - (float)MAZE_HEIGHT/2-0.5)};

    
    Maze world(MAZE_HEIGHT, MAZE_WIDTH);
    Player player;
    Player bot;

    int prev_time = (int)glfwGetTime();

    world.init();
    player.init(0.0, 0.0);
    bot.init(pos.ff, pos.ss);


    gltInit();
    GLTtext *text1 = gltCreateText();
	gltSetText(text1, "Hello World!");

    bool end_game = false;

    while(!glfwWindowShouldClose(window)){
        //glUseProgram(shaderProgram);
        processInput(window, world, player, bot);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(end_game){
            game_over_message(player);
        }
        else{
            world.draw(shaderProgram, window);
            world.update_lights(player.vertices, player.position);
            update_bot_visibility(player, bot, world);
            check_powerups(player, world);
            lights_off_score(player, world, prev_time);
            player.draw(shaderProgram, window);
            bot.draw(shaderProgram, window);
            
            if(!bot.dead && remove_bot(player, world)){
                bot.kill();
            }
            if(activate_powerup(player, bot, world)){
                world.activate_powerups();
            }

            if(bot_killed_player(player, bot, world))
                end_game = true;
            
            if(game_over(player, world))
                end_game = true;

            render_hud(player, world);
        }
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXT_SUCC;
}