#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 32;

class Player {
private:
    int x, y, SPEED, JUMP_VELOCITY;

public:
    Player() : x(SCREEN_WIDTH / 2), y(SCREEN_HEIGHT / 2), SPEED(3), JUMP_VELOCITY(15) {}
    Player(int X, int Y, int sp, int jv) : x(X), y(Y), SPEED(sp), JUMP_VELOCITY(jv) {}

    friend class GameEngine;
};

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    void Initialize(const char* title, int width, int height);
    void Run();
    void Shutdown();
    void Update();
    void RenderPauseMenu();
    int checkCollision(int choice = 0);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Player py;
    bool isRunning;
    bool left;
    bool right;
    bool jump;
    bool isJumping;
    int velocityY;
    bool isPaused;

    std::vector<std::vector<int>> levelData;

    // SDL_mixer variables
    Mix_Music* backgroundMusic;
    bool musicPlaying;

    // Start menu variables
    bool showPlayButton;
    bool enterPressed;
    bool gameStarted;

    void LoadLevelConfiguration(const std::string& configFile);
    void RenderScene();
    void handleInput();
};

GameEngine::GameEngine()
    : window(nullptr),
      renderer(nullptr),
      isRunning(false),
      left(false),
      right(false),
      jump(false),
      isJumping(false),
      velocityY(0),
      backgroundMusic(nullptr),
      musicPlaying(false),
      showPlayButton(true),
      enterPressed(false),
      gameStarted(false),
      isPaused(false) {} // Corrected here

GameEngine::~GameEngine() {
    Shutdown();
}

void GameEngine::Initialize(const char* title, int width, int height) {
    std::cout << "Init";
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL initialization error: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow(title, 50, 50, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation error: " << SDL_GetError() << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation error: " << SDL_GetError() << std::endl;
        return;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer initialization error: " << Mix_GetError() << std::endl;
        return;
    }

    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf initialization error: " << TTF_GetError() << std::endl;
        return;
    }

    LoadLevelConfiguration("level_config.txt");

    // Load music
    backgroundMusic = Mix_LoadMUS("bgmusic.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        // Handle loading error
    }

    isRunning = true;
}

void GameEngine::Run() {
    std::cout << "Run";
    while (isRunning) {
        handleInput();
        if (!showPlayButton) {
            Update();
        }
        RenderScene();
    }
}

void GameEngine::Shutdown() {
    std::cout << "Shutdown";
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

int GameEngine::checkCollision(int choice) {
    int X, Y;
    switch (choice) {
        case 0:
            X = py.x / TILE_SIZE;
            Y = py.y / TILE_SIZE;
            break;
        case 1:
            X = py.x / TILE_SIZE;
            Y = py.y / TILE_SIZE + 1;
            break;
        case 2:
            X = py.x / TILE_SIZE;
            Y = py.y / TILE_SIZE - 1;
            break;
        case 3:
            X = py.x / TILE_SIZE + 1;
            Y = py.y / TILE_SIZE;
            break;
        case 4:
            X = py.x / TILE_SIZE - 1;
            Y = py.y / TILE_SIZE;
            break;
        case 5:
            X = py.x / TILE_SIZE + 1;
            Y = py.y / TILE_SIZE - 1;
        default:
        case 6:
            X = py.x / TILE_SIZE + 1;
            Y = py.y / TILE_SIZE + 1;
            break;
    }
    if (X >= 0 && X < levelData[0].size() && Y >= 0 && Y < levelData.size()) {
        if (levelData[Y][X] == 1) {
            return 1;
        } else {
            return 0;
        }
    }
    return -1;
}

void GameEngine::Update() {
    int flag;
    if (left) {
        if (py.x / TILE_SIZE > 0.5) {
            py.x -= py.SPEED;
            flag = checkCollision();
            if (flag == 1) {
                py.x = ((py.x + TILE_SIZE) / TILE_SIZE) * TILE_SIZE;
            }
        }
    }
    if (right) {
        if (py.x / TILE_SIZE != levelData[0].size() - 1) {
            py.x += py.SPEED;
            if (checkCollision(3) == 1) {
                py.x = (py.x / TILE_SIZE) * TILE_SIZE;
            }
        }
    }

    // Gravity simulation
    if (isJumping) velocityY += 1;
    py.y += velocityY;

    // Check collision when falling down
    if (velocityY > 0 && (checkCollision(1) == 1 || checkCollision(6) == 1)) {
        // Adjust position and stop falling if there's a collision
        py.y = (py.y / TILE_SIZE) * TILE_SIZE;
        velocityY = 0;
        isJumping = false;
    }

    // Check collision when jumping
    if (jump && !isJumping) {
        isJumping = true;
        velocityY -= py.JUMP_VELOCITY;

        // Play background music when jumping
        if (!musicPlaying) {
            Mix_PlayMusic(backgroundMusic, -1);  // -1 for loop indefinitely
            musicPlaying = true;
        }
    }

    // Adjust position if there's a collision when jumping
    if (velocityY < 0 && checkCollision() == 1) {
        py.y = ((py.y + TILE_SIZE) / TILE_SIZE) * TILE_SIZE;
        velocityY = 0;
    }
    if (velocityY == 0 && checkCollision() != 1) {
        isJumping = true;
    }

    // Check if the player is out of bounds at the bottom
    if (py.y + TILE_SIZE > SCREEN_HEIGHT) {
        // Change the background music when out of bounds at the bottom
        Mix_HaltMusic();  // Stop the current music
        backgroundMusic = Mix_LoadMUS("bgmusic.mp3");
        if (!backgroundMusic) {
            std::cerr << "Failed to load another music: " << Mix_GetError() << std::endl;
            // Handle loading error
        }
        Mix_PlayMusic(backgroundMusic, -1);  // Play the new music
    }
}



void GameEngine::LoadLevelConfiguration(const std::string& configFile) {
    std::ifstream inFile(configFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open file for reading." << std::endl;
        return;
    }

    levelData.clear();  // Clear existing level data
    int tileType;
    std::string line;
    while (getline(inFile, line, '\n')) {
        std::vector<int> tileRow;
        std::istringstream ss(line);
        while (ss >> tileType) {
            tileRow.push_back(tileType);
        }
        levelData.push_back(tileRow);
    }

    inFile.close();

    // Print loaded level data for debugging
    for (const auto& row : levelData) {
        for (const auto& tile : row) {
            std::cout << tile << " ";
        }
        std::cout << std::endl;
    }
}

// ... (existing code)

// ... (existing code)

void GameEngine::RenderPauseMenu() {
    // Render pause menu in the center
    SDL_Rect menuRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);  // Gray color for the menu background
    SDL_RenderFillRect(renderer, &menuRect);

    // Render "Resume" text on the menu
    TTF_Font* resumeFont = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
    if (!resumeFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        // Handle loading error
    } else {
        SDL_Color resumeTextColor = {255, 255, 255, 255};  // White color
        SDL_Surface* resumeTextSurface = TTF_RenderText_Solid(resumeFont, "Resume", resumeTextColor);
        if (resumeTextSurface) {
            SDL_Texture* resumeTextTexture = SDL_CreateTextureFromSurface(renderer, resumeTextSurface);
            if (resumeTextTexture) {
                SDL_Rect resumeTextRect = {SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 - 15, 60, 30};
                SDL_RenderCopy(renderer, resumeTextTexture, nullptr, &resumeTextRect);

                SDL_DestroyTexture(resumeTextTexture);
            } else {
                std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
            }

            SDL_FreeSurface(resumeTextSurface);
        } else {
            std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
        }

        TTF_CloseFont(resumeFont);
    }

    // Render "Start" text on the menu
    TTF_Font* startFont = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
    if (!startFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        // Handle loading error
    } else {
        SDL_Color startTextColor = {255, 255, 255, 255};  // White color
        SDL_Surface* startTextSurface = TTF_RenderText_Solid(startFont, "Start New Game (S)", startTextColor);
        if (startTextSurface) {
            SDL_Texture* startTextTexture = SDL_CreateTextureFromSurface(renderer, startTextSurface);
            if (startTextTexture) {
                SDL_Rect startTextRect = {SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2 + 45, 180, 30};
                SDL_RenderCopy(renderer, startTextTexture, nullptr, &startTextRect);

                SDL_DestroyTexture(startTextTexture);
            } else {
                std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
            }

            SDL_FreeSurface(startTextSurface);
        } else {
            std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
        }

        TTF_CloseFont(startFont);
    }

    // Render "Exit" text on the menu
    TTF_Font* exitFont = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
    if (!exitFont) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        // Handle loading error
    } else {
        SDL_Color exitTextColor = {255, 255, 255, 255};  // White color
        SDL_Surface* exitTextSurface = TTF_RenderText_Solid(exitFont, "Exit (E)", exitTextColor);
        if (exitTextSurface) {
            SDL_Texture* exitTextTexture = SDL_CreateTextureFromSurface(renderer, exitTextSurface);
            if (exitTextTexture) {
                SDL_Rect exitTextRect = {SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 105, 120, 30};
                SDL_RenderCopy(renderer, exitTextTexture, nullptr, &exitTextRect);

                SDL_DestroyTexture(exitTextTexture);
            } else {
                std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
            }

            SDL_FreeSurface(exitTextSurface);
        } else {
            std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
        }

        TTF_CloseFont(exitFont);
    }
}

void GameEngine::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    jump = true;
                    break;
                case SDLK_LEFT:
                    left = true;
                    right = false; // Ensure that only one direction is active
                    break;
                case SDLK_RIGHT:
                    right = true;
                    left = false; // Ensure that only one direction is active
                    break;
                case SDLK_s:
                    // Start a new game when 'S' key is pressed
                    if (isPaused && showPlayButton && !gameStarted) {
                        showPlayButton = false;
                        gameStarted = true;  // Set gameStarted to true
                    }
                    break;
                case SDLK_e:
                    // Exit the application when 'E' key is pressed
                    if (isPaused && !gameStarted) {
                        isRunning = false;
                    }
                    break;
                case SDLK_RETURN:
                    if (!isPaused) {
                        // Resume the game when Enter key is pressed (only if not paused)
                        if (showPlayButton && !gameStarted) {
                            showPlayButton = false;
                            gameStarted = true;  // Set gameStarted to true
                        }
                    }
                    break;
                case SDLK_ESCAPE:
                    if (!gameStarted) {
                        // Exit the application when Esc key is pressed before starting
                        isRunning = false;
                    } else {
                        // Toggle pause when Esc key is pressed during the game
                        isPaused = !isPaused;
                    }
                    break;
                default:
                    break;
            }
        } else if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    jump = false;
                    break;
                case SDLK_RIGHT:
                    right = false;
                    break;
                case SDLK_LEFT:
                    left = false;
                    break;
                default:
                    break;
            }
        }
    }
}

// ... (existing code)


void GameEngine::RenderScene() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    if (showPlayButton && !gameStarted) {
        // Render play button in the center
        SDL_Rect playButtonRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 25, 100, 50};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green color for the play button
        SDL_RenderFillRect(renderer, &playButtonRect);

        // Render "Start" text on the play button
        TTF_Font* font = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            // Handle loading error
        } else {
            SDL_Color textColor = {255, 255, 255, 255};  // White color
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Start", textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                if (textTexture) {
                    SDL_Rect textRect = {SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 - 15, 60, 30};
                    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

                    SDL_DestroyTexture(textTexture);
                } else {
                    std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
                }

                SDL_FreeSurface(textSurface);
            } else {
                std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
            }

            TTF_CloseFont(font);
        }

        // Render exit button below the play button
        SDL_Rect exitButtonRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 30, 100, 50};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red color for the exit button
        SDL_RenderFillRect(renderer, &exitButtonRect);

        // Render "Exit" text on the exit button
        TTF_Font* exitFont = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
        if (!exitFont) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            // Handle loading error
        } else {
            SDL_Color exitTextColor = {255, 255, 255, 255};  // White color
            SDL_Surface* exitTextSurface = TTF_RenderText_Solid(exitFont, "Exit", exitTextColor);
            if (exitTextSurface) {
                SDL_Texture* exitTextTexture = SDL_CreateTextureFromSurface(renderer, exitTextSurface);
                if (exitTextTexture) {
                    SDL_Rect exitTextRect = {SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 + 45, 60, 30};
                    SDL_RenderCopy(renderer, exitTextTexture, nullptr, &exitTextRect);

                    SDL_DestroyTexture(exitTextTexture);
                } else {
                    std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
                }

                SDL_FreeSurface(exitTextSurface);
            } else {
                std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
            }

            TTF_CloseFont(exitFont);
        }
    } else {
                // Render the game scene as before
        for (size_t y = 0; y < levelData.size(); ++y) {
            for (size_t x = 0; x < levelData[y].size(); ++x) {
                SDL_Rect tileRect = {static_cast<int>(x * TILE_SIZE), static_cast<int>(y * TILE_SIZE), TILE_SIZE, TILE_SIZE};

                switch (levelData[y][x]) {
                    case 0:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White for empty space
                        SDL_RenderFillRect(renderer, &tileRect);
                        break;
                    case 1:
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red for solid tiles
                        SDL_RenderFillRect(renderer, &tileRect);
                        break;
                    case 2:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Blue for background tiles
                        SDL_RenderFillRect(renderer, &tileRect);
                        break;
                    default:
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_Rect PlayerRect = {py.x, py.y, TILE_SIZE, TILE_SIZE};
        SDL_RenderFillRect(renderer, &PlayerRect);
    }

    if (isPaused) {
        RenderPauseMenu();
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(8);
}

int main(int argc, char** argv) 
{
    GameEngine game;
    game.Initialize("Game Engine", SCREEN_WIDTH, SCREEN_HEIGHT);
    game.Run();
    game.Shutdown();
    return 0;
}