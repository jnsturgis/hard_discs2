/**
 * \file    RX.cpp
 * \author  James Sturgis
 * \date    February 1, 2018
 * \version 1.0
 * \brief   Run a collection of trajectories at different temperatures in the NVT ensemble.

TODO: Define command line as NVT compatible as possible...

 **/

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
#include <algorithm>
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

typedef struct {
	double		the_beta;
	double		the_pressure;
    double		the_energy;
	integrator	*the_integrator;
	config		*the_config;
	ogzstream	the_trajectory;
    ofstream	the_log;
} rep_data;

#define EXCHANGE_FREQ	( 20 * current_state->n_objects())
#define BETA_ADJUST		( 20 * EXCHANGE_FREQ)
#define swap( type, one, two )	{type s = one; one = two; two = s; }

void
advance( rep_data *pdata, int i, int step, force_field *the_forces, int n_print, int traj_freq )
{
	config	**handle = &(pdata->the_config);
   	pdata->the_integrator->run(handle, pdata->the_beta, pdata->the_pressure, step);
   	pdata->the_config = *handle;

   	int j = i + step;
   	if( j%n_print == 0 ){				// Is it time to print to the log file
       	pdata->the_log << "After " << j << " steps, P = " << pdata->the_pressure 
					<< ", beta = " << pdata->the_beta << "\n"; 
       	pdata->the_config->report(pdata->the_log, the_forces);
   	    pdata->the_integrator->report(pdata->the_log);
    }
	if( j%traj_freq == 0 ){				// Is it time to print to the trajectory
       	pdata->the_trajectory << "====" << j << "====\n";
   	    pdata->the_config->write( pdata->the_trajectory );
    }
    pdata->the_energy = pdata->the_config->energy(the_forces);
}

std::string
make_name( string root, int index )
{
	std::stringstream ss;
	fs::path p(root);
	
	assert(index<1000);			// Feature no more than 999 replicas
	
	ss << p.stem().c_str();
	ss << std::setw(3) << std::setfill('0') << index;
	ss << p.extension().c_str();
	
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

    std::cerr << "Command line dealt-with\n";

    i = 0;          // Counter for number of shifts.
    logger << "After " << i << " steps, P = " << pressure << ", beta = " << beta << "\n"; 
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
            std::cerr << "after" << std::to_string( i ) << " steps\n";
            current_state->report(std::cerr, the_forces);
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
    std::vector<int>			swaps(n_replica);		// Counters for exchanges between temperatures.
    std::vector<thread>         replicas;
    std::vector<rep_data>		data(n_replica);

    // Should have some error handling in this section...
    // Set up the different replicas

    for(int i=0; i < n_replica; i++ ){
        order[i]       			= i;
        data[i].the_beta		= beta * (i+1) / n_replica;
		data[i].the_pressure	= pressure;
        data[i].the_config		= new config( current_state );
        data[i].the_integrator	= new integrator( the_forces );

        if(verbose){
        	std::cerr << "Copied configuration " << i << ", beta =" << data[i].the_beta << "\n";
        }
        
        if( traj_freq > 0 ){
            data[i].the_trajectory.open( make_name(traj_name, i ).c_str() );
            if( verbose ){
                std::cerr << make_name(traj_name, i ) << " opened for replica trajectory.\n";
            }
        }

       if( log_name.length() == 0 ){
       		log_name = "toto.log";
       }
       data[i].the_log.open( make_name(log_name, i), std::ofstream::out );
       if( verbose ){
            std::cerr << make_name(log_name, i ) << " opened for replica log.\n";
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
        std::cerr << "With" << (current_state->is_periodic?" ":"out ") << "periodic boundary conditions.\n";
        std::cerr << "Boundary is " << (current_state->is_rectangle ? "rectangle" : "polygon") << "\n";
        std::cerr << "Starting iteration loop\n";
    }

    int exchange_count = 0;
    int exchange_max   = 0;
        
    for(i=0;i<it_max;){
		for( int r=0; r<n_replica; r++ ){		// Launch threads
         	/** This part should be parallelized  **/
//			replicas.push_back(std::thread(advance, &data[r], i, step, the_forces, n_print, traj_freq));	// compile problem
			advance( &data[r], i, step, the_forces, n_print, traj_freq);
	        /** End of parallel internal part **/
        }


		for (auto& th : replicas ) th.join();	// Wait for all threads to stop
												// We destroy threads in array
        i += step;
												// Need to check the algorithms for
												// Replica exchange and beta adjustment
        if( i%EXCHANGE_FREQ == 0 ){				// If possible do replica exchange (change beta's)
            for( int r = 0; r < n_replica - 1; r++ ){
            	/// Should scale with average temperature
            	double	delta    = data[order[r]].the_energy - data[order[r+1]].the_energy;
				double  beta_av	 = (data[order[r]].the_beta + data[order[r+1]].the_beta)/2.0;
		        double  prob_new = exp(- delta * beta_av );
        		prob_new = simple_min(1.0,prob_new);
        		
                if(prob_new > rnd_lin(1.0)){
					if( verbose ){
						std::cerr << "Swapping " << r << " and " << r+1 << " with energies " << data[order[r]].the_energy << " and " 
								<< data[order[r+1]].the_energy << " delta = " << delta << "\n";
                        std::cerr << "Betas were " << data[order[r]].the_beta << " and " << data[order[r+1]].the_beta << "\n";
					}
                	swaps[r]++;
                	exchange_count++;

					swap( int, order[r+1], order[r] );
					swap( double, data[order[r+1]].the_beta, data[order[r]].the_beta );
                }
                exchange_max++;
            }
        }
		if( i%BETA_ADJUST == 0){
			logger  << "At step " << i <<". Made " << exchange_count << " out of " << exchange_max << " swaps\n";
			std::vector<double>	c(n_replica);	// Serial set of spring compliances algorithm
			double	factor = (n_replica - 1.0)/ exchange_count;
			c[0] = 1.0;
 			for(int r=1; r<n_replica; r++){
				c[r]   = c[r-1]+(swaps[r-1]+0.2)*factor;
			}
 			for(int r=0; r<n_replica; r++){
				data[order[r]].the_beta = c[r] * beta / c[n_replica - 1];
            }
			exchange_count = 0;
			exchange_max   = 0;
 			for(int r=0; r<n_replica; r++){
				logger << "swaps [" << r << "] = " << swaps[r] << "\n";
				swaps[r] = 0;
			}
			logger << "Betas adjusted to :";
 			for(int r=0; r<n_replica; r++){
				logger << data[order[r]].the_beta << ",";
			}
			logger << "\n";
    	}
		step = simple_min(it_max-i+1,(n_print - (i%n_print)));
        step = simple_min(step,(traj_freq - (i%traj_freq)));
        step = simple_min(step,EXCHANGE_FREQ);
    }

/// TODO: Tidy up everything...
    /*
    if( traj_stream.good() ){				// If we are writing a trajectory
        traj_stream.close();				// Close the file
    }
    */

    if( verbose ) std::cerr << "Writing final coldest configuration. From replica #" << order[n_replica-1] << "\n";
    if( out_name.length() > 0 ){
        std::ofstream out_file(out_name);
        data[order[n_replica-1]].the_config->write(out_file);
        out_file.close();
    } else {
        data[order[n_replica-1]].the_config->write(std::cout);
    }
    if( verbose ) std::cerr << "Wrote configuration successfully.\n";

    for(int i=0; i < n_replica; i++ ){
		delete	data[i].the_config;
		delete	data[i].the_integrator;
    }
    
    delete current_state;
    delete the_forces;

    logger << "\n...Done...\n";

    // and close general log output
    if( log_name.length() > 0 ){log_file.close();}

    return EXIT_SUCCESS;
}

