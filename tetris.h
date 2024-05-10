#ifndef TETRIS_H
#define TETRIS_H

#include <set>
#include <map>
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
    
    TetrisPiece nextPiece;
    TetrisPiece activePiece;
    int activeRotation;
    int activePieceID;
    std::vector<TetrisPiece> bag;
    
    bool endgame = false; // Have we ended?
    
    TetrisGame() {
        width = 10;
        height = 20;
        grid = std::vector<std::vector<int>>(width, std::vector<int>(height, -1));
        FillBag();
    }
    
    void reset() { // Reset game
        grid = std::vector<std::vector<int>>(width, std::vector<int>(height, -1));
        FillBag();
        endgame = false;
    }
    
    void FillBag() { // Fill the bag of pieces so we know what pieces are going to be used.
        bag.clear();
        for (int i = 0; i < 7; i++) bag.push_back(TetrisPiece(PIECES[i]));
        std::random_shuffle(bag.begin(), bag.end());
    }
    
    void spawnPiece() { // Spawn in a piece. The next piece is always available as bag[bag.size() - 1].
        activePiece = bag[bag.size() - 1];
        bag.pop_back();
        if (bag.size() <= 0) FillBag(); // Refills the bag so there is a preview for the next piece (be aware the bag is temporarily at size 0 during this process).
        
        activePieceID = -1;
        activeRotation = 0;
        
        for (int i = 0; i < 7; i++) { // get the id
            if (activePiece == PIECES[i]) activePieceID = i;
        }
        
        activePiece.translate(4, 20);
    }
    
    bool inBounds(Point p) { // Is the point in the grid?
        if (p.x < 0 || p.y < 0) return false;
        if (p.x >= width || p.y >= height) return false;
        return true;
    }
    
    bool overlaps(TetrisPiece piece) { // Is the given piece entirely within or above the grid? 
        for (auto i : piece.cells) {
            if (i.x >= 0 && i.x < width && i.y >= height) continue;
            if (!inBounds(i)) return true;
            if (grid[i.x][i.y] >= 0) return true;
        }
        return false;
    }
    
    bool rotatePieceClockwise() { // oh boy here we go (Rotates a piece clockwise and does wall kicks)
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
    
    bool movePieceDown() { // Moves a piece down and binds it to the grid if necessary. Returns true upon descent and false upon binding.
        TetrisPiece test = TetrisPiece(activePiece);
        test.translate({0, -1});
        
        if (!overlaps(test)) activePiece.translate({0, -1});
        else {
            for (auto i : activePiece.cells) {
                if (!inBounds({i.x, i.y})) {
                    endgame = true; // If the game ended we set this to true.
                    return false;
                }
                grid[i.x][i.y] = activePieceID;
            }
            return false;
        }
        return true;
    }
    
    bool movePiece(Point p) { // Move a piece in the vector (p).
        TetrisPiece test = TetrisPiece(activePiece);
        test.translate(p);
        
        if (!overlaps(test)) activePiece.translate(p);
        return true;
    }
    
    int clearLines() { // Clear all clearable lines and apply gravity.
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
        return linesCleared;
    }
    
    void disp() { // Simple display of the grid. You can use the backend to make a GUI.
        for (int y = 19; y >= 0; y--) {
            for (int x = 0; x < 10; x++) {
                if (!activePiece.contains({x, y})) {
                    if (grid[x][y] >= 0) std::cout << SYMBOLS[grid[x][y]] << " ";
                    else std::cout << ". ";
                }
                else std::cout << SYMBOLS[activePieceID] << " ";
            }
            std::cout << "\n";
        }
    }
};

#endif
