#include <raylib.h>
#include <iostream>

Color darkBlue = {0, 0, 100, 255};
Color brightBlue = {0, 150, 255, 255};
Color darkRed = {139, 0, 0, 255};
Color brightRed = {255, 0, 0, 255};
Color ballColor = {255, 230, 0, 255};

int player_score = 0;
int cpu_score = 0;

class Ball {
 public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw() {
        DrawCircle(x, y, radius, ballColor); 
    }

    void Update() {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }

        if (x + radius >= GetScreenWidth()) {
            player_score++;
            ResetBall();
        }

        if (x - radius <= 0) {
            cpu_score++; 
            ResetBall();
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x = 7 * speed_choices[GetRandomValue(0, 1)];
        speed_y = 7 * speed_choices[GetRandomValue(0, 1)];
    }
};

class Paddle {
 protected:
    void LimitMovement() {
        if (y <= 0) y = 0;
        if (y + height >= GetScreenHeight()) y = GetScreenHeight() - height;
    }

 public:
    float x, y;
    float width, height;
    int speed;
    Color color;

    void Draw() {
        DrawRectangleRounded({x, y, width, height}, 0.8f, 0, color);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) y -= speed;
        if (IsKeyDown(KEY_DOWN)) y += speed;
        LimitMovement();
    }
};

class CpuPaddle : public Paddle {
 public:
    void Update(int ball_y) {
        if (y + height / 2 > ball_y) y -= speed;
        if (y + height / 2 <= ball_y) y += speed;
        LimitMovement();
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;

int main() {
    std::cout << "Starting the game" << std::endl;
    const int screen_width = 1280;
    const int screen_height = 720;
    InitWindow(screen_width, screen_height, "my pong game");
    SetTargetFPS(60);

    ball.radius = 18;
    ball.ResetBall();

    player.width = 25;
    player.height = 120;
    player.x = 10; 
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;
    player.color = brightBlue;

    cpu.width = 25;
    cpu.height = 120;
    cpu.x = screen_width - cpu.width - 10; 
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;
    cpu.color = brightRed;

    // --- NEW: Added state and button variables ---
    bool gameStarted = false;
    Rectangle startButton = { (float)screen_width/2 - 100, (float)screen_height/2 - 40, 200, 80 };
    // ---------------------------------------------

    while (WindowShouldClose() == false) {
        
        // --- MENU SCREEN ---
        if (gameStarted == false) {
            // Check if mouse clicks inside the button area
            if (CheckCollisionPointRec(GetMousePosition(), startButton)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    gameStarted = true; // Start the game!
                }
            }

            BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw a simple white button with black text
            DrawRectangleRec(startButton, WHITE);
            DrawText("START", startButton.x + 35, startButton.y + 20, 40, BLACK);
            
            EndDrawing();
        } 
        // --- GAME SCREEN ---
        else {
            ball.Update();
            player.Update();
            cpu.Update(ball.y);

            // Collisions
            if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {player.x, player.y, player.width, player.height})) {
                ball.speed_x *= -1;
            }
            if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {cpu.x, cpu.y, cpu.width, cpu.height})) {
                ball.speed_x *= -1;
            }

            BeginDrawing();
            
            ClearBackground(darkRed);
            DrawRectangle(0, 0, screen_width / 2, screen_height, darkBlue);

            Vector2 center = {(float)screen_width / 2, (float)screen_height / 2};
            DrawCircleSector(center, 150, -90, 90, 60, brightRed);
            DrawCircleSector(center, 150, 90, 270, 60, brightBlue);

            ball.Draw();
            cpu.Draw();
            player.Draw();

            // Scores
            DrawText(TextFormat("%i", player_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", cpu_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
