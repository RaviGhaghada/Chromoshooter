/*
    Chromoshooter

    Developed By:
    Ravi Ghaghada
    Class 12-A-boys (2017-18)
    The Rajkumar College
    Rajkot, India

    remvove space after end of entered string
*/

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;

#include <cstdio>
#include <math.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
using namespace std;

const int FPS=30;
const int block=30;
const string savedata = "savedata.dat";
Font fnt;
VideoMode videom = VideoMode::getFullscreenModes()[0];


/*
    ------------------------------------------------------------
                            SUPPORT FUNCTIONS
    ------------------------------------------------------------
*/

string IntToString (int a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}

template <class T>
void shrinkVector(vector<T> &v, int i)
{
    if (v.size()>1)
    {
        swap(v[i], v[v.size()-1]);
        v.resize(v.size()-1);
    }
    else
        v.clear();
}

bool areColliding (FloatRect f1, FloatRect f2)
{
    if (f1.intersects(f2))
        return true;
    return false;
}

string arrToString (char word[])
{
    string s;
    for (int i=0; word[i]!='\0'; i++)
        s.push_back(word[i]);
    return s;
}

char* stringToArr (string s)
{
    char arr[20];
    for (int i=0; i<s.size();i++)
        arr[i]=s[i];
    arr[s.size()]='\0';
    return arr;
}

bool strstri(string s1, string s2)
{
    for (int i=0; s1[i]!='\0'; i++)
        s1[i]=toupper(s1[i]);
    for (int i=0; s2[i]!='\0'; i++)
        s2[i]=toupper((s2[i]));

    for (int ssize=1;  ssize<=s1.size(); ssize++)
        if (s1.substr(0, ssize) == s2)
        {
            return true;
        }

    return false;
}

bool isEmpty (fstream& fi)
{
    return fi.peek() == ifstream::traits_type::eof();
}

void takeScreenShot(RenderWindow& window)
{

    Image ss = window.capture();
    ss.saveToFile("scorebpic.jpg");
}
/*
    ---------------------------------------------------------------
                                CLASSES
    ---------------------------------------------------------------
*/
class Bullet : public Drawable
{
    float speed; // speed of bullet
    Color clr;
    CircleShape body; // graphical body of bullet
    virtual void draw (RenderTarget& target, RenderStates states) const // Draw function
    {
        target.draw(body, states);
        states.texture=nullptr;
    }

public:

    Bullet () // empty constructor for swap(a,b) to work
    {
    }
    Bullet(float x, float y, Color cclr) // Constructor of use
    {
        clr=cclr;
        body.setRadius(block/2);
        body.setFillColor(clr);
        body.setPosition(Mouse::getPosition().x, y);
        speed=block/2;
    }
    bool update() // update position and other variable
    {
        if (body.getPosition().y<0)
            return false;
        body.move(0, -speed);
        if (body.getPosition().y<=3*block) // fade at top
            body.setFillColor(Color(body.getFillColor().r,body.getFillColor().g,body.getFillColor().b, max(body.getFillColor().a-50, 50)));
        return true;
    }

    FloatRect getGlobalBounds()
    {
        return body.getGlobalBounds();
    }
    Color getColor()
    {
        return clr;
    }
};

class Meteor : public Drawable
{
private:
    float speed;
    VertexArray quad; // graphical object for the body

    virtual void draw(RenderTarget& target, RenderStates states) const // draw function for drawing
    {
        target.draw(quad, states);
        states.texture=nullptr;
    }
public:
    Meteor()
    {

    }

    Meteor (Color cclr, float spd=1) // Constructor
    {
        speed = spd;
        quad.setPrimitiveType(Quads);
        quad.resize(4);

        // creation of random quadrilaterals
        quad[0].position=Vector2f(rand()%(videom.width-block)+block, -100);
        quad[1].position=Vector2f(quad[0].position.x + (rand()%(block) - block), rand()%(2*block)-100);
        quad[2].position=Vector2f(quad[1].position.x + (rand()%(2*block) - block), quad[1].position.y + rand()%(2*block)-90);
        quad[3].position=Vector2f(quad[2].position.x + (rand()%(3*block) - block), quad[2].position.y + rand()%(4*block)-80);

        for (int i=0; i<4; i++)
            quad[i].color=cclr;

    }

    void update () // to update the coordinates and stuff
    {
        for (int i=0; i<4; i++)
            quad[i].position=Vector2f(quad[i].position.x, quad[i].position.y+speed);
    }
    FloatRect getGlobalBounds()
    {
        return quad.getBounds();
    }

    float getSpeed()
    {
        return speed;
    }
    void setSpeed(float spd)
    {
        speed=spd;
    }
    Color getColor()
    {
        return quad[0].color;
    }
};

class Box : public Drawable
{
public:
    Text txt;
    RectangleShape rbox;

    Box()
    {

    }
    Box (string str, float x, float y, float width=videom.width/2, float height=3*block)
    {
        rbox.setPosition(x,y);
        rbox.setSize(Vector2f(width, height));
        rbox.setFillColor(Color::White);
        rbox.setOutlineColor(Color::Black);
        rbox.setOutlineThickness(3);
        txt.setString(str);
        txt.setCharacterSize(block);
        txt.setPosition(x+block, y+block);
        txt.setColor(Color::Black);
        txt.setFont(fnt);
    }

    FloatRect getGlobalBounds()
    {
        return rbox.getGlobalBounds();
    }
private:
    virtual void draw(RenderTarget& target, RenderStates states) const
    {
        target.draw(rbox, states);
        target.draw(txt, states);
        states.texture=nullptr;
    }
};

struct Score
{
    char name[20];
    int score;
    Score()
    {
    }
    Score(char sname[20], int sscore)
    {
        strcpy(name, sname);
        score=sscore;
    }
    void display()
    {
        cout << endl << name<< ' ' << score<<endl;
    }

};
bool lowerScore(Score a, Score b)
{
    return a.score>b.score;
}

bool updateString (Event &event, string &str)
{
    char ch=static_cast<char>(event.text.unicode);

    if (ch==8 && str.size()!=0) // BACKSPACE
        str.pop_back();
    else if (str.size()>15)
        return true;
    else if (isalpha(ch) || ch==32)
        str+=ch;
    else if (ch==13)
        return false;

    return true;
}

bool addScoreRecord (Score x)
{
    Score temp;
    fstream fo ("savedata.dat", ios::app | ios::binary);
    fo.close();
    fstream fi ("savedata.dat", ios::in | ios::binary);

    bool flag=false;;
    if (!isEmpty(fi))
    {
        fo.open("tempdata.dat", ios::app | ios::binary);
        while (!fi.eof() && fi.read((char*)&temp, sizeof(Score)))
        {
            if (temp.score>=x.score ^ flag)
                fo.write((char*)&temp, sizeof(Score));
            else
            {
                fo.write((char*)&x, sizeof(Score));
                fo.write((char*)&temp, sizeof(Score));
                flag=true;
            }

        }
        if (!flag)
            fo.write((char*)&x, sizeof(Score));
        fi.close();
        fo.close();

        remove ("savedata.dat");
        rename("tempdata.dat", "savedata.dat");
        return true;
    }
    else
    {
        fi.close();
        fstream fo2 ("savedata.dat", ios::app | ios::binary);
        fo2.write((char*)&x, sizeof(Score));
        fo2.close();
        return false;
    }

}
void peekScoreRecord()
{
    fstream fi ("savedata.dat", ios::in | ios::binary);
    if (isEmpty(fi))
        return;
    Score x;
    while (!fi.eof()&& fi.read((char*)& x, sizeof(Score)))
    {

        x.display();
    }
    fi.close();
}
/*
    ---------------------------------------------------------------
                                MAIN GAME
    ---------------------------------------------------------------
*/

int introScreen(RenderWindow& window)
{
    window.setMouseCursorVisible(false);

    Color clrs[] = {Color::Red, Color::Green, Color::Blue, Color::Magenta, Color::Cyan};
    int var=rand()%5;
    Text title ("CHROMO SHOOTER\n  RAVI GHAGHADA", fnt, block/10);
    title.setFillColor(Color::White);
    title.setPosition(0,0);
    title.setPosition(videom.width/2 - title.getLocalBounds().width/2, videom.height/2-title.getLocalBounds().height/2);

    Clock time_check;
    while (time_check.getElapsedTime().asSeconds()<5)
    {
        window.clear(Color::Black);
        if (title.getPosition().x>block)
            title.move(-block, 0);
        if (title.getPosition().y>block)
            title.move(0, -block);

        title.setCharacterSize(title.getCharacterSize()+block/20);
        title.setColor(clrs[var]);

        window.draw(title);
        window.display();

        var=rand()%5;

    }

/*
    title.setString("CHROMO SHOOTER");
    title.setCharacterSize(2*block);
    title.setFillColor(Color::White);


    time_check.restart();
    while (time_check.getElapsedTime().asSeconds()<5)
    {
        window.clear(Color::Black);

        pos-=block/4;
        var=rand()%5;

        title.setPosition(block, pos);
        title.setColor(clrs[var]);

        window.draw(title);
        window.display();

        if (title.getPosition().y<block)
            break;
    }
*/
    window.setMouseCursorVisible(true);
    return 0;

}

int playGame(RenderWindow& window)
{
    window.setFramerateLimit(FPS);
    window.setMouseCursorVisible(false);

    /*
    -------------------------
        State variables
    -------------------------
    */
    int csize=3; // range of colors to be taken
    int score=0; // player's score
    float spawnmax=1500;
    Clock spawntime, cooldown; // spawntime-time between two spawns; cooldown-cooldown of gun;
    int bullcolor=rand()%csize, mtrcolor=rand()%csize; // color of bullet

    /*
    --------------------
        Game objects
    --------------------
    */
    Color colors[3]={Color::Red, Color::Blue, Color::Green};
    vector <Bullet> bullets; // store all the bullets
    RectangleShape b_line (Vector2f(videom.width, block)); // black line not to be crossed
    b_line.setPosition(0, videom.height-2*block); // borderline
    b_line.setFillColor(colors[bullcolor]);

    RectangleShape gun (Vector2f(block, block)); // player's gun or turret
    gun.setPosition(videom.width/2, videom.height-3*block);
    gun.setFillColor(Color::White);
    vector <Meteor> vmeteors; // store all meteors
    Mouse::setPosition(Vector2i(gun.getPosition().x, block), window);

    /*
    -----------------
        FONT, TEXT
    -----------------
    */
    Text textscore("Score: 0", fnt, block/1.5);
    textscore.setFillColor(Color::White);
    textscore.setPosition(block, videom.height-block);

    while (window.isOpen())
    {
        /*
        ----------------------
            EVENT HANDLING
        ----------------------
        */
        Event event;
        while (window.pollEvent(event))
        {
            if (Mouse::isButtonPressed(Mouse::Left))
                if (cooldown.getElapsedTime().asMilliseconds()>300)
                {
                    bullets.push_back(Bullet(gun.getPosition().x-block/2, gun.getPosition().y-block, colors[bullcolor]));
                    bullcolor=rand()%csize;
                    b_line.setFillColor(colors[bullcolor]);
                    cooldown.restart();
                }

        }
        /*
        ----------------------------------------------------------
            CHECKING AND UPDATING ALL OBJECTS AND VARIABLES
        ----------------------------------------------------------
        */
        gun.setPosition(min(Mouse::getPosition(window).x, int(videom.width-(videom.width%block))), gun.getPosition().y);

        if (spawntime.getElapsedTime().asMilliseconds()>spawnmax)
        {
            vmeteors.push_back(Meteor(colors[mtrcolor]));
            mtrcolor=rand()%csize;
            spawntime.restart();
        }

        window.clear(Color::Black);
        for (int i=0; i<bullets.size(); i++)
        {
            for (int j=0; j<vmeteors.size(); j++)
                if (areColliding(vmeteors[j].getGlobalBounds(), bullets[i].getGlobalBounds()))
                {
                    if (vmeteors[j].getColor()==bullets[i].getColor())// if color is matching
                    {
                        shrinkVector(vmeteors, j); // remove meteor
                        score+=10; // increment score
                        textscore.setString("Score: " + IntToString(score)); // update score in text
                        spawnmax = max(500, int(2000-1000*atan(score/100)*cos(0.2617*score/10)));
                        // = MAX_DESIRED_COOLDOWN - atan((score/10)/10) * cos(pi/12*score/10)
                    }
                    shrinkVector(bullets,i); // remove bullet
                    i--; // to avoid skipping a bullet in next loop
                    j=0; // resetting loop for new unskipped bullet
                    if (i<0) // if the last bullet is destroyed, terminate the loop
                        goto nextloop;
                }
            if (!bullets[i].update())
            {
                shrinkVector(bullets, i);
                i--;
            }
            else
                window.draw(bullets[i]);
        }
        nextloop:
        for (int i=0; i<vmeteors.size(); i++)
        {

            if (areColliding(vmeteors[i].getGlobalBounds(), b_line.getGlobalBounds()))
            {
                window.clear(Color::White);
                window.setMouseCursorVisible(true);
                return score;
            }
            else
            {
                vmeteors[i].update();
                window.draw(vmeteors[i]);
            }
        }

        /*
        --------------------------------------
            DRAWING ALL OBJECTS TO WINDOW
        --------------------------------------
        */
        window.draw(b_line);
        window.draw(gun);
        window.draw(textscore);
        window.display();
    }
    return 0;
}
string getPlayerName(RenderWindow& window, string strprompt)
{

    Text prompt (strprompt, fnt, 2*block);
    prompt.setPosition((videom.width-prompt.getLocalBounds().width)/2, 3*block);
    prompt.setFillColor(Color::White);

    Text reply ("", fnt, 2*block);
    reply.setFillColor(Color::White);
    string name;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::TextEntered)
            {
                if (updateString(event, name))
                {
                    reply.setString(name);
                    reply.setPosition((videom.width-reply.getLocalBounds().width)/2, videom.height/2);
                }
                else
                    return name;
            }
        }
        window.clear(Color::Black);
        window.draw(prompt);
        window.draw(reply);
        window.display();
    }
}
int scoreBoard(RenderWindow& window)
{
    vector<Score> tscores; // object to read from savedata
    int start=0; // starting index
    const int items_per_page=10; // max items per page
    vector <Text> textscores; // to store all boxes
    Score tempscore;


    fstream fs (savedata, ios::in | ios::binary);
    if (!isEmpty(fs))
    {
        while (!fs.eof() && fs.read((char*)& tempscore, sizeof(Score)))
            tscores.push_back(tempscore);
    }
    fs.close();


    Text basetext ("", fnt, block);
    basetext.setColor(Color::White);

    for (int i=0; i<tscores.size(); i++)
    {
        textscores.push_back(basetext);
        textscores[i].setString(IntToString(i+1) + ". " + arrToString(tscores[i].name) + '\t' + IntToString(tscores[i].score));
    }

    basetext.setString("SCOREBOARD");
    basetext.setPosition(block,block);
    basetext.setCharacterSize(2*block);

    Box prevbox("Previous", 1.5*block, videom.height/4, 6*block, 3*block),
    nextbox("Next", 1.5*block, 4*block+videom.height/4, 6*block, 3*block),
    menubox("Return", 1.5*block, 0.8*videom.height, 6*block, 3*block);


    Text tsearch ("Search Name: ", fnt, block);
    tsearch.setPosition(1.5*block, videom.height/4+8*block);
    string ssearch;
    fstream sfile (savedata, ios::in | ios::binary);
    vector <Text> temptextscores = textscores;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (Keyboard::isKeyPressed(Keyboard::Num1))
                takeScreenShot(window);
            Vector2f point = Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y);

            if (menubox.getGlobalBounds().contains(point))
            {
                menubox.rbox.setFillColor(Color::Red);
                if (Mouse::isButtonPressed(Mouse::Left))
                    return 0;
            }
            else
                menubox.rbox.setFillColor(Color::White);
            if (prevbox.getGlobalBounds().contains(point))
            {
                prevbox.rbox.setFillColor(Color::Green);
                if (Mouse::isButtonPressed(Mouse::Left) && start>=10)
                    start-=items_per_page;
            }
            else
                prevbox.rbox.setFillColor(Color::White);

            if (nextbox.getGlobalBounds().contains(point))
            {
                nextbox.rbox.setFillColor(Color::Blue);
                if (Mouse::isButtonPressed(Mouse::Left) && start+items_per_page<textscores.size())
                    start+=items_per_page;
            }
            else
                nextbox.rbox.setFillColor(Color::White);

            if (event.type == Event::TextEntered)
            {
                updateString(event, ssearch);
                if (ssearch.size()!=0)
                {
                    textscores.clear();
                    for (int i=0; i<tscores.size(); i++)
                        if (strstri(arrToString(tscores[i].name), ssearch))
                            textscores.push_back(Text(IntToString(i+1) + ". " + arrToString(tscores[i].name) + '\t' + IntToString(tscores[i].score), fnt, block));
                }
                else
                    textscores=temptextscores;

                tsearch.setString("Search Name: " + ssearch);
            }
        }
        window.clear(Color::Black);

        for (int i=0; i<items_per_page && i+start<textscores.size(); i++)
        {
            textscores[start+i].setPosition(0.8*videom.width-textscores[start+i].getLocalBounds().width, block*(3+2*(i+1)));
            window.draw(textscores[start+i]);
        }

        window.draw(basetext);
        window.draw(nextbox);
        window.draw(prevbox);
        window.draw(menubox);
        window.draw(tsearch);
        window.display();
    }

}
int helpBoard(RenderWindow& window)
{
    Text helptitle ("HELP", fnt, 2*block);
    helptitle.setPosition(block, block);
    helptitle.setColor(Color::White);

    Text rules1 ("+ Use mouse to move\n+ Press Left Button to shoot\n+ Bullets will only damage shapes of the same color", fnt, block);
    Text rules2 ("\n\n\n+ If any shape touches the bottom line, then it's game over\n\n\nProject Made By\nRavi Ghaghada, Class 12A, The Rajkumar College, Rajkot (2017-18)", fnt, block);
    rules1.setFillColor(Color::White);
    rules1.setPosition(1.5*block, videom.height/4);
    rules2.setFillColor(Color::White);
    rules2.setPosition(1.5*block, videom.height/4);

    Box menubox("Return", 1.5*block, videom.height*0.8, 6*block, 3*block);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            Vector2f point = Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
            if (menubox.getGlobalBounds().contains(point))
            {
                menubox.rbox.setFillColor(Color::Red);
                if (Mouse::isButtonPressed(Mouse::Left))
                    return 0;
            }
            else
                menubox.rbox.setFillColor(Color::White);

        }

        window.clear(Color::Black);
        window.draw(helptitle);
        window.draw(rules1);
        window.draw(rules2);
        window.draw(menubox);
        window.display();
    }
}
int main()
{

    RenderWindow window(videom,"CHROMO SHOOTER by Ravi Ghaghada",Style::Fullscreen);
    window.setFramerateLimit(FPS);
    fnt.loadFromFile("BRLNSR.ttf");

    introScreen(window);

    Mouse::setPosition(Vector2i(0.9*videom.width, videom.height/2), window);

    Text maintxt("CHROMO SHOOTER", fnt, 2*block);
    maintxt.setPosition(block, block);
    maintxt.setFillColor(Color::White);
    maintxt.setOutlineColor(Color::Black);
    maintxt.setOutlineThickness(2);

    Box b1 ("Play", 1.5*block, videom.height/4), b2 ("Scoreboard", 1.5*block, 100+videom.height/4) ,
    b3("Help", 1.5*block, 200+videom.height/4), b4("Quit", videom.width-5*block, videom.height-4*block, 4*block, 3*block);
    b4.rbox.setFillColor(Color::White);

    vector <Meteor> meteors;
    Color colors[3]={Color::Red, Color::Green, Color::Blue};
    RectangleShape rectfade(Vector2f(videom.width, videom.height));
    rectfade.setFillColor(Color::Black);

    Clock spawn, c_fade;
    bool showquit=false;
    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            Vector2f point = Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y);


            if (b1.getGlobalBounds().contains(point)) // PLAY GAME
            {
                b1.rbox.setFillColor(Color::Red);
                if ((Mouse::isButtonPressed(Mouse::Left)))
                {
                    int tscore = playGame(window);
                    string name = getPlayerName(window, "Score: " + IntToString(tscore) + "\nEnter name:");
                    if (name.size()!=0)
                        addScoreRecord(Score(stringToArr(name), tscore));
                    meteors.clear();
                    b1.rbox.setFillColor(Color::White);
                }
            }
            else
                b1.rbox.setFillColor(Color::White);

            if (b2.getGlobalBounds().contains(point)) // SCORE BOARD
            {
                b2.rbox.setFillColor(Color::Green);
                if ((Mouse::isButtonPressed(Mouse::Left)))
                {
                    scoreBoard(window);
                    meteors.clear();
                    b2.rbox.setFillColor(Color::White);

                }
            }
            else
                b2.rbox.setFillColor(Color::White);

            if (b3.getGlobalBounds().contains(point))
            {
                b3.rbox.setFillColor(Color::Blue);
                if ((Mouse::isButtonPressed(Mouse::Left)))
                {
                    helpBoard(window);
                    meteors.clear();
                    b3.rbox.setFillColor(Color::White);
                }
            }
            else
                b3.rbox.setFillColor(Color::White);

            if (b4.getGlobalBounds().contains(point))
            {
                showquit=true;
                if ((Mouse::isButtonPressed(Mouse::Left)))
                    window.close();
            }
            else
                showquit=false;

        }

        if (spawn.getElapsedTime().asMilliseconds()>100)
        {
            meteors.push_back(Meteor(colors[rand()%3], 10));
            spawn.restart();
        }

        if (c_fade.getElapsedTime().asMilliseconds()%500)
            rectfade.setFillColor(Color(rectfade.getFillColor().r,
            rectfade.getFillColor().g, rectfade.getFillColor().b, max(int(rectfade.getFillColor().a-meteors.size()),0)));

        window.clear(Color::Black);

        for (int i=0; i<meteors.size(); i++)
        {
            if (meteors[i].getGlobalBounds().top>videom.height)
            {
                shrinkVector(meteors, i);
                i=max(0,i-1);
            }
            meteors[i].update();
            window.draw(meteors[i]);
        }
        window.draw(b1);
        window.draw(b2);
        window.draw(b3);
        if (showquit)
        {
            window.clear(Color::White);
            maintxt.setFillColor(Color::Black);
        }
        else
            maintxt.setFillColor(Color::White);
        window.draw(b4);
        window.draw(maintxt);

        if (rectfade.getFillColor().a>0)
            window.draw(rectfade);
        window.display();
    }
    return 0;

}
