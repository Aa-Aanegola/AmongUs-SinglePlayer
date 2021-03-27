#include "defs.hpp"
#include "world.hpp"
#include "player.hpp"

int update_bot_visibility(Player &player, Player &bot, Maze& world){
    if(world.lights){
        for(int i = 0; i<24; i+=6){
            bot.vertices[i+3] = 0.86f;
            bot.vertices[i+4] = 0.08f;
            bot.vertices[i+5] = 0.24f;
        }
        for(int i = 24; i<48; i+=6){
            bot.vertices[i+3] = 0.9000f;
            bot.vertices[i+4] =  0.9100f;
            bot.vertices[i+5] =  0.9800f;
        }
        for(int i = 48; i<bot.vertices.size(); i+=6){
            bot.vertices[i+3] = 0.86f;
            bot.vertices[i+4] = 0.08f;
            bot.vertices[i+5] = 0.24f;
        }
        return EXT_SUCC;
    }


    std::pair<std::pair<int, int>, std::pair<int, int>> bounds = world.get_bounds(player.vertices, player.position);

    std::vector<std::vector<int>> dist(world.rows, std::vector<int>(world.columns, -1));

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
        if(world.maze[cur.ss][cur.ff].north == PATH && dist[cur.ss-1][cur.ff] == -1){
            dist[cur.ss-1][cur.ff] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff, cur.ss-1));
        }

        if(world.maze[cur.ss][cur.ff].south == PATH && dist[cur.ss+1][cur.ff] == -1){
            dist[cur.ss+1][cur.ff] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff, cur.ss+1));
        }

        if(world.maze[cur.ss][cur.ff].east == PATH && dist[cur.ss][cur.ff+1] == -1){
            dist[cur.ss][cur.ff+1] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff+1, cur.ss));
        }

        if(world.maze[cur.ss][cur.ff].west == PATH && dist[cur.ss][cur.ff-1] == -1){
            dist[cur.ss][cur.ff-1] = dist[cur.ss][cur.ff] + 1;
            q.push(std::make_pair(cur.ff-1, cur.ss));
        }
    }

    std::pair<std::pair<int, int>, std::pair<int, int>> bot_bounds = world.get_bounds(bot.vertices, bot.position);

    int scale = 1000;
    scale = min(scale, dist[bot_bounds.ss.ff][bot_bounds.ff.ff]);
    scale = min(scale, dist[bot_bounds.ss.ff][bot_bounds.ff.ss]);
    scale = min(scale, dist[bot_bounds.ss.ss][bot_bounds.ff.ff]);
    scale = min(scale, dist[bot_bounds.ss.ss][bot_bounds.ff.ss]);
    
    for(int i = 0; i<24; i+=6){
            bot.vertices[i+3] = max(0.0, 0.86*(1.0 - GRADIENT*scale));
            bot.vertices[i+4] = max(0.0, 0.08*(1.0 - GRADIENT*scale));
            bot.vertices[i+5] = max(0.0, 0.24*(1.0 - GRADIENT*scale));
        }
    for(int i = 24; i<48; i+=6){
            bot.vertices[i+3] = max(0.0, 0.90*(1.0 - GRADIENT*scale));
            bot.vertices[i+4] = max(0.0, 0.91*(1.0 - GRADIENT*scale));
            bot.vertices[i+5] = max(0.0, 0.98*(1.0 - GRADIENT*scale));
        }
    for(int i = 48; i<bot.vertices.size(); i+=6){
            bot.vertices[i+3] = max(0.0, 0.86*(1.0 - GRADIENT*scale));
            bot.vertices[i+4] = max(0.0, 0.08*(1.0 - GRADIENT*scale));
            bot.vertices[i+5] = max(0.0, 0.24*(1.0 - GRADIENT*scale));
        }
}

bool remove_bot(Player &player, Maze &world){
    std::pair<std::pair<int, int>, std::pair<int, int>> bounds = world.get_bounds(player.vertices, player.position);
    if(bounds.ff.ff == world.bot_kill.ff && bounds.ff.ss == world.bot_kill.ff)
        if(bounds.ss.ff == world.bot_kill.ss && bounds.ss.ss == world.bot_kill.ss){
            player.score += 100;
            world.tasks -= 1;
            return true;
        }

    return false;
}

bool bot_killed_player(Player &player, Player &bot, Maze &world){
    if(bot.dead)
        return false;
    std::pair<std::pair<int, int>, std::pair<int, int>> pbounds = world.get_bounds(player.vertices, player.position);
    std::pair<std::pair<int, int>, std::pair<int, int>> bbounds = world.get_bounds(bot.vertices, bot.position);

    if(pbounds == bbounds)
        return true;
    return false;
}

bool activate_powerup(Player &player, Player &bot, Maze &world){
    if(world.powerup_activated == true)
        return false;
    std::pair<std::pair<int, int>, std::pair<int, int>> bounds = world.get_bounds(player.vertices, player.position);
    if(bounds.ff.ff == world.powerup.ff && bounds.ff.ss == world.powerup.ff)
        if(bounds.ss.ff == world.powerup.ss && bounds.ss.ss == world.powerup.ss){
            player.score += 100;
            world.tasks -= 1;
            return true;
        }
    
    return false;
}

bool game_over(Player &player, Maze &world){
    if(player.time -(int)glfwGetTime() <= 0)
        return true;
    if(world.tasks != 0)
        return false;
    
    std::pair<std::pair<int, int>, std::pair<int, int>> bounds = world.get_bounds(player.vertices, player.position);
    if(bounds.ff.ff == world.end.ff && bounds.ff.ss == world.end.ff)
        if(bounds.ss.ff == world.end.ss && bounds.ss.ss == world.end.ss){
            player.score += 100;
            world.tasks -= 1;
            return true;
        }
    
    return false;
}

void check_powerups(Player &player, Maze &world){
    std::pair<std::pair<int, int>, std::pair<int, int>> bounds = world.get_bounds(player.vertices, player.position);

    std::vector<int> to_remove;
    to_remove.clear();

    
    for(int i = 0; i<world.powerup_pos.size(); i++){
        if(to_remove.size())
            continue;
        if(world.powerup_pos[i].ff.ff == bounds.ff.ff && world.powerup_pos[i].ff.ff == bounds.ff.ss)
            if(world.powerup_pos[i].ff.ss == bounds.ss.ff && world.powerup_pos[i].ff.ss == bounds.ss.ss){
                if(world.powerup_pos[i].ss == 0)
                    player.score += 10;
                else
                    player.score -= 10;
                to_remove.push_back(i);
            }
    }

    if(to_remove.size()){
        int pos = to_remove[0];
        world.powerup_pos[pos] = std::make_pair(std::make_pair(-1, -1), -1);
        for(int i = 24*pos; i<24*(pos+1); i++)
            world.powerups_vertices[i] = 0;
    }
}

void lights_off_score(Player &player, Maze &world, int &prev_time){
    if(player.time - (int)glfwGetTime() != player.time - prev_time){
        if(world.lights == false)
            player.score += 2;
        prev_time = (int)glfwGetTime();
    }
}

void render_hud(Player &player, Maze &world){
    GLTtext *text1 = gltCreateText();
	gltSetText(text1, "Hello World!");
    char str[50];

    gltBeginDraw();

    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    sprintf(str, "Score: %d", player.score);
    gltSetText(text1, str);
    gltDrawText2D(text1, 10.0f, 10.0f, 2.0f);

    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    if(world.lights)
        sprintf(str, "Lights: On");
    else
        sprintf(str, "Lights: Off");
    gltSetText(text1, str);
    gltDrawText2D(text1, 10.0f, 50.0f, 2.0f);

    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    sprintf(str, "Tasks: %d/2", 2-world.tasks);
    gltSetText(text1, str);
    gltDrawText2D(text1, 10.0f, 90.0f, 2.0f);

    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    sprintf(str, "Time: %d", player.time - (int)glfwGetTime());
    gltSetText(text1, str);
    gltDrawText2D(text1, 10.0f, 130.0f, 2.0f);

    gltEndDraw();

}


void game_over_message(Player &player){
    GLTtext *text1 = gltCreateText();
	gltSetText(text1, "Hello World!");
    char str[50];

    gltBeginDraw();

    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    sprintf(str, "Game over!");
    gltSetText(text1, str);
    gltDrawText2DAligned(text1,
			(GLfloat)(SCR_WIDTH / 2),
			(GLfloat)(SCR_HEIGHT / 2.1),
			5.0f,
			GLT_CENTER, GLT_CENTER);
    sprintf(str, "Score: %d", player.score);
    gltSetText(text1, str);
    gltDrawText2DAligned(text1,
			(GLfloat)(SCR_WIDTH / 2),
			(GLfloat)(SCR_HEIGHT / 1.9),
			3.0f,
			GLT_CENTER, GLT_CENTER);
    gltEndDraw();   
}