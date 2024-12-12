#pragma once //para não incluir o arquivo multiplas vezes
#include <list>
#include <cmath>
#include <SDL.h>

#define GRID_SIZE 20.0
#define SCREEN_SIZE 600.0
#define TILE_OFFSET 0.5
#define TILE_SIZE ((SCREEN_SIZE - TILE_OFFSET - (TILE_OFFSET*GRID_SIZE)) / GRID_SIZE)

inline double calc_hypot(const double x, const double y) {
    return std::sqrt(x*x + y*y);
}

class Tile {
protected:
    int x, y;   //Posição do Tile no Grid
    Tile* origin{}; //Tile que originou este no caminho
    std::list<Tile*> neighbours;    //Vizinhos deste tile no grid
    SDL_FRect *rect;    //Retangulo que será desenhado pelo renderer
    Uint8 r, g, b, a;   //Cor do retangulo
public:
    Tile(int x, int y);
    ~Tile() {for (const auto tile : neighbours) delete tile;}
    int getX() const {return x; }
    int getY() const {return y; }
    Tile* getOrigin() const { return origin; }
    std::list<Tile*> getNeighbours() {return neighbours; }
    void addNeighbour(Tile* neighbour) { neighbours.push_back(neighbour); }
    void setOrigin(Tile* tile) { origin = tile; }
    int CalculateDistance(const Tile* start, const Tile* end) const;
    void Render(SDL_Renderer* renderer) const;
};

inline Tile::Tile(const int x, const int y) {
    //Define as coordenadas do Tile
    this->x = x;
    this->y = y;

    //Cria o retangulo do tile
    rect = new SDL_FRect;

    //Define a posição do retangulo
    rect->x = static_cast<float>(x * (TILE_SIZE + TILE_OFFSET) + TILE_OFFSET);
    rect->y = static_cast<float>(y * (TILE_SIZE + TILE_OFFSET) + TILE_OFFSET);

    //Define o tamanho do retangulo
    rect->w = rect->h = TILE_SIZE;

    //Define a cor do retangulo
    r = g = b = a = 0xFF;
}

//Calcula a distancia entre o Tile Start, Este tile e o Tile final
inline int Tile::CalculateDistance(const Tile* start, const Tile* end) const {
    double temp = calc_hypot(start->getX() - x, start->getY() - y) * 10.0;
    const int distance_from_start = static_cast<int>(std::round(temp));
    temp = calc_hypot(end->getX() - x, end->getY() - y) * 10.0;
    const int distance_from_end = static_cast<int>(std::round(temp));
    return std::abs(distance_from_start) + std::abs(distance_from_end);
}

//Define a cor do retangulo e o preenche
inline void Tile::Render(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRectF(renderer, rect);
}