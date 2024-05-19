#pragma once
#include "spaceship.hpp"
#include "obstacle.hpp"
#include "alien.hpp"
#include "mysteryship.hpp"
using namespace std;

class Game{
    public:
        Game();
        ~Game();
        void Draw();
        void Update();
        void HandleInput();
        bool run;
        int lives;
        int score;
        int highscore;
        int level;

        Music music;

    private:
        void DeleteInactiveLasers();
        vector<Obstacle> CreateObstacles();
        vector<Alien> CreateAliens();
        void MoveAliens();
        void MoveDownAliens(int distance);
        void AlienShootLaser();
        void CheckForCollisions();
        void GameOver();
        void InitGame();
        void checkForHighscore();
        void saveHighscoreToFile(int highscore);
        int loadHighscoreFromFile();
        void Reset();
        void DrawStartScreen();
        void ResetAliens();

        Spaceship spaceship;
        vector<Obstacle> obstacles;
        vector<Alien> aliens;
        int aliensDirection;
        vector<Laser> alienLasers;
        constexpr static float alienLaserShootInterval = 0.35;
        float timeLastAlienFired;
        MysteryShip mysteryship;
        float mysteryShipSpawnInterval;
        float timeLastSpawn;
        Sound explosionSound;
};
