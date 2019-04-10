# The configuration file format {#config_file}
\brief   Description of the requirements for a force field file

 * \author  James Sturgis
 * \date    9 April 2019
 * \version 1.0

The force field file is structured as follows:

 * line 1: x_size y_size
 * line 2: nobject
 * line 3...: type x_pos y_pos rotation

Comments?

x_size, y_size, x_pos, y_pos and rotation are floats while n_object and type are integers. 
Rotation is measured in degrees?
Size and position are measured in arbitrary length units?

Files of this type are used exclusively by the config class.



