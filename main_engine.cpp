#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

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

    vector<vector<int>> levelData;

    Mix_Music* backgroundMusic;
    bool musicPlaying;

    bool showPlayButton;
    bool enterPressed;
    bool gameStarted;

    void LoadLevelConfiguration(const string& configFile);
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
      isPaused(false) {}

GameEngine::~GameEngine() {
    Shutdown();
}

void GameEngine::Initialize(const char* title, int width, int height) {
    cout << "Init";
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL initialization error: " << SDL_GetError() << endl;
        return;
    }

    window = SDL_CreateWindow(title, 50, 50, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Window creation error: " << SDL_GetError() << endl;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Renderer creation error: " << SDL_GetError() << endl;
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "SDL_mixer initialization error: " << Mix_GetError() << endl;
        return;
    }

    if (TTF_Init() < 0) {
        cerr << "SDL_ttf initialization error: " << TTF_GetError() << endl;
        return;
    }

    LoadLevelConfiguration("level_config.txt");

    backgroundMusic = Mix_LoadMUS("bgmusic.mp3");
    if (!backgroundMusic) {
        cerr << "Failed to load music: " << Mix_GetError() << endl;
    }

    isRunning = true;
}

void GameEngine::Run() {
    cout << "Run";
    while (isRunning) {
        handleInput();
        if (!showPlayButton) {
            Update();
        }
        RenderScene();
    }
}

void GameEngine::Shutdown() {
    cout << "Shutdown";
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

    if (isJumping) velocityY += 1;
    py.y += velocityY;

    if (velocityY > 0 && (checkCollision(1) == 1 || checkCollision(6) == 1)) {
        py.y = (py.y / TILE_SIZE) * TILE_SIZE;
        velocityY = 0;
        isJumping = false;
    }

    if (jump && !isJumping) {
        isJumping = true;
        velocityY -= py.JUMP_VELOCITY;

        if (!musicPlaying) {
            Mix_PlayMusic(backgroundMusic, -1);
            musicPlaying = true;
        }
    }

    if (velocityY < 0 && checkCollision() == 1) {
        py.y = ((py.y + TILE_SIZE) / TILE_SIZE) * TILE_SIZE;
        velocityY = 0;
    }
    if (velocityY == 0 && checkCollision() != 1) {
        isJumping = true;
    }

    if (py.y + TILE_SIZE > SCREEN_HEIGHT) {
        Mix_HaltMusic();
        backgroundMusic = Mix_LoadMUS("bgmusic.mp3");
        if (!backgroundMusic) {
            cerr << "Failed to load another music: " << Mix_GetError() << endl;
        }
        Mix_PlayMusic(backgroundMusic, -1);
    }
}

void GameEngine::LoadLevelConfiguration(const string& configFile) {
    ifstream inFile(configFile);
    if (!inFile.is_open()) {
        cerr << "Error: Could not open file for reading." << endl;
        return;
    }

    levelData.clear();
    int tileType;
    string line;
    while (getline(inFile, line, '\n')) {
        vector<int> tileRow;
        istringstream ss(line);
        while (ss >> tileType) {
            tileRow.push_back(tileType);
        }
        levelData.push_back(tileRow);
    }

    inFile.close();

    for (const auto& row : levelData) {
        for (const auto& tile : row) {
            cout << tile << " ";
        }
        cout << endl;
    }
}

void GameEngine::RenderPauseMenu() {
    SDL_Rect menuRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderFillRect(renderer, &menuRect);

    TTF_Font* resumeFont = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
    if (!resumeFont) {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
    } else {
        SDL_Color resumeTextColor = {255, 255, 255, 255};
        SDL_Surface* resumeTextSurface = TTF_RenderText_Solid(resumeFont, "Resume", resumeTextColor);
        if (resumeTextSurface) {
            SDL_Texture* resumeTextTexture = SDL_CreateTextureFromSurface(renderer, resumeTextSurface);
            if (resumeTextTexture) {
                SDL_Rect resumeTextRect = {SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 - 15, 60, 30};
                SDL_RenderCopy(renderer, resumeTextTexture, nullptr, &resumeTextRect);

                SDL_DestroyTexture(resumeTextTexture);
            } else {
                cerr << "Failed to create texture from surface: " << SDL_GetError() << endl;
            }

            SDL_FreeSurface(resumeTextSurface);
        } else {
            cerr << "Failed to render text surface: " << TTF_GetError() << endl;
        }

        TTF_CloseFont(resumeFont);
    }

    TTF_Font* startFont = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
    if (!startFont) {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
    } else {
        SDL_Color startTextColor = {255, 255, 255, 255};
        SDL_Surface* startTextSurface = TTF_RenderText_Solid(startFont, "Start New Game (S)", startTextColor);
        if (startTextSurface) {
            SDL_Texture* startTextTexture = SDL_CreateTextureFromSurface(renderer, startTextSurface);
            if (startTextTexture) {
                SDL_Rect startTextRect = {SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2 + 45, 180, 30};
                SDL_RenderCopy(renderer, startTextTexture, nullptr, &startTextRect);

                SDL_DestroyTexture(startTextTexture);
            } else {
                cerr << "Failed to create texture from surface: " << SDL_GetError() << endl;
            }

            SDL_FreeSurface(startTextSurface);
        } else {
            cerr << "Failed to render text surface: " << TTF_GetError() << endl;
        }

        TTF_CloseFont(startFont);
    }

    TTF_Font* exitFont = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
    if (!exitFont) {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
    } else {
        SDL_Color exitTextColor = {255, 255, 255, 255};
        SDL_Surface* exitTextSurface = TTF_RenderText_Solid(exitFont, "Exit (E)", exitTextColor);
        if (exitTextSurface) {
            SDL_Texture* exitTextTexture = SDL_CreateTextureFromSurface(renderer, exitTextSurface);
            if (exitTextTexture) {
                SDL_Rect exitTextRect = {SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 105, 120, 30};
                SDL_RenderCopy(renderer, exitTextTexture, nullptr, &exitTextRect);

                SDL_DestroyTexture(exitTextTexture);
            } else {
                cerr << "Failed to create texture from surface: " << SDL_GetError() << endl;
            }

            SDL_FreeSurface(exitTextSurface);
        } else {
            cerr << "Failed to render text surface: " << TTF_GetError() << endl;
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
                    right = false;
                    break;
                case SDLK_RIGHT:
                    right = true;
                    left = false;
                    break;
                case SDLK_s:
                    if (isPaused && showPlayButton && !gameStarted) {
                        py.x = SCREEN_WIDTH / 2;
                        py.y = SCREEN_HEIGHT / 2;
                    } else {
                        if (isPaused && !gameStarted) {
                            showPlayButton = false;
                            gameStarted = true;
                        }
                    }
                    break;

                case SDLK_e:
                    if (isPaused && !gameStarted) {
                        isRunning = false;
                    } else if (gameStarted) {
                        isRunning = false;
                        SDL_DestroyWindow(window);
                        SDL_Quit();
                        return;
                    }
                    break;
                case SDLK_RETURN:
                    if (!isPaused) {
                        isPaused = true;
                    } else {
                        if (showPlayButton && !gameStarted) {
                            showPlayButton = false;
                            gameStarted = true;
                        }
                        isPaused = false;
                    }
                    break;
                case SDLK_ESCAPE:
                    if (!gameStarted) {
                        isRunning = false;
                    } else {
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


void GameEngine::RenderScene() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    if (showPlayButton && !gameStarted) {
        SDL_Rect playButtonRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 25, 100, 50};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &playButtonRect);

        TTF_Font* font = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
        if (!font) {
            cerr << "Failed to load font: " << TTF_GetError() << endl;
        } else {
            SDL_Color textColor = {255, 255, 255, 255};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Start", textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                if (textTexture) {
                    SDL_Rect textRect = {SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 - 15, 60, 30};
                    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

                    SDL_DestroyTexture(textTexture);
                } else {
                    cerr << "Failed to create texture from surface: " << SDL_GetError() << endl;
                }

                SDL_FreeSurface(textSurface);
            } else {
                cerr << "Failed to render text surface: " << TTF_GetError() << endl;
            }

            TTF_CloseFont(font);
        }

        SDL_Rect exitButtonRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 30, 100, 50};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &exitButtonRect);

        TTF_Font* exitFont = TTF_OpenFont("C:\\Users\\ASUS\\Downloads\\Press_Start_2P\\PressStart2P-Regular.ttf", 24);
        if (!exitFont) {
            cerr << "Failed to load font: " << TTF_GetError() << endl;
        } else {
            SDL_Color exitTextColor = {255, 255, 255, 255};
            SDL_Surface* exitTextSurface = TTF_RenderText_Solid(exitFont, "Exit", exitTextColor);
            if (exitTextSurface) {
                SDL_Texture* exitTextTexture = SDL_CreateTextureFromSurface(renderer, exitTextSurface);
                if (exitTextTexture) {
                    SDL_Rect exitTextRect = {SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 + 45, 60, 30};
                    SDL_RenderCopy(renderer, exitTextTexture, nullptr, &exitTextRect);

                    SDL_DestroyTexture(exitTextTexture);
                } else {
                    cerr << "Failed to create texture from surface: " << SDL_GetError() << endl;
                }

                SDL_FreeSurface(exitTextSurface);
            } else {
                cerr << "Failed to render text surface: " << TTF_GetError() << endl;
            }

            TTF_CloseFont(exitFont);
        }
    } else {
        for (size_t y = 0; y < levelData.size(); ++y) {
            for (size_t x = 0; x < levelData[y].size(); ++x) {
                SDL_Rect tileRect = {static_cast<int>(x * TILE_SIZE), static_cast<int>(y * TILE_SIZE), TILE_SIZE, TILE_SIZE};

                switch (levelData[y][x]) {
                    case 0:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderFillRect(renderer, &tileRect);
                        break;
                    case 1:
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        SDL_RenderFillRect(renderer, &tileRect);
                        break;
                    case 2:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
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

int main(int argc, char** argv) {
    GameEngine game;
    game.Initialize("Game Engine", SCREEN_WIDTH, SCREEN_HEIGHT);
    game.Run();
    game.Shutdown();
    return 0;
}

