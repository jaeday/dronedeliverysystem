#include <algorithm>
#include <cmath>
#include <getopt.h>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include "Delivery.h"

using namespace std;

/*
*  MODIFIES: mode, mode_set
*  EFFECTS: reads in command line options, prints help message if necessary
*/
void Delivery::get_options(int argc, char **argv) {

  string mode_in;
  int opt;
  int option_index = 0;

  option long_options[] = {
    {"mode",    required_argument, nullptr, 'm'},
    {"help",    no_argument,       nullptr, 'h'},
    { nullptr,  0,                 nullptr, '\0'}
  }; // long_options[]

  while((opt = 
          getopt_long(argc, argv, "m:h", long_options, &option_index)) != -1) {

    switch(opt) {

      case 'm': {

        mode_in = optarg;

        if(mode_in == "MST") {
          mode = Mode::MST;
          mode_set = true;
          break;
        }
        else if(mode_in == "FASTTSP") {
          mode = Mode::FASTTSP;
          mode_set = true;
          break;
        }
        else if(mode_in == "OPTTSP") {
          mode = Mode::OPTTSP;
          mode_set = true;
          break;
        }
        // Any other mode
        if(!mode_in.empty()) {
          throw runtime_error("Error: Invalid mode");
        }

        break;

      } // case m

      case 'h': {
        cout << "usage: " << argv[0] << " -h | -m <MODE>" << endl;
        exit(0);
      } // case h

      default: {
        throw runtime_error("Error: Invalid command line option");
      } // default

    } // switch

  } // while

  if(!mode_set) {
    throw runtime_error("Error: No mode specified");
  }

} // Delivery::get_options()


/*
*  MODIFIES: coordinates, non_medical, medical, border, num_nodes
*  EFFECTS:  reads in the input file
*/
void Delivery::read_input() {

  cin >> num_nodes;
  int x, y;
  size_t i = 0;
  
  coords.resize(num_nodes);

  while(cin >> x >> y) {

    coords[i++] = Coord(x, y);

    if(!non_medical) {
      if(!is_medical(x, y)) {
        non_medical = true;
      }
    } // if(!non_medical)

    if(!medical) {
      if(is_medical(x, y)) {
        medical = true;
      }
    } // if(!medical)

    if(!border) {
      if(is_border(x, y)) {
        border = true;
      }
    } // if(!border)

  } // while

} // Delivery::read_input()


/*
*  EFFECTS:  calls mst(), fast_tsp or optimal_tsp()
*            as specified by the command line options
*/
void Delivery::solve() {

  switch(mode) {

    case Mode::MST: {
      mst();
      return;
    }

    case Mode::FASTTSP: {
      fast_tsp();
      return;
    }

    case Mode::OPTTSP: {
      optimal_tsp();
      return;
    }

  } // switch

} // Delivery::solve()


/*
*  EFFECTS:  calculates the Euclidean distance between 2 Coords
*            (in MST mode only) 
*            returns std::numeric_limits<double>::infinity()
*            if c1 and c2 are on different campuses
*            and neither are on the border
*            returns 0.0 if c1 == c2
*/
double Delivery::euclidean_dist(const Coord &c1, const Coord &c2) {

  if(c1 == c2) {
    return 0.0;
  }

  if(mode == Mode::MST) {

    // The distance is infinity when c1 and c2 are not on the same campus
    // and one of them is NOT on the border
    if((is_medical(c1) != is_medical(c2)) 
        && !(is_border(c1) || is_border(c2))) {
      return numeric_limits<double>::infinity();
    }

  } // if

  double x = static_cast<double>(c1.x - c2.x);
  double y = static_cast<double>(c1.y - c2.y);
  return sqrt(pow(x, 2) + pow(y, 2));

} // Delivery::euclidian_dist()

double Delivery::euclidean_dist(const Coord &c1, const Coord &c2, 
    const size_t i, const size_t j) {

  return (i == j) ? numeric_limits<double>::infinity() : euclidean_dist(c1, c2);

} // Delivery::euclidian_dist()


/*
*  EFFECTS:   searches through the distmat to find the closest node to node
*             that has not been visited
*             Works on all modes
*             May return -1 if there is no closest node
*             (although this shouldn't really be possible)
*/
pair<int, double> Delivery::find_closest_node(int node) {

  double min_dist = numeric_limits<double>::infinity();
  int closest = -1;

  for(size_t i = 0; i < coords.size(); ++i) {

    double temp = euclidean_dist(coords[i], coords[node]);

    if(static_cast<int>(i) != node && temp < min_dist) {

      min_dist = temp;
      closest = static_cast<int>(i);
    }

  } // for i

  return {closest, min_dist};

} // Delivery::find_closest_node()


/*
*  EFFECTS:  attempts to find an MST for the coordinates
*            calls exit(1) and prints a message to cerr 
*            if an MST cannot be found
*/
void Delivery::mst() {

  if(non_medical && medical && !border) {
    cerr << "Cannot construct MST" << endl;
    exit(1);
  }

  // This function does the actual MST work
  mst_helper();

  cout << mst_dist << endl;

  // Printing the path taken (with the lower numbered node first)
  for(int i = 1; i < static_cast<int>(prim.size()); ++i) {

    int p = prim[i].parent;
    cout << min(i, p) << " " << max(i, p) << "\n";

  } // for i

} // Delivery::mst()


/*
*  EFFECTS:  attempts to find an MST from coords[start]
*            created to avoid code duplication in opt_tsp()
*/
 void Delivery::mst_helper(size_t start) {

  prim.clear();

  // Prim_c objects are default constructed to {infinity, -1, false}
  prim.resize(num_nodes);
  // Resetting the MST distance
  mst_dist = 0.0;

  int current = static_cast<int>(start);
  size_t num_visited = start;

  // Using a while loop to avoid accidentally using the outer loop variable
  // in any of the code
  while(num_visited++ < num_nodes) {

    // next node is unknown
    int next = -1;

    for(size_t i = start; i < prim.size(); ++i) {

      if(!prim[i].visited) {

        // Default initializing next
        if(next == -1) {
          next = static_cast<int>(i);
        }

        double dist;

        if(mode == Mode::OPTTSP) {
          dist = distmat[path[current]][path[i]];
        }
        // This will return infinity if current and i are the same node
        // Probably not necessary but is a useful backup
        else {
          dist = euclidean_dist(coords[current], coords[i], current, i);
        }
        
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

    } // for j

    prim[next].visited = true;
    // Start from the closest node to what used to be current
    current = next;

  } // while

  for(size_t i = start + 1; i < prim.size(); ++i) {
    mst_dist += prim[i].distance;
  } // for i

} // Delivery::mst_helper()


/*
*  EFFECTS:  returns the index in path where inserting the node coords[to_add]
*            results in the shortest increases in the overall distance
*/
int Delivery::find_min_insertion_idx(int to_add) {

  double min_dist = numeric_limits<double>::infinity();
  int insert_idx = -1;

  // using reserve() allows the use of path.size()
  for(size_t i = 0; i < path.size() - 1; ++i) {
    double dist1 = euclidean_dist(coords[path[i]], coords[to_add]);
    double dist2 = euclidean_dist(coords[path[i + 1]], coords[to_add]);
    double dist3 = euclidean_dist(coords[path[i]], coords[path[i + 1]]);
    double dist_diff = (dist1 + dist2) - dist3;

    if(dist_diff < min_dist) {
      min_dist = dist_diff;
      insert_idx = static_cast<int>(i) + 1;
    } // if
  } // for i

  // This should never return -1
  return insert_idx;

} // find_min_insertion_idx()


/*
*  EFFECTS:  prints the path taken by FASTTSP and OPTTSP
*/
void Delivery::print_total_and_path() {

  cout << total_dist << endl;

  for(size_t i = 0; i < path.size(); ++i) {
    cout << path[i] << " ";
  }

} // Delivery::print_path()


/*
*  MODIFIES: total_dist
*  EFFECTS:  calculates the distance of the Hamiltonian circuit
*            created by the nodes in path
*/
void Delivery::calc_total_dist() {

  for(size_t i = 0; i < path.size() - 1; ++i) {
    total_dist += euclidean_dist(coords[path[i]], coords[path[i + 1]]);
  }
 
  // Adding the last link to create the Hamiltonian cycle
  total_dist += euclidean_dist(coords[path[0]], coords[path[path.size() - 1]]);

} // Delivery::calc_total_dist()

double Delivery::calc_total_dist(size_t perm_length) {

  double dist = 0.0;

  for(size_t i = 0; i < perm_length; ++i) {
    dist += distmat[path[i]][path[i + 1]];
  }

  return dist;

} // Delivery::calc_total_dist()


/*
*  EFFECTS:  attempts to find a solution to the TSP for the given coordinates
*            using the random insertion heuristic
*/
void Delivery::fast_tsp() {

  path.reserve(num_nodes);
  int num_visited = 1; // also serves as the index of nodes in coords

  // Adding the first two nodes to the path
  coords[0].visited = true;
  int to_add = find_closest_node(0).first;
  coords[to_add].visited = true;
  path.push_back(0);
  path.push_back(to_add);
  
  // This implementation adds the 0th node of coords and the closest node to it
  // to the path. 
  // Other nodes are added sequentially (in the order they were read in)
  while(num_visited < (int)num_nodes) {

    // num_visited is the index of the node to be added
    to_add = num_visited++;

    if(coords[to_add].visited) {
      continue;
    }

    int insert_idx = find_min_insertion_idx(to_add);
    path.insert(path.begin() + insert_idx, to_add);
    coords[to_add].visited = true;

  } // while

  calc_total_dist();

  if(mode == Mode::FASTTSP) {
    print_total_and_path();
  }

} // Delivery::fast_tsp()


/*
*   MODIFIES: prim, mst_dist
*   EFFECTS:  returns true if a node is promising
*             i.e. is (s_dist + e_dist + dist_so_far + mst_dist < opt_dist)
*/
bool Delivery::promising(size_t perm_length) {

  if(dist_so_far > opt_dist) {
    return false;
  }
  
  // Creating an MST of all points after perm_length
  // In this case, mst_helper() ignores the medical campus constraint
  mst_helper(perm_length);

  // Finding the points in the MST that are closest to the start and end points
  // of the optimal path found so far
  double s_dist = numeric_limits<double>::infinity();
  double e_dist = numeric_limits<double>::infinity();
  
  for(size_t i = perm_length; i < path.size(); ++i) {

    double dist1 = distmat[path.front()][path[i]];
    double dist2 = distmat[path[i]][path[perm_length - 1]];

    if(dist1 < s_dist) {
      s_dist = dist1;
    }
    if(dist2 < e_dist) {
      e_dist = dist2;
    }

  } // for i

  return (s_dist + e_dist + dist_so_far + mst_dist < opt_dist);

} // Delivery::promising()


/*
*   MODIFIES: dist_so_far, path, opt_dist, opt_path
*   EFFECTS:  recursively generates and checks all possible paths from
*             perm_length to path[path.length() - 1];
*/
void Delivery::gen_perms(size_t perm_length) {

  if (perm_length == path.size()) {

    // Adding the last link to create a Hamiltonian cycle
    double last_link = distmat[path.front()][path.back()];

    if(dist_so_far + last_link < opt_dist) {
      opt_path = path;
      opt_dist = dist_so_far + last_link;
    }
    return;

  } // if

  // Return if not promising
  if (!promising(perm_length)) {
    return;
  }

  for (size_t i = perm_length; i < path.size(); ++i) {

    swap(path[perm_length], path[i]);
    // Generating permutations with the last distance added
    dist_so_far += distmat[path[perm_length - 1]][path[perm_length]];
    gen_perms(perm_length + 1);
    // Removing the last distance
    dist_so_far -= distmat[path[perm_length - 1]][path[perm_length]];
    swap(path[perm_length], path[i]);

  } // for

} // Delivery::gen_perms()


/*
*  EFFECTS:  attempts to find the optimal TSP solution for the given 
*            coordinates
*/
void Delivery::optimal_tsp() {

  // In OPTTSP mode, fast_tsp will not insert nodes before node 0
  fast_tsp();

  // Setting lower bounds
  opt_dist = total_dist;
  opt_path = path;

  // A more elegant way of resizing a matrix
  distmat.resize(num_nodes, vector<double>(num_nodes));

  // Filling the distant matrix
  for(size_t i = 0; i < distmat.size(); ++i) {
    for(size_t j = 0; j < distmat[i].size(); ++j) {
      distmat[i][j] = euclidean_dist(coords[i], coords[j], i, j);
    } // for j
  } // for i

  // The initial perm_length is 1
  gen_perms(1);

  cout << opt_dist << endl;
  
  for(size_t i = 0; i < opt_path.size(); ++i) {
    cout << opt_path[i] << " ";
  }

} // Delivery::optimal_tsp()


/*
*   EFFECTS: returns true if c is on the border of the medical and normal
*            campuses
*/
bool Delivery::is_border(const Coord &c) const {

  return ((c.x == 0 && c.y <= 0) || (c.x <= 0 && c.y == 0));

} // Delivery::is_border()

bool Delivery::is_border(const int x, const int y) const {

  return is_border(Coord(x, y));

} // Delivery::is_border()


/*
*   EFFECTS: returns true if c is on the medical campus
*/
bool Delivery::is_medical(const Coord &c) const {

  return (c.x < 0 && c.y < 0);

} // Delivery::is_medical()

bool Delivery::is_medical(const int x, const int y) const {

  return is_medical(Coord(x, y));

} // Delivery::is_medical()