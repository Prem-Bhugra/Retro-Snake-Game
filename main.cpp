#include <iostream>
#include <raylib.h>
#include <deque>
using namespace std;

int cellSize = 30;                  // Size of each cell int the grid (in pixels).
int cellCount = 25;                 // Number of cells in one row and one column. So, the dimensions of grid are 750 x 750.
int margin = 75;                    // Offset of border from window's edges.
double lastUpdate = 0;              // Keeps track of the time at which the last update of the snake's position occurred.
Color color1 = {173, 204, 96, 255}; // We have defined a custom color object. The arguments are {red,green,blue,alpha}. Alpha value represents the transparency.
Color color2 = {43, 51, 24, 255};
Sound eatSound; // Sound object in raylib.
Sound collisionSound;

bool pairInDeque(pair<int, int> p, deque<pair<int, int>> q) // Checks whether a pair p is present in a deque.
{
    for (unsigned long long int i = 0; i < q.size(); i++)
    {
        if (q[i] == p)
        {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval)
{
    double current = GetTime(); // Gives the time since the game window was created.
    if (current - lastUpdate >= interval)
    { // If the difference is greater than interval then it mean that the interval has passed.
        lastUpdate = current;
        return true;
    }
    return false;
}

class Food
{
public:
    pair<int, int> coordinates;
    Texture2D texture; // A data type which optimised to be rendered into the game graphics.
    Image img;         // Image object in raylib.
    Food(deque<pair<int, int>> snakeBody)
    {
        cout << "New Food Created" << endl;
        this->coordinates = generateRandomPosition(snakeBody); // The default position of the food is 6th column and 7th row.
        img = LoadImage("./Food.png");                         // LoadImage() loads image from file into CPU memory (RAM).
        texture = LoadTextureFromImage(img);                   // Loads texture from image data which can be rendered into graphics of the game.
        UnloadImage(img);                                      // Unloads image from RAM.
    }
    ~Food()
    {
        UnloadTexture(texture); // Unloads texture from graphics.
    }
    void Draw()
    {
        DrawTexture(texture, margin + coordinates.first * cellSize, margin + coordinates.second * cellSize, WHITE); // Draws a Texture2D. Color tint st to white means that no color filtering is applied.
    }
    pair<int, int> generateRandomPosition(deque<pair<int, int>> snakeBody) // Generates a new position for the food which is outside the snake's body.
    {
        int x = GetRandomValue(0, cellCount - 1), y = GetRandomValue(0, cellCount - 1);
        while (pairInDeque(make_pair(x, y), snakeBody))
        {
            x = GetRandomValue(0, cellCount - 1);
            y = GetRandomValue(0, cellCount - 1);
        }
        return {x, y};
    }
};

class Snake
{
public:
    deque<pair<int, int>> body;
    pair<int, int> direction = {1, 0};
    bool addBody;
    Snake()
    {
        body.push_back({6, 9});
        body.push_back({5, 9});
        body.push_back({4, 9});
        addBody = false;
    }
    void Draw()
    {
        for (auto i : body)
        {
            Rectangle rect = {(float)(margin + i.first * cellSize), (float)(margin + i.second * cellSize), (float)cellSize, (float)cellSize}; // Rectangle object in raylib.
            DrawRectangleRounded(rect, 0.5, 10, color2);                                                                                      // Draws a rectangle with rounded edges.
        }
    }
    void Update()
    {
        body.push_front({body[0].first + this->direction.first, body[0].second + this->direction.second});
        if (addBody)
        { // If snakes size increases because of eating food then it will not move because snake's movement and snake's increase in size happening together will lead to a jumpy movement of the snake. So, whenever the snake's head reaches food, the addBody attribute is set to true and the Update() method leads to increase in size rather than movement.
            addBody = false;
        }
        else
        {
            body.pop_back();
        }
    }
};

class Game
{
public:
    Snake naag;
    Food bhojan;
    bool pause, allow;
    Game() : bhojan(naag.body)
    {
        pause = true;
        allow = true;
        InitAudioDevice();                  // Initializes the audio device.
        eatSound = LoadSound("./Food.mp3"); // Loads the sound from a file into the memory.
        collisionSound = LoadSound("./Collision.mp3");
    }
    void Draw()
    {
        bhojan.Draw();
        naag.Draw();
    }
    void Update()
    {
        allow = true;
        if (!pause)
        {
            naag.Update();
            checkFood();
            checkEdges();
            checkBody();
        }
    }
    void checkFood() // Checks if the snake's head has reached the food or not.
    {
        if (naag.body[0] == bhojan.coordinates)
        {
            cout << "Food found" << endl;
            PlaySound(eatSound);                                           // Plays a sound.
            bhojan.coordinates = bhojan.generateRandomPosition(naag.body); // Foods position is updated when snakes head reaches the food.
            naag.addBody = true;
        }
    }
    void checkEdges()
    {
        if (naag.body[0].first == -1 || naag.body[0].first == cellCount || naag.body[0].second == -1 || naag.body[0].second == cellCount)
        { // Grid is from 0 to cellCount-1 (both inclusive) in x direction.
            gameOver(false);
        }
    }
    void checkBody()
    {
        deque<pair<int, int>> temp = naag.body;
        temp.pop_front();
        if (pairInDeque(naag.body[0], temp))
        {
            gameOver(true);
        }
    }
    void gameOver(bool collision_with_self) // When the game ends due to collision of snake, the snake's cooridnates and direction are reset to the initial value and the food is given a new position.
    {
        if (collision_with_self)
        {
            cout << "Game over due to snake's collision with itself." << endl;
        }
        else
        {
            cout << "Game over due to snake's collision with walls." << endl;
        }
        PlaySound(collisionSound);
        naag.body = {{6, 9}, {5, 9}, {4, 9}};
        naag.direction = {1, 0};
        bhojan.coordinates = bhojan.generateRandomPosition(naag.body);
        pause = true; // Pauses the game before the user presses any key.
    }
    ~Game()
    {
        UnloadSound(eatSound); // Unloads the sound from the memory.
        UnloadSound(collisionSound);
        CloseAudioDevice(); // Closes the audio device.
    }
};

int main()
{
    cout << "Starting the game..." << endl;

    InitWindow(2 * margin + cellSize * cellCount, 2 * margin + cellSize * cellCount, "🐍 Retro Snake Game"); // Creates the game window with arguments as width, heught and title of the window respectively.
    SetTargetFPS(60);                                                                                        // Determines the frame rate of the game so that the game runs at the same speed on every computer. If it is not specified then the game runs as fast as the respective computer can run it. 60 FPS means that we want to update the game window 60 times per second.

    Game khel;
    // Game Loop
    while (!WindowShouldClose())
    { // If the escape key is pressed or if the cross button on the top right corner of the game window is pressed then this WindowShouldClose() function returns true. So, the game loop runs until the mentioned key and button are pressed.

        // Event Handling: Events like keypressing and quitting the game are detected.
        if (IsKeyPressed(KEY_UP) && khel.naag.direction != make_pair(0, 1) && khel.allow)
        {
            khel.naag.direction = {0, -1};
            khel.pause = false;
            khel.allow = false;
        }
        else if (IsKeyPressed(KEY_DOWN) && khel.naag.direction != make_pair(0, -1) && khel.allow)
        {
            khel.naag.direction = {0, 1};
            khel.pause = false;
            khel.allow = false;
        }
        else if (IsKeyPressed(KEY_RIGHT) && khel.naag.direction != make_pair(-1, 0) && khel.allow)
        {
            khel.naag.direction = {1, 0};
            khel.pause = false;
            khel.allow = false;
        }
        else if (IsKeyPressed(KEY_LEFT) && khel.naag.direction != make_pair(1, 0) && khel.allow)
        {
            khel.naag.direction = {-1, 0};
            khel.pause = false;
            khel.allow = false;
        }

        // Updating Positions
        if (eventTriggered(0.2)) // We need to update the snake's position after every 0.2 seconds.
        {
            khel.Update();
        }

        // Drawing: Drawing the object to appear at the next position in the game.
        BeginDrawing();                                                                                                                                               // Creates a blank canvas on which we can draw out game objects.
        ClearBackground(color1);                                                                                                                                      // Fills the game window with background with color1 before drawing the new object at the new position.
        DrawRectangleLinesEx(Rectangle{(float)(margin - 5), (float)(margin - 5), (float)(cellSize * cellCount + 10), (float)(cellSize * cellCount + 10)}, 5, color2); // Draws the outline of a rectangle.
        DrawText("RETRO SNAKE", margin - 5, 20, 40, color2);                                                                                                          // Draws text charactersusing default font of raylib.
        DrawText(TextFormat("SCORE: %i", khel.naag.body.size() - 3), margin - 5, margin + cellSize * cellCount + 15, 40, color2);                                     // TextDraw() method formats the given variables into text.                                                                                       // Draws text charactersusing default font of raylib.
        khel.Draw();
        EndDrawing(); // Ends the canvas drawing.
    }

    cout << "Game ended" << endl;
    CloseWindow(); // Closes the game window before the game is closed.
}