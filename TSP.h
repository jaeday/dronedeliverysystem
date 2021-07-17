#pragma once

#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

class TSP {

public:
    
    TSP(): {}

    void readInput();

    /**
     * @brief Attempts to find the optimal Hamiltonian cycle
     * connecting all of the points from the input
     */
    void solve();

private:

    struct Coord {

        // Default Constructor
        Coord() : x{0}, y{0}, visited{false} {}

        // Parameterized Constructor
        Coord(int x_in, int y_in): x{x_in}, y{y_in}, visited{false} {}

        bool operator==(const Coord &rhs) const {
            return (this->x == rhs.x) && (this->y == rhs.y);
        } // operator==()

        int x, y;
        bool visited;

    }; // Coord{}

    struct PrimsObj {
        
        // Default Constructor
        PrimsObj(): distance{std::numeric_limits<double>::infinity()}, parent{-1}, visited{false} {}

        // Parameterized Constructor
        PrimsObj(double distance_in, int parent_in, bool visited_in): distance{distance_in}, parent{parent_in}, visited{visited_in} {}

        double distance;
        int parent;
        bool visited;

    }; // PrimsObj{}

        

}; // TSP{}