// Project Identifier: 1761414855B69983BD8035097EFBD312EB0527F0

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <iomanip>
#include "routeplanning.h"
using namespace std;

void controlcenter::get_options(int argc, char** argv){
    int option_index = 0, option = 0;
    
    // Don't display getopt error messages about options
    opterr = false;

    
    // use getopt to find command line options
    struct option longOpts[] = {{ "help", no_argument, nullptr, 'h' },
                                { "mode", required_argument, nullptr, 'm' },
                                { nullptr, 0, nullptr, '\0' }};
    
    while ((option = getopt_long(argc, argv, "m:h", longOpts, &option_index)) != -1) {
        switch (option) {
            case 'h':
                std::cout << "This program reads a txt file that contains a list of delivery \n"
                          << "coordinates for coffee orders. It then outputs the order for the drone \n"
                          <<  "to go, in the most optimal way to save energy and time. \n"
                          <<  "Usage: \'./drone\n\t[--stack | -m] <initialize method>\n"
                          <<                      "\t[--help | -h]\n"
                          <<                      "\t< <txt Map/List Input>\n'"
                            <<                      "\t> <txt Map/List Output>\n'";
                exit(0);

            case 'm':
                string a = optarg;
                if(a == "OPTTSP"){optMode = true;}
                else if(a!= "OPTTSP"){
                    throw std::runtime_error("Error: invalid algorithm mode. \n Error in algorithm option, should be either MST or FASTTSP or OPTTSP. Please revisit");
                }

                break;

        }
    }
}

void controlcenter::readInput(){
    // ifstream inFile(argv[1]);
    int numpoint, x, y;
    int i = 0;
    cin >> numpoint;
    points.resize(numpoint);

    while(cin >> x){
        cin >> y;
        point temp = {x, y, i};
        if(medical(temp)){
            temp.cat = Cat::medical;
        }
        else if(border(temp)){
            temp.cat = Cat::border;
        }
        points[i] = temp;
        ++i;
    }
}


//--------------------------------------------------------------------------------------PART A MST--------------------------------------------------------------------------------------
int controlcenter::findSmallest(vector<vertex>& vec){
    double temp = numeric_limits<double>::infinity();
    int ans = 0;

    for(size_t i = 0; i < vec.size(); ++i){
        if(!vec[i].k){
            if(vec[i].d <= temp){
                ans = int(i);
                temp = vec[i].d;
            }
        }
    }

    return ans;
}


//--------------------------------------------------------------------------------------PART B TSP--------------------------------------------------------------------------------------

int controlcenter::findTriDist(size_t i){
    int ans = -1;
    double minCost = numeric_limits<double>::infinity();
    double currentCost = 0.0;
    for(size_t j = 0; j < FASTpath.size() - 1; ++j){
        currentCost = insertionCost(points[FASTpath[j]], points[FASTpath[j+1]], points[i]);
        if(currentCost < minCost){
            minCost = currentCost;
            ans = int(j) + 1;
        }
    }
    FASTweight += minCost;
    return ans;
}

double controlcenter::insertionCost(point p1, point p2, point p3){
    return dist(p1, p3) + dist(p3, p2) - dist(p1, p2);
}


void controlcenter::arbitraryInsertion(){

    //Initialization: start with the 0th node & make a subtour
    FASTpath.push_back(0);
    FASTpath.push_back(1);
    FASTpath.push_back(0);
    
    FASTweight += dist(points[0], points[1]) + dist(points[1], points[0]);

    //Starting from the fact that we have visited 0 and 1st node.
    size_t numVisited = 2;
    

    // Outer loop for trying out different positions that are not in the subtour.
    while(numVisited != points.size()){
             

        //Inner loop for comparing the values with the stuff that's already in the subcycle.
        int index = findTriDist(numVisited);

        if(index == int(FASTpath.size())){
            FASTpath.push_back(int(numVisited));
        }
        else{
            FASTpath.insert(FASTpath.begin() + index, int(numVisited));
        }
        

        ++numVisited;

    }

}



//--------------------------------------------------------------------------------------PART C TSP--------------------------------------------------------------------------------------

void controlcenter::unvisitedInit(size_t permLength){
    unvisitedPrim.clear();
    unvisitedPrim.resize(points.size() - permLength);
    for(int i = 0; i < int(unvisitedPrim.size()); ++i){      
        unvisitedPrim[i].id = OPTPath[i + int(permLength)];
    }
}

double controlcenter::primProcessingOPT(size_t permLength){
    cout << std::setprecision(2);
    cout << std::fixed;

    unvisitedInit(permLength);    

    double weight = 0.0;
    unvisitedPrim[0].d = 0.0;

    for(size_t j = 0; j < unvisitedPrim.size(); ++j){
        int v = findSmallest(unvisitedPrim);
        unvisitedPrim[v].k = true;
        weight += unvisitedPrim[v].d;
        for(size_t k = 0; k < unvisitedPrim.size(); ++k){
            double distance = 0.0;  
            if(!unvisitedPrim[k].k){
                distance = dist(points[unvisitedPrim[v].id], points[unvisitedPrim[k].id]);

                if(unvisitedPrim[k].d > distance){
                    
                    unvisitedPrim[k].d = distance;
                    unvisitedPrim[k].p = unvisitedPrim[v].id;
                }
            }
            
        }

    }

    
    return weight;

}

void controlcenter::OPTTSP(){
    arbitraryInsertion();
    OPTInit();
    distBest = FASTweight;
    genPerms(1);
}

//closest point from the subtour to the MST
double controlcenter::closest(int index){
    double temp = numeric_limits<double>::infinity();
    for(size_t i = 0; i < unvisitedPrim.size(); ++i){
        if(dist( points[OPTPath[index]], points[unvisitedPrim[i].id]) < temp){
            temp = dist( points[OPTPath[index]], points[unvisitedPrim[i].id]);
        }
    }
    return temp;
}

bool controlcenter::promising(size_t permLength){
    // if current distance > fastweight, it's not promising at all so just exit immediately
    if(currDist > FASTweight){
        return false;
    }
    double MSTweight = primProcessingOPT(permLength) + closest(0) + closest(int(permLength) - 1) + currDist;

    // cout << MSTweight << endl;

    if(MSTweight <= distBest){
        return true;
    }
    return false;
}
