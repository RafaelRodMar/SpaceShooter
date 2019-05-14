class Entity
{
    public:
        float x,y,dx,dy,R;
        bool life;
        std::string name;
        Animation anim;
        int scrw=640,scrh=180;

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

    virtual void update(sf::Time dt){};

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
        dx=-150;
        dy=0;
        name="asteroid";
    }

    void  update(sf::Time dt)
    {
        x+=dx * dt.asSeconds();

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

void  update(sf::Time dt)
    {
        dx=350;

        x+=dx * dt.asSeconds();

        if (x>scrw || x<0 || y>scrh || y<0) life=0;
    }

};

class player: public Entity
{
public:

   player()
   {
     name="player";
   }

   void update(sf::Time dt)
   {
    x+=dx * dt.asSeconds();
    y+=dy * dt.asSeconds();

    dx=dy=0.0;
    if (x>scrw) x=scrw;
    if (x<0) x=0;
    if (y>scrh) y=scrh;
    if (y<0) y=0;
   }

};

bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}
