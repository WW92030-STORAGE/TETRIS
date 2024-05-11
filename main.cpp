#include "tetris.h"

#include <iostream>

void manual() {
    TetrisGame game;
    
    char line;
    
    game.spawnPiece();
    
    int counter = 0;
    
    while (std::cin >> line) {
        if (line == 'x') {
            while (game.movePieceDown());
            game.spawnPiece();
            std::cout << game.clearLines() << " LINES CLEARED\n";
            counter = 0;
        }
        else if (line == 's') {
            counter = 0; // move piece down
        }
        else if (line == 'e') {
            game.rotatePieceClockwise();
        }
        else if (line == 'a') game.movePiece({-1, 0});
        else if (line == 'd') game.movePiece({1, 0});
    
        if (counter == 0) {
            bool a = game.movePieceDown();
                if (!a) {
                game.spawnPiece();
                std::cout << game.clearLines() << " LINES CLEARED\n";
            }
        }
        game.disp();
        
        counter = (counter + 1) % 4;
        
        if (game.endgame) break;
    }
    
    std::cout << "END\n";
}

#include "genetic.h"

using namespace std;
#include <vector>
#include <algorithm>

int main()
{
    
    srand(time(0));
    
    // manual();

	// Train the model!

	TetrisAI ai;

	vector<TetrisAI> res;
	for (int i = 0; i < 64; i++) res.push_back(TetrisAI((0.5 - Genetics::rand01()) * 8, (0.5 - Genetics::rand01()) * 8, (0.5 - Genetics::rand01()) * 8, (0.5 - Genetics::rand01()) * 8, (0.5 - Genetics::rand01()) * 8));
    
    
    for (int i = 0; i < 16; i++) {
		cout << "GEN " << (i + 1) << "\n";
        
        res = Genetics::top(res, 32, true, 1);
    
        for (auto i : res) cout << i.toString() << " = " << Genetics::test(i, 1) << endl;

		vector<TetrisAI> blockies;
		for (int k = 0; k < 4; k++) {
			std::random_shuffle(res.begin(), res.end());
			for (int i = 0; i < res.size(); i += 2) {
				blockies.push_back(Genetics::mutate(Genetics::cross(res[i], res[i + 1])));
			}
		}
		res = blockies;
    }

	// Reduction

	res = Genetics::top(res, 32, true, 2);
	vector<TetrisAI> blockies;
	for (int i = 0; i < 32; i += 2) blockies.push_back(Genetics::cross(res[i], res[i + 1])); // 16

	res = Genetics::top(res, 8, true, 4);
	blockies.clear();
	for (int i = 0; i < 8; i += 2) blockies.push_back(Genetics::cross(res[i], res[i + 1])); // 4

	res = Genetics::top(res, 2, true, 8);
	ai = Genetics::cross(res[0], res[1]);

	cout << Genetics::test(ai, 16) << endl;
	cout << ai.toString() << endl; // Model weights

    return 0;
}
