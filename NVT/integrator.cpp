/**
 * @file    integrator.cpp
 * @author  James Sturgis
 * @date    April 8, 2018
 *
 * Implementation of an integrator object that can be created, the parameters
 * used for the integration manipulated or extracted and steps of integration
 * run. Currently it runs an NVT integration on the configuration.
 */

#include <math.h>
#include "integrator.h"
#include "common.h"

/**
 * Constructor function that takes as a parameter the force field that will be
 * used to calculate the energy of the system during integration.
 *
 * @param forces The force field to use for energy calculations
 */
integrator::integrator(force_field *forces) {
    n_good     =
    n_bad      =
    n_step     = 0;
    dl_max     = 1.0;
    i_adjust   = 1000;
    the_forces = forces;
}

/**
 * Constructor function that will copy an integrator and make a new one. That
 * is identical to the original.
 *
 * @param orig The integrator to copy.
 */
integrator::integrator(const integrator& orig) {
    n_good     = orig.n_good;
    n_bad      = orig.n_bad;
    dl_max     = orig.dl_max;
    n_step     = orig.n_step;
    i_adjust   = orig.i_adjust;
    the_forces = orig.the_forces;
}

/**
 * Destructor to destroy an integrator.
 */
integrator::~integrator() {
}

/**
 * @brief Function to run a series of steps of integration on a configuration.
 *
 * Currently this integration is performed by at each time point:
 * - If necessary adjusting the integrator parameters and resetting the tallies.
 * - Moving an object in the configuration.
 * - Working out which parts of the energy need to be re-evaluated.
 * - Calculating a new energy for the configuration.
 * - Accepting or rejecting the configuration based on the metropolis criterion.
 * - Updating the integrator tallies.
 *
 * @param state_h a handle to the configuration. This will be updated during
 *                the run, so the_state at the end is different if a change is
 *                made.
 * \param beta    The reciprocal temperature (scaled by the boltzman constant)
 *                to use for the integration.
 * \param P       The pressure.
 * @param n_steps The number of requested steps to make.
 * @return        The total number of steps so far performed.
 *
 */
int
integrator::run(config **state_h, double beta, double P, int n_steps){
    int     i;              ///< Iteration counter
    int     obj_number;     ///< Index of object to modify
    double  dU;             ///< Internal energy change.
    double  prob_new;       ///< Acceptance probability.
    config  *the_state = *state_h;
    config  *new_state;     ///< Pointer to modified state.

    for(i = 0; i < n_steps; i++){
        /* If necessary adjust integrator parameters and tallies */
        if((n_step > 0) && ((n_step % i_adjust)== 0)){
            if(((float)n_good/(n_good+n_bad)) < 0.3) dl_max /= 3.9;
            if(((float)n_good/(n_good+n_bad)) > 0.7) dl_max *= 3.0;
            dl_max = simple_min( dl_max, the_state->x_size);
            dl_max = simple_min( dl_max, the_state->y_size);
            n_good = n_bad = 0;
        }

        /** Clone configuration and move an object in the new configuration */
        /** @todo   Chose between different types of modification           */
        new_state = new config(*the_state);

        /// The integrator move function.
        obj_number = rnd_lin(1.0)*the_state->n_objects();
        new_state->move(obj_number, dl_max);
        new_state->invalidate_within(the_forces->cut_off, obj_number);
        new_state->unchanged = false;

        /* Calculate probability of accepting the new state                */
        dU = new_state->energy(the_forces)
                - the_state->energy(the_forces);
        prob_new = exp(- beta * dU );
        prob_new = simple_min(1.0,prob_new);

        /* Accept or reject the new state according to the probability     */
        if(rnd_lin(1.0)<= prob_new ){
            n_good++;
            delete(the_state);
            the_state = new_state;
        } else {
            n_bad++;
            delete(new_state);
        }
        n_step++;
    }
    *state_h = the_state;
    return n_step;
}
