/**
 * @brief	2DOrder a programme to analyse the organization around non 
 *          symmetric objects in a configuration.
 *
 * The program is designed take a configuration and analyse for each 
 * object, of a selected type, the organization of objects of a second
 * type around it. This is conceived as an extension of the pcf, where
 * not only the radial distribution is considered but also angular 
 * dependence and examined is not only the organization of distribution
 * but also of orientation.
 *
 * The program should work correctly on all types of configuration,
 * that is those with rectangular or polygonal boundaries, those with
 * periodic boundary conditions or not.
 *
 * Input can be in the form of either a series of configurations or
 * a compressed trajectory file.
 *
 * TODO Stop error condition on normal end of file
 */

#include "../Classes/config.h"
#include <iostream>
#include "../Libraries/gzstream.h"

/***
 * @brief usage() Print a brief help message on program utilisation to std::cerr.
 */
void
usage()
{
    std::cerr << "Usage: 2DOrder [-v] [-z] [-o output] [-d dist] [-r rotation][-t type1] [-u type2] file1...\n" ;
    std::cerr << "-v verbose output to stderr,\n" 
        << "-z the input files are compressed trajectory files,\n"
        << "-o output send output to file output (default stdout),\n" 
        << "-d dist set the integration bin size to dist (default 1.0),\n" 
        << "-r rotation, symmetry to apply for organization of orientation (default 1),\n "
        << "-t type1 look at distances between objects of this type and type2 (default 0),\n" 
        << "-u type2 look at distances between objects of this type and type1 (default 0),\n" 
        << "file1... series of configuration or trajectory files to read, if none are given use stdin.\n" ;
}

/***
 * Main program
 */
int
main( int argc, char **argv )
{
	// Initialize program control variables
	double	dist 		= 1.0;
	int		rotation 	= 1;
	char	*out_name	= (char *)NULL;
	int		type1		= 0;
	int		type2		= 0;
	bool	verbose		= false;
	bool	trajectory	= false;
	char	c;
	
    // Getopt based argument handling.
    while( ( c = getopt (argc, argv, "vhzo:d:r:t:u:") ) != -1 )
    {
        switch(c)
        {
            case 'v': verbose = true; break;
            case 'z': trajectory = true; break;
            case 'd':				// rotational symmetry parameter
                if (optarg) dist = atof(optarg);
                break;            
            case 'r':				// rotational symmetry parameter
                if (optarg) rotation = atoi(optarg);
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
                if (optopt == 'o' or optopt == 'r' or optopt == 'd' or optopt == 't' or optopt == 'u'){
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
         std::cerr << "The verbose flag is set.\n"
                   << "Reading " << (trajectory?"":"un") << "compressed trajectories.\n"
                   << "Step size is " << dist << ".\n"
                   << "Rotational parameter is " << rotation << ".\n"
                   << "First object type is  " << type1 << ".\n"
                   << "Second object type is " << type2 << ".\n"
                   << "Sending output to " << ((out_name)?out_name:"str::cout");
         std::cerr << ".\n";
    }
    
    // TODO Should seed random number generator somewhere here.

    // Read in the first configuration

    igzstream	traj_stream;					// Stream for compressed trajectory
    											// A buffer size (current separator is 9 + number)
    #define		LINE_LENGTH		128				
	char		line[LINE_LENGTH];				// Buffer for frame separators.	
	config		*a_config = (config *)NULL;		// Where to find the configuration being treated.
	
    try{
        if(( argc - optind ) > 0 ){	            // Read source configuration
        	if( trajectory ){
        		traj_stream.open( argv[ optind ] );
				if( ! traj_stream.good()) throw(1);
        		traj_stream.getline(line, LINE_LENGTH);		// Separator
        		a_config = new config( traj_stream );
    	        if( verbose )
        	        std::cerr << "Input read " << line 
        	            << " from " << argv[ optind ] << "\n";        		
        	} else {
                a_config = new config( argv[ optind ] );
    	        if( verbose )
        	        std::cerr << "Input read from " << argv[ optind ] << "\n";
        	}
            optind++;
        } else {					
            a_config = new config(std::cin);
            if( verbose )
                std::cerr << "Input read from 'stdin'\n";
        }
    }
    catch(...){
        std::cerr << "Failed to read first configuration\n"
                  << "Program exiting\n";
        if( a_config ) delete a_config;
        exit(EXIT_FAILURE);        
    }

	if(verbose)
		std::cerr << "Conditions are " << ((a_config->is_periodic)?" ":"not ") << "periodic\n";
	// Initialize arrays for accumulating results.
	double	**e_array;
	double	**de_array;
	double	**n_array;
	double	**dx_array;
	double	**dy_array;
	
	int		bin_x, bin_y, bin_xmax, bin_ymax;
	double	theta;
	double	r_max = sqrt(a_config->width()*a_config->width()+a_config->height()*a_config->height());
	if(a_config->is_periodic) r_max /= 2.0;
	bin_xmax = bin_ymax = 2*floor(r_max/dist)+1;	// 2 for left and right + 1 for middle		
	
	if(verbose)
		std::cerr << "Maximum distance is " << r_max << " maximum bin is " << bin_xmax << ".\n";

	// Allocate arrays
	e_array  = new double*[bin_xmax];
	de_array = new double*[bin_xmax];
	n_array  = new double*[bin_xmax];
	dx_array = new double*[bin_xmax];
	dy_array = new double*[bin_xmax];
	for(int i=0; i< bin_xmax; i++ ){
		e_array[i]  = new double[bin_ymax];
		de_array[i] = new double[bin_ymax];
		n_array[i]  = new double[bin_ymax];
		dx_array[i] = new double[bin_ymax];
		dy_array[i] = new double[bin_ymax];
	}
	for(int k=0; k < bin_xmax; k++)
	    for(int l=0; l< bin_ymax; l++){
	    	e_array[k][l] =
	    	n_array[k][l] =
	    	dx_array[k][l] =
	    	dy_array[k][l] = 0.0;
	}
	
	if(verbose)
		std::cerr << "Arrays allocated.\n";

    // For periodic conditions precalculate de array
    if(a_config->is_periodic){
    	for(int k=0; k < bin_xmax; k++)
    	    for(int l=0; l< bin_ymax; l++){
    	        de_array[k][l] = (1.00/((bin_xmax-1.00)*(bin_ymax-1.00)));
    	        if(k == 0)            de_array[k][l] /= 2.0;
    	        if(l == 0)            de_array[k][l] /= 2.0;
    	        if(k == (bin_xmax-1)) de_array[k][l] /= 2.0;
    	        if(l == (bin_ymax-1)) de_array[k][l] /= 2.0;
    	    }
		if(verbose)
			std::cerr << "Precalculated de_array for periodic conditions.\n";
    }
    
    do {
    	for(int i = 0; i < a_config->n_objects(); i++ ){
    		if(a_config->get_object(i)->o_type == type1 ){
    			// Calculate de_array if necessary (non-periodic conditions)
    			if(!a_config->is_periodic){
					bool is_inside;
   					int	counter = 0;
					double x, y;
    				for(int k=0; k < bin_xmax; k++){
    					for(int l=0; l < bin_ymax; l++){
    						de_array[k][l] = 0.0;
    					}
    				}
					{
    					int n_tries = (a_config->width()/dist)*(a_config->height()/dist);	// Maximum number of bins
    					n_tries *= 1000;													// Factor determining precision
    					for(int k=0; k<n_tries; k++){
    						x = rnd_lin(a_config->width());
    						y = rnd_lin(a_config->height());
    						if(a_config->is_rectangle){
    							is_inside = true;
    						} else {
    							x += a_config->poly->x_min();
    							y += a_config->poly->y_min();
    							is_inside = a_config->poly->is_inside(x,y);
    						}
    						if(is_inside){
	    						// Calculate bin number.    					
    							double dx = a_config->get_object(i)->pos_x - x;
    							double dy = a_config->get_object(i)->pos_y - y;
	    						theta = atan2(dy, dx);
    							theta += a_config->get_object(i)->orientation;
    							double r = sqrt(dx*dx+dy*dy);
    							dx = r * sin(theta);
    							dy = r * cos(theta);

    							bin_x = floor( bin_xmax * (r_max+dx+dist/2.0)/(2.0*r_max));
    							bin_y = floor( bin_ymax * (r_max+dy+dist/2.0)/(2.0*r_max));
    							de_array[bin_x][bin_y] += 1.0;
    							counter++;
    						}
    					}
    				}
    				for(int k=0; k < bin_xmax; k++){
    					for(int l=0; l < bin_ymax; l++){
    						de_array[k][l] = de_array[k][l]/counter;
    					}
    				}
    			}
    			for(int j = 0; j < a_config->n_objects(); j++ ){
    				if(a_config->get_object(j)->o_type == type2 ){
   					
    					// Calculate bin number.    					
    					double dx = a_config->get_object(j)->pos_x - a_config->get_object(i)->pos_x;
    					double dy = a_config->get_object(j)->pos_y - a_config->get_object(i)->pos_y;
    					
    					// If necessary adjust for closest image.
    					if(a_config->is_periodic){
    						if( dx >  a_config->width() /2.0 ) dx -= a_config->width() /2.0; 
    						if( dy >  a_config->height()/2.0 ) dy -= a_config->height()/2.0; 
    						if( dx < -a_config->width() /2.0 ) dx += a_config->width() /2.0; 
    						if( dy < -a_config->height()/2.0 ) dy += a_config->height()/2.0; 
    					}
    					
    					theta = atan2(dy, dx);
    					theta += a_config->get_object(i)->orientation;
    					double r = sqrt(dx*dx+dy*dy);
    					dx = r * sin(theta);
    					dy = r * cos(theta);

    					bin_x = floor( bin_xmax * (r_max+dx+dist/2.0)/(2.0*r_max));
    					bin_y = floor( bin_ymax * (r_max+dy+dist/2.0)/(2.0*r_max));
    					// Increment arrays of <n>, n
    					n_array[bin_x][bin_y] += 1.0;
    					for(int k=0; k < bin_xmax; k++)
    					    for(int l=0; l < bin_ymax; l++ )
    					        e_array[k][l] += de_array[k][l];
    					// Calculate relative orientation dx, dy including symmetry rotation #
    					theta = a_config->get_object(i)->orientation - a_config->get_object(j)->orientation;
    					theta *= rotation;
    					
    					// Increment dx, dy arrays
    					dx_array[bin_x][bin_y] += sin(theta);
    					dy_array[bin_x][bin_y] += cos(theta);
    				}
    			}
    		}
    	}
    	
    	// Finished with this configuration move on to next if there is one.
    	// (currently normal file termination and error conditions are not properly distinguished)
    	
    	delete a_config;
    	a_config = (config *)NULL;
    	
    	// Try to read a new configuration from trajectory
    	if(trajectory){
    		if( traj_stream.good() ){
    			try{
    				traj_stream.getline(line, LINE_LENGTH);		// Separator
        			a_config = new config( traj_stream );
    	        	if( verbose )
        	        	std::cerr << "Input read " << line 
        	            	<< " from " << argv[ optind - 1 ] << "\n";        		
    			}
    			catch(...){
    				if( !traj_stream.eof() )
    					std::cerr << "Error reading trajectory file\n";
    				traj_stream.close();
    				if(a_config){
    					delete a_config;
    					a_config = (config *)NULL;
    				}
    			}
    		} else {
    			if(verbose)
    				std::cerr << "End of trajectory file\n";
    		}
    	}
    	// if still no config try reading another file
    	if(( !a_config ) && (( argc - optind ) > 0)){
    		try {
	        	if( trajectory ){
    	    		traj_stream.open( argv[ optind ] );
        			if( ! traj_stream.good()) throw(1);
        			traj_stream.getline(line, LINE_LENGTH);		// Separator
        			a_config = new config( traj_stream );
    	        	if( verbose )
        	        	std::cerr << "Input read " << line 
        	            	<< " from " << argv[ optind ] << "\n";        		
        		} else {
                	a_config = new config( argv[ optind ] );
    	     	    if( verbose )
        	    	    std::cerr << "Input read from " << argv[ optind ] << "\n";
        		}
    			optind++;
    		}
    		catch(...){
    			std::cerr << "Error reading " << argv[optind] << ". Premature termination.\n";
    			if (a_config){
    				delete a_config;
    				a_config = (config *)NULL;
    			}
    		}
    	}
    } while ( a_config != (config *)NULL );
    
    if(verbose)
    	std::cerr << "Calculations finished... writing results.\n";

    // Output results
												///////////////////////////////////
    std::streambuf * buf;						// These lines set dest to write
    std::ofstream of;							// either to an ofstream (of)
    if(out_name) {								// or to std::cout depending 
        of.open(out_name);						// on if a filename 'out_name'
        buf = of.rdbuf();						// has been defined by recovering
    } else {									// the appropriate output buffer
        buf = std::cout.rdbuf();				// and attaching it to the ostream
    }											// dest. NB no error handling.
	std::ostream dest(buf);						///////////////////////////////////
	
	dest << bin_xmax << " " << bin_ymax << "\n";
    for(int k=0; k < bin_xmax; k++)
        for(int l=0; l < bin_ymax; l++){
        	dest << k << " " << l << " " << n_array[k][l]/e_array[k][l] << " " 
        	     << e_array[k][l] << " " << dx_array[k][l] << " " << dy_array[k][l] << "\n";
        }

    if(out_name) of.close();
    
    if(verbose)
    	std::cerr << "Output finished... tidying up.\n";
    	
    for(int i=0; i< bin_xmax; i++ ){
		delete[] e_array[i];
		delete[] de_array[i];
		delete[] n_array[i];
		delete[] dx_array[i];
		delete[] dy_array[i];
	}

	delete[] e_array;
	delete[] de_array;
	delete[] n_array;
	delete[] dx_array;
	delete[] dy_array;
}
