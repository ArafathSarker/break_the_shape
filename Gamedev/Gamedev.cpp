#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<ctime>
#include<cmath>
#include<algorithm>
#include<iostream>
#include<vector>

using namespace std;
using namespace sf;

#define PI 3.1415926535

//all the global variables

//window width and height
const float wwidth = 1800;
const float wheight = 1050;
//all the entities speed and fadeoutrate here
float herospeed = 10.0f, bulletspeed = 15.0f, fadeoutrate = 2.3f, small_speed = 5.0f,
enemyfadeoutrate = 3.0f;

//Creating the random number generator function with a range
int  random_number(int lower_limit, int upper_limit)
{

    int randomnum = lower_limit + rand() % ((upper_limit + 1) - lower_limit);
    return randomnum;
}

//making the center origin function
void center_origin(CircleShape& shape)
{

    shape.setOrigin(shape.getRadius(), shape.getRadius());

}


//creating don't go out of the window boundary
void window_boundary(CircleShape& shape)
{
    if (shape.getPosition().x - shape.getRadius() <= 0)
    {
        shape.setPosition(shape.getRadius(), shape.getPosition().y);
    }

    if (shape.getPosition().y - shape.getRadius() <= 0)
    {
        shape.setPosition(shape.getPosition().x, shape.getRadius());
    }

    if (shape.getPosition().x + shape.getRadius() >= wwidth)
    {
        shape.setPosition(wwidth - shape.getRadius(), shape.getPosition().y);
    }

    if (shape.getPosition().y + shape.getRadius() >= wheight)
    {
        shape.setPosition(shape.getPosition().x, wheight - shape.getRadius());
    }

}


//The bounsing entity in the window
void  bounse_window(CircleShape& shape, float& bounsespeedx, float& bounsespeedy)
{

    if (shape.getPosition().x - shape.getRadius() <= 0 ||
        shape.getPosition().x + shape.getRadius() >= wwidth)
    {
        bounsespeedx *= -1.0f;
    }

    if (shape.getPosition().y - shape.getRadius() <= 0 ||
        shape.getPosition().y + shape.getRadius() >= wheight)
    {
        bounsespeedy *= -1;
    }

}


//Creating Collusion checking function
bool check_collusion(CircleShape& shape1, CircleShape& shape2)
{
    Vector2f center1 = shape1.getPosition();
    Vector2f center2 = shape2.getPosition();
    float distance = sqrt(pow(center2.x - center1.x, 2) + pow(center2.y - center1.y, 2));
    return distance <= shape1.getRadius() + shape2.getRadius();
}


//The small enemies direction vector function 
vector <Vector2f> smallenemy_direction(CircleShape& shape)
{
    vector <Vector2f> tempvec;

    for (int i = 0; i < shape.getPointCount(); i++)
    {
        float angle = (360 / shape.getPointCount()) * i;
        float radian = (angle * PI) / 180;
        tempvec.push_back(Vector2f(shape.getPosition().x + shape.getRadius() * cos(radian),
            shape.getPosition().y + shape.getRadius() * sin(radian)));
    }
    return tempvec;
}


//The main function
int main()
{
    //creating the window
    RenderWindow window(VideoMode(wwidth, wheight), "Break the Shape", Style::Default);
    //setting the frame rate of game
    window.setFramerateLimit(60);


    //controling the rand function
    srand(time(NULL));

    //Game background pic setup
    Texture backtext;
    backtext.loadFromFile("images/gamebackground.png");
    if (!backtext.loadFromFile("images/gamebackground.png"))
    {
        cerr << "Sorry! Your game background image didn't loaded successfully" << endl;
        exit(-1);
    }
    Sprite backsp(backtext);
    //fiting the window here
    backsp.setScale(wwidth / backtext.getSize().x, wheight / backtext.getSize().y);


    //Creating our mane shape hero 
    CircleShape hero(100, 8);
    hero.setFillColor(Color::Transparent);
    hero.setOutlineColor(Color::Blue);
    hero.setOutlineThickness(10);
    center_origin(hero);
    hero.setPosition(random_number(0, wwidth), random_number(0, wheight));


    //creating the bulllet
    CircleShape bullet(20, 6);
    bullet.setFillColor(Color::Transparent);
    center_origin(bullet);
    bool  bulletth = false;
    float bulletopa = 255;
    Vector2f direction;


    //Creating the enemy shapes here
    vector <CircleShape> enemies;
    Clock enemyclock;
    vector<Vector2f> enemiesspeed;


    //Creating the small enemies here
    vector <CircleShape> smallenemies;
    vector <Vector2f> smallenemydir;
    vector < pair<CircleShape, float> > handlecolor;

    //creating the main enemy
    CircleShape mainenemy(100, 8);
    mainenemy.setFillColor(Color::Transparent);
    mainenemy.setOutlineThickness(12);
    bool mainenemyblink = false;
    center_origin(mainenemy);
    bool mainenemyswitch = false;
    Clock mainenemyclock,mainenemyblinkclock;
    Vector2f mainenemydistance;

    //creating the Score part
    Font font;
    if (!font.loadFromFile("font.ttf"))
    {
        cerr << "Your font didn't loaded successfully";
        exit(-1);
    }
    Text text("SCORE:",font,30);
    float score = 0;


    //The pause key
    bool pausekey = false;

    //Creating the gameover text
    Text gameovertext("GAMEOVER", font, 150);
    bool gameoverkey = false;
    FloatRect gameoversize = gameovertext.getLocalBounds();
    gameovertext.setOrigin(gameoversize.width / 2.0f + gameoversize.left,
    gameoversize.height / 2.0f + gameoversize.top);
    bool gameblink = false;
    Clock gameoverblink,collusionclock;


    //Creating the audio part
    SoundBuffer buffer;
    if (!buffer.loadFromFile("audio/break.wav"))
    {
        return -1;
    }
    Sound audio(buffer);


    //Creating the gameover music
    Music gameoveraudio;
    if (!gameoveraudio.openFromFile("audio/gameover.wav"))
    {
        return -1;
    }
    bool startmusic = true;
    
    //looping the window for each frame
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == event.Closed)
            {
                window.close();
            }

            //The mouse keys
            if (event.type == event.MouseButtonPressed)
            {
                if (Mouse::isButtonPressed(Mouse::Left) && (!bulletth)&&(!pausekey) &&(!gameoverkey))
                {
                    bullet.setPosition(hero.getPosition());
                    bullet.setFillColor(Color(255, 0, 0));
                    Vector2i mouse_position = Mouse::getPosition(window);
                    direction = Vector2f(mouse_position) - bullet.getPosition();
                    float length = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
                    direction /= length;
                    bulletth = true;
                }
            }

            //Keyboard key codes
            if (event.type == event.KeyPressed)
            {

                //The pause key
                if (Keyboard::isKeyPressed(Keyboard::Key::P))
                {
                    pausekey = !pausekey;
                }


                //The second closing key
                if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                {
                    window.close();
                }

                if (!pausekey && !gameoverkey)
                {
                    //Control keys of the hero shape
                    if (event.key.code == Keyboard::Key::W || event.key.code == Keyboard::Key::Up)
                    {
                        hero.move(0, -herospeed);
                    }

                    if (Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down))
                    {
                        hero.move(0, herospeed);
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Key::A) || Keyboard::isKeyPressed(Keyboard::Key::Left))
                    {
                        hero.move(-herospeed, 0);
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::Right))
                    {
                        hero.move(herospeed, 0);
                    }
                }
            }
        }


        //gameover blink
        if (gameoverblink.getElapsedTime().asSeconds() >=0.5f)
        {
            gameblink = !gameblink;
            gameoverblink.restart();
        }
        
        //mainenemy stwiching
        if (!pausekey && !gameoverkey)
        {
            if (mainenemyclock.getElapsedTime().asSeconds() >= 10.0f )
            {
                mainenemy.setPosition(random_number(0, wwidth), random_number(0, wheight));
                mainenemyswitch = !mainenemyswitch;
                mainenemyclock.restart();
            }
        }


        //Gameover collusion
        if (check_collusion(hero, mainenemy) && mainenemyswitch)
        {
            
            if (startmusic)
            {
                gameoveraudio.play();
                startmusic = false;
            }
            if (!gameblink)
            {
                gameovertext.setFillColor(Color::Transparent);

            }
            else {
                gameovertext.setFillColor(Color::Red);
            }
            gameovertext.setPosition(wwidth / 2.0f, wheight / 2.0f);

            gameoverkey = true;
        }

        //blinking the outline of the main enemy
        if (!pausekey && !gameoverkey)
        {
            if (mainenemyblinkclock.getElapsedTime().asSeconds() >= 0.2f)
            {
                mainenemyblink = !mainenemyblink;
                mainenemyblinkclock.restart();
            }
        }
        window_boundary(hero);

        //moving the bullet from hero to mouse position
        if (!pausekey && !gameoverkey)
        {
            if (bulletth)
            {
                bulletopa -= fadeoutrate;
                if (bulletopa < 0) bulletopa = 0;
                bullet.setFillColor(Color(255, 0, 0, bulletopa));
                bullet.move(direction * bulletspeed);
            }
        }


        //reloading the bullet
        if (bullet.getPosition().x - bullet.getRadius() <= 0 ||
            bullet.getPosition().y - bullet.getRadius() <= 0 ||
            bullet.getPosition().x + bullet.getRadius() >= wwidth ||
            bullet.getPosition().y + bullet.getRadius() >= wheight ||
            bulletopa <= 0)
        {
            bulletth = false;
            bullet.setPosition(hero.getPosition());
            bulletopa = 255;
        }

        //creating the enemies after sometime from a random place
        if (!pausekey && !gameoverkey)
        {
            if (enemyclock.getElapsedTime().asSeconds() >= 2.0f)
            {
                CircleShape enemy(100, random_number(3, 9));
                center_origin(enemy);
                enemy.setPosition(random_number(0, wwidth), random_number(0, wheight));
                enemy.setFillColor(Color(random_number(0, 255), random_number(0, 255), random_number(0, 255)));
                enemy.setOutlineColor(Color(random_number(0, 255), random_number(0, 255), random_number(0, 255)));
                enemy.setOutlineThickness(7);
                enemies.push_back(enemy);
                Vector2f enemyspeed(random_number(1, 7), random_number(1, 4));
                enemiesspeed.push_back(enemyspeed);
                enemyclock.restart();
            }
        }


        //moving the enemies
        if (!pausekey && !gameoverkey)
        {
            for (int i = 0; i < enemies.size(); i++)
            {
                window_boundary(enemies[i]);
                enemies[i].move(enemiesspeed[i]);
                bounse_window(enemies[i], enemiesspeed[i].x, enemiesspeed[i].y);
                enemies[i].rotate(4);
            }
        }


        //The collusion part of the game
        if (!pausekey && !gameoverkey)
        {
            for (auto& i : enemies)
            {
                if (check_collusion(bullet, i))
                {
                    audio.play();

                    vector <Vector2f> smallenemiesdir;
                    smallenemiesdir = smallenemy_direction(i);
                    for (int j = 0; j < i.getPointCount(); j++)
                    {
                        //Creating the smallenemies
                        CircleShape smallenemy(i.getRadius() / 3, i.getPointCount());
                        center_origin(smallenemy);
                        smallenemy.setFillColor(i.getFillColor());
                        smallenemy.setOutlineColor(i.getOutlineColor());
                        smallenemy.setOutlineThickness(i.getOutlineThickness());
                        smallenemy.setPosition(i.getPosition());
                        handlecolor.push_back(make_pair(smallenemy, 255));
                        smallenemies.push_back(smallenemy);
                        Vector2f smalldir;
                        smalldir = smallenemiesdir[j] - i.getPosition();
                        float length = sqrt(pow(smalldir.x, 2) + pow(smalldir.y, 2));
                        smalldir /= length;
                        smallenemydir.push_back(smalldir);
                    }
                    i.setFillColor(Color::Transparent);
                    i.setOutlineColor(Color::Transparent);
                    score += i.getPointCount();
                    text.setString("SCORE:" + to_string(score));
                }
            }
        }


        //smallenemies movement speed and rotation and fadeout effet
        if (!pausekey && !gameoverkey)
        {
            for (int j = 0; j < smallenemies.size(); ++j)
            {
                handlecolor[j].second -= enemyfadeoutrate;
                if (handlecolor[j].second < 0) handlecolor[j].second = 0;
                smallenemies[j].setFillColor(Color(handlecolor[j].first.getFillColor().r,
                    handlecolor[j].first.getFillColor().g,
                    handlecolor[j].first.getFillColor().b, handlecolor[j].second));
                smallenemies[j].setOutlineColor(Color(handlecolor[j].first.getOutlineColor().r,
                    handlecolor[j].first.getOutlineColor().g,
                    handlecolor[j].first.getOutlineColor().b, handlecolor[j].second));
                smallenemies[j].move(smallenemydir[j] * small_speed);
                smallenemies[j].rotate(4);
            }
        }


        //earasing the transparent colored shapes
        enemies.erase(
            remove_if(enemies.begin(), enemies.end(), [](CircleShape &destroy) {
                return destroy.getFillColor() == Color::Transparent;
                }), enemies.end());


        //mainenemy direction toward to the hero
        if (!pausekey && !gameoverkey)
        {
            if (mainenemyswitch)
            {
                if (!mainenemyblink)
                {
                    mainenemy.setOutlineColor(Color::Transparent);

                }
                else {
                    mainenemy.setOutlineColor(Color::Red);
                }
                window_boundary(mainenemy);
                mainenemydistance = hero.getPosition() - mainenemy.getPosition();
                float length = sqrt(pow(mainenemydistance.x, 2) + pow(mainenemydistance.y, 2));
                mainenemydistance /= length;
                mainenemy.move(mainenemydistance);
                mainenemy.rotate(4);
            }
        }


        //The drawing Part start from here *********
        //*********************************************************************

        window.clear();

        //1st Positon background drawing
        window.draw(backsp);

        //drawing the enemies
        for (auto& i : enemies)
        {

            window.draw(i);
        }


        //drawing the small enemies
        for (int i=0;i<smallenemies.size();++i)
        {
            if (handlecolor[i].second <= 255)
            {
                window.draw(smallenemies[i]);
                if (!pausekey && !gameoverkey)
                {
                    smallenemies[i].rotate(4);
                }
            }
        }


        //drawing the main enemy
        if (mainenemyswitch)
        {
            window.draw(mainenemy);
        }


        //drawing the bullet
        if (bulletopa <= 255 && bulletth)
        {
            window.draw(bullet);
            if (!pausekey && !gameoverkey)
            {
                bullet.rotate(4);
            }
        }


        //drwing the hero shape
        window.draw(hero);
        if (!pausekey && !gameoverkey)
        {
            hero.rotate(4);
        }

        //drawing the text
        window.draw(text);
        if (gameoverkey)
        {
            window.draw(gameovertext);
        }

        window.display();
    }

}