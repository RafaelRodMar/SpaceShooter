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

//dimensiones de la pantalla
const int screenwidth = 640;
const int screenheight = 180;

enum game_states {MENU, GAME, END_GAME};
int state = -1;
std::vector<int> vhiscores;
int lives, score;

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

class Animation
{
public:
	float Frame, speed;
	sf::Sprite sprite;
    std::vector<sf::IntRect> frames;

	Animation(){}

    Animation (sf::Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
	    Frame = 0;
        speed = Speed;

		for (int i=0;i<count;i++)
         frames.push_back( sf::IntRect(x+i*w, y, w, h)  );

		sprite.setTexture(t);
		sprite.setOrigin(w/2,h/2);
        sprite.setTextureRect(frames[0]);
	}


	void update()
	{
    	Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n>0) sprite.setTextureRect( frames[int(Frame)] );
	}

	bool isEnd()
	{
	  return Frame+speed>=frames.size();
	}
};

class Entity
{
    public:
        float x,y,dx,dy,R;
        bool life;
        std::string name;
        Animation anim;

    Entity()
    {
        life=1;
    }

    void settings(Animation &a,int X,int Y,int radius=1)
    {
        anim = a;
        x=X; y=Y;
        R = radius;
    }

    virtual void update(){};

    void draw(sf::RenderWindow &app)
    {
      anim.sprite.setPosition(x,y);
      app.draw(anim.sprite);
    }

    virtual ~Entity(){};
};

class asteroid: public Entity
{
public:
    asteroid()
    {
        dx=-4;
        dy=0;
        name="asteroid";
    }

    void  update()
    {
        x+=dx;

        if (x<0) life=0;
    }

};

class bullet: public Entity
{
public:
    bullet()
    {
        name="bullet";
    }

void  update()
    {
        dx=6;

        x+=dx;

        if (x>screenwidth || x<0 || y>screenheight || y<0) life=0;
    }

};


class player: public Entity
{
public:

   player()
   {
     name="player";
   }

   void update()
   {
    x+=dx;
    y+=dy;

    if (x>screenwidth) x=screenwidth;
    if (x<0) x=0;
    if (y>screenheight) y=screenheight;
    if (y<0) y=0;
   }

};

void UpdateHiScores(int newscore)
{
    //new score to the end
    vhiscores.push_back(newscore);
    //sort
    sort(vhiscores.rbegin(), vhiscores.rend());
    //remove the last
    vhiscores.pop_back();
}

void ReadHiScores()
{
    std::ifstream in("hiscores.dat");
    if(in.good())
    {
        std::string str;
        getline(in,str);
        std::stringstream ss(str);
        int n;
        for(int i=0;i<5;i++)
        {
            ss >> n;
            vhiscores.push_back(n);
        }
        in.close();
    }
    else
    {
        //if file does not exist fill with 5 scores
        for(int i=0;i<5;i++)
        {
            vhiscores.push_back(0);
        }
    }
}

void WriteHiScores()
{
    std::ofstream out("hiscores.dat");
    for(int i=0;i<5;i++)
    {
        out << vhiscores[i] << " ";
    }
    out.close();
}

void Text(sf::RenderWindow &app, std::string pstr, float px, float py,sf::Color pcolor, int psize, sf::Font pfont)
{
    sf::Text str;
    str.setString(pstr);
    str.setFont(pfont);
    str.setCharacterSize(psize);
    str.setPosition(px, py);
    str.setFillColor(pcolor);
    app.draw(str);
}

bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}

//keyboard handling
// The keyboard's state in the current- and the previous frame
bool CurrentKeyState[sf::Keyboard::KeyCount];
bool PreviousKeyState[sf::Keyboard::KeyCount];

bool KeyPressed(sf::Keyboard::Key Key)
{
    return (CurrentKeyState[Key] && !PreviousKeyState[Key]);
}

bool KeyReleased(sf::Keyboard::Key Key)
{
    return (!CurrentKeyState[Key] && PreviousKeyState[Key]);
}

bool KeyHeld(sf::Keyboard::Key Key)
{
    return CurrentKeyState[Key];
}


int main()
{
    srand(time(0));

    // Create the main window
    sf::RenderWindow app(sf::VideoMode(640, 180), "SpaceShooter");
    //app.setFramerateLimit(60); //ya no hace falta.
    app.setPosition(sf::Vector2i(400,200));

    if( !gamemusic.openFromFile("sounds/DST-TowerDefenseTheme.ogg"))
        return -1;
    gamemusic.setLoop(true);

    //load textures
    sf::Texture t1,t2,t3,t4,t5;
    t1.loadFromFile("images/asteroid.png");
    t2.loadFromFile("images/background.png");
    t3.loadFromFile("images/explosion.png");
    t4.loadFromFile("images/player.png");
    t5.loadFromFile("images/shot.png");

    //assign textures to sprites
    t2.setRepeated(true);
    int bgx = 0;
    sf::IntRect bgrect(bgx,0,640,180);
    sf::Sprite background(t2,bgrect);

    //crear animaciones
    Animation sPlayer(t4,0,0,16,16,2,0.2);
    Animation sAster(t1,0,0,16,16,2,0.1);
    Animation sShot(t5,0,0,16,16,2,0.2);
    Animation sExpl(t3,0,0,32,32,1,0.05);

    // Load the text font
    font.loadFromFile("hour5.ttf");

    // Load sounds
    ExpBuffer2.loadFromFile("sounds/Explosion+6.wav");
    Explosion2.setBuffer(ExpBuffer2);
    ExpBuffer.loadFromFile("sounds/Explosion+3.wav");
    Explosion.setBuffer(ExpBuffer);
    LaserBuffer.loadFromFile("sounds/LaserBlasts.wav");
    Laser.setBuffer(LaserBuffer);

    //keyboard buffers initialization
    memset(CurrentKeyState,     false, sizeof(CurrentKeyState));
    memset(PreviousKeyState,    false, sizeof(PreviousKeyState));

    std::list<Entity*> entities;

    player *p = new player();
    p->settings(sPlayer,20,85,8);
    entities.push_back(p);

    ReadHiScores();

    sf::Clock clock;
    const float timePerFrame = 1.0 / 60.0;  //60fps.

    state = MENU;

	// Start the game loop
    while (app.isOpen())
    {
        sf::Time elapsed = clock.getElapsedTime();

        //HandleKeys();
        if(state==MENU)
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
        }

        if(state==GAME && elapsed.asSeconds() > timePerFrame)
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

            if (KeyHeld(sf::Keyboard::Right)) p->x += 3;
            if (KeyHeld(sf::Keyboard::Left))  p->x -= 3;
            if (KeyHeld(sf::Keyboard::Up)) p->y -= 3;
            if (KeyHeld(sf::Keyboard::Down)) p-> y += 3;
        }

        if(state==END_GAME)
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
        }

        //Game_cycle();
        if(state==GAME && elapsed.asSeconds() > timePerFrame)
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
                        UpdateHiScores(score);
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

            if (rand()%100==0)
             {
               asteroid *a = new asteroid();
               a->settings(sAster, 640 ,rand()%screenheight, 8);
               entities.push_back(a);
             }

            for(auto i=entities.begin();i!=entities.end();)
            {
              Entity *e = *i;

              e->update();
              e->anim.update();

              if (e->life==false) {i=entities.erase(i); delete e;}
              else i++;
            }

            //update background
            if( elapsed.asSeconds() > timePerFrame )
            {
                bgx++;
                bgrect.left = bgx;
                background.setTextureRect(bgrect);
                if(bgx >= screenwidth) bgx = 0;
            }

            clock.restart();
        }

        //Game_paint();
        //////draw//////
        app.draw(background);
        if(state==MENU)
        {
            app.draw(background);

            //Show hi scores
            std::string histr="SPACE SHOOTER\n  HI-SCORES\n";
            for(int i=0;i<5;i++)
            {
                histr = histr + "    " + std::to_string(vhiscores[i]) + "\n";
            }
            histr += "PRESS S TO START";
            Text(app,histr,280.f,20.f,sf::Color::Cyan,24,font);
        }

        if(state==GAME)
        {
            for(auto i:entities)
                i->draw(app);

            // Draw the score
            std::string sc = "LIVES: " + std::to_string(lives) + "   SCORE: " + std::to_string(score);
            Text(app,sc,450.f,0.f,sf::Color::Cyan,24,font);
        }

        if(state==END_GAME)
        {
            app.draw(background);

            Text(app,"GAME OVER",220.f,30.f,sf::Color::Cyan,50,font);
            Text(app,"PRESS M", 250.f, 100.f, sf::Color::Cyan,25,font);
        }

        app.display();
    }

    WriteHiScores();

    return EXIT_SUCCESS;
}
