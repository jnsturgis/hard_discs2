# The format of topology files. {#topology_file}
\brief   Description of the structure, requirements and use of topology files.

\author  James Sturgis
\date    9 April 2019
\version 1.0

The topology file is structured as follows it contains two sections:
> A first section describes the different atom types
> The first line gives the number of atom types in the topology file
> The next lines (1 for each atom type) provide a name for the atom and a size for drawing.

The name is currently unused, and the size is only for drawing and has no 
impact on the interactions of the atoms.

> The second section describes a series of molecules (each made out
> of atoms).
> The first line gives the number of molecules to expect.

> This is followed by for each molecule:
> A line with the label for the molecule
> A line with the number of atoms in the molecule
> and then for each atom a line with an atom type (refering to the atoms above),
> a position x and y coordinates relative to the object center, and a
> color to use for drawing.

Currently several colors are predefined these are red, green, orange, blue and 
black however other colors can be used as for example '1.0 1.0 0.0 setrgbcolor'
will give yellow. As the colors are used only by postscript in theory any valid
piece of postscript can be inserted here! It is executed after moving to the
molecules position and before drawing a circle of the required size.

