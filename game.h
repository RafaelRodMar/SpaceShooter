class Game{
public:
    int screenwidth = -1;
    int screenheight = -1;
    sf::RenderWindow app;

    enum game_states {MENU, GAME, END_GAME};
    int state = MENU;
    std::vector<int> vhiscores;
    int lives = -1, score = -1;

    //keyboard handling
    // The keyboard's state in the current- and the previous frame
    bool CurrentKeyState[sf::Keyboard::KeyCount];
    bool PreviousKeyState[sf::Keyboard::KeyCount];

    //music
    sf::Music gamemusic;

    //sound buffers
    sf::SoundBuffer ExpBuffer;
    sf::SoundBuffer ExpBuffer2;
    sf::SoundBuffer LaserBuffer;

    //sound play
    sf::Sound Explosion;
    sf::Sound Explosion2;
    sf::Sound Laser;

    //fonts
    sf::Font font;
    sf::Text showScore;

    //textures
    sf::Texture t1,t2,t3,t4,t5;

    //animations
    Animation sPlayer;
    Animation sAster;
    Animation sShot;
    Animation sExpl;

    //background data
    int bgx = 0;
    sf::IntRect bgrect;
    sf::Sprite background;

    //player, asteroids and bullets list.
    std::list<Entity*> entities;

    //the player
    player *p;

    void init(int pscrw, int pscrh, int pposx, int pposy, std::string pname);
    void input();
    void update(sf::Time delta);
    void draw();

    //keyboard functions
    bool KeyPressed(sf::Keyboard::Key Key)
        { return (CurrentKeyState[Key] && !PreviousKeyState[Key]); }

    bool KeyReleased(sf::Keyboard::Key Key)
        { return (!CurrentKeyState[Key] && PreviousKeyState[Key]); }

    bool KeyHeld(sf::Keyboard::Key Key)
        { return CurrentKeyState[Key]; }
};

void Game::init(int pscrw, int pscrh, int pposx, int pposy, std::string pname)
{
    screenwidth = pscrw;
    screenheight = pscrh;
    app.create(sf::VideoMode(pscrw,pscrh),pname);
    app.setPosition(sf::Vector2i(pposx,pposy));

    //load music.
    gamemusic.openFromFile("sounds/DST-TowerDefenseTheme.ogg");
    gamemusic.setLoop(true);

    //load textures
    t1.loadFromFile("images/asteroid.png");
    t2.loadFromFile("images/background.png");
    t3.loadFromFile("images/explosion.png");
    t4.loadFromFile("images/player.png");
    t5.loadFromFile("images/shot.png");

    //prepare background
    t2.setRepeated(true);
    bgrect.left = bgx;
    bgrect.top = 0;
    bgrect.width = screenwidth;
    bgrect.height = screenheight;
    background.setTexture(t2,true);

    // Load the text font
    font.loadFromFile("hour5.ttf");

    // Load sounds
    ExpBuffer2.loadFromFile("sounds/Explosion+6.wav");
    Explosion2.setBuffer(ExpBuffer2);
    ExpBuffer.loadFromFile("sounds/Explosion+3.wav");
    Explosion.setBuffer(ExpBuffer);
    LaserBuffer.loadFromFile("sounds/LaserBlasts.wav");
    Laser.setBuffer(LaserBuffer);

    //create animations
    sPlayer.init(t4,0,0,16,16,2,0.2);
    sAster.init(t1,0,0,16,16,2,0.1);
    sShot.init(t5,0,0,16,16,2,0.2);
    sExpl.init(t3,0,0,32,32,1,0.05);

    //keyboard buffers initialization
    memset(CurrentKeyState,     false, sizeof(CurrentKeyState));
    memset(PreviousKeyState,    false, sizeof(PreviousKeyState));

    //player creation
    p = new player();
    p->settings(sPlayer,20,85,8);
    entities.push_back(p);
}

void Game::input()
{
    switch(state)
        {
        case MENU:
            {
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();

                // S key pressed: change state to GAME
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::S))
                {
                    state=GAME;
                    lives = 3;
                    score = 0;
                    gamemusic.play();
                }
            }
            break;
            }
        case GAME:
            {
            sf::Event event;
            while (app.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    app.close();
            }

            // Save the state of each keyboard key (must be done before any Key* function is executed)
            for(unsigned int i = 0; i < sf::Keyboard::KeyCount; ++i)
            {
                // Save the keyboard's state from the previous frame
                PreviousKeyState[i] = CurrentKeyState[i];

                // And save the keyboard's state in the current frame
                CurrentKeyState[i] = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)i);
            }

            if(KeyPressed(sf::Keyboard::Escape))
                app.close();

            // Space is the fire key
            if(KeyPressed(sf::Keyboard::Space))
            {
                bullet *b = new bullet();
                b->settings(sShot,p->x,p->y,10);
                entities.push_back(b);
                Laser.play();
            }

            if (KeyHeld(sf::Keyboard::Right)) p->dx = 300;
            if (KeyHeld(sf::Keyboard::Left))  p->dx = -300;
            if (KeyHeld(sf::Keyboard::Up)) p->dy = -300;
            if (KeyHeld(sf::Keyboard::Down)) p->dy = 300;
            break;
            }
        case END_GAME:
            {
            gamemusic.stop();
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();

                // Any key pressed: change state to MENU
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::M))
                {
                    state=MENU;
                }
            }
            break;
            }
        default:
            break;
        }
}

void Game::update(sf::Time delta)
{
    if( state==GAME )
    {
        //Game_cycle();
        if(state==GAME)
        {
         for(auto a:entities)
         {
            for(auto b:entities)
            {
              if (a->name=="asteroid" && b->name=="bullet")
               if ( isCollide(a,b) )
                {
                    a->life=false;
                    b->life=false;

                    //explosion
                    Entity *e = new Entity();
                    e->settings(sExpl,a->x,a->y);
                    e->name="explosion";
                    entities.push_back(e);
                    Explosion.play();
                    score += 10;
                }

              if (a->name=="player" && b->name=="asteroid")
               if ( isCollide(a,b) )
                {
                    b->life=false;

                    //ship explosion
                    Entity *e = new Entity();
                    e->settings(sExpl,a->x,a->y);
                    e->name="explosion";
                    entities.push_back(e);
                    if(lives==1)
                        Explosion2.play();
                    else
                        Explosion.play();
                    lives--;
                    if(lives<=0)
                    {
                        UpdateHiScores(vhiscores, score);
                        state=END_GAME;
                    }

                    //relocate the ship
                    p->settings(sPlayer,20,85,8);
                    p->dx=0;
                }
            }
         }

            for(auto e:entities)
             if (e->name=="explosion")
              if (e->anim.isEnd()) e->life=0;

            if ( rand()%100 == 0 )
             {
               asteroid *a = new asteroid();
               a->settings(sAster, 640 , rand()%screenheight, 8);
               entities.push_back(a);
             }

            for(auto i=entities.begin();i!=entities.end();)
            {
              Entity *e = *i;

              e->update(delta);
              e->anim.update();

              if (e->life==false) {i=entities.erase(i); delete e;}
              else i++;
            }

            //update background
            bgx++;
            bgrect.left = bgx;
            background.setTextureRect(bgrect);
            if(bgx >= screenwidth) bgx = 0;
        }
    }
}

void Game::draw()
{
    app.clear();
    app.draw(background);

    switch(state)
    {
    case MENU:
        {
        //Show hi scores
        std::string histr="SPACE SHOOTER\n  HI-SCORES\n";
        for(int i=0;i<5;i++)
        {
            histr = histr + "    " + std::to_string(vhiscores[i]) + "\n";
        }
        histr += "PRESS S TO START";
        Text(app,histr,280.f,20.f,sf::Color::Cyan,24,font);
        break;
        }
    case GAME:
        {
            for(auto i:entities)
                i->draw(app);

            // Draw the score
            std::string sc = "LIVES: " + std::to_string(lives) + "   SCORE: " + std::to_string(score);
            Text(app,sc,450.f,0.f,sf::Color::Cyan,24,font);
        }
        break;
    case END_GAME:
        Text(app,"GAME OVER",220.f,30.f,sf::Color::Cyan,50,font);
        Text(app,"PRESS M", 250.f, 100.f, sf::Color::Cyan,25,font);
        break;
    default:
        break;
    }
    app.display();
}
