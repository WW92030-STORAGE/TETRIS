#ifndef GENETIC_H
#define GENETIC_H

#include "point.h"
#include "tetris.h"
#include <climits>
#include <cfloat>
#include <algorithm>
#include <cassert>

class TetrisAI {
    public:
    
    // Weights for the AI https://luckytoilet.wordpress.com/2011/05/27/coding-a-tetris-ai-using-a-genetic-algorithm/
    
    double clear = 8;
    double height = -0.03;
    double hole = -7.5;
    double blockade = -3.5;
    
    TetrisAI() {
        clear = 8;
        height = -0.03;
        hole = -7.5;
        blockade = -3.5;
    }

	TetrisAI(double cl, double he, double ho, double bl) {
		clear = cl;
		height = he;
		hole = ho;
		blockade = bl;
	}
    
    TetrisAI(const TetrisAI& ai) {
        clear = ai.clear;
        height = ai.height;
        hole = ai.hole;
        blockade = ai.blockade;
    }
    
    TetrisAI clone() {
        TetrisAI res;
        res.clear = clear;
        res.height = height;
        res.hole = hole;
        res.blockade = blockade;
        return res;
    }
    
    bool operator<(TetrisAI& other) {
        if (clear != other.clear) return clear < other.clear;
        if (height != other.height) return height < other.height;
        if (hole != other.hole) return hole < other.hole;
        return blockade < other.blockade;
    }
    
    
    double getScore(TetrisGame g, bool verbose = false) {
        double heights = 0;
        double holes = 0;
        double clears = 0;
        double blocks = 0;
        
        for (int x = 0; x < g.width; x++) {
            bool containsholes = false;
            bool heightcheck = false;
            int lasthole = g.height - 1;
            for (int y = g.height - 1; y >= 0; y--) {
                if (g.grid[x][y] >= 0) {
                    if (!heightcheck) {
                        if (verbose) std::cout << "(" << x << " " << y << ")";
                        heights += (y + 1);
                        heightcheck = true;
                    }
                }
                else if (heightcheck) {
                    holes++;
                    containsholes = true;
                    lasthole = y;
                }
            }
            
            if (containsholes) {
                for (int y = lasthole; y < g.height; y++) {
                    if (g.grid[x][y] >= 0) blocks++;
                }
            }
        }
        
        for (int y = 0; y < g.height; y++) {
            int cnt = 0;
            for (int x = 0; x < g.width; x++) {
                if (g.grid[x][y] >= 0) cnt++;
            }
            if (cnt == g.width) clears++;
        }
        
        if (verbose) std::cout << "\n" << clears << " " << heights << " " << holes << " " << blocks << "<\n";
        
        double res = clear * clears + height * heights + hole * holes + blockade * blocks;
        if (g.endgame) res = -1 * DBL_MAX; // Dont want to lose obvoiusly
        return res;
    }
    
    
    // Scans the grid for a single piece move.
    Point fuckaround(TetrisGame& game, bool verbose = false) {
        int origin = 5; // How much we move left and right to place the block (up to +/-5)
        int basis = 0; // How much we rotate the piece and then move it. The returned value is of teh form (Basis, Origin)
        
        Point res(0, 0);
        double maxscore = -1 * DBL_MAX;
        
        for (basis = 0; basis < 4; basis++) {
            for (origin = 5; origin >= 0; origin--) {
                TetrisGame game2(game);
                for (int i = 0; i < basis; i++) game2.rotatePieceClockwise();
                for (int i = 0; i < origin; i++) game2.movePiece({-1, 0});
                while (game2.movePieceDown());
                
                double score = getScore(game2, verbose);
                if (verbose) std::cout << ">" << basis << " " << -1 * origin << " " << score << " " << maxscore << "\n";
                if (score > maxscore) {
                    maxscore = score;
                    res.x = basis;
                    res.y = -1 * origin;
                }
            }
            for (origin = 1; origin <= 5; origin++) {
                TetrisGame game2(game);
                for (int i = 0; i < basis; i++) game2.rotatePieceClockwise();
                for (int i = 0; i < origin; i++) game2.movePiece({1, 0});
                while (game2.movePieceDown());
                
                double score = getScore(game2, verbose);
                if (verbose) std::cout << ">" << basis << " " << origin << " " << score << " " << maxscore << "\n";
                if (score > maxscore) {
                    maxscore = score;
                    res.x = basis;
                    res.y = origin;
                }
                
                
            }
        }
        
        return res;
    }
    
    double scorearound(TetrisGame game, bool verbose = false) {
        int origin = 5; // How much we move left and right to place the block (up to +/-5)
        int basis = 0; // How much we rotate the piece and then move it. The returned value is of teh form (Basis, Origin)
        
        Point res(0, 0);
        double maxscore = -1 * DBL_MAX;
        
        for (basis = 0; basis < 4; basis++) {
            for (origin = 5; origin >= 0; origin--) {
                TetrisGame game2(game);
                for (int i = 0; i < basis; i++) game2.rotatePieceClockwise();
                for (int i = 0; i < origin; i++) game2.movePiece({-1, 0});
                while (game2.movePieceDown());
                
                double score = getScore(game2, verbose);
                if (verbose) std::cout << ">" << basis << " " << -1 * origin << " " << score << " " << maxscore << "\n";
                if (score > maxscore) {
                    maxscore = score;
                    res.x = basis;
                    res.y = -1 * origin;
                }
            }
            for (origin = 1; origin <= 5; origin++) {
                TetrisGame game2(game);
                for (int i = 0; i < basis; i++) game2.rotatePieceClockwise();
                for (int i = 0; i < origin; i++) game2.movePiece({1, 0});
                while (game2.movePieceDown());
                
                double score = getScore(game2, verbose);
                if (verbose) std::cout << ">" << basis << " " << origin << " " << score << " " << maxscore << "\n";
                if (score > maxscore) {
                    maxscore = score;
                    res.x = basis;
                    res.y = origin;
                }
                
                
            }
        }
        
        return maxscore;
    }
    
    // Scans the next 2 moves
    Point fuckaround2(TetrisGame game, bool verbose = false) {
        int origin = 5; // How much we move left and right to place the block (up to +/-5)
        int basis = 0; // How much we rotate the piece and then move it. The returned value is of teh form (Basis, Origin)
        
        Point res(0, 0);
        double maxscore = -1 * DBL_MAX;
        
        for (basis = 0; basis < 4; basis++) {
            for (origin = 5; origin >= 0; origin--) {
                TetrisGame game2(game);
                for (int i = 0; i < basis; i++) game2.rotatePieceClockwise();
                for (int i = 0; i < origin; i++) game2.movePiece({-1, 0});
                while (game2.movePieceDown());
                
                
                
                double score = scorearound(game2, verbose);
                if (score > maxscore) {
                    maxscore = score;
                    res.x = basis;
                    res.y = -1 * origin;
                }
            }
            for (origin = 1; origin <= 5; origin++) {
                TetrisGame game2(game);
                for (int i = 0; i < basis; i++) game2.rotatePieceClockwise();
                for (int i = 0; i < origin; i++) game2.movePiece({1, 0});
                while (game2.movePieceDown());
                
                double score = scorearound(game2, verbose);
                if (score > maxscore) {
                    maxscore = score;
                    res.x = basis;
                    res.y = origin;
                }
                
                
            }
        }
        
        return res;
    }
    
    std::string toString() {
        std::string res = "CLEAR " + std::to_string(clear) + " HEIGHT " + std::to_string(height);
        res = res + " HOLE " + std::to_string(hole) + " BLOCKADE " + std::to_string(blockade);
        return res;
    }
};

namespace Genetics {

double test(TetrisAI ai, int trials = 16) {
    TetrisGame game;
    game.spawnPiece();
    double total = 0;
    for (int i = 0; i < trials; i++) {
    
    while (true) {
        Point fuckyou = ai.fuckaround(game);
        // std::cout << fuckyou.x << " " << fuckyou.y << "\n";
        for (int i = 0; i < fuckyou.x; i++) game.rotatePieceClockwise();
        for (int i = 0; i < std::abs(fuckyou.y); i++) {
            if (fuckyou.y < 0) game.movePiece({-1, 0});
            else game.movePiece({1, 0});
        }
        while (game.movePieceDown());
        if (game.endgame) break;
        game.clearLines();
        game.spawnPiece();
        // game.disp();
        
        std::string x;
        // std::cin >> x;
    }
    total += game.clears;
    game.reset();
    }
    
    return (double)(total) / trials;
}

TetrisAI cross(TetrisAI& p1, TetrisAI& p2) {
    TetrisAI p3;
    p3.clear = (rand() % 2 == 0) ? p1.clear : p2.clear;
    p3.height = (rand() % 2 == 0) ? p1.height : p2.height;
    p3.hole = (rand() % 2 == 0) ? p1.hole : p2.hole;
    p3.blockade = (rand() % 2 == 0) ? p1.blockade : p2.blockade;
    
    return p3;
}

double rand01() {
    return (double)(rand()) / (double)(RAND_MAX);
}

TetrisAI mutate(TetrisAI ai2) {
    TetrisAI ai;
    ai.clear = ai2.clear;
    ai.height = ai2.height;
    ai.hole = ai2.hole;
    ai.blockade = ai2.blockade;
    
    int x = rand() % 5;
    if (x == 0) ai.clear += 0.5 - 1 * rand01();
    if (x == 1) ai.height += 0.5 - 1 * rand01();
    if (x == 2) ai.hole += 0.5 - 1 * rand01();
    if (x == 3) ai.blockade += 0.5 - 1 * rand01();
    return ai;
}

std::vector<TetrisAI> generateMutations(TetrisAI ai, int num = 16) {
    std::vector<TetrisAI> res;
    for (int i = 0; i < num; i++) res.push_back(mutate(ai));
    return res;
}
std::vector<TetrisAI> top(std::vector<TetrisAI> ais, int k = 2, bool verbose = true) {
    std::vector<std::pair<double, int>> scores;
    for (int i = 0; i < ais.size(); i++) {
		scores.push_back({-1 * Genetics::test(ais[i]), i});
		if (verbose) std::cout << "X";
	}
	if (verbose) std::cout << "DONE\n";
    std::sort(scores.begin(), scores.end());
    
    std::vector<TetrisAI> thing;
    for (int i = 0; i < k && i < scores.size(); i++) thing.push_back(ais[scores[i].second].clone());
    return thing;
}

};

#endif