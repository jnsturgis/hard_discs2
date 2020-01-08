
#define	MAXBIN		5000

#include "../Classes/config.h"
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

/*
**	Local routines
*/

void
usage()
{
    std::cerr << "Usage: pair_config file1 type1 type2 dx\n" ;
    std::cerr << "file1 contains a particle configuration,\n" ;
    std::cerr << "type1 and type2 are particle types to examine,\n" ;
    std::cerr << "dx is integration step in coordinate units.\n" ;
}

int
main( int argc, char **argv )
{
    config      	*a_config = (config *)NULL;
    bool		verbose = false;
    char        	c;
    char		*out_name = (char *)NULL;
    int			type1 = 0;
    int         	type2 = 0;
    double		dr = 1;
    double		r;
    int 		maxbin = 0;
    int 		newmax;
    int			bin;
    double		rmax;
    std::vector<int>	count;
    std::vector<double>	area;
    std::vector<double>	d_area;
    int			n_type2;
    double		x, y, x2, y2;

    // Getopt based argument handling.
    // Need to fix this for this programme...
    while( ( c = getopt (argc, argv, "vho:t:u:") ) != -1 )
    {
        switch(c)
        {
            case 'v': verbose = true; break;
            case 'r':				// step size
                if (optarg) dr = atof(optarg);  //TODO should check its a number
                break;            
            case 'o':				// Output file (default stdout)
                if (optarg) out_name = optarg;
                break;
            case 't':				// type 1 object (default 0)
                if (optarg) type1 = atoi(optarg); //TODO should check its a number
                break;
            case 'u':				// type 2 object (default 0)
                if (optarg) type2 = atoi(optarg);
                break;
            case 'h':
                usage();
                exit(EXIT_SUCCESS);
            case '?':				// Something wrong.
                if (optopt == 'o'){
                    std::cerr << "The -" << optopt << " option is missing a parameter!\n";
                } else {
                    std::cerr << "Unknown option " << optopt << "!\n";
                }
            default :                           // Something very wrong.
                usage();
                exit(EXIT_FAILURE);
        }
    }
    if( verbose ){
         std::cerr << "Verbose flag is set.\n";
         std::cerr << "Step size is " << dr << ".\n";
         std::cerr << "First object type is  " << type1 << ".\n";
         std::cerr << "Second object type is " << type2 << ".\n";
    }
// Read in the first configuration

    try{
        if(( argc - optind ) > 0 ){	                // Read source configuration
            a_config = new config( argv[ optind ] );
            if( verbose )
                std::cerr << "Input read from " << argv[ optind ] << "\n";
            optind++;
        } else {					
            a_config = new config(std::cin);
            if( verbose )
                std::cerr << "Input read from 'stdin'\n";
        }
    }
    catch(...){
        std::cerr << "Failed to read first configuration\n";
        std::cerr << "Program exiting\n";
        if( a_config ){
            delete a_config;
        }
        exit(EXIT_FAILURE);        
    }

    // Set up initial vectors for count and area... 

    // Start while loop over input files...
    do {
        if(verbose)
            std::cerr << "Starting treatment of first configuration\n";
        /// Adjust vector sizes if necessary.
        if( a_config->is_rectangle){
            rmax = sqrt( a_config->x_size * a_config->x_size + a_config->y_size * a_config->y_size );
            if( a_config->is_periodic ) rmax /= 2.0;
        } else {
            rmax = a_config->poly->max_dist();
        }
        newmax = 1 + floor( rmax/dr );
        if( newmax > maxbin ){
            count.resize(newmax);
            area.resize(newmax);
            d_area.resize(newmax);
            for(int i=maxbin; i<newmax; i++ ){
                count[i] = 0;
                area[i] = d_area[i] = 0.0;
            }
            maxbin = newmax;
        }

        if(verbose)
            std::cerr << "Array sizes adjusted\n";

        if( a_config->is_periodic ){ 		// Fill in d_area array as same for all objects
	    for( int i=0; i< maxbin; i++ ){
                double theta1 = 0.0;
                double theta2 = M_PI/2.0;
                if( i * dr > a_config->x_size / 2.0 ) theta1 = acos( a_config->x_size / (2.0 * i * dr ));
                if( i * dr > a_config->y_size / 2.0 ) theta2 = M_PI/2.0 - acos( a_config->y_size / (2.0 * i * dr ));
                d_area[i] = dr * (2*i+1) * (theta2 - theta1) * 2.0;
                d_area[i] /= a_config->area();
std::cerr << i << " " << d_area[i] << " "<< theta1 << " "<< theta2 << "\n";
            }
        }

        if(verbose)
            std::cerr << "Starting loop over objects\n";

        n_type2 = 0;
        for(int i=0; i< a_config->n_objects(); i++ )
            if(a_config->get_object(i)->o_type == type2) n_type2++;

        /// Loop over objects in configuration
        if( n_type2 > 0 )
        for(int i=0; i< a_config->n_objects(); i++ )
            if( a_config->get_object(i)->o_type == type1 ){
            x = a_config->get_object(i)->pos_x;
            y = a_config->get_object(i)->pos_y;
            //// Add to areas array
            if( ! a_config->is_periodic ){	// Can't use precalculated array as d_area depends on x,y
                for( int i=0; i< maxbin; i++ ) d_area[i] = 0.0;
                // TODO Fill d_area for x,y point with probablity at dist...
            }
            for( int i=0; i< maxbin; i++ ){     // Add probable number at each distance...
                area[i] += d_area[i] * n_type2 ;
            }
            //// Loop over other objects and add to count array
            for(int j=0; j<a_config->n_objects(); j++ )
                if( a_config->get_object(j)->o_type == type2 ){
                x2 = a_config->get_object(j)->pos_x;
                y2 = a_config->get_object(j)->pos_y;
                // If periodic get closest image to x,y
                if( a_config->is_periodic ){
                   if(  x2 < x ) x2 += a_config->x_size;
                   if(( x2 - x ) > (x - x2 + a_config->x_size)) x2 -= a_config->x_size;
                   if(  y2 < y ) y2 += a_config->y_size;
                   if(( y2 - y ) > (y - y2 + a_config->y_size)) y2 -= a_config->y_size;
                }
                r = (x2 - x)*(x2 - x)+(y2 - y)*(y2 - y);
                r = sqrt(r);
                bin = floor( r/dr );
                assert( bin <= maxbin );
                count[bin]++;
            }
        }
        /// Read in the next configuration if any

        if(verbose)
            std::cerr << "Look for another file\n";

        delete a_config;
        a_config = (config *)NULL;
        try{
            if(( argc - optind ) > 0 ){	                // Read source configuration
                a_config = new config( argv[ optind ] );
                if( verbose )
                    std::cerr << "Input read from " << argv[ optind ] << "\n";
            }
        }
        catch(...){
            std::cerr << "Failed to read configuration from " << argv[ optind ] << "\n";
            std::cerr << "Program exiting\n";
            if( a_config ){
                delete a_config;
            }
        }
        optind++;

    // End while loop when no more data to analyse
    } while (a_config != (config *)NULL );

    // Output the datafile to out_file or std::cout

    if(verbose)
        std::cerr << "Output results\n";

    FILE *dest = stdout;
    if(out_name != NULL){			// Open output stream if necessary
        dest = fopen(out_name, "w" );
	if( dest == NULL ){
	    std::cerr << "Unable to open" << out_name << "for writing, using stdout!\n";
            dest = stdout;
        }
    }
   
    assert( dest );
    //Should check area array is non-zero... truncate might be necessary
    //Write out r g(r) n(r) A(r)
    for(int i=0; i<maxbin; i++){
        fprintf(dest,"%f\t%g\t%d\t%g\n", (i+0.5)*dr, count[i]/area[i], count[i] , area[i] );
    }

    if( dest != stdout ) fclose( dest );
    return EXIT_SUCCESS;
}
