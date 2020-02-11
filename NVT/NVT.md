# Montecarlo integrators {#Integrators}

There are various thermodynamic integration programmes that modify the configurations producing a trajectory file. These will eventually include:

* _NVT_
* _NPT_
* _Gibbs_ an integrator in the Gibbs ensemble
* _RE-NVT_ a replica exchange NVT integrator

# The NVT Integrator {#NVT}
\brief   Run a montecarlo trajectory on a configuration in the NVT ensemble.

 * Authors James Sturgis
 * Date    April 6, 2018
 * Version 1.0

The programme loads a configuration and then runs a monte carlo integration
in the NVT ensemble in which for each move a random object is selected and
moved to a new location and rotated to a new orientation. This modified
configuration is accepted according to the Metropolis criterion, the
energy change (dE) is negative or the probability of the move at the 
simulation temperature, calculated as e^(-dE beta), is higher than a uniform 
variant on the interval 0..1. 
In this equation dE is the energy difference between the old and new
configurations, and beta (which is a parameter to the program) is 1/ kb T
where kb is the boltzman constant and T the absolute temperature.

## Usage

To use the program the command line is:

   NVT [-vp][-t topology][-f forcefield][-c config][-o end_config]
       [-l log_file] [-n frame_freq] [-s traj_file] n_steps print_frequency 
       beta pressure

The different parameters can be present in any order, those introduced with a **-?**
flag are **optional** except for the topology and the force field file names, which are
**required**. The default behavour is also described below. The last 4 parameters
are **required**.
Where the various parameter meanings are as follows:
 *     -v             The verbose flag, results in extra messages being sent 
                      to the output during execution. Useful for finding errors
                      and debugging.
 *     -p             The periodic flag, signals that periodic boundary conditions
                      should be used. This is only possible with rectangular
                      configurations, a polygonal configuration that happens to be
                      a rectangle will be converted to a rectangle (by rotation and
                      translation if necessary) if periodic boundary conditions are
                      requested.
 *     -t topology    The topology parameter designates a file to use for the topology
                      of the molecules in the configuration. The format of the topology 
                      file is described [here](@ref topology_file).
                      This parameter is **required**, if no force_field is declared 
                      it will cause an error.
 *     -f force_field The force field parameter designates a file to use for the force
                      field governing interactions between objects. The format of
                      the forcefield file is described [here](@ref force_field_file).
                      This parameter is **required**, if no force_field is declared 
                      it will cause an error.
 *     -c config      The config parameter designates the file containing the initial
                      configuration for the calculation. The format for the configuration
                      file is described [here](@ref config_file). 
                      If no config is declared the configuration is expected on the
                      console input (std::cin).
 *     -o end_config  The output parameter designates a file for writing the final
                      configuration. This can be used for a restart.
                      If no output destination is declared the configurationis written
                      to the console (std::cout).
 *     -l log_file    The logging parameter designates a destination for messages
                      and output of the evolution of the energy and integration
                      data. If this parameter is absent the information is sent
                      to the terminal (std::cout).
 *     -n frame_freq  The frame frequency defines the frequency of writing to the
                      trajectory file. If this parameter is not defined then no
                      trajectory is written, if it is defined then a destination
                      **must** be defined with the -s parameter (below).
 *     -s traj_file   The save parameter names a destination file for the trajectory
                      this contains a gzip compressed series of configurations
                      separated by lines giving the step count. As the file is
                      compressed using gzip is is good practice to end the file name
                      with **.gz** but this is not enforced. If this parameter is present
                      the frame_frequency parameter (above) **must** also be present.
                      If this parameter is absent the frame_frequency parameter (above) 
                      **must** also be absent.
 *     n_steps        The number of simulation steps to make.
 *     print_freq     The number of steps between reports to the log file
                      of how the integration is progressing.
 *     beta           The reciprocal temperature parameter 1/(kb T) that scales the
                      force field energies.
 *     pressure       The pressure (this is not used but is for compatibility
                      with other ensembles such as NPT or the Gibbs ensemble.

The program does not use the standard input stream, but writes a log of progress
to the standard output stream (this can or *should* be redirected to the log file).
debugging and error messages are written to the standard error stream. The
program ends with the standard exit codes EXIT_SUCCESS or EXIT_FAILURE.

## Log file format:
 The format of the log file is determined in this file by the print statements:
     lines 196-201   After loading the file.
     lines 228-232   After the initial adjustments
     lines 249-257   Every print_frequency steps during the integration.
     line 268        At the end of the program.

Each report, except the last, contains 3 lines of slightly variable content.

 \todo log file       Use a dedicated function for writing data so it is easier
                      to parse after and control the structure.  Perhaps in
                      xml format.

