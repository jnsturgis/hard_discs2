/**
 * @file    integrator.h
 * @author  James Sturgis
 * @date    April 8, 2018
 * \brief   Header file for the integrator class
 *
 * @class   integrator integrator.h
 * @brief   An integrator to run dynamics on a configuration.
 *
 * This class defines an integrator object that can be used to do numerical
 * integrations with configurations. Currently the integrator is associated
 * with a force field at creation and is run by calling a run method with
 * a configuration and the integration parameters (temperature and pressure)
 * together with the number of integration steps to take.
 *
 * Currently the nature of the steps is hard coded as are the various integration
 * counters and control parameters.
 *
 * @todo    The integrator should incorporate more of the choices about
 *          integration to allow different types of dynamics. So there should
 *          be choices about the configuration manipulations possible and their
 *          probabilities in an integration step.
 */

#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "config.h"

class integrator {
public:
    integrator(force_field *the_forces);    ///< Constructor with force field
    integrator(const integrator& orig);     ///< Constructor with copy
    virtual ~integrator();                  ///< Destructor
    int     run(config **state_handle, double beta,
                double P, int n_step);      ///< Run n_step integration steps
    int     n_good;                         ///< Integrator tally, number of accepted moves.
    int     n_bad;                          ///< Integrator tally, number of rejected moves.
    int     i_adjust;                       ///< Frequency of integrator adjustment.
    double  dl_max;                         ///< Maximum move distance.
private:
    int     n_step;                         ///< Number of integrator steps made so far.
    force_field *the_forces;
};

#endif /* INTEGRATOR_H */

