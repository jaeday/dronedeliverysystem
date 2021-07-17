#pragma once // a better (albeit nonstandard) include guard

#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

class Delivery {

public:

  // Constructor
  Delivery(): mst_dist{0.0}, total_dist{0.0}, opt_dist{0.0}, dist_so_far{0.0}, 
    num_nodes{0}, mode_set{false}, non_medical{false}, medical{false},
    border{false} {}

  /*
  *  MODIFIES: mode, mode_set
  *  EFFECTS: reads in command line options, prints help message if necessary
  */
  void get_options(int argc, char **argv);

  /*
  *  MODIFIES: coordinates, non_medical, medical, border, num_nodes
  *  EFFECTS:  reads in the input file
  */
  void read_input();

  /*
  *  EFFECTS:  calls mst(), fast_tsp or optimal_tsp()
  *            as specified by the command line options
  */
  void solve();

private:

  enum class Mode : u_int8_t {

    MST, // Minimum spanning tree
    FASTTSP, // Fast travelling salesperson
    OPTTSP // Optimal travelling salesperson

  }; // Mode{}


  struct Coord {

    // Default Constructor
    Coord(): x{0}, y{0}, visited{false} {}

    // Parameterized Constructor 
    Coord(int x_in, int y_in): x{x_in}, y{y_in}, visited{false} {}
  
    bool operator==(const Coord &rhs) const {
      return (this->x == rhs.x) && (this->y == rhs.y);
    } // operator==()

    int x, y;
    bool visited;

  }; // Coord{}


  struct Prims_c {

    // Constructor
    Prims_c(): distance{std::numeric_limits<double>::infinity()}, 
      parent{-1}, visited{false} {}

    // Parameterized Constructor
    Prims_c(double distance_in, int parent_in, bool visited_in) :
      distance{distance_in}, parent{parent_in}, visited{visited_in} {}

    double distance;
    int parent;
    bool visited;

  }; // Prims_c


  /*
  *  EFFECTS:  calculates the Euclidean distance between 2 Coords
  *            (in MST mode only) 
  *            returns std::numeric_limits<double>::infinity()
  *            if c1 and c2 are on different campuses
  *            and neither are on the border
  *            returns 0.0 if c1 == c2
  */
  double euclidean_dist(const Coord &c1, const Coord &c2);
  double euclidean_dist(const Coord &c1, const Coord &c2, 
      const size_t i, const size_t j);

  /*
  *  EFFECTS:   searches through the distmat to find the closest node to node
  *             that has not been visited
  *             Works on all modes
  *             May return -1 if there is no closest node
  *             (although this shouldn't really be possible)
  */
  std::pair<int, double> find_closest_node(int node);

  /*
  *  EFFECTS:  attempts to find an MST for the coordinates
  *            calls exit(1) and prints a message to cerr 
  *            if an MST cannot be found
  */
  void mst();

  /*
  *  EFFECTS:  attempts to find an MST from coords[start]
  *            created to avoid code duplication in opt_tsp()
  */
  void mst_helper(size_t start = 0);

  /*
  *  EFFECTS:  returns the index in path where inserting the node coords[to_add]
  *            results in the shortest increase in the overall distance
  */
  int find_min_insertion_idx(int to_add);

  /*
  *  EFFECTS:  prints the total distance and path taken by FASTTSP and OPTTSP
  */
  void print_total_and_path();

  /*
  *  MODIFIES: total_dist
  *  EFFECTS:  calculates the distance of the Hamiltonian circuit
  *            created by the nodes in path
  */
  void calc_total_dist();
  double calc_total_dist(size_t perm_length);

  /*
  *  EFFECTS:  attempts to find a solution to the TSP for the given coordinates
  *            using the random insertion heuristic
  */
  void fast_tsp();

  /*
  *   MODIFIES: prim, mst_dist
  *   EFFECTS:  returns true if a node is promising
  *             i.e. is (s_dist + e_dist + dist_so_far + mst_dist < opt_dist)
  */
  bool promising(size_t perm_length);

  /*
  *   MODIFIES: dist_so_far, path, opt_dist, opt_path
  *   EFFECTS:  recursively generates and checks all possible paths from
  *             perm_length to path[path.length() - 1];
  */
  void gen_perms(size_t perm_length);

  /*
  *  EFFECTS:  attempts to find the optimal TSP solution for the given 
  *            coordinates
  */
  void optimal_tsp();

  /*
  *   EFFECTS: returns true if c is on the border of the medical and normal
  *            campuses
  */
  bool is_border(const Coord &c) const;
  bool is_border(const int x, const int y) const;

  /*
  *   EFFECTS: returns true if c is on the medical campus
  */
  bool is_medical(const Coord &c) const;
  bool is_medical(const int x, const int y) const;

  // For use with OPTTSP mode only
  std::vector<std::vector<double>> distmat;
  // The index in these vectors is the node number (as it appeared
  // in the input file)
  std::vector<Prims_c> prim;
  std::vector<Coord> coords;
  // Path vectors
  std::vector<int> opt_path, path;

  double mst_dist, total_dist, opt_dist, dist_so_far;
  size_t num_nodes;
  Mode mode;
  bool mode_set, non_medical, medical, border;

}; // Delivery{}
