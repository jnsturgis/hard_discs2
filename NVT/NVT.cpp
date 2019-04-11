/**
 * \file    NVT.cpp
 * \author  James Sturgis
 * \date    April 6, 2018
 * \version 1.0
 * \brief   Run a trajectory in the NVT ensemble.
 *
 * This file contains the main routine for the NVT program that is part of
 * the Very Coarse Grained disc simulation programmes.
 *
 * The programme loads a configuration and then runs a monte carlo integration
 * in the NVT ensemble in which for each move a random object is selected and
 * moved to a new location and rotated to a new orientation. This modified
 * configuration is accepted according to the Metropolis criterion, the
 * energy is lower or the probability of the move at the simulation temperature,
 * calculated as e^(-dE beta), is higher than a uniform variant on the interval
 * 0..1. In this equation dE is the energy difference between the old and new
 * configurations, and beta (which is a parameter to the program) is 1/ kb T
 * where kb is the boltzman constant and T the absolute temperature.
 *
 * To use the program the command line is:
 *
 *      NVT n_steps print_frequency beta pressure initial_config final_config
 *
 * Where the various parameters are:
 *      n_steps         The number of simulation steps to make.
 *      print_frequency The number of steps between reports to the log file
 *                      of how the integration is progressing.
 *      beta            The temperature parameter 1/(kb T) that scales the
 *                      force field energies.
 *      pressure        The pressure (this is not used but is for compatibility
 *                      with other ensembles such as NPT or the Gibbs ensemble.
 *      initial_config  The name of an existing file containing a valid
 *                      configuration, that is read as the starting point.
 *      final_config    The name of a file to which will be written the final
 *                      configuration, if a file with this name exists already
 *                      it is deleted.
 *
 * The program does not use the standard input stream, but writes a log of progress
 * to the standard output stream (this can /should be redirected to a log file).
 * debugging and error messages are written to the standard error stream. The
 * program ends with the standard exit codes EXIT_SUCCESS or EXIT_FAILURE.
 *
 * Log file format:
 * The format of the log file is determined in this file by the print statements:
 *      lines 196-201   After loading the file.
 *      lines 228-232   After the initial adjustments
 *      lines 249-257   Every print_frequency steps during the integration.
 *      line 268        At the end of the program.
 *
 * Each report, except the last, contains 3 lines of slightly variable content.
 *
 * \todo log file       Use a dedicated function for writing data so it is easier
 *                      to parse after and control the structure.  Perhaps in
 *                      xml format.
 *
 * Configuration file format:
 * The configuration is read by the routine in config.cpp, and then object.cpp
 * It has a simple format:
 *      line 1          The area x and y dimensions.
 *      line 2          The number of objects.
 *      line 3-n        For each object four numbers:
 *                          the type (this refers to the topology)
 *                          the x and y positions should be in the area.
 *                          the orientation in radians.
 *
 * \todo config         Include non-rectangular surfaces in file.
 * \todo config         Include info on boundary conditions.
 * \todo config         Include comments.
 *
 * Topology:
 * The topology describes the relationship between objects and their constituent
 * atoms. It is currently hard coded in the file topology.cpp, lines 28-40. The
 * len[] array and data[][] matrix. The array len[] describes for each object i
 * the number of atoms it contains. The data matrix [][] in valid positions i, j
 * contains an atom with the properties (in order): atom_type, x_position,
 * y_position. The atom type refers to the atom_types used in the force_field file
 * and the positions are relative to the object position, which should be the
 * center of mass, at an orientation of 0.0 radians (or degrees).
 *
 * \todo topology       Read topology data from a file.
 *
 * Force field:
 * The force field describes the interactions between the different atom types.
 * Currently the force field is hard coded in the file force_field.cpp, lines
 * 11 to 21. The meaning of the different parts are as follows:
 *      BIGVALUE        A large finite number, used in place of infinity to avoid
 *                      NaN errors it should be less than MAX_DOUBLE divided by
 *                      twice the number of objects.
 *      my_radius[]     An array for each atom type of the hard radius of the
 *                      atom.
 *      my_color[]      An array for each atom type of the color to use for the
 *                      atom when drawing it to postscript (config2eps).
 *      my_energy[][]   A (symmetric) array of the potentiel well depths for
 *                      interactions between two different types of atom.
 *      my_cut_off      The distance between objects beyond which the interaction
 *                      energy is presumed to be 0.
 *      my_length       The length scale for interactions.
 *
 * \todo force_field    Convert lengths to by interaction basis (a matrix).
 * \todo force_field    Read force_field from a control file or a force field file.
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "../Classes/integrator.h"
#include "../Classes/common.h"

// program_options, to parse arguments
#include <boost/program_options.hpp>
#include <boost/format.hpp>
// Smart pointer

using boost::format;

using namespace std;

#define fatal_error(format, value) {\
                    fprintf(stderr, format, value ); \
                    usage(); \
                    exit(EXIT_FAILURE); \
                }

void usage(){
    fprintf(stderr, "Usage: NVT %s\n",
        "n_steps print_frequency beta pressure initial_config final_config");
}

/*
 *
 */
int main(int argc, char** argv) {
    
    // Use c++ string
    string       in_name;
    string       out_name;
    string       ff_filename;
    string       log_name;
    string       topology_file;
    
    // Objects in headers
    config      *current_state;
    config      **state_h;

    force_field *the_forces = NULL; 
    integrator  *the_integrator = NULL;
    topology    *a_topology = NULL;

    std::ofstream log_file;

    int         N1;
    double      U1, V1;
    int         i, step;

    /* Need to redo command line handling */
    // Barbaric hard-coded defaults
    int         it_max  =  10000;
    int         n_print =   1000;
    double      beta    =    1.0;
    double      dl_max  =  100.0;
    double      P1      =    1.0;

    // Initialization

    srand((long)&argv[0]);

    // Handle command line
    // Try with boost's program_options
    namespace po = boost::program_options;
    po::options_description desc("Generic options");
    desc.add_options()
        ("help", "Produce help message")
        // It seems easy to simply store things in variables
        ("nsteps,n", po::value<int>(&it_max), "Number of steps (default 10000)")
        ("pfreq,p", po::value<int>(&n_print), "Print frequency (default 1000)")
        ("beta,b", po::value<double>(&beta), "Beta (default 1)")
        ("pressure,r", po::value<double>(&P1), "Pressure (default 1)")
        ("forcefield,f", po::value<string>(&ff_filename)->default_value("forcefield"), "Force-field filename (default 'forcefield')")
        ("topology,t", po::value<string>(&topology_file)->default_value("topology"), "Topology filename (default 'topology')")
        ("logfile,l", po::value<string>(&log_name)->default_value("log"), "Log file (default 'log')")
        ("initial,c", po::value<string>(&in_name)->required(), "Initial configuration (input)")
        ("final,o", po::value<string>(&out_name)->required(), "Final configuration (output)")
    ;

    // Store arguments
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    
    // Print the help if needed or if it's run without arguments 
    if (vm.count("help") || argc == 1) {
        cout << desc << "\n";
        return 1;        
    }
    
    po::notify(vm);
    
    // Start the log file
    cout << "Opening log file '" << log_name << "'\n";
    log_file.open(log_name, std::ofstream::out);
    // Check if we could open it
    if(!log_file) {
        cout << "Cannot open file " << log_name << " for writing, exiting ...\n";
        return 1;
    }
    // Write the header of the log file
    log_file << "hard_discs2 log file - NVT\n\n"; 
    
    // Get the two file names
    if (vm.count("initial")) {
        cout << "Configuration file: " << in_name << "\n";
        log_file << "Configuration file: " << in_name << "\n";
    }
    
    if (vm.count("final")) {
        cout << "Output configuration file: " << out_name << "\n";
        log_file << "Output configuration file: " << out_name << "\n";
    }

    // Load the initial configuration
    /* On catching exceptions from constructors
     * After some reading, it doesn't seem like we need to handle
     * exceptions (try/catch block) if we don't need to propagate 
     * an exception from another constructed member within that constructor
     * So, keep it that way */
    // Should maybe pass a stream to the constructor, or a name is ok ?
    current_state = new config(in_name);
    
    // Load the force field from the force field file
    if (vm.count("forcefield")) {
        cout << "\nLoading the force-field from the file '" << ff_filename << "'\n";
    }
    the_forces = new force_field(ff_filename.c_str());
    cout << "Force-field loaded\n";
    log_file << "\nForce-field file: " << ff_filename << "\n";
    the_forces->write(log_file);
    
    // Load the topology from the topology file
    if (vm.count("topology")) {
        cout << "Loading the topology from the file '" << topology_file << "'\n";
    }
    a_topology = new topology( topology_file.c_str() );    // Create or load the object topologies
    cout << "Topology loaded\n";
    log_file << "Topology file: " << topology_file << "\n";
    // a_topology->write(log_file);                          // Does not exist currently
 
    // Add the topology to the configuration.
    current_state->add_topology(a_topology);
        
    U1 = current_state->energy(the_forces);
    V1 = current_state->area();
    N1 = current_state->n_objects();

    // Print report of state, both in terminal and log
    cout << "Configuration loaded\n\n";
    log_file << "Configuration loaded\n";
    cout << format("N objects = %9d Pressure = %9g   Beta = %9g\n") % N1 % P1 % beta;
    cout << format("Area      = %9g  Density = %9g Energy = %9g\n\n") % V1 % (N1/V1) % U1;
    log_file << format("N objects = %9d Pressure = %9g   Beta = %9g\n") % N1 % P1 % beta;
    log_file << format("Area      = %9g  Density = %9g Energy = %9g\n\n") % V1 % (N1/V1) % U1;

    dl_max = simple_min(current_state->x_size, current_state->y_size)/2.0;

    // Jiggle everything to remove bad contacts from save/load

    i = 0;          // Counter for number of shifts.
    while(U1>the_forces->big_energy){
        if( i> 2000*N1 ){
            fatal_error(
                "Unable to adjust initial configuration in %d steps", i );
        }
        the_integrator = new integrator(the_forces);
        the_integrator->dl_max = dl_max;
        state_h = &current_state;
        the_integrator->run(state_h, beta, P1, 2*N1);
        current_state = *state_h;
        dl_max = the_integrator->dl_max;
        i += 2*N1;

        U1 = current_state->energy(the_forces);
    }

    if( the_integrator ){
        delete the_integrator;
        i = 0;
        cout << "After initial adjustments:\n";
        cout << format("N objects = %9d Pressure = %9g   Beta = %9g\n") % N1 % P1 % beta;
        cout << format("Area      = %9g  Density = %9g Energy = %9g\n\n") % V1 % (N1/V1) % U1;
        log_file << "After initial adjustments:\n";
        log_file << format("N objects = %9d Pressure = %9g   Beta = %9g\n") % N1 % P1 % beta;
        log_file << format("Area      = %9g  Density = %9g Energy = %9g\n\n") % V1 % (N1/V1) % U1;
    }

    // Start NVT montecarlo loop
    step = simple_min(n_print,it_max);
    the_integrator = new integrator(the_forces);
    the_integrator->dl_max = dl_max;

    for(i=0;i<it_max;i+=step){
        state_h = &current_state;
        the_integrator->run(state_h, beta, P1, step);
        current_state = *state_h;

        U1 = current_state->energy(the_forces);
        V1 = current_state->area();
        N1 = current_state->n_objects();

        cout << format("After %d steps N = %d, P = %g, beta = %g\n") 
                % (i+step) % N1 % P1 % beta;
        cout << format("Area = %g, Density = %g Energy = %g\n") 
                % V1 % (N1/V1) % U1;
        cout << format("Moves %d in %d, Dist_max = %g\n\n") 
                % (the_integrator->n_good)
                % (the_integrator->n_good + the_integrator->n_bad)
                % (the_integrator->dl_max);
        log_file << format("After %d steps N = %d, P = %g, beta = %g\n") 
                % (i+step) % N1 % P1 % beta;
        log_file << format("Area = %g, Density = %g Energy = %g\n") 
                % V1 % (N1/V1) % U1;
        log_file << format("Moves %d in %d, Dist_max = %g\n\n") 
                % (the_integrator->n_good)
                % (the_integrator->n_good + the_integrator->n_bad)
                % (the_integrator->dl_max);

        step = simple_min(step,it_max-i);
    }
    delete the_integrator;
    
    // Save result - call write with an open stream
    ofstream _out(out_name.c_str());
    
    // Check if we could open it
    if(!_out) {
        cout << "Cannot open file " << out_name << " for writing, exiting ...\n";
        return 1;
    }
    
    // Pass it to the config to write
    current_state->write(_out);
    // Clean up
    //~ delete a_topology;
    delete current_state;
    delete the_forces;

    cout << "\n...Done...\n";
    log_file << "\n...Done...\n";

    // And close the log and output
    log_file.close();
    _out.close();
    return 0;
}
