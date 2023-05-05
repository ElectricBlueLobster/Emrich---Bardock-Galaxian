// Emrich - Bardock Galaxian.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>
#include <cstdlib>  // for rand() and srand()
#include <ctime>    // for time()
#include <cmath>    // for sqrt() and atan2()
#include <chrono>
#include <thread>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

sf::Clock enemyFireClock;
sf::Time enemyFireInterval = sf::seconds(2.0f); // Fire every 2 seconds
sf::Time timeSinceLastEnemyFire = sf::Time::Zero;
sf::Vector2f enemyPos;



//this states that, if a texture is unable to load, the program will inform me.

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

//  ==PLAYER MOVEMENT==

void MoveBardock(PhysicsSprite& Bardock, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        sf::Vector2f newPos(Bardock.getCenter());
        newPos.x = newPos.x + (KB_SPEED * elapsedMS);
        Bardock.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        sf::Vector2f newPos(Bardock.getCenter());
        newPos.x = newPos.x - (KB_SPEED * elapsedMS);
        Bardock.setCenter(newPos);
    }
}

//New crap



sf::Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return sf::Vector2f(r.width, r.height);
}



int main()
{
 


    //== TITLE SCREEEN ==

    // Create a new window for the title screen
    RenderWindow titleScreen(VideoMode(800, 600), "Bardock Galaxian - Title Screen");

    // Load your title screen assets here
    Texture titleScreenTex;
    if (!titleScreenTex.loadFromFile("images/Bardock Galaxian Title.png")) {
        cout << "Could not load Bardock Galaxian Title.png" << endl;
        return -1;
    }
    Sprite titleScreenSprite(titleScreenTex);

    // Display your title screen and wait for the spacebar to be pressed
    while (titleScreen.isOpen()) {
        Event event;
        while (titleScreen.pollEvent(event)) {
            if (event.type == Event::Closed) {
                titleScreen.close();
                return 0;
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space) {
                titleScreen.close();
                break;
            }
        }

        titleScreen.clear();
        titleScreen.draw(titleScreenSprite);
        titleScreen.display();
    }






    // Create the main window for the game
    RenderWindow window(VideoMode(800, 600), "Bardock Galaxian");

    int playerHealth = 100;
    while (window.isOpen() && playerHealth > 0) {

        World world(sf::Vector2f(0, 0));
        int score(0);
        int Blasts(5);

        //Sounds

        SoundBuffer popBuffer;
        if (!popBuffer.loadFromFile("sounds/Laser-1.ogg")) {
            cout << "could not load balloonpop.ogg" << endl;
            exit(5);
        }
        Sound popSound;
        popSound.setBuffer(popBuffer);

         //Clock

        //timeSinceLastEnemyFire += Clock.restart();



        //The Player Avatar

        PhysicsSprite& Bardock = *new PhysicsSprite();
        Texture BardockTex;
        LoadTex(BardockTex, "images/BARDOCK.png");

        Bardock.setTexture(BardockTex);
        sf::Vector2f sz = Bardock.getSize();
        Bardock.setCenter(sf::Vector2f(400, 600 - (sz.y / 2)));



        //The Projectile

        PhysicsSprite Blast;
        Texture projectileTex;
        LoadTex(projectileTex, "images/Blast.png");
        Blast.setTexture(projectileTex);
        bool drawingBlast(false);

        //Enemy's Projectile

        PhysicsSprite Blast2;
        Texture Projectile2Tex;
        LoadTex(Projectile2Tex, "images/Blast2.png");
        Blast2.setTexture(Projectile2Tex);
        bool drawingBlast2(false);




        //Boundaries

        PhysicsRectangle top;
        top.setSize(sf::Vector2f(800, 10));
        top.setCenter(sf::Vector2f(400, 5));
        top.setStatic(true);
        world.AddPhysicsBody(top);

        PhysicsRectangle left;
        left.setSize(sf::Vector2f(10, 600));
        left.setCenter(sf::Vector2f(5, 300));
        left.setStatic(true);
        world.AddPhysicsBody(left);

        PhysicsRectangle right;
        right.setSize(sf::Vector2f(10, 600));
        right.setCenter(sf::Vector2f(795, 300));
        right.setStatic(true);
        world.AddPhysicsBody(right);


        Texture E1Tex;
        LoadTex(E1Tex, "images/Enemy Sprite 1.png");
        PhysicsShapeList<PhysicsSprite> Enemy1s;

        int i = 1;
        int n = 9;
        int EnemiesLeft = (n - i)  /* + (b - a) */; //Later on when there are multiple enemy types, I can make it so that EnemiesLeft is determined by the combination of all the integers, ex: int EnemiesLeft = (n - i) + (a - b) + (c - d);

        for (int z(0); z < 3; z++) {
            for (i; i < n; i++) {
                PhysicsSprite& Enemy1 = Enemy1s.Create();
                Enemy1.setTexture(E1Tex);
                int x = ((700 / 9) * i);
                sf::Vector2f sz = Enemy1.getSize();
                Enemy1.setCenter(sf::Vector2f(x, 20 + (sz.y / 2) * (1.5 * z) + 100));
                Enemy1.setVelocity(sf::Vector2f(0.25, 0));
                world.AddPhysicsBody(Enemy1);

                //ERROR LINES
           
                sf::Clock clock;
                timeSinceLastEnemyFire += clock.restart();
                if (timeSinceLastEnemyFire >= enemyFireInterval) {
                   // int randEnemyIndex = rand() % Enemy1s.size();
                    //PhysicsSprite& randEnemy = Enemy1s.get_shape_list();


                    Blast2.setCenter(Enemy1.getCenter());
                    Blast2.setVelocity(sf::Vector2f(0, 1));
                    drawingBlast2 = true;
                    timeSinceLastEnemyFire = Time::Zero;
                }

                Enemy1.onCollision = [&drawingBlast, &world, &Blast, &Enemy1, &Enemy1s, &score, &popSound, &left, &right, &E1Tex, &EnemiesLeft](PhysicsBodyCollisionResult result) {


                    if (result.object2 == Blast) {
                        popSound.play();
                        drawingBlast = false;
                        world.RemovePhysicsBody(Blast);
                        world.RemovePhysicsBody(Enemy1);
                        Enemy1s.QueueRemove(Enemy1);
                        score += 10;
                        EnemiesLeft = EnemiesLeft - 1;
                    }





                    else if (result.object2 == left) {
                        for (PhysicsSprite& Enemy1 : Enemy1s) {
                            Enemy1.setVelocity(sf::Vector2f(0.25, 0));
                        }
                    }
                    else if (result.object2 == right) {
                        for (PhysicsSprite& Enemy1 : Enemy1s) {
                            Enemy1.setVelocity(sf::Vector2f(-0.25, 0));
                        }
                    }
                };
            }


            i = 1 - z; // Lower the variable to create a larger spawn region for sprites
            n = n + 1; // Raise the variable to create a larger loop spawning more sprites

            //Code Below is for when The Player is hit by an enemy projectile
            Bardock.onCollision = [&drawingBlast2, &world, &Blast2, &Bardock, &playerHealth]
            (PhysicsBodyCollisionResult result) {

                if (result.object1 == Blast2) {
                    world.RemovePhysicsBody(Blast2);
                    playerHealth += -40;

                }


            };

                top.onCollision = [&drawingBlast, &world, &Blast]
                (PhysicsBodyCollisionResult result) {
                drawingBlast = false;
                world.RemovePhysicsBody(Blast);
            };

            Font fnt;
            if (!fnt.loadFromFile("arial.ttf")) {
                cout << "Could not load font." << endl;
                exit(3);
            }

            Clock clock;
            Time lastTime(clock.getElapsedTime());
            Time currentTime(lastTime);

            long timeran = 0;


            // THE GAMEPLAY

            while (((EnemiesLeft > 0) || drawingBlast) && playerHealth > 0) {
                currentTime = clock.getElapsedTime();
                Time deltaTime = currentTime - lastTime;
                long deltaMS = deltaTime.asMilliseconds();
                if (deltaMS > 9) {

                    timeran += deltaMS;

                    lastTime = currentTime;
                    
                    //World Update
                    world.UpdatePhysics(deltaMS);


                    MoveBardock(Bardock, deltaMS);
                    if (Keyboard::isKeyPressed(Keyboard::Space) &&
                        !drawingBlast) {
                        drawingBlast = true;
                        Blast.setCenter(Bardock.getCenter());
                        Blast.setVelocity(sf::Vector2f(0, -1));
                        world.AddPhysicsBody(Blast);

                    }
            // N E W
                /*    void createEnemyBlast(PhysicsShapeList<PhysicsSprite>& Blast2s, sf::Texture& Blast2Tex); {
                        // Create a new blast at the enemy's position
                        PhysicsSprite Blast2;
                        Blast2.setTexture(Projectile2Tex);
                        Blast2.setCenter(enemyPos);
                        world.AddPhysicsBody(Blast2);
                    }
                  */  

                    //Testing for health system: PRESS N TO LOSE ALL HEALTH
                    if (Keyboard::isKeyPressed(Keyboard::N)) {
                        playerHealth = playerHealth - 101;


                    }
                    //Health system functional; game ends when health falls under 0


                    if (timeran > 500) {
                        timeran = 0;
                    }

                    window.clear();
                    if (drawingBlast) {
                        window.draw(Blast);
                    }
                    
                    if (drawingBlast2) {
                        window.draw(Blast2);
                    }



                }
                Enemy1s.DoRemovals();
                for (PhysicsShape& Enemy1 : Enemy1s) {
                    window.draw((PhysicsSprite&)Enemy1);
                }

                window.draw(Bardock);


                Text scoreText;
                scoreText.setString("SCORE: " + to_string(score));
                scoreText.setFont(fnt);
                scoreText.setPosition(sf::Vector2f(780 - GetTextSize(scoreText).x, 550));
                window.draw(scoreText);

                Text PlayerHealthText;
                PlayerHealthText.setString(to_string(playerHealth));
                PlayerHealthText.setFont(fnt);
                PlayerHealthText.setPosition(sf::Vector2f(50 - GetTextSize(PlayerHealthText).x, 550));
                window.draw(PlayerHealthText);
                // world.VisualizeAllBounds(window);

                window.display();

            }
            //End of while loop

            if (EnemiesLeft == 0) {
            

            }




            window.display(); // this is needed to see the last frame
            Text gameOverText;
            gameOverText.setString("GAME OVER");
            gameOverText.setFont(fnt);
            sz = GetTextSize(gameOverText);
            gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
            window.draw(gameOverText);
            window.display();




            while (true) {
                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                    main();
                }
            }
        }
    }
}


    



        

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
