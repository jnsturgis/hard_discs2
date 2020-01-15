# The configuration file format {#config_file}
\brief   Description of the requirements for a configuration file

 * \author  James Sturgis
 * \date    9 April 2019
 * \version 1.0

## File structure

The V1 cofiguration file is structured as follows:

 * line 1: x_size y_size
 * line 2: nobject
 * line 3...: type x_pos y_pos rotation

**x_size**, **y_size** are two real numbers describing the size of the rectangular patch of membrane.
**nobject** is an integer that gives the number of object descriptions to expect.
Each object in the configuration is described by:
an interger **type**, this references the topology file;
three real numbers describing the position (**x_pos**, **y_pos**) on the surface,
and the **rotation** of the object (in degrees) from the reference orientation in the topology file.

Size and position are measured in arbitrary length units?

The V2 configuration file is backwards compatibl with V1
If x_size and y_size a 0.0 then...
before the nobject line is a polygon definition if the form
 * n_vertices
 * x_coord, y_coord...

### Comments can be included in the file.
All contents after a # in a line are ignored.
Blank lines (containing only whitespace) are ignored.


