# Configuration manipulation programs

* author  James Sturgis
* date    9 February 2020

There are a number of programs designed to create and manipulate configurations.

# Creating configurations {#Creating_configurations}

## The makeconfig program {#makeconfig}

* author  James Sturgis
* date    17 April 2018
* version 1.0

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

# Modifying configurations {#Modifying_configurations}

## The srinkconfig program {#shrinkconfig}

* author  James Sturgis
* date    21 April 2018
* version 1.0

The function of this program is to change the size of a configuration this will use a
montecarlo procedure to avoid clashes if necessary.

    Usage:
        shrinkconfig [-v][-t topology][-f force_field][-o output][-a attempts][-s scale]
        [source_file]

The algorithm will read the source_file (or by default stdin) and rescale it using the scale factor
(a scale factor of 1.0 is equivalent to the identity operator).
If rescaling results in clashes then various attempts are made to move the offending options,
unless "attempts" <= 1.

The optional arguments are:

| Argument | Value | Function |
|:--------:|:-----:|----------|
| -v       |None   | Give verbose messages |
| -t       |Topology file | Specify molecular size and shape and interaction energy if present |
| -f       |Forcefield file | If present used to evaluate energy |
| -o       |Filename | Send result to a file (default stdout) |
| -a       |Interger | Number of attempts at placing objects (default 1) |
| -s       |Float  | Scaling parameter (default 1.0) |
|          |Filename | Soure filename (default stdin) |

The output is usually sent to standard output however if the -o argument has been used to set a destination file name output is sent to the file.

In the absence of a topology file all objects are considered to have a single central atom of type 0 and size 1.

If the change in size results in hard clashes between objects, as determined from the topology file, then the program will try to adjust the positions and orientations of objects to remove these clashes if the -a parameter is set to a value other than 1. The parameter determines the number of attempts to make to remove clashes. If the program fails to remove clashes then it will exit with a failure status and not write the output file.

## The wrap program {#wrap}

* author  James Sturgis
* date    9 February 2020
* version 1.0

This program calculates a convex hull around the objects in the configuration and then uses this as the boundary for the configuration. This is to allow configurations that have been extracted from microscopy observations, such as AFM images, to be entered in a large rectangular area and then a reasonable boundary for the observed area calculated.

    Usage:
        wrap [-v][-t topology] < input.config > output.config

The program reads a configuration file from the standard input and writes the modified configuration to the standard output.

The optional arguments are:
| Argument | Value | Function |
|:--------:|:-----:|----------|
| -v       |None   | Give verbose messages |
| -t       |Topology file | Specify molecular size and shape |

### Known bugs and issues.

Multiple colinear objects on the boundary cause problems, that can be relatively easilly removed from the resulting output file. See Issue #36 on GitHub.

