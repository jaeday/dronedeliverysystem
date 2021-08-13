// tsp.cpp

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using OutCode = int;
#define NUM_CORNERS 4

class TSP {

public:

    void readFiles() {

        // Reading in the points to be traversed
        std::cin >> numPoints;
        int x, y;
        int count = 0;

        coords.resize(numPoints);

        while(std::cin >> x >> y) {
            coords[count] = Coord(x, y);
            if(count == static_cast<int>(numPoints) - 1) {
                break;
            }
            ++count;
        }

        // Reading in the no-go zones
        std::cin >> numNoGoPoints;

        if(numNoGoPoints % NUM_CORNERS != 0) {
            std::cerr << "Error: the number of no-go zone points must be a" << 
                " multiple of " << NUM_CORNERS << std::endl;
            exit(1);
        }

        std::vector<Coord> temp;

        for(int i = 0; i < static_cast<int>(numNoGoPoints) / 4; ++i) {
            for(int j = 0; j < NUM_CORNERS; ++j) {
                std::cin >> x >> y;
                temp.push_back(Coord(x,y));
            }
            NoGoZone tempNGZ = NoGoZone(temp);
            noGoZones.push_back(tempNGZ);
            temp.clear();
        }

    } // getOptions()

    void run() {
        optimalTSP();
    }

private:

    // Coordinate Struct
    struct Coord {

        // Default Constructor
        Coord(): x{0}, y{0}, visited{false} {}

        // Parameterized Constructor 
        Coord(int x_in, int y_in): x{x_in}, y{y_in}, visited{false} {}

        // Used to calculate the midPoint in NoGoZone objects
        Coord operator+(const Coord &rhs) {
            return Coord(x + rhs.x, y + rhs.y);
        } // operator+()
    
        // Overloaded operator= ignores visited status
        void operator=(const Coord &rhs) {
            x = rhs.x;
            y = rhs.y;
        } // operator=()

        // Overloaded operator== ignores visited status
        bool operator==(const Coord &rhs) const {
            return (this->x == rhs.x) && (this->y == rhs.y);
        } // operator==()

        // A coordinate c0 is considered "less than" another coordinate c1
        // if c0 is to the northwest of c1
        // Coordinates with the same x or y coordinate are broken
        // by the differing coordinate (y or x respectively)
        bool operator<(const Coord &rhs) const {

            if(*this == rhs) {
                return false;
            }

            else if(x == rhs.x) {
                return y < rhs.y;
            }
            
            else if(y == rhs.y) {
                return x < rhs.x;
            }

            return (x < rhs.x) && (y < rhs.y);

        } // operator<()

        int x, y;
        bool visited;

    }; // Coord{}


    // Struct to store the 4 corner coordinates of the rectangular no-go zones
    struct NoGoZone {

        NoGoZone(const std::vector<Coord> &vertices_in) {

            assert(vertices_in.size() == NUM_CORNERS);
            vertices.resize(NUM_CORNERS);

            for(int i = 0; i < NUM_CORNERS; ++i) {
                vertices[i] = vertices_in[i];
            }

            // std::sort(begin(vertices), end(vertices));

        } // NoGoZone()

        // /*
        // *   Note: operator< is used by std::sort() and compares
        // *   vertices[0] of each NoGoZone object.
        // *   As such, it relies on the no-go zone coordinates being
        // *   inputted in the following order:
        // *   
        // *   3 -- 0
        // *   |    |
        // *   2 -- 1
        // *   
        // *   (i.e. in the same clockwise manner as quadrants on a 2-D plane)
        // */
        bool operator<(const NoGoZone &rhs) const {
        
            if(vertices[0].x == rhs.vertices[0].x) {
                if(vertices[0].y == rhs.vertices[0].y) {
                    return true;
                }
                return vertices[0].y < rhs.vertices[0].y;
            }

            else if(vertices[0].y == rhs.vertices[0].y) {
                return vertices[0].x < rhs.vertices[0].x;
            }

            return (vertices[0].x < rhs.vertices[0].x) && \
                (vertices[0].y < rhs.vertices[0].y);

        } // operator<()

        std::vector<Coord> vertices;

    }; // NoGoZone{}


    // Prim's Algorithm Object Struct
    struct PrimObj {

        // Constructor
        PrimObj(): distance{std::numeric_limits<double>::infinity()}, 
            parent{-1}, visited{false} {}

        // Parameterized Constructor
        PrimObj(double distance_in, int parent_in, bool visited_in) :
            distance{distance_in}, parent{parent_in}, visited{visited_in} {}

        double distance;
        int parent;
        bool visited;

    }; // PrimObj{}


    /*
    *   EFFECTS:  calculates the Euclidean distance between 2 Coords
    *   euclideanDist = sqrt(x^2 + y^2)
    *   NOTE:     this function should only be used to fill the distmat
    */
    double euclideanDist(const Coord &c0, const Coord &c1) const {

        if(c0 == c1) {
            return 0.0;
        }

        double x = static_cast<double>(c0.x - c1.x);
        double y = static_cast<double>(c0.y - c1.y);
        return sqrt((pow(x, 2) + pow(y, 2)));

    } // euclideanDist()


    /**
     * https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
     * https://www.geeksforgeeks.org/line-clipping-set-1-cohen-sutherland-algorithm/
     *
     * @brief Cohen-Sutherland Algorithm
     *  The Cohen-Sutherland algorithm is used for line clipping; it divides
     *  a 2-D space into 9 regions and determines the line segments and portions
     *  of lines/line segments that are visible in the "central region"
     * 
     *  The algortithm is based on the following diagram and some
     *  bitwise operations:
     *  
     *           left    central     right
     *  top      1001      1000       1010
     *  central  0001      0000       0010
     *  bottom   0101      0100       0110
     * 
     *  in base 10:
     *           left    central     right
     *  top       9         8          7
     *  central   1         0          2
     *  bottom    5         4          6
     * 
     *  Where the (central, central) region 0000 is the region of interest
     * 
     *  In terms of no-go zones, the 0000 region can be though of as the no-go
     *  zone and any line segments that clip through said region must be
     *  redrawn to go around the region.
     *  
     */

    // An enum class would be preferable here but bitwise operations appear
    // to not be well supported on them
    static const OutCode TopLeft =         0b1000;
    static const OutCode TopCentral =      0b1000;
    static const OutCode TopRight =        0b1010;
    static const OutCode CentralLeft =     0b0001;
    static const OutCode CentralCentral =  0b0000; // The rectangular no-go zone
    static const OutCode CentralRight =    0b0010;
    static const OutCode BottomLeft =      0b0101;
    static const OutCode BottomCentral =   0b0100;
    static const OutCode BottomRight =     0b0110;

    OutCode computeOutCode(const NoGoZone &n, const Coord &c) const {

        int x = c.x;
        int y = c.y;
        int xMin = (n.vertices[3].x < n.vertices[0].x) ? n.vertices[3].x : n.vertices[0].x;
        int xMax = (n.vertices[3].x > n.vertices[0].x) ? n.vertices[3].x : n.vertices[0].x;
        int yMin = (n.vertices[2].y < n.vertices[1].y) ? n.vertices[2].y : n.vertices[1].y;
        int yMax = (n.vertices[2].y > n.vertices[1].y) ? n.vertices[2].y : n.vertices[1].y;

        // Intialized as being inside the no-go zone
        OutCode code = CentralCentral;

        // Note: |= is a bitwise-OR assignment
        // To the left of the no-go zone
        if(x < xMin) {
            code |= CentralLeft;
        }
        // To the right of the no-go zone
        else if(x > xMax) {
            code |= CentralRight;
        }
        // Below the no-go zone
        if(y < yMin) {
            code |= BottomCentral;
        }
        // Above the no-go zone
        else if(y > yMax) {
            code |= TopCentral;
        }

        return code;

    } // computeOutCode()
    
    
    void CohenSutherland(const NoGoZone &n, const Coord &c0, const Coord &c1) {

        OutCode outcode0 = computeOutCode(n, c0);
        OutCode outcode1 = computeOutCode(n, c1);

        // Temporary variables to maintain consistency with the
        // Wikipedia implementation
        int xMin = (n.vertices[3].x < n.vertices[0].x) ? n.vertices[3].x : n.vertices[0].x;
        int xMax = (n.vertices[3].x > n.vertices[0].x) ? n.vertices[3].x : n.vertices[0].x;
        int yMin = (n.vertices[2].y < n.vertices[1].y) ? n.vertices[2].y : n.vertices[1].y;
        int yMax = (n.vertices[2].y > n.vertices[1].y) ? n.vertices[2].y : n.vertices[1].y;
        // These are the intersection points
        int x0 = c0.x;
        int y0 = c0.y;
        int x1 = c1.x;
        int y1 = c1.y;

        // Resetting the last values
        lastAdditionalPoints.clear();
        lastNoGoIntersect = false;

        // Cases where the points are directly across a no-go zone
        // Directly above/below
        if(x0 == x1) {
            bool up = (y0 < y1);
            // The drone will travel right-up
            if(up) {
                lastAdditionalPoints.push_back(Coord(x0, yMin));
                lastAdditionalPoints.push_back(Coord(xMin, yMin));
                lastAdditionalPoints.push_back(Coord(xMin, yMax));
                lastAdditionalPoints.push_back(Coord(x0, yMax));
                lastNoGoIntersect = true;
                return;
            }
            // The drone will travel left-down
            else {
                lastAdditionalPoints.push_back(Coord(x0, yMax));
                lastAdditionalPoints.push_back(Coord(xMax, yMax));
                lastAdditionalPoints.push_back(Coord(xMax, yMin));
                lastAdditionalPoints.push_back(Coord(x0, yMin));
                lastNoGoIntersect = true;
                return;
            }
        }

        // Directly left/right
        else if(y0 == y1) {
            bool right = (x0 < x1);
            // The drone will travel up-right
            if(right) {
                lastAdditionalPoints.push_back(Coord(xMin, y0));
                lastAdditionalPoints.push_back(Coord(xMin, yMax));
                lastAdditionalPoints.push_back(Coord(xMax, yMax));
                lastAdditionalPoints.push_back(Coord(xMax, y0));
                lastNoGoIntersect = true;
                return;
            }
            // The drone will travel down-left
            else {
                lastAdditionalPoints.push_back(Coord(xMax, y0));
                lastAdditionalPoints.push_back(Coord(xMax, yMin));
                lastAdditionalPoints.push_back(Coord(xMin, yMin));
                lastAdditionalPoints.push_back(Coord(xMin, y0));
                lastNoGoIntersect = true;
                return;
            }
        }

        // Cases where the points are on opposite OutCodes
        // Left/right
        else if((outcode0 == CentralLeft || outcode1 == CentralLeft) \
            && (outcode0 == CentralRight || outcode1 == CentralRight)) {
            lastAdditionalPoints.push_back(Coord(xMin, y0));
            lastAdditionalPoints.push_back(Coord(xMin, yMax));
            lastAdditionalPoints.push_back(Coord(xMax, yMax));
            lastAdditionalPoints.push_back(Coord(xMax, y1));
            lastNoGoIntersect = true;
            return;
        }
        else if((outcode0 == TopCentral || outcode1 == TopCentral) \
            && (outcode0 == BottomCentral || outcode1 == BottomCentral)) {
            lastAdditionalPoints.push_back(Coord(xMax, y1));
            lastAdditionalPoints.push_back(Coord(xMin, yMax));
            lastAdditionalPoints.push_back(Coord(xMin, yMin));
            lastAdditionalPoints.push_back(Coord(xMin, y0));
            lastNoGoIntersect = true;
            return;
        }

        while(true) {

            // In this implementation, there cannot be any points inside the
            // no-go zone, thus the following test is commented out:

            // If the bitwise OR of the two outcodes is 0, both points are
            // inside the no-go zone
            if(!(outcode0 | outcode1)) {
                lastNoGoIntersect = true;
                break;
            }

            // If the bitwise AND of outcode0 and outcode1 is not 0,
            // both points must share a zone outside the no-go zone and thus
            // the line segement they create does not intersect the
            // NoGoZone n
            else if(outcode0 & outcode1) {
                break;
            }

            else  {

                int x, y;
                // Choosing the point outside the no-go zone
                // Because both points are outside the no-go zone in this 
                // implementation, the point is chosen arbitrarily
                // (this does not matter, we simply need a starting point)
                OutCode outCodeOut = (outcode1 > outcode0) ? outcode1 : outcode0;

                // Finding the intersection points using the formulae:
                // slope = (y1 - y0) / (x1 - x0)
                // x = x0 + (1 / slope) * (ym - y0) {where ym is yMin or yMax}
                // y = y0 + slope * (xm - x0) {where xm is xMin or xMax}
                // Dividing by 0 is not possible here as the opcode bit being tested
                // guarantees a non-zero denominator

                // Integer division is used - this is not a mistake

                // Above the no-go zone
                if(outCodeOut & TopCentral) {
                    x = x0 + (x1 - x0) * (yMax -y0) / (y1 - y0);
                    y = yMax;
                }
                // Below the no-go zone
                else if(outCodeOut & BottomCentral) {
                    x = x0 + (x1 - x0) * (yMin - y0) / (y1 - y0);
                    y = yMin;
                }
                // To the right of the no-go zone
                else if(outCodeOut & CentralRight) {
                    y = y0 + (y1 - y0) * (xMax - x0) / (x1 - x0);
                    x = xMax;
                }
                // To the left of the no-go zone
                else if(outCodeOut & CentralLeft) {
                    y = y0 + (y1 - y0) * (xMin - x0) / (x1 - x0);
                    x = xMin;
                }

                // Moving on to the next point
                if(outCodeOut == outcode0){
                    x0 = x;
                    y0 = y;
                    outcode0 = computeOutCode(n, Coord(x0, y0));
                }
                else {
                    x1 = x;
                    y1 = y;
                    outcode1 = computeOutCode(n, Coord(x1, y1));
                }

            } // else

        } // while

        if(!lastNoGoIntersect) {
            return;
        }

        lastNoGoIntersect = true;
        outcode0 = computeOutCode(n, c0);
        outcode1 = computeOutCode(n, c1);

        bool right = (x0 < x1);
        bool up = (y0 < y1);

        if(right && up) {

            switch(outcode0) {

                case CentralLeft: {
                    lastAdditionalPoints.push_back(Coord(xMin, y0));
                    lastAdditionalPoints.push_back(Coord(xMin, yMax));
                    lastAdditionalPoints.push_back(Coord(x1, yMax));
                    break;
                }
                case BottomCentral: {
                    lastAdditionalPoints.push_back(Coord(x0, yMin));
                    lastAdditionalPoints.push_back(Coord(xMax, yMin));
                    lastAdditionalPoints.push_back(Coord(xMax, y1));
                    break;
                }

            } // switch

        } // if

        else if(right && !up) {

            switch(outcode0) {

                case TopCentral: {
                    lastAdditionalPoints.push_back(Coord(x0, yMax));
                    lastAdditionalPoints.push_back(Coord(xMax, yMax));
                    lastAdditionalPoints.push_back(Coord(xMax, y1));
                    break;
                }
                case CentralLeft: {
                    lastAdditionalPoints.push_back(Coord(xMin, y0));
                    lastAdditionalPoints.push_back(Coord(xMin, yMin));
                    lastAdditionalPoints.push_back(Coord(x1, yMin));
                    break;
                }
                
            } // switch

        } // else if

        else if(!right && up) {

            switch(outcode0) {
                
                case BottomCentral: {
                    lastAdditionalPoints.push_back(Coord(x0, yMin));
                    lastAdditionalPoints.push_back(Coord(xMin, yMin));
                    lastAdditionalPoints.push_back(Coord(xMin, y1));
                    break;
                }

                case CentralRight: {
                    lastAdditionalPoints.push_back(Coord(xMax, y0));
                    lastAdditionalPoints.push_back(Coord(xMax, yMax));
                    lastAdditionalPoints.push_back(Coord(x1, yMax));
                    break;
                }

            } // switch

        } // else if

        else { // (!right && !up)

            switch(outcode0) {

                case TopCentral: {
                    lastAdditionalPoints.push_back(Coord(x0, yMax));
                    lastAdditionalPoints.push_back(Coord(xMin, yMax));
                    lastAdditionalPoints.push_back(Coord(xMin, y1));
                    break;
                }

                case CentralRight: {
                    lastAdditionalPoints.push_back(Coord(xMax, y0));
                    lastAdditionalPoints.push_back(Coord(xMax, yMin));
                    lastAdditionalPoints.push_back(Coord(x0, yMin));
                    break;
                }

            } // switch

        } // else 

    } // CohenSutherland()


    void initDistmat() {

        distmat.resize(numPoints);

        for(size_t i = 0; i < distmat.size(); ++i) {
            distmat[i].resize(numPoints);
        }

        for(size_t i = 0; i < distmat.size(); ++i) {

            for(size_t j = 0; j < distmat[i].size(); ++j) {

                if(i == j) {
                    distmat[i][j].second = std::numeric_limits<double>::infinity();
                    continue;
                }

                for(size_t k = 0; k < noGoZones.size(); ++k) {

                    // Checking intersections with no-go zones
                    CohenSutherland(noGoZones[k], coords[i], coords[j]);

                    // Concatenating intersection points
                    if(lastNoGoIntersect) {
                        distmat[i][j].first.insert(end(distmat[i][j].first), \
                            begin(lastAdditionalPoints), \
                            end(lastAdditionalPoints));
                    }

                } // for k

                // If there was no intersection with any no-go
                if(!lastNoGoIntersect) {
                    distmat[i][j].second += \
                        euclideanDist(coords[i], coords[j]);
                }
                
                else {

                    distmat[i][j].second += \ 
                        euclideanDist(coords[i], distmat[i][j].first[0]);

                    // Adding the additional points required to traverse the no-go zone
                    for(size_t k = 0; k < distmat[i][j].first.size() - 1; ++k) {
                        distmat[i][j].second += euclideanDist(distmat[i][j].first[k], distmat[i][j].first[k + 1]);
                    }

                    distmat[i][j].second += euclideanDist(distmat[i][j].first[distmat[i][j].first.size() - 1], coords[j]);
                    
                } // if

            } // for j

        } // for i

    } // initDistmat


    void mst(const size_t start = 0) {

        prim.clear();
        prim.resize(numPoints);
        mstDist = 0.0;

        int current = static_cast<int>(start);
        size_t numVisited = start;

        // Using a while loop to avoid accidentally using the outer loop variable
        // in any of the code
        while(numVisited++ < numPoints) {

            // The next point is unknown
            int next = -1;

            for(size_t i = start; i < prim.size(); ++i) {

                if(!prim[i].visited) {

                    if(next == -1) {
                        next = static_cast<int>(i);
                    }

                    double dist = distmat[path[current]][path[i]].second;

                    // If the distance between current and i
                    // is less than the distance already recorded in i,
                    // set the distance at i to be the parent
                    if(dist < prim[i].distance) {
                        prim[i].distance = dist;
                        prim[i].parent = current;
                    }

                    // If prim[i] has the shortest distance from current,
                    // make next = i
                    if(prim[i].distance < prim[next].distance) {
                        next = static_cast<int>(i);
                    }

                } // if

            } // for i

            prim[next].visited = true;
            current = next;

        } // while

        for(size_t i = start + 1; i < prim.size(); ++i) {
            mstDist += prim[i].distance;
        }

    } // mst()


    int findClosestPoint(const int point) {

        double minDist = std::numeric_limits<double>::infinity();
        int closest = -1;

        for(size_t i = 0; i < coords.size(); ++i) {

            double temp = euclideanDist(coords[i], coords[point]);

            if(static_cast<int>(i) != point && temp < minDist) {
                minDist = temp;
                closest = static_cast<int>(i);
            }

        } // for i

        return closest;

    } // findClosestPoint()


    int findMinInsertionIdx(const int toAdd) {
        
        double minDist = std::numeric_limits<double>::infinity();
        int insertIdx = -1;

        for(size_t i = 0; i < path.size() - 1; ++i) {

            double dist1 = distmat[path[i]][toAdd].second;
            double dist2 = distmat[path[i + 1]][toAdd].second;
            double dist3 = distmat[path[i]][path[i + 1]].second;
            double distDiff = (dist1 + dist2) - dist3;

            if(distDiff < minDist) {
                minDist = distDiff;
                insertIdx = static_cast<int>(i) + 1;
            }

        } // for i

        return insertIdx;

    } // findMinInsertionIdx()


    void calcTotalDist() {

        for(size_t i = 0; i < path.size(); ++i) {
            totalDist += distmat[path[i]][path[i + 1]].second;
        }

        totalDist += distmat[path[0]][path[path.size() - 1]].second;

    } // calcTotalDist()

    double calcTotalDist(const size_t permLength) {

        double dist = 0.0;
        
        for(size_t i = 0; i < permLength; ++i) {
            dist += distmat[path[i]][path[i + 1]].second;
        }

        return dist;

    } // calcTotalDist()


    // Attempts to find the solution to the given TSP using the random
    // insertion heuristic
    void fastTSP() {

        path.reserve(numPoints);
        int numVisited = 0; // also serves as the index of points in coords

        coords[0].visited = true;

        int toAdd = findClosestPoint(0);
        coords[toAdd].visited = true;
        path.push_back(0);
        path.push_back(toAdd);

        while(numVisited < static_cast<int>(numPoints)) {

            toAdd = numVisited++;

            if(coords[toAdd].visited) {
                continue;
            }

            int insertIdx = findMinInsertionIdx(toAdd);
            path.insert(begin(path) + insertIdx, toAdd);
            coords[toAdd].visited = true;

        } // while

        calcTotalDist();

    } // fastTSP()


    // returns true if inserting a point is promising
    // i.e. is (sDist + eDist + distSoFar + mstDist < optDist)
    bool promising(const size_t permLength) {

        // Creating an MST of all points after perm_length
        mst(permLength);

        // Finding the points in the MST that are closest to the start and end points
        // of the optimal path found so far
        double sDist = std::numeric_limits<double>::infinity();
        double eDist = std::numeric_limits<double>::infinity();

        for(size_t i = permLength; i < path.size(); ++i) {

            double dist1 = distmat[path.front()][path[i]].second;
            double dist2 = distmat[path[i]][path.back()].second;
            
            if(dist1 < sDist) {
                sDist = dist1;
            }
            if(dist2 < eDist) {
                eDist = dist2;
            }

        } // for i

        return (sDist + eDist + distSoFar + mstDist < optDist);

    } // promising()


    void genPerms(size_t permLength) {

        if(permLength == path.size()) {

            double lastLink = distmat[path.front()][path.back()].second;

            if(distSoFar + lastLink < optDist) {
                optPath = path;
                optDist = distSoFar + lastLink;
            }

        } // if

        if(!promising(permLength)) {
            return;
        }

        for(size_t i = permLength; i < path.size(); ++i) {

            std::swap(path[permLength], path[i]);
            // Generating permutations with the last distance added
            distSoFar += distmat[path[permLength - 1]][path[permLength]].second;
            genPerms(permLength + 1);
            // Removing the last distance
            distSoFar -= distmat[path[permLength - 1]][path[permLength]].second;
            std::swap(path[permLength], path[i]);

        } // for i

    } // genPerms()


    // attempts to find the optimal TSP solution for the given coordinates
    void optimalTSP() {

        initDistmat();

        fastTSP();

        // Setting lower bounds
        optDist = totalDist;
        optPath = path;

        // The initial permLength is 1
        genPerms(1);

        std::cout << optDist << std::endl;

        std::cout << coords[0].x << " " << coords[0].y << '\n';

        for(size_t i = 0; i < optPath.size() - 1; ++i) {

            // For readability
            const auto &dm = distmat[optPath[i]][optPath[i + 1]].first;

            for(size_t j = 0; j < dm.size(); ++j) {

                std::cout << dm[j].x << " " << dm[j].y << '\n'; 

            } // for j

            std::cout << coords[optPath[i + 1]].x << " " << coords[optPath[i + 1]].y << '\n';

        } // for i

        // Printing out the last link
        const auto &dm = distmat[optPath[optPath.size() - 1]][optPath[0]].first;

        for(size_t i = 0; i < dm.size(); ++i) {

            std::cout << dm[i].x << " " << dm[i].y << '\n';

        }

        std::cout << coords[0].x << " " << coords[0].y << '\n';

    } // optimalTSP()

    // The distmat stores the distance and the number of no-go zones that must be traversed
    std::vector<std::vector<std::pair<std::vector<Coord>, double> > > distmat;
    std::vector<NoGoZone> noGoZones;
    // The index in these vectors is the point number
    // (as it appeared in the input file)
    std::vector<PrimObj> prim;
    std::vector<Coord> coords;

    std::vector<Coord> lastAdditionalPoints;
    bool lastNoGoIntersect;

    // Path vectors
    std::vector<int> optPath, path;

    double mstDist, totalDist, optDist, distSoFar;
    size_t numPoints, numNoGoPoints;


}; // TSP{}


int main(void) {
    
    // To speed up I/O
    std::ios_base::sync_with_stdio(false);

    // Always show 2 decimal places
    // Disable scientific notation for large numbers
    std::cout << std::setprecision(2) << std::fixed;

    TSP t;

    t.readFiles();
    t.run();

    return 0;

} // main()