#ifndef POINT_H
#define POINT_H

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <climits>
#include <iostream>

struct Point {
    int x = 0;
    int y = 0;
    
    Point() {
        x = y = 0;
    }
    
    Point(int a, int b) {
        x = a;
        y = b;
    }
    
    Point(const Point& other) {
        x = other.x;
        y = other.y;
    }
    
    bool operator<(Point& other) {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
    
    bool operator==(Point& other) {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(Point& other) {
        return x != other.x || y != other.y;
    }
    
    Point operator+(Point& other) {
        return {x + other.x, y + other.y};
    }
    
    Point operator-(Point& other) {
        return {x - other.x, y - other.y};
    }
    
    Point scale(int p) {
        return {x * p, y * p};
    }
    
    Point inv() {
        return {-1 * x, -1 * y};
    }
    
    Point rotate(int rot) {
        while (rot < 0) rot += 16;
        rot = rot % 4;
        
        if (rot <= 0) return {x, y};
        Point nextone(y, -1 * x);
        return nextone.rotate(rot - 1);
    }
};

class TetrisPiece {
    public:
    std::vector<Point> cells; // The four cells in the piece represented as absolute coordinates.
    
    Point bottomleft; // Two opposing corners of the bounding box
    Point topright;
    
    void shrinkBoundingBox() { // Shrink the bounding box to a square. Do note that this should not be done in normal gameplay and only in emergencies.
        bottomleft = {INT_MAX, INT_MAX};
        topright = {INT_MIN, INT_MIN};
        for (auto i : cells) {
            bottomleft.x = std::min(bottomleft.x, i.x);
            bottomleft.y = std::min(bottomleft.y, i.y);
            topright.x = std::max(topright.x, i.x);
            topright.y = std::max(topright.y, i.y);
        }
        int width = std::max(topright.y - bottomleft.y, topright.x - bottomleft.x);
        topright = {bottomleft.x + width, bottomleft.y + width};
    }
    
    TetrisPiece() {
        cells = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
        std::sort(cells.begin(), cells.end());
        shrinkBoundingBox();
    }
    
    TetrisPiece(Point a, Point b, Point c, Point d) {
        cells = {a, b, c, d};
        std::sort(cells.begin(), cells.end());
        shrinkBoundingBox();
    }
    
    TetrisPiece(Point a, Point b, Point c, Point d, Point TL, Point BR) {
        cells = {a, b, c, d};
        std::sort(cells.begin(), cells.end());
        
        bottomleft = {std::min(TL.x, BR.x), std::min(TL.y, BR.y)};
        topright = {std::max(TL.x, BR.x), std::max(TL.y, BR.y)};
    }
    
    TetrisPiece(const TetrisPiece& other) {
        cells = std::vector<Point>(4);
        for (int i = 0; i < 4; i++) cells[i] = Point(other.cells[i]);
        bottomleft = Point(other.bottomleft);
        topright = Point(other.topright);
    }
    
    void translate(Point p) { // Move a piece by the vector (p)
        for (int i = 0; i < 4; i++) cells[i] = cells[i] + p;
        bottomleft = bottomleft + p;
        topright = topright + p;
        std::sort(cells.begin(), cells.end());
    }
    
    void translate(int x, int y) { // Move the piece without having to make a point struct (however it does it anyways :P)
        translate(Point(x, y));
    }
    
    void rotate(int p) { // Rotate the piece in its bounding box.
        while (p < 0) p = p + 16;
        p = p % 4;
        if (p <= 0) {
            std::sort(cells.begin(), cells.end());
            return;
        }
        
        int width = topright.x - bottomleft.x + 1;
        
        for (int i = 0; i < 4; i++) {
            Point norm = cells[i] - bottomleft;
            Point next(norm.y, width - norm.x - 1);
            cells[i] = next + bottomleft;
        }
        
        rotate(p - 1);
    }
    
    bool operator<(TetrisPiece& other) {
        for (int i = 0; i < 4; i++) {
            if (cells[i] != other.cells[i]) return cells[i] < other.cells[i];
        }
        return 0;
    }
    
    bool operator==(TetrisPiece& other) {
        for (int i = 0; i < 4; i++) {
            if (cells[i] != other.cells[i]) return false;
        }
        return topright == other.topright && bottomleft == other.bottomleft;
    }
    
    bool contains(Point p) {
        for (int i = 0; i < 4; i++) {
            if (cells[i] == p) return true;
        }
        return false;
    }
    
    bool inBB(Point p) {
        if (p.x < bottomleft.x || p.x > topright.x) return false;
        if (p.y < bottomleft.y || p.y > topright.y) return false;
        return true;
    }
    
    std::string toString() {
        std::string res = "[";
        for (int i = 0; i < 4; i++) {
            res = res + "(" + std::to_string(cells[i].x) + " " + std::to_string(cells[i].y) + ")";
        }
        return res + "]";
    }
};

#endif