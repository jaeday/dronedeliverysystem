// Project Identifier: 1761414855B69983BD8035097EFBD312EB0527F0

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <stack>
#include <cmath>
#include <ctype.h>
#include "routeplanning.h"
using namespace std;

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    try {
        // Instantiate
        
        controlcenter lmao;
        
        
        // Read and process the command line options.
        lmao.get_options(argc, argv);

        lmao.readInput();

        lmao.OPTTSP();
        lmao.OPTPrint();
        return 0;


        

    }

    // Catch runtime_errors, print the message, and exit the
    // program with a non-zero status.
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // All done!


    return 0;
}