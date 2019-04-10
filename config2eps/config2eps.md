# The config2eps program {#config2eps}

\brief   Convert a configuration into a postscript figure.
\author  James Sturgis
\date    April 12, 2018
\version 1.0

The program converts a configuration file, read from the input, to an encapuslated
postscript file containing a representation of the configuration in which each atom
of each object is drawn as a circle of the radius given in the force field and
the color given in the force field. The area of the forcefield is scaled to fit into
the output area.

    Usage:
        config2eps < config_file > eps_file.

\todo    Introduce non square areas scaled correctly.
\todo    More control on preamble and ending of output.

