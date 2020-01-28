/**
 * @file    makeconfig.cpp
 * @author  James Sturgis
 * @brief   Main function for makeconfig application
 * @date    17 April 2018
 * @version 1.0
 *
 * This file contains the main routine for the make_config program that is part of
 * the Very Coarse Grained disc simulation programmes.
 *
 * See makeconfig.md for details of usage and file formats.
 */

#include "../Classes/config.h"
// #include <stdio.h>
// #include <math.h>
#include <iostream>
#include <unistd.h>

using namespace std;

#define MAX_TESTS	1000

string placement_failure  = ""
"Fatal Error: Unable to place objects without "
"collisions! You could try changing the number of "
"attempts using the -a option, or try placing them "
"initially in a crystaline array with 'makecrystal', or "
"try initially placing them in a larger area and then "
"resizing the configuration with 'shrinkconfig'.";

void usage()
{
    std::cerr << "Usage: makeconfig [-v][-p][-t topo_file][-o out_file][-f force_file]"
        "[-d scale][-a attempts] \n\t x_size y_size n_obj0 ... \n";
}

int 
main(int argc, char **argv)
{
    int     	n, c;
    float   	x_size, y_size;
    double  	pos_x, pos_y, orient;
    char  	*out_name, *topo_name, *force_name;
    bool        verbose = false;
    bool        clash = true;
    int		max_try = MAX_TESTS;
    object      *my_object;

    config      *a_config = new config();
    force_field	*the_force;
    topology	*a_topology;
    a_config->is_periodic = false;

    float   scale = 1.0;
    out_name = force_name = topo_name = NULL;
    the_force = (force_field *)NULL;
    a_topology = (topology *)NULL;

    // Getopt based argument handling.

    while( ( c = getopt (argc, argv, "vpd:f:t:o:a:") ) != -1 )
    {
        switch(c)
        {
            case 'v': verbose = true; break;
            case 'p': a_config->is_periodic = true; break;
            case 'd':				// Handle optional arguments
                if (optarg) scale = std::atof(optarg);
                break;
            case 'a':				// Handle optional arguments
                if (optarg) max_try = std::atof(optarg);
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
                if (optopt == 'd' or optopt == 'f' or optopt =='t' or 
                    optopt == 'o' or optopt == 'a' ){
                    std::cerr << "The -" << optopt << " option is missing a parameter!\n";
                } else {
                    std::cerr << "Unknown option " << optopt << "!\n";
                }
            default :                           // Something very wrong.
                usage();
                exit(EXIT_FAILURE);
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
    try{
        if( force_name != NULL ){
            the_force  = new force_field(force_name); // Need to implement
        } else {
            the_force  = new force_field(1.0);	// Default force field 1 atom hard disk
        }
    }
    catch(...){
        std::cerr << "Unable to setup force field. Aborting!\n";
        if (the_force) delete the_force;
        delete a_config;
        return(EXIT_FAILURE);
    }

    try{
        if( topo_name != NULL){			// Why does topology depend on force field???!!!
            a_topology = new topology(topo_name);   // Need to implement
        } else {
            a_topology = new topology(1.0);		// Default topology 1 unit circle
        }
    }
    catch(...){
        std::cerr << "Unable to setup topology. Aborting!\n";
        if(a_topology) delete a_topology;
        delete the_force;
        delete a_config;
        return(EXIT_FAILURE);
    }

    a_config->add_topology(a_topology);         // Assocuate topology with the configuration.
    if( verbose ){
        std::cerr << "Set up topology:";
        a_topology->write( std::cerr );
        std::cerr << "================\n";
    }

    // Start adding things to the empty configuration
    x_size /= scale;				// Rescale space if requested.
    y_size /= scale;

    a_config->x_size = x_size;
    a_config->y_size = y_size;
    
    for( int i = 0; i < (argc - optind); i++) {
        n = std::atof( argv[ optind+i ] );
        if(verbose){
            std::cerr << "Adding " << n << " objects of type " << i << ".\n";
        }
        if( topo_name == NULL && i > 0){	// Need to add another simple molecule to the topology
            a_topology->add_molecule(1.0);
        }
        if( i >= (int)a_topology->n_molecules ){
            std::cerr << "The topology does not contain sufficient molecule types " << (i+1) << " required! Aborting!\n";
            delete a_config;
            if( the_force ) delete the_force;
            return EXIT_FAILURE;
        }

        for(int j = 0; j < n; j++ ){            // Try to place 'n' new objects of type i
            clash = true;
            for( int k = 0; ((k < max_try) && clash ); k++ ){
                pos_x  = rnd_lin(a_config->x_size);
                pos_y  = rnd_lin(a_config->y_size);
                orient = rnd_lin(M_2PI);
                my_object = new object( i, pos_x, pos_y, orient );
                clash = a_config->test_clash( my_object );
                if( clash ) delete my_object;
            }
            if( !clash ){
                a_config->add_object( my_object );
            } else {
		std::cerr << placement_failure;
                delete a_config;
                if(the_force) delete the_force;
                exit(EXIT_FAILURE);
            }
        }
    }
    if(verbose){
        std::cerr << "Finished placing objects.\n";
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
    if(the_force) delete the_force;
    return EXIT_SUCCESS;
}

