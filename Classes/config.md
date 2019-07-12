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

## Enhancements to implement

Allow comments in the file: # or % to end of line ?
Allow blank lines in the file.
Implement V2 (maintain backward compatibility) with polygonal areas not rectangle 0,0,x_size,y_size. 
This relates to issue #1 and is necessary for analysis of AFM image patches.


