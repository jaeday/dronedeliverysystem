#include <iomanip> // setprecision
#include <iostream>
#include "Delivery.h"
using std::cout;
using std::cerr;
using std::endl;
using std::runtime_error;

int main(int argc, char **argv) {

  // The magic line of code
  std::ios_base::sync_with_stdio(false);

  // Always show 2 decimal places
  // Disable scientific notation for large numbers
  cout << std::setprecision(10) << std::fixed;
  
  Delivery d;

  try {
    d.get_options(argc, argv);
  }

  catch(runtime_error &e) {
    cerr << e.what() << endl;
    return 1;
  }

  d.read_input();
  d.solve();

  return 0;

} // main()