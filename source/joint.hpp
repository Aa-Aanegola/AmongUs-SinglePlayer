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