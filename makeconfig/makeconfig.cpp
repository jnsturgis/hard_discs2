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
 * See makeconfig.md for details.
 */

#include "../Classes/config.h"
#include "../Classes/common.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <unistd.h>

using namespace std;

void usage()
{
    cerr << "Usage: makeconfig [-t topo_file][-o out_file][-f force_file][-d scale] \n\t";
    cerr << "x_size y_size n_obj0 ... \n";
}

int main(int argc, char **argv)
{
    int     	n, c;
    float   	x_size, y_size;
    double  	pos_x, pos_y, orient;
    char  	*out_name, *topo_name, *force_name;
    bool        verbose = false;

    config      *a_config = new config();
    force_field	*the_force;
    topology	*a_topology;

    float   scale = 1.0;
    out_name = force_name = topo_name = NULL;

    // Getopt based argument handling.

    while( ( c = getopt (argc, argv, "vd:f:t:o:") ) != -1 )
    {
        switch(c)
        {
            case 'v': verbose = true;
                break;
            case 'd':				// Handle optional arguments
                if (optarg) scale = std::atof(optarg);
                break;
            case 'f':
                if (optarg) force_name = optarg;
                break;
            case 't':
                if (optarg) topo_name = optarg;
                break;
            case 'o':
                if (optarg) out_name = optarg;
                break;
            case 'h':
                usage();
                return 0;
            case '?':				// Something wrong.
                if (optopt == 'd' or optopt == 'f' or optopt =='t' or optopt == 'o' ){
                    std::cerr << "The -" << optopt << "option is missing a parameter!\n";
                } else {
                    std::cerr << "Unknown option " << optopt << "!\n";
                }
            default :                           // Something very wrong.
                usage();
                return 1;
        }
    }
    if( verbose ){                              // Report on situation
        std::cerr << "Verbose flag set\n";
    }

    if(( argc - optind ) < 3 ){			// Check enough parameters
        std::cerr << "Not enough parameters!\n";
        usage();
        return 1;
    }

    x_size = std::atof( argv[ optind++ ] );	// Find size for configuration
    y_size = std::atof( argv[ optind++ ] );

    if((x_size * y_size) <= 0.0 ){
        std::cerr << "Negative or zero surface area!\n";
	usage();
        return 1;
    }
    // Setup topology and forcefield for insertions.
    if( force_name != NULL ){
        the_force  = new force_field(force_name); // Need to implement
    } else {
        the_force  = new force_field(1.0);	// Default force field 1 atom hard disk
    }

    if( topo_name != NULL){			// Why does topology depend on force field???!!!
        a_topology = new topology(topo_name);   // Need to implement
    } else {
        a_topology = new topology(1.0);		// Default topology 1 unit circle
    }

    a_config->add_topology(a_topology);         // Assocuate topology with the configuration.
    if( verbose ){
        std::cerr << "Set up topology:";
        a_topology->write( stderr );
        std::cerr << "================";
    }

    // Start adding things to the empty configuration
    x_size /= scale;				// Rescale space if requested.
    y_size /= scale;

    a_config->x_size = x_size;
    a_config->y_size = y_size;
    
    for( int i = 0; i < (argc - optind); i++) {
        n = std::atof( argv[ optind+i ] );
        for(int j = 0; j < n; j++ ){		// Try to place a new object of type i

            pos_x  = rnd_lin(a_config->x_size); // TODO: check OK
            pos_y  = rnd_lin(a_config->y_size);
            orient = rnd_lin(M_2PI);
            a_config->add_object( new object( i, pos_x, pos_y, orient ));
						// If fail then suggest larger and compress or crystal.
        }
    }

    a_config->expand( scale );			// Rescale configuration after placement.

    FILE *dest = stdout;
    if(out_name != NULL){			// Open output stream if necessary
        dest = fopen(out_name, "w" );
	if( dest == NULL ){
	    std::cerr << "Unable to open" << out_name << "for writing, using stdout!\n";
            dest = stdout;
        }
    }    
    assert( dest );

    // Write the configuration
    a_config->write(dest);

    if( dest != stdout ) fclose( dest );
    delete a_config;
    return 0;
}

