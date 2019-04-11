# The format of topology files. {#topology_file}
\brief   Description of the requirements for a topology file

 * \author  James Sturgis
 * \date    9 April 2019
 * \version 1.0

The force field file is structured as follows:

\todo	Implement a proper topology file.

 * line 1: single                         // Topology type - here single
 * line 2: 0                              // Bead type
 * line 3: square                         // Type again, a square object (4 sub atoms)
 * line 4: 0 1 2 2                        // The types of the 4 sub atoms
 * line 5: triangle                       // A triangle, equilateral, with 3 beads
 * line 6: 0 2 0                          // The type of each bead

Needs to define the number type and relative position of the different
atoms that make up an object of a specific type.

This file type is used exclusively by the topology class.
