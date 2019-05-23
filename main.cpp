#include <SFML/Graphics.hpp>
#include <sfml/audio.hpp>
#include <vector>
#include <list>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <cstring>
#include <random>
#include <chrono>

#include "various.h"
#include "Animation.h"
#include "entity.h"
#include "game.h"

int main()
{
    srand(time(0));

    //the game class
    Game game;

    //create the main window
    game.init(640,180,400,200,"SpaceShooter");

    ReadHiScores(game.vhiscores);

    sf::Clock clock;
    const sf::Time timePerFrame = sf::seconds(1.f/60.f); //60fps
    sf::Time elapsed = sf::Time::Zero;

    while(game.app.isOpen())
    {
        elapsed += clock.restart();

        game.input();

        while( elapsed >= timePerFrame )
        {
            game.update(elapsed);
            elapsed -= timePerFrame;
        }

        game.draw();
    }

    WriteHiScores(game.vhiscores);

    return EXIT_SUCCESS;
}
