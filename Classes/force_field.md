# The force field file format {#force_field_file}
\brief   Description of the requirements for a force field file

 * \author  James Sturgis
 * \date    9 April 2019
 * \version 1.0

The force field file is structured as follows:

 * line 1: the number of different types of atom
 * line 2: the radius of each bead type (float)
 * line 3: the color for each bead type
 * line 4: two numbers, the interaction cut-off for calculations and the length scale.
 * line 5..: an n_atom by n_atom matrix of interaction strengths.

Colors are taken from the ist of defined colors which is currently: red, green, blue, orange...

## Comments
Blank lines are ignored and lines with a # as the first printing character are considered comments.

Continuation lines?

This file format is used exclusively in the force_field class.
