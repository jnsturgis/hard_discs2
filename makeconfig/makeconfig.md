# The makeconfig application {#makeconfig}
\brief   Application to create a configuration file containing multiple objects.

\author  James Sturgis
\date    17 April 2018
\version 1.0

The function of this program is to produce a configuration of the desired configuration
with the composition requested in a relatively random organization.

    Usage:
        makeconfig [-vp][-t topology][-f force_field][-o output][-d scale][-a attempts]
        x_size y_size obj0...

The algorithm will create an empty configuration with the desired geometry (size x_size 
by y_size) and then try to randomly place the objects into the space. The number of 
objects is defined in a list obj0 is the number of objects of type 0 to place this is 
followed by the number of objects of type 1 etc.

The optional arguments are:
| Argument | Value | Function |
|:--------:|:-----:|----------|
| -v       |None   | Give verbose messages |
| -p       |None   | Periodic boundary conditions |
| -t       |Topology file | Specify molecular size and shape |
| -f       |Forcefield file | Not used |
| -o       |Filename | Send result to a file |
| -d       |float    | Scaling parameter to use |
| -a       |Interger | Number of attempts at placing objects |

By default objects are placed for non-periodic boundaries (ie with a repulsive
box) however the -p flag will set periodic boundary conditions.

The output is usually sent to standard output however if the -o argument has been 
used to set a destination file name output is sent to the file.

In the absence of a topology file all objects are considered to have a single 
central atom of type 0 and size 1.

The -d flag can be used to change the default behaviour, if a scaling parameter is given then the
size is divided by scale before placing the objects, and then after placement the
surface is rescaled to the desired size. Thus a scale greater than 1 results is well
separated objects, while a scale less than 1 will result in crowding.
During object placement overlap between objects is avoided.
Rescaling (with a scale less than 1.0) can result in object overlap.

By default the program will try 1000 times to place an object and if in that time it
does not succede it will exit with an error message. This number of attempts can be
changed using the -a argument.

