#include "raylib.h"
#include "raymath.h"
#include <cstdlib>

inline float Random(float min, float max)
{
    return min + (rand() / ((float)RAND_MAX / (max - min)));
}

constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 800.0f;
constexpr Vector2 CENTER{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };

constexpr float BALL_SPEED = SCREEN_WIDTH * 0.5f;
constexpr float BALL_SIZE = 40.0f;

constexpr float PADDLE_SPEED = SCREEN_HEIGHT * 0.5f;
constexpr float PADDLE_WIDTH = 40.0f;
constexpr float PADDLE_HEIGHT = 80.0f;

struct Box
{
    float xMin;
    float xMax;
    float yMin;
    float yMax;
};

bool BoxOverlap(Box box1, Box box2)
{
    bool x = box1.xMax >= box2.xMin && box1.xMin <= box2.xMax;
    bool y = box1.yMax >= box2.yMin && box1.yMin <= box2.yMax;
    return x && y;
}

Rectangle BoxToRec(Box box)
{
    Rectangle rec;
    rec.x = box.xMin;
    rec.y = box.yMin;
    rec.width = box.xMax - box.xMin;
    rec.height = box.yMax - box.yMin;
    return rec;
}

Box BallBox(Vector2 position)
{
    Box box;
    box.xMin = position.x - BALL_SIZE * 0.5f;
    box.xMax = position.x + BALL_SIZE * 0.5f;
    box.yMin = position.y - BALL_SIZE * 0.5f;
    box.yMax = position.y + BALL_SIZE * 0.5f;
    return box;
}

Box PaddleBox(Vector2 position)
{
    Box box;
    box.xMin = position.x - PADDLE_WIDTH * 0.5f;
    box.xMax = position.x + PADDLE_WIDTH * 0.5f;
    box.yMin = position.y - PADDLE_HEIGHT * 0.5f;
    box.yMax = position.y + PADDLE_HEIGHT * 0.5f;
    return box;
}

void ResetBall(Vector2& position, Vector2& direction)
{
    position = CENTER;
    direction.x = rand() % 2 == 0 ? -1.0f : 1.0f;
    direction.y = 0.0f;
    direction = Vector2Rotate(direction, Random(-50.0f, 50.0f) * DEG2RAD);
}

void DrawBall(Vector2 position, Color color)
{
    Box ballBox = BallBox(position);
    DrawRectangleRec(BoxToRec(ballBox), color);
}

void DrawPaddle(Vector2 position, Color color)
{
    Box paddleBox = PaddleBox(position);
    DrawRectangleRec(BoxToRec(paddleBox), color);
}

int main()
{
    Vector2 ballPosition;
    Vector2 ballDirection;
    ResetBall(ballPosition, ballDirection);

    Vector2 paddle1Position, paddle2Position;
    paddle1Position.x = SCREEN_WIDTH * 0.05f;
    paddle2Position.x = SCREEN_WIDTH * 0.95f;
    paddle1Position.y = paddle2Position.y = CENTER.y;

	int player1Score = 0;

	int player2Score = 0;

    bool player1Win = false;
	bool player2Win = false;
	bool player1WinVisibility = false;
	bool player2WinVisibility = false;
    bool pongWinnerFinished = false;
    
  

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    InitAudioDevice();
    SetTargetFPS(60);

    Sound ballImpact = LoadSound("audio/PongBall.wav");
    Sound winnerScreen = LoadSound("audio/PongWinner.wav");
    SetSoundVolume(ballImpact,0.75f);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        float ballDelta = BALL_SPEED * dt;
        float paddleDelta = PADDLE_SPEED * dt;

    
        if (IsKeyDown(KEY_W))
            paddle1Position.y -= paddleDelta;
        if (IsKeyDown(KEY_S))
            paddle1Position.y += paddleDelta;

        paddle2Position.y = paddle1Position.y;

        float phh = PADDLE_HEIGHT * 0.5f;
        paddle1Position.y = Clamp(paddle1Position.y, phh, SCREEN_HEIGHT - phh);
        paddle2Position.y = Clamp(paddle2Position.y, phh, SCREEN_HEIGHT - phh);

 
        Vector2 ballPositionNext = ballPosition + ballDirection * ballDelta;
        Box ballBox = BallBox(ballPositionNext);
        Box paddle1Box = PaddleBox(paddle1Position);
        Box paddle2Box = PaddleBox(paddle2Position);

    	
        if (ballBox.xMin < 0.0f)
        {
            player2Score++;
        }
        if (ballBox.xMax > SCREEN_WIDTH)
        {
            player1Score++;
        }
        	
    	
    	if (ballBox.xMin < 0.0f || ballBox.xMax > SCREEN_WIDTH)
        {
            ResetBall(ballPosition, ballDirection);
        }
        if (ballBox.yMin < 0.0f || ballBox.yMax > SCREEN_HEIGHT)
        {
            ballDirection.y *= -1.0f;
            PlaySound(ballImpact);
        }
        if (BoxOverlap(ballBox, paddle1Box) || BoxOverlap(ballBox, paddle2Box))
        {
            ballDirection.x *= -1.0f;
            PlaySound(ballImpact);
        }


        if (player1Score >= 5)
        {
            player1WinVisibility = true;
            player1Win = true;
        }
        else
        {
            player1WinVisibility = false;
            player1Win = false;
        }


    	if (player2Score >= 5)
        {
            player2WinVisibility = true;
            player2Win = true;
        }
        else
        {
            player2WinVisibility = false;
            player2Win = false;
        }

        const char* player1ScoreText = TextFormat("Player 1:  %i ", player1Score);

        const char* player2ScoreText = TextFormat("Player 2:  %i ", player2Score);

        const char* winnerPlayer1Text = TextFormat("WINNER", player1Win);

        const char* winnerPlayer2Text = TextFormat("WINNER", player2Win);

        BeginDrawing();
        ClearBackground(BLACK);

        if (!player1WinVisibility && !player2WinVisibility)
        {
            DrawText(player1ScoreText, 125, 50, 35, BLUE);

            DrawText(player2ScoreText, 875, 50, 35, RED);

        	DrawPaddle(paddle1Position, BLUE);
            DrawPaddle(paddle2Position, RED);
            
            Color multiColor = ColorFromHSV(GetTime() * 500.0f, 1.0f, 1.0f);
        	DrawBall(ballPosition, multiColor);

        }

        ballPosition = ballPosition + ballDirection * ballDelta;

    	if (player1WinVisibility)
        {
            DrawText(winnerPlayer1Text, 450, 350, 75, BLUE);
        }

        if (player2WinVisibility)
        {
            DrawText(winnerPlayer2Text, 450,350, 75, RED);
        }

        if (!player1WinVisibility && !player2WinVisibility)
        {
        	PlaySound(winnerScreen);
        	SetSoundVolume(winnerScreen, 0.75f);
        }

        
    	EndDrawing();
    }
    UnloadSound(ballImpact);
    UnloadSound(winnerScreen);
    CloseAudioDevice();
	CloseWindow();
    return 0;
}

// Step 1 - I used the previous score and turned it into two separate player scores that are specifically positioned, scaled and coloured

// Step 2 - Instead of the ball switching direction when colliding with the left or right wall, it now resets back into it's center spawn point thanks to a simple if-statement change

// Step 3 - Simply reused the first part of step 2's if-statement and split it into two parts for this logic; xMin < 0.0f means the left side of the ball colliding with the left wall
// And then xMax > SCREEN_WIDTH means when the ball collides with the "end" of the screen which is the right wall

// Step 4 - I then took my knowledge from booleans from this and the last two semesters and abused them to make a chain of if-statements that created smooth and cool-looking logic for the game over screen
// Which is everything disappearing and the word "WINNER" pops up in the center of the screen in the winning player's colour

// Step 5 - I then followed some Raylib audio guides in the audio section and created a custom sound effect using; https://raylibtech.itch.io/rfxgen
// I exported it and put it in my project directory. With the load and unload audio commands as well as the init and close audio device commands, the sound was successfully implemented

// Step 6 - For the custom implementation, I added a second sound for when the winner screen is being displayed, it is an FL Studio cheesily created 8-bit game end sound
// I chose this feature because not only does it give the game more life and fun, but it also helps me practise more on my audio skills
// I also used the Color ColorFromHSV(float hue, float saturation, float value); with GetTime() from the Raylib site to create an animated multi-coloured ball 
// This and the paddle colour changes give this game a whole new fresh look


