#ifndef TETRIS_H
#define TETRIS_H

#include <vector>
#include <algorithm>
#include <string>
#include <climits>
#include <iostream>

#include "point.h"
#include "wallkicks.h"

class TetrisGame {
    public:
    
    // CONSTANTS
    
    const TetrisPiece PIECE_I = TetrisPiece({-1, 1}, {0, 1}, {1, 1}, {2, 1}, {-1, -1}, {2, 2});
    const TetrisPiece PIECE_O = TetrisPiece({0, 0}, {0, 1}, {1, 0}, {1, 1}, {0, 0}, {1, 1});
    const TetrisPiece PIECE_L = TetrisPiece({-1, 0}, {0, 0}, {1, 0}, {1, 1}, {-1, -1}, {1, 1});
    const TetrisPiece PIECE_J = TetrisPiece({-1, 0}, {0, 0}, {1, 0}, {-1, 1}, {-1, -1}, {1, 1});
    const TetrisPiece PIECE_S = TetrisPiece({-1, 0}, {0, 0}, {0, 1}, {1, 1}, {-1, -1}, {1, 1});
    const TetrisPiece PIECE_T = TetrisPiece({-1, 0}, {0, 0}, {1, 0}, {0, 1}, {-1, -1}, {1, 1});
    const TetrisPiece PIECE_Z = TetrisPiece({1, 0}, {0, 0}, {0, 1}, {-1, 1}, {-1, -1}, {1, 1});
    
    std::vector<TetrisPiece> PIECES = std::vector<TetrisPiece>({PIECE_I, PIECE_J, PIECE_L, PIECE_O, PIECE_S, PIECE_T, PIECE_Z});
    
    const char SYMBOLS[7] = {'@', '#', '%', '0', '#', 'X', '+'};
    
    // VALUES
    
    int width, height;
    std::vector<std::vector<int>> grid;
    
    TetrisPiece activePiece;
    int activeRotation;
    int activePieceID;
    std::vector<TetrisPiece> bag;
    
    // Scores and Flags
    
    bool endgame = false;
    
    int clears = 0;
    
    TetrisGame() {
        clears = 0;
        endgame = false;
        activePieceID = -1;
        activePiece = TetrisPiece();
        width = 10;
        height = 20;
        grid = std::vector<std::vector<int>>(width, std::vector<int>(height, -1));
        FillBag();
    }
    
    TetrisGame(const TetrisGame& g) {
        clears = 0;
        endgame = false;
        activePieceID = -1;
        width = g.width;
        height = g.height;
        grid = std::vector<std::vector<int>>(width, std::vector<int>(height, -1));
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) grid[i][j] = g.grid[i][j];
        }
        
        bag = std::vector<TetrisPiece>();
        for (auto p : g.bag) bag.push_back(TetrisPiece(p));
        
        activePiece = TetrisPiece(g.activePiece);
        
        activeRotation = g.activeRotation;
        activePieceID = g.activePieceID;
    }
    
    void FillBag() {
        bag.clear();
        for (int i = 0; i < 7; i++) bag.push_back(TetrisPiece(PIECES[i]));
        std::random_shuffle(bag.begin(), bag.end());
    }
    
    void reset() {
        grid = std::vector<std::vector<int>>(width, std::vector<int>(height, -1));
        FillBag();
        endgame = false;
        clears = 0;
    }
    
    void spawnPiece() {
        activePiece = bag[bag.size() - 1];
        bag.pop_back();
        if (bag.size() <= 0) FillBag();
        
        activePieceID = -1;
        activeRotation = 0;
        
        for (int i = 0; i < 7; i++) {
            if (activePiece == PIECES[i]) activePieceID = i;
        }
        
        activePiece.translate(4, 20);
    }
    
    bool inBounds(Point p) {
        if (p.x < 0 || p.y < 0) return false;
        if (p.x >= width || p.y >= height) return false;
        return true;
    }
    
    bool overlaps(TetrisPiece piece) {
        for (auto i : piece.cells) {
            if (i.x >= 0 && i.x < width && i.y >= height) continue;
            if (!inBounds(i)) return true;
            if (grid[i.x][i.y] >= 0) return true;
        }
        return false;
    }
    
    bool rotatePieceClockwise() { // oh boy here we go
        int startingrotation = activeRotation;
        
        if (activePieceID == 0) {
            for (int i = 0; i < 5; i++) {
                TetrisPiece test = TetrisPiece(activePiece);
                test.rotate(1);
                test.translate(KICKS_I[startingrotation][i]);
                if (!overlaps(test)) {
                    activePiece = test;
                    activeRotation = (activeRotation + 1) % 4;
                    return true;
                }
            }
        }
        else {
            for (int i = 0; i < 5; i++) {
                TetrisPiece test = TetrisPiece(activePiece);
                test.rotate(1);
                test.translate(KICKS_NONI[startingrotation][i]);
                if (!overlaps(test)) {
                    activePiece = test;
                    activeRotation = (activeRotation + 1) % 4;
                    return true;
                }
            }
        }
        
        return false;
    }
    
    bool movePieceDown() {
        TetrisPiece test = TetrisPiece(activePiece);
        test.translate({0, -1});
        
        if (!overlaps(test)) activePiece.translate({0, -1});
        else {
            for (auto i : activePiece.cells) {
                if (!inBounds({i.x, i.y})) {
                    endgame = true;
                    return false;
                }
                grid[i.x][i.y] = activePieceID;
            }
            return false;
        }
        return true;
    }
    
    bool movePiece(Point p) {
        TetrisPiece test = TetrisPiece(activePiece);
        test.translate(p);
        
        if (!overlaps(test)) activePiece.translate(p);
        return true;
    }
    
    int clearLines() {
        int linesCleared = 0;
        for (int y = 0; y < height; y++) {
            int count = 0;
            for (int x = 0; x < width; x++) {
                if (grid[x][y] >= 0) count++;
            }
            if (count == width) {
                linesCleared++;
                for (int x = 0; x < width; x++) grid[x][y] = -1;
                for (int h = y; h < height - 1; h++) {
                    for (int x = 0; x < width; x++) grid[x][h] = grid[x][h + 1];
                }
                for (int x = 0; x < width; x++) grid[x][height - 1] = -1;
            }
        }
        clears += linesCleared;
        return linesCleared;
    }
    
    void disp() {
        for (int y = height - 1; y >= 0; y--) {
            for (int x = 0; x < width; x++) {
                if (!activePiece.contains({x, y})) {
                    if (grid[x][y] >= 0) std::cout << SYMBOLS[grid[x][y]] << " ";
                    else std::cout << ". ";
                }
                else if (activePieceID >= 0) std::cout << SYMBOLS[activePieceID] << " ";
                else std::cout << ". ";
            }
            std::cout << "\n";
        }
    }
};

#endif