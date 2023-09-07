#include <SFML/Graphics.hpp> //includes SFML libraries
#include <iostream>
#include <math.h>
#include <algorithm>

using namespace std;
using namespace sf; //for graphics

Color RandColor() {
    return Color(rand()%150+105,0,rand()%150+105,1);
}

int main()
{
    RenderWindow window(VideoMode::getDesktopMode(),"Screen Saver", Style::Fullscreen);
    window.setFramerateLimit(60); //sets the game loop to run 60 times per second

    vector<Vertex> line;
    vector<float> CurrentRotation;
    // adding first point into the vector
    line.push_back(Vertex(Vector2f(rand()%window.getSize().x, rand()%window.getSize().y), RandColor()));
    CurrentRotation.push_back(atan2f(line.back().position.y - window.getSize().y/2, line.back().position.x - window.getSize().x/2)*180/3.14159);

    Clock mainClock;
    float deltaTime;
    Clock RandColorTimer;
    RandColorTimer.restart();
    float LastPointTime = 0;
    unsigned int erase_create_Step = 2;

    unsigned int createPointQueue = 1; // has to start at 1
    float createCooldown = 0;

    unsigned int erasePointQueue = 0;
    float eraseCooldown = 0;

    int maxDisBetweenPoints = 250; // used only when creating new points

    Text informationText;
    Font font;
    font.loadFromFile("Testing Items\\Roboto-Regular.ttf");
    informationText.setFont(font);
    informationText.setCharacterSize(14);
    int fpsCount = 0;
    int fps = 0;
    float secondTimer = 0;
    Vector2f infoMove;

    while (window.isOpen()&& !Keyboard::isKeyPressed(Keyboard::Escape))     //the main game loop, exits if someone closes the window
    {
        deltaTime = mainClock.getElapsedTime().asSeconds();
        mainClock.restart();

        secondTimer += deltaTime;
        LastPointTime += deltaTime;
        eraseCooldown += deltaTime;
        createCooldown += deltaTime;

        Event event; //creates an event object, events include mouse clicks, mouse movement, keyboard presses, etc..
        while (window.pollEvent(event)) //loop that checks for events
        {
            if (event.type == Event::Closed) //checks if window is closed
                window.close();
        }   //ends the event loop

        // where the erasing of points is handled
        if (erasePointQueue >= 1 && eraseCooldown >= 0.1f) // not working
        {
            eraseCooldown = 0;
            // checking if the first point should be deleated
            while ((int)(line[0].color.a-erase_create_Step) < 0) {
                line.erase(line.begin());
                CurrentRotation.erase(CurrentRotation.begin());
                erasePointQueue--;
            }
            for (int i = 0; i < erasePointQueue; i++)
            {
                line[i].color.a -= erase_create_Step;
            }
        }

        //the creation of new points is handled here
        if (LastPointTime >= 0.5)
        {
            LastPointTime = 0;

            Vector2f startPoint = line.back().position;
            // makeing a vector for the new point
            Vector2f newPoint = Vector2f((int)(rand()%(maxDisBetweenPoints*2) - maxDisBetweenPoints + startPoint.x) % window.getSize().x,
                                            (int)(rand()%(maxDisBetweenPoints*2) - maxDisBetweenPoints + startPoint.y) % window.getSize().y);

            // creating focus point from the middle of the new line
            Vector2f FocusPoint = Vector2f((int)(rand()%(maxDisBetweenPoints*2) - maxDisBetweenPoints + startPoint.x) % window.getSize().x,
                                            (int)(rand()%(maxDisBetweenPoints*2) - maxDisBetweenPoints + startPoint.y) % window.getSize().y);

            Vector2f _distance = Vector2f((newPoint.x - startPoint.x), (newPoint.y - startPoint.y));

            int numOfFocusPoints = (int)(sqrt(pow(_distance.x, 2) + pow(_distance.y, 2)))/25;

            _distance = Vector2f(_distance.x / numOfFocusPoints, _distance.y / numOfFocusPoints);

            for (int i = 1; i <= numOfFocusPoints; i++)
            {

                // adding a point to create a bottom line
                line.push_back(Vertex(Vector2f(_distance.x*i + startPoint.x, _distance.y*i + startPoint.y), RandColor()));
                CurrentRotation.push_back(atan2f(line.back().position.y - window.getSize().y/2, line.back().position.x - window.getSize().x/2)*180/3.14159);

                // adding a point at focus
                line.push_back(Vertex(FocusPoint, RandColor()));
                CurrentRotation.push_back(atan2f(line.back().position.y - window.getSize().y/2, line.back().position.x - window.getSize().x/2)*180/3.14159);

                // adding a point back at the first in for loop to contiune bottom line next iteration
                line.push_back(Vertex(Vector2f(_distance.x*i + startPoint.x, _distance.y*i + startPoint.y), RandColor()));
                CurrentRotation.push_back(atan2f(line.back().position.y - window.getSize().y/2, line.back().position.x - window.getSize().x/2)*180/3.14159);

                createPointQueue+=3;
            }
        }

        // used to slowly increase the alpha of the new points;
        if (createPointQueue >= 1 && createCooldown >= 0.05f)
        {
            createCooldown = 0;
            for (int i = 0; i < createPointQueue; i++)
            {
                if (line[line.size() - createPointQueue].color.a+erase_create_Step >= 255)
                {
                    line[line.size() - createPointQueue].color.a = 255;
                    createPointQueue--;

                    if (line.size() >= 35)
                        erasePointQueue++; // adding a point to be erased to the queue
                }
                line[line.size() - i-1].color.a += erase_create_Step;
            }
        }

        // handling the rotation of the lines
        for (int i = 0; i < line.size(); i++) {
            // rotation vars
            CurrentRotation[i] += 10 * deltaTime;
            CurrentRotation[i] = CurrentRotation[i] < 360 ? CurrentRotation[i] : 0; // increasing current rotation by 1 and keeping it between 0 and 359 degrees
            float rad = (CurrentRotation[i]*3.14159)/180;

            // distance vars
            Vector2f x_y_comp = Vector2f(line[i].position.x - window.getSize().x/2, line[i].position.y - window.getSize().y/2);
            float distance = sqrt(pow(x_y_comp.x,2) + pow(x_y_comp.y,2));

            x_y_comp.x = distance * cos(rad);
            x_y_comp.y = distance * sin(rad);

            line[i].position = Vector2f(x_y_comp.x + window.getSize().x/2, x_y_comp.y + window.getSize().y/2);
        }


        window.clear(); //clears the screen

        window.draw(&line[0], line.size(), PrimitiveType::LineStrip);

        fpsCount++;
        if (secondTimer >= 1)
        {
            fps = fpsCount;
            fpsCount = 0;
            secondTimer = 0;
        }
        informationText.setString("Points #: " + to_string(line.size()) + "\nCreate Queue: " + to_string(createPointQueue)
                                  + "\nErase Queue: " + to_string(erasePointQueue) + "\n'Idle' Points: " + to_string(line.size()-createPointQueue-erasePointQueue) +
                                  "\nCreate Cooldown: " + to_string(createCooldown) + "\nErase Cooldown: " + to_string(eraseCooldown)
                                  + "\nDelta Time: " + to_string(deltaTime) + "\nFPS: " + to_string(fps));
        // checking if the text has changed size and correcting its position base on that (only needed in two cases)
        if (infoMove.y > 0)
            informationText.setPosition(window.getSize().x - ((int)informationText.getLocalBounds().width / 10 )*10 - 10, informationText.getPosition().y);
        else if (infoMove.x < 0)
            informationText.setPosition(informationText.getPosition().x, window.getSize().y - informationText.getLocalBounds().height - 5);
        Vector2f temp = informationText.getPosition();
        // checking if the text box needs to change direction
        if (temp.x <= 0 && temp.y <= 0)
        {
            infoMove = Vector2f(100,0);
        }
        else if (temp.x + informationText.getLocalBounds().width + 2.5 >= window.getSize().x && temp.y <= 0)
        {
            infoMove = Vector2f(0, 100);
        }
        else if (temp.y + informationText.getLocalBounds().height >= window.getSize().y && temp.x + informationText.getLocalBounds().width + 10 >= window.getSize().x)
        {
            infoMove = Vector2f(-100,0);
        }
        else if (temp.x <= 0 && temp.y >= window.getSize().y - informationText.getLocalBounds().height - 5)
        {
            infoMove = Vector2f(0,-100);
        }
        informationText.move(infoMove.x * deltaTime, infoMove.y * deltaTime);
//        window.draw(informationText);

        window.display();   //displays everything on the video card to the monitor
    }   //ends the game loop

    return 0;
}
