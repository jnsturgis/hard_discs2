/**
 * @file    shrinkconfig.cpp
 * @author  James Sturgis
 * @brief   Main function for shrinkconfig application
 * @date    21 April 2018
 * @version 1.0
 *
 * This file contains the main routine for the shrink_config program that is part of
 * the Very Coarse Grained disc simulation programmes.
 *
 * See shrinkconfig.md for details of usage and file formats.
 */

#include "../Classes/config.h"
// #include <stdio.h>
// #include <math.h>
#include <iostream>
#include <unistd.h>

using namespace std;

void usage()
{
    std::cerr << "Usage: shrinkconfig [-v][-p][-t topo_file][-o out_file][-f force_file]"
        "[-s scale_factor][-a attempts] [source] \n";
}

int 
main(int argc, char **argv)
{
    int     	c;

    config      *a_config = (config *)NULL;
    topology	*a_topology = (topology *)NULL;
 
    double      scale = 1.0;                    // Set default values
    char        *out_name, *topo_name;
    out_name = topo_name = NULL;
    bool        verbose = false;
    int         max_try = 1.0;

    // Getopt based argument handling.

    while( ( c = getopt (argc, argv, "hvs:t:o:a:") ) != -1 )
    {
        switch(c)
        {
            case 'v': verbose = true; break;
            case 's':				// Handle optional arguments
                if (optarg) scale = std::atof(optarg);
                break;
            case 'a':				// Handle optional arguments
                if (optarg) max_try = std::atof(optarg);
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
                if (optopt == 's' or optopt =='t' or 
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
        std::cerr << "Options parsed\n";
        std::cerr << "Scale factor is " << scale << "\n";
        std::cerr << "Attempts is " << max_try << "\n";
        if( topo_name ){
            std::cerr << "Topology file is " << topo_name << "\n";
	} else {
            std::cerr << "No Topology file specified\n";
	}
        if( out_name ){
            std::cerr << "Output will be sent to " << out_name << "\n";
	} else {
            std::cerr << "Output will be sent to 'stdout'\n";
	}
    }

    try{
        if(( argc - optind ) > 0 ){	                // Read source configuration
            a_config = new config( argv[ optind ] );
            if( verbose )
                std::cerr << "Input read from " << argv[ optind ] << "\n";
        } else {
            a_config = new config(std::cin);
            if( verbose )
                std::cerr << "Input read from 'stdin'\n";
        }
    }
    catch(...){
        std::cerr << "Failed to read configuration\n";
        std::cerr << "Program aborting\n";
        if( a_config ){
            delete a_config;
        }
        exit(EXIT_FAILURE);        
    }

    try{
        if( topo_name ){			    // Why does topology depend on force field???!!!
            a_topology = new topology(topo_name);   // Need to implement
        } else {
            a_topology = new topology(1.0);         // Default topology 1 unit circle
            for( int i = 0; i < a_config->object_types(); i++ ){
                a_topology->add_molecule(1.0);
            }
        }
    }
    catch(...){
        std::cerr << "Failed to read topology\n";
        std::cerr << "Program aborting\n";
        delete a_config;
        if( a_topology ){
            delete a_topology;
        }
        exit(EXIT_FAILURE);        
    }

    if( ((int) a_topology->n_molecules) <= a_config->object_types() ){
        std::cerr << "Not enough molecule types in the topology " << a_config->object_types()+1 << " are required.\n";
        delete a_config;
        delete a_topology;
        exit(EXIT_FAILURE);
    }

    if( verbose ){
        std::cerr << "Topology is loaded\n";
        a_topology->write( std::cerr );
        std::cerr << "================\n";
    }

    a_config->add_topology(a_topology);         // Associate topology with the configuration.
    a_topology = (topology *)NULL;		// Unnecessary but to be tidy and avoid double deletes.

    if(a_config->expand( scale , max_try )){    // Rescale configuration after placement.
        if( verbose ){
            std::cerr << "Unable to remove clashes... try increasing attempts or scale\n";
        }
        delete a_config;
        return EXIT_FAILURE;
    }

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
    return EXIT_SUCCESS;
}

