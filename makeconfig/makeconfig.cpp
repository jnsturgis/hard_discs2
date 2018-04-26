/**
 * \file    makeconfig.cpp
 * \author  James Sturgis
 * \brief   Main function for makeconfig application
 * \date    17 April 2018
 * \version 1.0
 *
 * This file contains the main routine for the make_config program that is part of
 * the Very Coarse Grained disc simulation programmes.
 *
 * The function of this program is to produce a configuration of the desired
 * proteins in the composition requested in a relatively random organization.
 *
 * The algorithm will create an empty configuration with the desired geometry
 * and then try to randomly place the objects into the space. The first two
 * parameters give the size of the configuration and then the following parameters
 * are the number of object of the different types, O, 1, ...
 *
 * \todo    Make sure that the configuration has non-infinate energy and if
 *          it does, take steps to find one that does not.
 */

#include "../NVT/config.h"
#include "../NVT/object.h"
#include "../NVT/common.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <boost/program_options.hpp>

using namespace std;

#define fatal_error(format, value) {\
                    fprintf(stderr, format, value ); \
                    usage(); \
                    exit(EXIT_FAILURE); \
                }

void usage(){
    fprintf(stderr, "Usage: makeconfig %s\n",
        "x_size y_size (n_type_0 (n_type_1 (...))");
}


int main(int argc, char **argv)
{
    int     n, xsize, ysize, type0, type1;
    double  pos_x, pos_y, orient;
    config  *a_config = new config();
    string  out_name;

    namespace po = boost::program_options;
    po::options_description desc("Generic options");
    desc.add_options()
        ("help", "Produce help message")
        ("xsize,x", po::value<int>(&xsize)->required(), "Size in the x direction")
        ("ysize,y", po::value<int>(&ysize)->required(), "Size in the y direction")
        ("type0,a", po::value<int>(&type0)->required(), "Number of type 0 atoms")
        ("type1,b", po::value<int>(&type1)->required(), "Number of type 1 atoms")
        ("output,o", po::value<string>(&out_name)->required(), "Output configuration name")
    ;

    // Store arguments
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    
    // Print the help if needed
    if (vm.count("help") || argc == 1) {
        cout << desc << "\n";
        return 1;        
    }
    
    po::notify(vm);

    a_config->x_size = xsize;
    a_config->y_size = ysize;
    
    // Handle each bead type separately
    // Throw an error if none have been provided
    if ( vm.count("type0") || vm.count("type1") ) {
        if (vm.count("type0")) {
            n = type0;
            for(int i = 0; i < n; i++ ){
                pos_x  = rnd_lin(a_config->x_size);
                pos_y  = rnd_lin(a_config->y_size);
                orient = rnd_lin(M_2PI);
                a_config->add_object( new object( 0, pos_x, pos_y, orient ));
            }
        }
        if (vm.count("type1")) {
            n = type1;
            for(int i = 0; i < n; i++ ){
                pos_x  = rnd_lin(a_config->x_size);
                pos_y  = rnd_lin(a_config->y_size);
                orient = rnd_lin(M_2PI);
                a_config->add_object( new object( 1, pos_x, pos_y, orient ));
            }
        }
    }
    else {
        cout << "Missing at least a bead number\n";
    }

    // Open a stream to write the output
    ofstream _out(out_name.c_str());
    
    // Check if we could open it
    if(!_out) {
        cout << "Cannot open file " << out_name << ", exiting ...\n";
        return 1;
    }
    
    // Write
    a_config->write(_out);

    return 0;
}
