/**
 * \file    RX.cpp
 * \author  James Sturgis
 * \date    February 1, 2018
 * \version 1.0
 * \brief   Run a collection of trajectories at different temperatures in the NVT ensemble.

TODO: Define command line as NVT compatible as possible...

 */

#include <cstdlib>

// #include <filesystem>
// namespace fs = std::filesystem;
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <thread>
#include "../Classes/integrator.h"
#include "../Classes/common.h"

#include "../Libraries/gzstream.h"

using namespace std;

#define fatal_error(format, value) {\
                    fprintf(stderr, format, value ); \
                    exit(EXIT_FAILURE); \
                }

void 
usage(int val){
    std::cerr << "RX [-vp][-t topology][-f forcefield][-o final_config][-c initial_config] "
        << "[-l log_file] [-n save-frequency] [-s save_file] [-r n_replica] "
        << "n_steps print_frequency beta pressure \n";
    exit(val);
}

#define EXCHANGE_FREQ	current_state->n_objects()

std::string
make_name( string root, int index )
{
	std::stringstream ss;
	fs::path p(root);
	
	assert(index<1000);			// Feature no more than 999 replicas
	
	ss << p.stem() << p.filename();
	ss << std::setw(3) << std::setfill('0') << index;
	ss << "." << p.extension();
	
    std::string s = ss.str();
	return s;
}

int main(int argc, char** argv) {

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

#define RX
#include "command_line.cpp"

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
        if( i > 2000*N1 ){
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

    // Set up multiple temperatures, integrators, logfiles, configurations and handles, trajectory files if necessary

    std::vector<int>            order(n_replica);		// Which replica is at which temperature.
    std::vector<double>         betas(n_replica);
    std::vector<config *>       states(n_replica);
    std::vector<config **>      handles(n_replica);
    std::vector<integrator *>   integrators(n_replica);
    std::vector<ogzstream>		traj_streams(n_replica);
    std::vector<ofstream>		logs(n_replica);
    std::vector<double>			energies(n_replica);
    std::vector<thread *>       replicas(n_replica);

    // Should have some error handling in this section...
    // Set up the different replicas

    for(int i=0; i < n_replica; i++ ){
        order[i]       = i;
        betas[i]       = beta * (i+1) / n_replica;
        states[i]      = new config( current_state );
        handles[i]     = &states[i];
        integrators[i] = new integrator( the_forces );

        if( traj_freq > 0 ){
            traj_streams[i].open( make_name(traj_name, i ).c_str() );
            if( verbose ){
                logger << make_name(traj_name, i ) << " opened for replica trajectory.\n";
            }
        }

       if( log_name.length() > 0 ){
           logs[i].open( make_name(log_name, i), std::ofstream::out );
            if( verbose ){
                logger << make_name(log_name, i ) << " opened for replica log.\n";
            }
       }

    }

    if( traj_freq <= 0 ) traj_freq = it_max + 1;
    dl_max = simple_min(current_state->width(), current_state->height())/2.0;

    // Calculate next step size...
    step = simple_min(n_print,it_max);
    step = simple_min(step, traj_freq);
    step = simple_min(step,EXCHANGE_FREQ);

    // Loop until number of steps done...
    if( verbose ){
        logger << "With" << (current_state->is_periodic?" ":"out ") << "periodic boundary conditions.\n";
        logger << "Boundary is " << (current_state->is_rectangle ? "rectangle" : "polygon") << "\n";
        logger << "Starting iteration loop\n";
    }

    for(i=0;i<it_max;){

		for( int r=0; r<n_replica; r++ ){		// Launch threads
            /* replicas[r] = new thread( integrators[j]->run, handles[r], betas[r], pressure, step); */
         	/** This part should be parallelized  **/
        	handles[r] = &states[i];
        	the_integrator->run(handles[r], betas[r], pressure, step);
        	states[r] = *handles[r];
        	int j = i + step;
        	if( j%n_print == 0 ){				// Is it time to print to the log file
            	logs[r] << "After " << i << " steps , P = " << pressure << ", beta = " << betas[r] << "\n"; 
            	current_state->report(logs[r], the_forces);
        	    the_integrator->report(logs[r]);
    	    }
			if( j%traj_freq == 0 ){				// Is it time to print to the trajectory
            	traj_streams[r] << "====" << i << "====\n";
        	    states[r]->write( traj_streams[r] );
    	    }
    	    energies[r] = states[r]->energy(the_forces);
    	    
	        /** End of parallel internal part **/
        }

	    for( int r=0; r<n_replica; r++ ){		// Wait for threads to stop
			/* replicas[r]->join(); */
        }

        i += step;

        if( i%EXCHANGE_FREQ == 0 ){			// If possible do replica exchange (change beta's)
            int count = 0;
            for( int r = 0; r < n_replica - 1; r++ ){
            	/// Check signs here
            	double	delta = energies[order[r]] - energies[order[r+1]];
		        double  prob_new = exp(- betas[r+1] * delta );
        		prob_new = simple_min(1.0,prob_new);
        		
                if(prob_new > rnd_lin(1.0)){
                	int swap;
                	double beta_swap;
                	swap       = order[r+1];
                	beta_swap  = betas[r+1];
                	order[r+1] = order[r];
                	betas[r+1] = betas[r];
                	order[r]   = swap;
                	betas[r]   = beta_swap;
                	count++;
                }
            }
            logger << "At step " << i <<". Made " << count << " out of " << n_replica << " swaps\n";
        }

        step = simple_min(it_max-i+1,(n_print - (i%n_print)));
        step = simple_min(step,(traj_freq - (i%traj_freq)));
        step = simple_min(step,EXCHANGE_FREQ);
    }

/// TODO: Tidy up everything...
    // delete the multiple integrators
    // close the multiple trajectories
    /*
    if( traj_stream.good() ){				// If we are writing a trajectory
        traj_stream.close();				// Close the file
    }
    */

    if( verbose ) logger << "Writing final coldest configuration.\n";
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

    // and close general log output
    if( log_name.length() > 0 ){log_file.close();}

    return EXIT_SUCCESS;
}

