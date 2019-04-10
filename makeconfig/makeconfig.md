# The makeconfig application {#makeconfig}
\brief   Application to create a configuration file containing multiple objects.

\author  James Sturgis
\date    17 April 2018
\version 1.0

The function of this program is to produce a configuration of the desired configuration
with the composition requested in a relatively random organization.

    Usage:
        makeconfig [-t topology][-f force_field][-o output][-d scale] x_size y_size obj0...

The algorithm will create an empty configuration with the desired geometry (size x_size by y_size)
and then try to randomly place the objects into the space. The number of objects is 
defined in a list obj0 is the number of objects of type 0 to place this is followed 
by the number of objects of type 1 etc.
The output is sent to standard output unless the -o flag has been used to set a 
destination file name.
The optional parameters are a topology file, signaled with the -t flag, a force field 
file, signaled with the -f flag, and an output file, signaled with the -o flag.
In the absence of a force field flag atoms are considered to be 1 unit in diameter.
In the absence of a topology file objects are considered to have a single central atom
of type 0.
The -d flag can be used to change the default behaviour, if a scale is given then the
size is divided by scale before placing the objects, and then after placement the
surface is rescaled to the desired size. Thus a scale greater than 1 results is well
separated objects, while a scale less than 1 will result in crowding.
During object placement overlap between objects is avoided.
Rescaling (with a scale less than 1.0) can result in object overlap.

\todo    Make sure that the configuration has non-infinate energy and if it does, 
      take steps to find one that does not.
\todo	 This has been broken and the description nolonger describes the functioning.
