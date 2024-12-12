#pragma once
#include <fstream>
#include "grid.h"
#include <iostream>
#include <iterator>
#include <format>

using namespace std;

class Vector2
{
public:
    Vector2 (const int x, const int y) : m_x(x), m_y(y) {}

    friend istream& operator >> (istream& in, Vector2& obj);
    friend ostream& operator << (ostream& out, const Vector2& obj);
    int get_x() const { return m_x; }
    int get_y() const { return m_y; }

private:
    int m_x;
    int m_y;
};

inline istream& operator >> (istream& in, Vector2& obj)
{
    in >> obj.m_x;
    in >> obj.m_y;
    return in;
}

inline ostream& operator << (ostream& out, const Vector2& obj)
{
    out << obj.m_x << ' ';
    out << obj.m_y << endl;
    return out;
}

class Engine{
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Grid* grid{};
    std::vector<std::vector<Vector2>> paths;
    int chosen_path;
    void Initialize();

    bool Update();
    void Render() const;
public:
    Engine();
    ~Engine();
    void Run();
};

inline Engine::Engine() {
    SDL_Init(SDL_INIT_VIDEO);   //Inicializa a biblioteca SDL
    window = SDL_CreateWindow("Blank Window", SDL_WINDOWPOS_CENTERED,   //Cria uma janela
        SDL_WINDOWPOS_CENTERED, SCREEN_SIZE, SCREEN_SIZE, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        exit(1);
    }

    std::cout << "Window created" << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);    //Cri um renderizador

    if (renderer == nullptr) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    std::cout << "Renderer created" << std::endl;
}

inline Engine::~Engine() {  //Destruidor para encerrar o SDL, a janela e o renderizador
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

inline void Engine::Initialize() {  //Cria o grid
    paths = std::vector<std::vector<Vector2>>(3);
    chosen_path = 0;
    grid = new Grid();
    std::cout << "Grid created" << std::endl;
}

inline bool Engine::Update() {
    SDL_Event event;
    bool quit = false;

    while (SDL_PollEvent(&event)) { //Resolve os eventos
        switch (event.type) {
            case SDL_QUIT:  //Evento acionado quando o programa é encerrado
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:   //Evento acionado quando o botão do mouse é pressionado
                if (event.button.button == SDL_BUTTON_LEFT) {   //Verifica se o botão pressionado é o esquerdo
                    //Converte as cordenadas do mouse para as do grid
                    const int x = std::floor(event.button.x / (TILE_SIZE + TILE_OFFSET));
                    const int y = std::floor(event.button.y / (TILE_SIZE + TILE_OFFSET));

                    Vector2 pos = {x, y};
                    paths.at(chosen_path).emplace_back(pos);    //Coloca a posição do mouse dentro do vetor de caminhos
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    if (paths.at(chosen_path).size() > 1 && paths.at(chosen_path).size() <= 2) {
                        paths.at(chosen_path).pop_back();
                        paths.at(chosen_path).pop_back();
                    }
                    else if (paths.at(chosen_path).size() > 2) {
                        paths.at(chosen_path).pop_back();
                    }
                }
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_1:
                        chosen_path = 0;
                        break;
                    case SDL_SCANCODE_2:
                        chosen_path = 1;
                        break;
                    case SDL_SCANCODE_3:
                        chosen_path = 2;
                        break;
                    case SDL_SCANCODE_4: {
                        Vector2 a(0, 0);
                        for (int i = 0; i < 3; ++i) {
                            ofstream outFile (format("path_{}.txt", i));
                            outFile << paths.at(i).size() << endl;
                            for (const auto j : paths.at(i)) {
                                a = j;
                                outFile << a;
                            }
                            outFile.close();
                        }
                    }
                        break;
                    case SDL_SCANCODE_5: {
                        Vector2 b(0, 0);
                        for (int i = 0; i < 3; ++i) {
                            ifstream inFile (format("path_{}.txt", i));
                            paths.at(i).clear();
                            int size = 0;
                            inFile >> size;
                            for (int j = 0; j < size; ++j) {
                                inFile >> b;
                                paths.at(i).push_back(b);
                            }

                            inFile.close();
                        }
                    }
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }

    return quit;
}

inline void Engine::Render() const {
    //Define a cor que será desenhada pelo render para preto
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    //Limpa o render
    SDL_RenderClear(renderer);

    //Renderiza todos os Tiles
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        const int x = i % static_cast<int>(GRID_SIZE);
        const int y = std::floor(i / GRID_SIZE);

        const Tile* tile = grid->GetTileAt(x, y);
        if (tile != nullptr) { tile->Render(renderer); }
    }

    for(int i = 0; i < paths.size(); i++) {
        switch (i) {
            case 0:
                SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
            break;
            case 1:
                SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
            break;
            case 2:
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
            break;
            default:
                break;
        }

        if(paths.at(i).size() > 1) {
            for (int j = 0; j < paths.at(i).size()-1; j++) {
                //Pega um duas cordenadas na lista
                Tile *start = grid->GetTileAt(paths.at(i).at(j).get_x(), paths.at(i).at(j).get_y());
                Tile *end = grid->GetTileAt(paths.at(i).at(j+1).get_x(), paths.at(i).at(j+1).get_y());

                //Cria o caminho entre as duas
                const Points points = grid->CreatePath(start, end);
                SDL_Point point_arr[points.count];

                //Cria um array com os pontos
                for (int k = 0; k < points.count; k++)
                    point_arr[k] = points.point_arr[k];

                //Desenha as linhas
                SDL_RenderDrawLines(renderer, point_arr, points.count);
            }
        }
    }


    //Renderiza oque está no render
    SDL_RenderPresent(renderer);
}

inline void Engine::Run() {
    bool quit = false;

    Initialize();

    while(!quit) {
        quit = Update();
        Render();
    }
}