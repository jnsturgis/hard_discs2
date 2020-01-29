/**
 * @brief	Wrap a programme to set a polygonal boundary as a convex hull 
 *          just big enough to hold the objects.
 *
 * The program is designed to find a polygon that wraps snugly around the
 * objects in a configuration. First it finds the set of points forming the
 * convex hull and then expands this sufficiently to include the finite sized
 * objects the original boundary is discarded and the new polygon is used as
 * the boundary.
 *
 */

#include "../Classes/config.h"
#include <iostream>

void
usage()
{
		std::cerr << "Usage: wrap [-v][-t topology] < input.config > output.config \n";
}

int
main( int argc, char **argv )
{
	char 			c;
	bool			verbose     = false;
	char			*topo_name  = (char *)NULL;
    config      	*a_config   = (config *)NULL;
	polygon			*a_poly     = (polygon *)NULL;
	topology		*a_topology = (topology *)NULL;
	
    // Getopt based argument handling.
    // Need to fix this for this programme...
    while( ( c = getopt (argc, argv, "vt:") ) != -1 )
    {
        switch(c)
        {
            case 'v': verbose = true; break;
            case 't':				// Output file (default stdout)
                if (optarg) topo_name = optarg;
                break;
            case 'h':
                usage();
                exit(EXIT_SUCCESS);
            case '?':				// Something wrong.
                if (optopt == 't'){
                    std::cerr << "The -" << optopt << " option is missing a parameter!\n";
                } else {
                    std::cerr << "Unknown option " << optopt << "!\n";
                }
            default :                           // Something very wrong.
                usage();
                exit(EXIT_FAILURE);
        }
    }
    if(verbose){
    	std::cerr << "Verbose flag is set. ";
    	if(topo_name){
    		std::cerr << "Loading topology from '" << topo_name << "'.\n";
    	} else {
    		std::cerr << "No topology file specified.\n";
    	}
    }

	// Read the configuration from std::cin
	
    try{
        a_config = new config(std::cin);
        if( verbose )
            std::cerr << "Input read from 'stdin'\n";
    }
    catch(...){
        std::cerr << "Failed while reading configuration, "
                  << "Program exiting\n";
        if( a_config ){
            delete a_config;
        }
        exit(EXIT_FAILURE);        
    }
    if(verbose)
    	std::cerr << "Configuration read\n";

	// Read the topology file if necessary and attach topology to configuration
	
    if( topo_name ){
        a_topology = new topology(topo_name);
    } else {
        a_topology = new topology(1.0);			// Default topology 1 unit circle
    }
    a_config->add_topology(a_topology);    		// Associate topology with the configuration,
    if( verbose )
        std::cerr << "Set up topology:\n";

	// The calculation
    a_poly = a_config->convex_hull(true);		// Calculate convex hull, expanded to include objects.
    a_config->set_poly(a_poly);					// Set as container
    
    // Output the result...

    try{
    	a_config->write(std::cout);				// Write new configuration to std::cout
    }
    catch(...){
        std::cerr << "Failed while writing new configuration, "
                  << "Program exiting\n";
        delete a_config;
        exit(EXIT_FAILURE);        
    }
    delete a_config;
    exit(EXIT_SUCCESS);
}