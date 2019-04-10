# The NVT application {#NVT}
\brief   Run a trajectory in the NVT ensemble.

 * \author  James Sturgis
 * \date    April 6, 2018
 * \version 1.0

The programme loads a configuration and then runs a monte carlo integration
in the NVT ensemble in which for each move a random object is selected and
moved to a new location and rotated to a new orientation. This modified
configuration is accepted according to the Metropolis criterion, the
energy is lower or the probability of the move at the simulation temperature,
calculated as e^(-dE beta), is higher than a uniform variant on the interval
0..1. In this equation dE is the energy difference between the old and new
configurations, and beta (which is a parameter to the program) is 1/ kb T
where kb is the boltzman constant and T the absolute temperature.

To use the program the command line is:

      NVT n_steps print_frequency beta pressure initial_config final_config

Where the various parameters are:
      n_steps         The number of simulation steps to make.
      print_frequency The number of steps between reports to the log file
                      of how the integration is progressing.
      beta            The temperature parameter 1/(kb T) that scales the
                      force field energies.
      pressure        The pressure (this is not used but is for compatibility
                      with other ensembles such as NPT or the Gibbs ensemble.
      initial_config  The name of an existing file containing a valid
                      configuration, that is read as the starting point.
      final_config    The name of a file to which will be written the final
                      configuration, if a file with this name exists already
                      it is deleted.

The program does not use the standard input stream, but writes a log of progress
to the standard output stream (this can /should be redirected to a log file).
debugging and error messages are written to the standard error stream. The
program ends with the standard exit codes EXIT_SUCCESS or EXIT_FAILURE.

Log file format:
 The format of the log file is determined in this file by the print statements:
     lines 196-201   After loading the file.
     lines 228-232   After the initial adjustments
     lines 249-257   Every print_frequency steps during the integration.
     line 268        At the end of the program.

Each report, except the last, contains 3 lines of slightly variable content.

 \todo log file       Use a dedicated function for writing data so it is easier
                      to parse after and control the structure.  Perhaps in
                      xml format.

Configuration file format:
The configuration is read by the routine in config.cpp, and then object.cpp
 It has a simple format:
      line 1          The area x and y dimensions.
      line 2          The number of objects.
      line 3-n        For each object four numbers:
                          the type (this refers to the topology)
                          the x and y positions should be in the area.
                          the orientation in radians.

\todo config         Include non-rectangular surfaces in file.
\todo config         Include info on boundary conditions.
\todo config         Include comments.

Topology:
The topology describes the relationship between objects and their constituent
atoms. It is currently hard coded in the file topology.cpp, lines 28-40. The
len[] array and data[][] matrix. The array len[] describes for each object i
the number of atoms it contains. The data matrix [][] in valid positions i, j
contains an atom with the properties (in order): atom_type, x_position,
y_position. The atom type refers to the atom_types used in the force_field file
and the positions are relative to the object position, which should be the
center of mass, at an orientation of 0.0 radians (or degrees).

\todo topology       Read topology data from a file.

Force field:
The force field describes the interactions between the different atom types.
Currently the force field is hard coded in the file force_field.cpp, lines
11 to 21. The meaning of the different parts are as follows:
     BIGVALUE        A large finite number, used in place of infinity to avoid
                     NaN errors it should be less than MAX_DOUBLE divided by
                     twice the number of objects.
     my_radius[]     An array for each atom type of the hard radius of the
                     atom.
     my_color[]      An array for each atom type of the color to use for the
                     atom when drawing it to postscript (config2eps).
     my_energy[][]   A (symmetric) array of the potentiel well depths for
                     interactions between two different types of atom.
     my_cut_off      The distance between objects beyond which the interaction
                     energy is presumed to be 0.
     my_length       The length scale for interactions.

\todo force_field    Convert lengths to by interaction basis (a matrix).
\todo force_field    Read force_field from a control file or a force field file.

