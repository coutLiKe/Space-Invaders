#include "game.hpp"
#include <iostream>
#include <fstream>
using namespace std;

Game::Game() {
    music = LoadMusicStream("Sounds/music.ogg");
    explosionSound = LoadSound("Sounds/explosion.ogg");
    PlayMusicStream(music);
    InitGame();
}

Game::~Game() {
    Alien::UnloadImages();
    UnloadMusicStream(music);
    UnloadSound(explosionSound);
}

void Game::Update() {
    if (run) {
        double currentTime = GetTime();
        if (currentTime - timeLastSpawn > mysteryShipSpawnInterval) {
            mysteryship.Spawn();
            timeLastSpawn = GetTime();
            mysteryShipSpawnInterval = GetRandomValue(10, 20);
        }

        for (auto& laser : spaceship.lasers) {
            laser.Update();
        }
        MoveAliens();

        AlienShootLaser();
        for (auto& laser : alienLasers) {
            laser.Update();
        }

        DeleteInactiveLasers();
        mysteryship.Update();
        CheckForCollisions();

        if (aliens.empty()){
            level++;
            ResetAliens();
        }
    } else {
        if (IsKeyDown(KEY_SPACE)) {
            Reset();
            InitGame();
            run = true;
        }
    }
}

void Game::Draw() {
    if (run) {
        spaceship.Draw();

        for (auto& laser : spaceship.lasers) {
            laser.Draw();
        }

        for (auto& obstacle : obstacles) {
            obstacle.Draw();
        }

        for (auto& alien : aliens) {
            alien.Draw();
        }
        for (auto& laser : alienLasers) {
            laser.Draw();
        }
        mysteryship.Draw();

    } else {
        DrawStartScreen(); // Draw the start screen if the game is not running
    }
}

void Game::HandleInput() {
    if (run) {
        if (IsKeyDown(KEY_LEFT)) {
            spaceship.MoveLeft();
        } else if (IsKeyDown(KEY_RIGHT)) {
            spaceship.MoveRight();
        } else if (IsKeyDown(KEY_SPACE)) {
            spaceship.FireLaser();
        }
    }
}

void Game::DeleteInactiveLasers() {
    for (auto it = spaceship.lasers.begin(); it != spaceship.lasers.end();) {
        if (!it->active) {
            it = spaceship.lasers.erase(it);
        } else {
            ++it;
        }
    }
    for (auto it = alienLasers.begin(); it != alienLasers.end();) {
        if (!it->active) {
            it = alienLasers.erase(it);
        } else {
            ++it;
        }
    }
}

vector<Obstacle> Game::CreateObstacles() {
    int obstaclewidth = Obstacle::grid[0].size() * 3;
    float gap = (GetScreenWidth() - (4 * obstaclewidth)) / 5;

    for (int i = 0; i < 4; i++) {
        float offsetX = (i + 1) * gap + i * obstaclewidth;
        obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight() - 200)}));
    }
    return obstacles;
}

vector<Alien> Game::CreateAliens() {
    vector<Alien> aliens;

    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 11; c++) {
            int alienType;
            if (r == 0) {
                alienType = 3;
            } else if (r == 1 || r == 2) {
                alienType = 2;
            } else {
                alienType = 1;
            }

            float x = 75 + c * 55;
            float y = 110 + r * 55;
            aliens.push_back(Alien(alienType, {x, y}));
        }
    }
    return aliens;
}

void Game::MoveAliens() {
    for (auto& alien : aliens) {
        if (alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth() - 10) {
            aliensDirection = -1;
            MoveDownAliens(4);
        }
        if (alien.position.x < 10) {
            aliensDirection = 1;
            MoveDownAliens(4);
        }
        alien.Update(aliensDirection);
    }
}

void Game::MoveDownAliens(int distance) {
    for (auto& alien : aliens) {
        alien.position.y += distance;
    }
}

void Game::AlienShootLaser() {
    double currentTime = GetTime();
    if (currentTime - timeLastAlienFired >= alienLaserShootInterval && !aliens.empty()) {
        int randomIndex = GetRandomValue(0, aliens.size() - 1);

        Alien& alien = aliens[randomIndex];
        alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type - 1].width / 2, alien.position.y + alien.alienImages[alien.type - 1].height}, 6));

        timeLastAlienFired = GetTime();
    }
}

void Game::CheckForCollisions() {
    // Spaceship's Lasers
    for (auto& laser : spaceship.lasers) {
        auto it = aliens.begin();
        while (it != aliens.end()) {
            if (CheckCollisionRecs(it->getRect(), laser.getRect())) {
                PlaySound(explosionSound);
                if (it->type == 1) {
                    score += 10;
                } else if (it->type == 2) {
                    score += 15;
                } else if (it->type == 3) {
                    score += 25;
                }
                checkForHighscore();

                it = aliens.erase(it);
                laser.active = false;
            } else {
                ++it;
            }
        }
        for (auto& obstacle : obstacles) {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end()) {
                if (CheckCollisionRecs(it->getRect(), laser.getRect())) {
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        }
        if (CheckCollisionRecs(mysteryship.getRect(), laser.getRect())) {
            mysteryship.alive = false;
            laser.active = false;
            score += 50;
            checkForHighscore();
            PlaySound(explosionSound);
        }
    }
    // Alien's Lasers
    for (auto& laser : alienLasers) {
        if (CheckCollisionRecs(laser.getRect(), spaceship.getRect())) {
            laser.active = false;
            lives--;
            if (lives == 0) {
                GameOver();
            }
        }
        for (auto& obstacle : obstacles) {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end()) {
                if (CheckCollisionRecs(it->getRect(), laser.getRect())) {
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        }
    }

    // Alien's Collision with Obstacles
    for (auto& alien : aliens) {
        for (auto& obstacle : obstacles) {
            auto it = obstacle.blocks.begin();
            while (it != obstacle.blocks.end()) {
                if (CheckCollisionRecs(it->getRect(), alien.getRect())) {
                    it = obstacle.blocks.erase(it);
                } else {
                    it++;
                }
            }
        }
        if (CheckCollisionRecs(alien.getRect(), spaceship.getRect())) {
            GameOver();
        }
    }
}

void Game::GameOver() {
    run = false;
}

void Game::InitGame() {
    obstacles = CreateObstacles();
    aliens = CreateAliens();
    aliensDirection = 1;
    timeLastAlienFired = 0.0;
    timeLastSpawn = 0.0;
    lives = 3;
    score = 0;
    highscore = loadHighscoreFromFile();
    level = 1;
    run = false; // Initially set run to false
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
}

void Game::checkForHighscore() {
    if (score > highscore) {
        highscore = score;
        saveHighscoreToFile(highscore);
    }
}

void Game::saveHighscoreToFile(int highscore) {
    ofstream highscoreFile("highscore.txt");

    if (highscoreFile.is_open()) {
        highscoreFile << highscore;
        highscoreFile.close();
    } else {
        cerr << "Failed to save highscore to file" << endl;
    }
}

int Game::loadHighscoreFromFile() {
    int loadedHighscore = 0;
    ifstream highscoreFile("highscore.txt");

    if (highscoreFile.is_open()) {
        highscoreFile >> loadedHighscore;
        highscoreFile.close();
    } else {
        cerr << "Failed to save highscore to file" << endl;
    }
    return loadedHighscore;
}

void Game::Reset() {
    spaceship.Reset();
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
}

void Game::DrawStartScreen() {
    DrawText("Press SPACE to Start", GetScreenWidth() / 2 - MeasureText("Press SPACE to Start", 20) / 2, GetScreenHeight() / 2, 20, YELLOW);
}

void Game::ResetAliens(){
    aliens = CreateAliens();
    aliensDirection = 1;
}

