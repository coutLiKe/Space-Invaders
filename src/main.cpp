#include <raylib.h>
#include "game.hpp"
#include <string>
using namespace std;

string FormatWithLeadingZeros(int number, int width){
    string numberText = to_string(number);
    int leadingZeros = width - numberText.length();
    return numberText = string(leadingZeros, '0') + numberText;
}

int main() {
    Color grey = {29, 29, 27, 255};
    Color yellow = {243, 216, 63, 255};
    int windowWidth = 1080;
    int windowHeight = 800;

    InitWindow(windowWidth, windowHeight, "Space Invaders");
    InitAudioDevice();

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);
    Texture2D spaceshipImage = LoadTexture("Graphics/spaceship.png");

    SetTargetFPS(60);

    Game game;

    while (WindowShouldClose() == false) {
        UpdateMusicStream(game.music);

        game.HandleInput();
        game.Update();

        BeginDrawing();
        ClearBackground(grey);

        int borderThickness = 5; // Thickness of the border
        DrawRectangle(0, 0, windowWidth, borderThickness, yellow); // Top border
        DrawRectangle(0, windowHeight - borderThickness, windowWidth, borderThickness, yellow); // Bottom border
        DrawRectangle(0, 0, borderThickness, windowHeight, yellow); // Left border
        DrawRectangle(windowWidth - borderThickness, 0, borderThickness, windowHeight, yellow); // Right border

        DrawLineEx({15, 730}, {1065, 730}, 3, yellow);

        if (game.run) {
            DrawTextEx(font, TextFormat("LEVEL %02d", game.level), {870, 745}, 34, 2, yellow); // Display current level
        } else {
            DrawTextEx(font, "GAME OVER", {870, 745}, 34, 2, yellow);
        }

        float x = 50.0;
        for (int i = 0; i < game.lives; i++) {
            DrawTextureV(spaceshipImage, {x, 745}, WHITE);
            x += 50;
        }

        DrawTextEx(font, "SCORE", {50, 15}, 34, 2, yellow);
        string scoreText = FormatWithLeadingZeros(game.score, 5);
        DrawTextEx(font, scoreText.c_str(), {50, 40}, 34, 2, yellow);

        DrawTextEx(font, "HIGH-SCORE", {870, 15}, 34, 2, yellow);
        string highscoreText = FormatWithLeadingZeros(game.highscore, 5);
        DrawTextEx(font, highscoreText.c_str(), {960, 40}, 34, 2, yellow);

        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    CloseAudioDevice();
}
