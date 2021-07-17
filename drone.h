// Project Identifier: 1761414855B69983BD8035097EFBD312EB0527F0
#ifndef INTERFACE_H
#define INTERFACE_H

#include <getopt.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <queue>
#include <vector>
#include <cmath>
#include <unordered_map>
using namespace std;

enum class Cat : char{
    none,
    medical,
    border
};

struct point{
    int x;
    int y;
    int id;
    Cat cat = Cat::none;
};

struct vertex{
    int id;
    bool k;
    double d;
    int p;
    vertex() : k(false), d(numeric_limits<double>::infinity()), p(-1) {};
};




class controlcenter
{
public:
    bool mstMode = false;
    bool fastMode = false;
    bool optMode = false;
    void get_options(int arc, char **argv);
    void readInput();


//-------------------------------------------------------------------------------------- HELPERS --------------------------------------------------------------------------------------
    double dist(point p1, point p2){
        double x = pow((p1.x - p2.x), 2);
        double y = pow((p1.y - p2.y), 2);
        return sqrt(x+y);
    }

    bool medical(point p1){
        return ((p1.x < 0) && (p1.y < 0));
    }

    bool border(point p1){
        return (((p1.x < 0) && (p1.y == 0))||
                ((p1.x == 0) && (p1.y < 0))||
                ((p1.x == 0) && (p1.y == 0)));
    }

    void fill_adjMat(const vector<point>& points){
        adjMat.resize(points.size());
        for(size_t i = 0; i < points.size(); ++i){
            adjMat[i].resize(points.size());
            for(size_t j = 0; j < points.size(); ++j){
                if(i == j){
                    adjMat[i][j] = numeric_limits<double>::infinity();
                }
                else if((points[i].cat == Cat::medical && points[j].cat == Cat::none) ||
                (points[i].cat == Cat::none && points[j].cat == Cat::medical)){
                    adjMat[i][j] = numeric_limits<double>::infinity();
                }
                else{
                    adjMat[i][j] = dist(points[i], points[j]);
                }
            }
        }
    }


    void pointsPrint();
    void adjMatPrint();
//--------------------------------------------------------------------------------------PART A MST--------------------------------------------------------------------------------------
    void primPreProcessing(vector<point>& vec);
    double primProcessing();
    void mstOutput();
    int findSmallest(vector<vertex>& vec);
    //testing purpose only 
    void primDSPrint();

//------------------------------------------------------------------------------------PART B FASTTSP--------------------------------------------------------------------------------------

    double insertionCost(point p1, point p2, point p3);
    void arbitraryInsertion();
    int findTriDist(size_t i);
    void printFAST();

    

//--------------------------------------------------------------------------------------PART C TSP--------------------------------------------------------------------------------------
    double primProcessingOPT(size_t permLength);

    bool promising(size_t permLength);

    void OPTTSP();

    double closest(int index);

    //Gen perms as described by the video
    void genPerms(size_t permLength) {
        if (permLength == OPTPath.size()) {
            currDist += dist(points[OPTPath[OPTPath.size() - 1]], points[OPTPath[0]]);
            if(currDist < distBest){
                distBest = currDist;
                BESTPath = OPTPath;
            }
            currDist -= dist(points[OPTPath[OPTPath.size() - 1]], points[OPTPath[0]]);
            return;
        } // if
        if (!promising(permLength))
            return;
        for (size_t i = permLength; i < OPTPath.size(); ++i) {
            swap(OPTPath[permLength], OPTPath[i]);
            currDist += dist(points[OPTPath[permLength - 1]], points[OPTPath[permLength]]);
            genPerms(permLength + 1);
            currDist -= dist(points[OPTPath[permLength - 1]], points[OPTPath[permLength]]);
            swap(OPTPath[permLength], OPTPath[i]);
        } // for
    } // genPerms()

    void OPTPrint(){
        cout << distBest << '\n';
        for(auto x : BESTPath){
            cout << x << " ";
        }
    }

    void unvisitedInit(size_t permLength);

    void OPTInit(){
        //basically copying the path from the FASTTSP
        OPTPath.resize(FASTpath.size() - 1);
        BESTPath.resize(FASTpath.size() - 1);
        for(size_t i = 0; i < FASTpath.size() - 1; ++i){
            OPTPath[i] = FASTpath[i];
            BESTPath[i] = FASTpath[i];
        }
    }

    


private:


    //DS for universal input
    vector<point> points;

    //DS for PRIM
    vector<vertex> primDS;
    vector<vector<int>> MSTPath;

    //DS for FASTTSP
    vector<int> FASTpath;
    double FASTweight = 0.0;
    

    //DS for OPTTSP
    vector<vertex> unvisitedPrim;
    vector<int> BESTPath;
    vector<int> OPTPath;
    vector< vector<double>> adjMat;
    double distBest = 0;
    double currDist = 0;

};

#endif