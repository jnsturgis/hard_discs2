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
    int     type, n;
    double  pos_x, pos_y, orient;
    config  *a_config = new config();

    if( argc < 4 ){
        fatal_error("%s\n", "Wrong number of arguments");
    }

    a_config->x_size = atof(argv[1]);
    a_config->y_size = atof(argv[2]);
    type = 0;
    while( type+3 < argc ){
        n = atoi(argv[3+type]);
        for(int i = 0; i < n; i++ ){
            pos_x  = rnd_lin(a_config->x_size);
            pos_y  = rnd_lin(a_config->y_size);
            orient = rnd_lin(M_2PI);
            a_config->add_object( new object( type, pos_x, pos_y, orient ));
        }
        type++;
    }

    a_config->write(stdout);

    return 0;
}
