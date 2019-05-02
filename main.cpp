#include <SFML/Graphics.hpp>
#include <SFML/Graphics.hpp>
#include <sfml/audio.hpp>
#include <vector>
#include <list>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

//dimensiones de la pantalla
const int screenwidth = 640;
const int screenheight = 180;

enum game_states {MENU, GAME, END_GAME};
int state = -1;
std::vector<int> vhiscores;
int lives, score;

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
        dx=rand()%8-4;
        dy=rand()%8-4;
        name="asteroid";
    }

    void  update()
    {
        x+=dx;
        y+=dy;

        if (x>screenwidth) x=0;
        if (x<0) x=screenwidth;
        if (y>screenheight) y=0;
        if (y<0) y=screenheight;
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
        dy=0;

        x+=dx;
        y+=dy;

        if (x>screenwidth || x<0 || y>screenheight || y<0) life=0;
    }

};


class player: public Entity
{
public:
   bool thrust;

   player()
   {
     name="player";
   }

   void update()
   {
     if (thrust)
      { dx+=0.2;
        dy+=0.2; }
     else
      { dx*=0.99;   //reduce thrust
        dy*=0.99; }

    int maxSpeed=15;
    float speed = sqrt(dx*dx+dy*dy);
    if (speed>maxSpeed)
     { dx *= maxSpeed/speed;
       dy *= maxSpeed/speed; }

    x+=dx;
    y+=dy;

    if (x>screenwidth) x=0;
    if (x<0) x=screenwidth;
    if (y>screenheight) y=0;
    if (y<0) y=screenheight;
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

bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}

int main()
{
    srand(time(0));

    // Create the main window
    sf::RenderWindow app(sf::VideoMode(640, 180), "SpaceShooter");
    app.setFramerateLimit(60);
    app.setPosition(sf::Vector2i(200,0));

    //load textures
    sf::Texture t1,t2,t3,t4,t5,t6,t7,t8,t9,t10;
    t1.loadFromFile("images/asteroid.png");
    t2.loadFromFile("images/background.png");
    t3.loadFromFile("images/explosion.png");
    t4.loadFromFile("images/player1.png");
    t5.loadFromFile("images/shot1.png");

    //assign textures to sprites
    sf::Sprite background(t3);

	// Start the game loop
    while (app.isOpen())
    {
        // Process events
        sf::Event event;
        while (app.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                app.close();
        }

        // Clear screen
        app.clear();

        // Draw the sprite
        //app.draw(sprite);

        // Update the window
        app.display();
    }

    return EXIT_SUCCESS;
}
