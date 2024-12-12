#pragma once
#include <vector>
#include "tile.h"
#include <iostream>
#include <cmath>
#include <list>
#include <algorithm>

inline bool tile_exist(const int x, const int y) {
    return x >= 0 && y >= 0 && x < GRID_SIZE && y < GRID_SIZE;
}

struct {
    SDL_Point *point_arr;
    int count;
} typedef Points;

class Grid {
private:
    std::vector<Tile*> *tiles;
public:
    Grid();
    ~Grid();
    Tile* GetTileAt(int x, int y) const;
    Points CreatePath(Tile *start, Tile *end) const;
};

inline Grid::Grid() {
    //Cria o vetor de tiles
    tiles = new std::vector<Tile*>(GRID_SIZE*GRID_SIZE);

    //Cria cada tile dentro do vetor
    for (int i = 0; i < tiles->size(); i++) {
        const int x = i % static_cast<int>(GRID_SIZE);
        const int y = std::floor(i / static_cast<double>(GRID_SIZE));

        tiles->at(i) = new Tile{x, y};
    }

    //Define os vizinhos de cada tile
    for (int i = 0; i < tiles->size(); i++) {
        const int x = i % static_cast<int>(GRID_SIZE);
        const int y = std::floor(i / GRID_SIZE);

        for (int j = 0; j < 4; j++) {
            const int a = j >= 2 ? 1 : 0;
            const int b = j < 2 ? 1 : 0;

            const int n_x = j % 2 == 0 ? x + 1 * a : x - 1 * a;
            const int n_y = j % 2 == 1 ? y + 1 * b : y - 1 * b;

            if(tile_exist(n_x, n_y)) {
                tiles->at(i)->addNeighbour(tiles->at(n_y * GRID_SIZE + n_x));
            }
        }
    }
}

//Retorna um tile baseado em sua coordenada dentro do vetor
inline Tile *Grid::GetTileAt(const int x, const int y) const {
    if (tile_exist(x, y)) return tiles->at(y * GRID_SIZE + x);
    return nullptr;
}

//Cria um caminho entre dois tiles
//O algoritmo descobre os vizinhos ao redor do Tile inicial,
//Verifica o de menor distancia entre o inicial e o final,
//E repete este processo até chagar no fim
//Então utiliza a variavel origin do tile para descobrir o caminho
inline Points Grid::CreatePath(Tile* start, Tile* end) const {
    std::vector<Tile*> tiles;
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        tiles.push_back(this->tiles->at(i));
    }

    tiles.erase(tiles.begin() + (start->getY() * GRID_SIZE + start->getX()));
    auto seen_tiles = std::list<Tile*>();
    seen_tiles.emplace_back(start);

    int _dist;

    while (seen_tiles.back() != end) {
        auto seen_neighbours = std::list<Tile*>();
        for (const auto seen_tile : seen_tiles) {
            for (const auto neighbour : seen_tile->getNeighbours()) {
                if(!std::any_of(seen_tiles.begin(), seen_tiles.end(), [neighbour]
                (const Tile* other) {return neighbour == other;})) {
                    neighbour->setOrigin(seen_tile);
                    seen_neighbours.emplace_back(neighbour);
                }
            }
        }

        Tile* next = nullptr;
        int* dist = nullptr;
        int i = 0;
        int j = i;
        for (const auto neighbour : seen_neighbours) {
            const int n_dist = neighbour->CalculateDistance(start, end);
            if (dist == nullptr || n_dist < *dist) {
                dist = &_dist;
                *dist = n_dist;
                next = neighbour;
                j = i;
            }
            i++;
        }
        _dist = 0;
        seen_tiles.emplace_back(next);
        tiles.erase(tiles.begin()+j);

        seen_neighbours = std::list<Tile*>();
    }

    std::vector<Tile*> path;
    auto tile = end;
    path.emplace_back(end);

    while (tile != start ) {
        path.emplace_back(tile->getOrigin());
        tile = tile->getOrigin();
    }

    SDL_Point points[path.size()];
    for (int i = static_cast<int>(path.size()) - 1; i >= 0; i--) {
        auto *point = new SDL_Point();
        point->x = path.at(i)->getX() * (TILE_SIZE + TILE_OFFSET) + TILE_OFFSET + (TILE_SIZE + TILE_OFFSET) / 2;
        point->y = path.at(i)->getY() * (TILE_SIZE + TILE_OFFSET) + TILE_OFFSET + (TILE_SIZE + TILE_OFFSET) / 2;
        points[i] = *point;
    }

    return {points, static_cast<int>(path.size())};
}
