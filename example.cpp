#include "tetris.h"
#include <iostream>
#include <string>

int main() // Example code -- you move pieces manually
{
    
    srand(time(0));
    
    TetrisGame game;
    
    // for (auto i : game.bag) std::cout << i.toString() << "\n";
    
    char line;
    
    game.spawnPiece();
    
    // CONTROLS - S to move down, X to slam, E to rotate clockwise, A/D to move sideways.
    
    while (std::cin >> line) { // Each time you input the piece moves down and possibly in another direction
        if (line >= 'A' && line <= 'Z') line = line + 'a' - 'A';
        if (line == 'x') {
            while (game.movePieceDown());
            game.spawnPiece();
            std::cout << game.clearLines() << " LINES CLEARED\n";
        }
        else if (line == 's') {
            
        }
        else if (line == 'e') {
            game.rotatePieceClockwise();
        }
        else if (line == 'a') game.movePiece({-1, 0});
        else if (line == 'd') game.movePiece({1, 0});
        bool a = game.movePieceDown();
        if (!a) {
            game.spawnPiece();
            std::cout << game.clearLines() << " LINES CLEARED\n";
        }
        game.disp();
        
        if (game.endgame) break;
    }
    
    std::cout << "END\n";

    return 0;
}
