# The shrinkconfig application {#shrinkconfig}
\brief   Application to change the size a configuration file containing multiple objects.

\author  James Sturgis

\date    21 April 2018
\version 1.0

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
