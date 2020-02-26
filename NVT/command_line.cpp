    /**
     * \file    command_line.cpp
     * \author  James Sturgis
     * \date    February 20, 2020
     * \version 1.0
     * \brief   Handle command lines for integrator programmes, to ensure homogeneity.
     * 
     * Before inclusion of this file the target integrator should be declared to 
     * permit selection of programme dependent parts.
     *
     * This code section expects the the presence of a usage( int return_code ) routine.
     *
     * The command lines that can be handled are...
     *    
     *      NVT [-vp][-t topology][-f forcefield][-o final_config][-c initial_config]
     *          [-l log_file] [-n frame_freq] [-s traj_name]
     *          n_steps print_frequency beta pressure
     *
     *      RX  [-vp][-t topology][-f forcefield][-o final_config][-c initial_config]
     *          [-l log_file] [-n frame_freq] [-s traj_name] [-r n_replica]
     *          n_steps print_frequency beta pressure
     *
     * Where the various parameters are:
     *      n_steps         The number of simulation steps to make.
     *      print_frequency The number of steps between reports to the log file
     *                      of how the integration is progressing.
     *      beta            The temperature parameter 1/(kb T) that scales the
     *                      force field energies. (the maximum in the series for RX)
     *      pressure        The pressure (this is not used but is for compatibility
     *                      with other ensembles such as NPT or the Gibbs ensemble).
     *
     *      -t topology     The topology file to use for the integration (required).
     *      -f forcefield   The forcefield file to use for the integration (required).
     *      -c              Optional flag for the initial configuration if none is
     *                      given the stdin will be read.
     *      initial_config  The name of an existing file containing a valid
     *                      configuration, that is read as the starting point.
     *      -o              Optional flag for the output if none is given then 
     *                      stdout will be used.
     *      final_config    The name of a file to which will be written the final
     *                      configuration, if a file with this name exists already
     *                      it is deleted.
     *      -l log_file     Optional file for logging output, if none is given then
     *                      stderr will be used. 
     *                      (For RX multiple extra log files will be used
     *                      one for each replica - the file names are constructed by
     *                      inserting before the suffix (if present) a different number
     *                      for each replica).
     *
     *      -n frame_freq   The frequency to save frames to the trajectory file.
     *
     *      -s traj_file    Optional file for logging the trajectory a gzipped format.
     *                      (For RX multiple trajectory files will be used
     *                      one for each replica - the file names are constructed by
     *                      inserting before the suffix (if present) a different number
     *                      for each replica).     *
     *      -r n_replica    The number of different replicas (RX only)    
     */

    #ifdef NVT
    #define	ARGSTRING   "vpc:f:t:o:l:n:s:"
    #endif
    #ifdef RX
    #define	ARGSTRING   "vpc:f:t:o:l:n:s:r:"
    #endif

    // Declarations of common variables.
    int         c;
    bool	verbose  = false;
    bool	periodic = false;
    int         it_max = 0;
    int         n_print = 0;
    int		traj_freq = 0;		// Frequency for saving frames to trajectory (0=never)
    double      beta = 1.0;
    double      pressure = 1.0;

    #ifdef RX
    int		n_replica = 1;		// Default number of replicas.
    #endif

    string       in_name;
    string       out_name;
    string       force_name;
    string       log_name;
    string       topo_name;
    string	 traj_name;

    while( ( c = getopt (argc, argv, ARGSTRING) ) != -1 )
    {
        switch(c)
        {
            case 'v': verbose  = true; break;
            case 'p': periodic = true; break;
            case 'c': if (optarg) in_name = optarg;
                break;
            case 'l': if (optarg) log_name = optarg;
                break;
            case 'f': if (optarg) force_name = optarg;
                break;
            case 't': if (optarg) topo_name = optarg;
                break;
            case 'o': if (optarg) out_name = optarg;
                break;
            case 'n': if (optarg) traj_freq = std::atoi(optarg);
                break;
            case 's': if (optarg) traj_name = optarg;
                break;
#ifdef RX
            case 'r': if (optarg) n_replica = std::atoi(optarg);
#endif
            case 'h': usage(EXIT_SUCCESS);
            case '?':				// Something wrong.
                if (optopt == 'c' or optopt == 'f' or optopt == 't' or 
                    optopt == 'o' or optopt == 'l' or optopt == 'n' or
                    optopt == 's' 
#ifdef RX
                    or optopt == 'r'
#endif
                    ){
                    std::cerr << "The -" << optopt << " option is missing a parameter!\n";
                } else {
                    std::cerr << "Unknown option " << optopt << "!\n";
                }
            default :                           // Something very wrong.
                usage(EXIT_FAILURE);
        }
    }

    if(( argc - optind ) != 4 ){	        // Check enough parameters
        std::cerr << "Not right number of parameters!\n";
        usage(EXIT_FAILURE);
    }

    it_max   = std::atoi( argv[ optind++ ] );	// Find size for configuration
    n_print  = std::atoi( argv[ optind++ ] );
    beta     = std::atof( argv[ optind++ ] );
    pressure = std::atof( argv[ optind++ ] );

    std::ofstream log_file;
    #define logger ((log_file.is_open())? log_file : std::cout )
    if( log_name.length() > 0 ){
       log_file.open( log_name, std::ofstream::out );
    }

    if( verbose ) logger << "Verbose flag set\n";
    if(( log_name.length() > 0 ) && verbose ) logger << "opened " << log_name << "as logfile.";

    if(it_max <= 0 ){
        std::cerr << "Nothing to do, number of steps invalid.\n";
	usage(EXIT_FAILURE);
    }
    if(n_print <= 0 ){
        std::cerr << "Negative or zero print frequency invalid.\n";
	usage(EXIT_FAILURE);
    }
    if( beta < 0 ){
        std::cerr << "Negative temperature invalid.\n";
	usage(EXIT_FAILURE);
    }
    if( pressure < 0 ){
        std::cerr << "Negative pressure invalid.\n";
	usage(EXIT_FAILURE);
    }

    if( verbose ) logger << "Reading configuration.\n";
    try{
        if( in_name.length() > 0 ){
            current_state = new config(in_name);
        } else {
            current_state = new config(std::cin);
        }
    }
    catch(...){
        std::cerr << "Error reading configuration aborting.\n";
        if( current_state ) delete current_state;
        exit( EXIT_FAILURE );
    }
    if( verbose ) logger << "Read configuration successfully.\n";

    // Load the force field from the force field file
    if( force_name.length() == 0 ){
        std::cerr << "Error the force field file was required but was not declared. Aborting.\n";
        if( current_state ) delete current_state;
        exit( EXIT_FAILURE );
    }

    if( verbose ) logger << "Reading force field from " << force_name << ".\n";
    try{
        the_forces = new force_field(force_name.c_str());
    }
    catch(...){
        std::cerr << "Error reading force field. Aborting.\n";
        delete current_state;
        if( the_forces ) delete the_forces;
        exit( EXIT_FAILURE );
    }
    if( verbose ){
        logger << "Read force_field successfully.\n";
        logger << "==============================\n";
        the_forces->write(logger);
        logger << "==============================\n";
    }
    // Load the topology from the topology file
    if( topo_name.length() == 0 ){
        std::cerr << "Error the topology file is required but was not declared. Aborting.\n";
        delete current_state;
        delete the_forces;
        exit( EXIT_FAILURE );
    }

    if( verbose ) logger << "Reading topology from" << topo_name << ".\n";
    try{
        a_topology = new topology(topo_name.c_str());
    }
    catch(...){
        logger << "Error reading topology. Aborting.\n";
        delete current_state;
        delete the_forces;
        if( a_topology ) delete a_topology;
        exit( EXIT_FAILURE );
    }
    if( verbose ){
        logger << "Read topology file successfully.\n";
        logger << "==============================\n";
        a_topology->write(logger);
        logger << "==============================\n";
    }

    // Setup to save trajectory
    if( traj_freq > 0 ){
        if( traj_name.length() == 0 ){
            std::cerr << "You must specify a file name for saving a trajectory (-s option)\n";
            delete current_state;
            delete the_forces;
            delete a_topology;
            exit( EXIT_FAILURE );
        }
        logger << "Snap shots will be saved every " << traj_freq << " steps\n";
    }

#ifdef NVT
    ogzstream	traj_stream;

    if( traj_freq > 0 ){
        traj_stream.open( traj_name.c_str() );
        if( ! traj_stream.good() ){
            std::cerr << "Error while opening file " << traj_name << " for the trajectory.\n";
            delete current_state;
            delete the_forces;
            delete a_topology;
            exit( EXIT_FAILURE );
        }
        if( verbose ){
            logger << traj_name << " opened for the trajectory.\n";
        }
    } else {
        traj_freq = it_max + 1;					// Don't want a trajectory
    }
#endif

    // Add the topology to the configuration.
    current_state->add_topology(a_topology);

    if( current_state->is_rectangle ){
        current_state->is_periodic = periodic;
    } else if( periodic ){					/// TODO convert parallelogram to rectangle
        if( current_state->poly->is_parallelogram() ){
            if( current_state->poly_2_rect() ){
                current_state->is_periodic = periodic;
            }
        }
        if( ! current_state->is_rectangle ){
            std::cerr << "Periodic conditions for non-rectangular configurations not supported - ignoring flag\n";
        }
    }

