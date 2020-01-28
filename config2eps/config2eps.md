# The config2eps program {#config2eps}

* brief   Convert a configuration into a postscript figure.
* author  James Sturgis
* date    April 12, 2018
* version 1.0

The program converts a configuration file, read from the input, to an encapuslated
postscript file containing a representation of the configuration in which each atom
of each object is drawn as a circle of the radius given in the force field and
the color given in the force field. The area of the forcefield is scaled to fit into
the output area.

    Usage:
        config2eps [-t topology] < config_file > eps_file.

The optional argument '-t topology' allows you to define a topology file and so 
control the representation of the different objects in the configuration. 
The format of the topology file is described [here](@ref topology_file).

The input is expected to be a configuration file describing the configuration, and
the different objects. The structure of this file is documented [here](@ref config_file).

The output is (should be) a valid encapsulated postscript file.
