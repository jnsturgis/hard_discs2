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
 *      NVT [-vp][-t topology][-f forcefield][-o final_config][-c initial_config]
 *          [-l log_file] n_steps print_frequency beta pressure
 *
 * Where the various parameters are:
 *      n_steps         The number of simulation steps to make.
 *      print_frequency The number of steps between reports to the log file
 *                      of how the integration is progressing.
 *      beta            The temperature parameter 1/(kb T) that scales the
 *                      force field energies.
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
 *
 *      -n frame_freq	The frequency to save frames to the trajectory file.
 *
 *      -s traj_file	Optional file for logging the trajectory a gzipped format.
 *
 * \todo log file       Use a dedicated function for writing data so it is easier
 *                      to parse after and control the structure.  Perhaps in
 *                      xml format.
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "../Classes/integrator.h"
#include "../Classes/common.h"

#include "../Libraries/gzstream.h"

// program_options, to parse arguments
// #include <boost/program_options.hpp>
#include <boost/format.hpp>
// Smart pointer

using boost::format;

using namespace std;


#define fatal_error(format, value) {\
                    fprintf(stderr, format, value ); \
                    exit(EXIT_FAILURE); \
                }


void 
usage(int val){
    std::cerr << "NVT [-vp][-t topology][-f forcefield][-o final_config][-c initial_config]"
        << "[-l log_file] [-n save-frequency] [-s save_file] n_steps print_frequency beta pressure \n";
    exit(val);
}

/*
 *
 */
int main(int argc, char** argv) {
    // These are set up during command line handling.
    config      *current_state = NULL;
    force_field *the_forces = NULL; 
    topology    *a_topology = NULL;

    integrator  *the_integrator = NULL;
    int         N1;
    double      U1;
    int         i, step;
    double      dl_max = 1.0;

    // Initialization
    srand((long)&argv[0]);

    #define	NVT
    #include    "command_line.cpp"

    // Report on initial status

    i = 0;          // Counter for number of shifts.
    logger << "After " << i << " steps , P = " << pressure << ", beta = " << beta << "\n"; 
    current_state->report(logger, the_forces);

    // Jiggle everything to remove bad contacts from save/load
    dl_max = 0.5;   // Only small shifts if any needed.

    U1 = current_state->energy(the_forces);
    N1 = current_state->n_objects();

    if((U1 > the_forces->big_energy) && verbose ){
        logger << "Jiggle is necessary.\n";
    } else {
        logger << "No jiggle is necessary.\n";
    }
    
    while(U1 > the_forces->big_energy){
        if( the_integrator ) delete the_integrator;
        if( i > 2000*N1){
            delete the_forces;
            delete current_state;
            fatal_error("Unable to adjust initial configuration in %d steps", i );
        }
        the_integrator = new integrator(the_forces);
        the_integrator->dl_max = dl_max;
        config **state_h = &current_state;
        the_integrator->run(state_h, beta, pressure, 2*N1);
        current_state = *state_h;
        dl_max = the_integrator->dl_max;
        i += 2*N1;

        U1 = current_state->energy(the_forces);
        if( verbose ){
            logger << "after" << std::to_string( i ) << " steps\n";
            current_state->report(logger, the_forces);
        }
    }

    // If appropriate report on new initial condition
    if( the_integrator ){
        delete the_integrator;
        i = 0;
        logger << "After initial adjustments:\n";
        current_state->report(logger, the_forces);
    }

    // Start NVT montecarlo loop

    dl_max = simple_min(current_state->width(), current_state->height())/2.0;

    // Calculate next step size...
    step = simple_min(n_print,it_max);
    step = simple_min(step, traj_freq);
    the_integrator = new integrator(the_forces);
    the_integrator->dl_max = dl_max;

    if( verbose ){
        logger << "With" << (current_state->is_periodic?" ":"out ") << "periodic boundary conditions.\n";
        logger << "Boundary is " << (current_state->is_rectangle ? "rectangle" : "polygon") << "\n";
        logger << "Starting iteration loop\n";
    }

    for(i=0;i<it_max;){
 
         /** This part should be isolated for parallelization  **/
        config **state_h = &current_state;
        the_integrator->run(state_h, beta, pressure, step);
        current_state = *state_h;
        int j = i + step;
        if( j%n_print == 0 ){				// Is it time to print to the log file
            logger << "After " << i << " steps , P = " << pressure << ", beta = " << beta << "\n"; 
            current_state->report(logger, the_forces);
            the_integrator->report(logger);
        }
        if( j%traj_freq == 0 ){				// Is it time to print to the trajectory
            traj_stream << "====" << i << "====\n";
            current_state->write( traj_stream );
        }
        /** End of internal part **/
        i += step;
        step = simple_min(it_max-i+1,(n_print - (i%n_print)));
        step = simple_min(step,(traj_freq - (i%traj_freq)));
    }
    delete the_integrator;
    
    if( traj_stream.good() ){				// If we are writing a trajectory
        traj_stream.close();				// Close the file
    }
 
    if( verbose ) logger << "Writing final configuration.\n";
    if( out_name.length() > 0 ){
        std::ofstream out_file(out_name);
        current_state->write(out_file);
        out_file.close();
    } else {
        current_state->write(std::cout);
    }
    if( verbose ) logger << "Wrote configuration successfully.\n";

    delete current_state;
    delete the_forces;

    logger << "\n...Done...\n";

    // And close the log and output
    if( log_name.length() > 0 ){log_file.close();}

    return 0;
}
