# The format of topology files. {#topology_file}
\brief   Description of the structure, requirements and use of topology files.

* Author  James Sturgis
* Date    9 April 2019
* Version 1.0

The topology file is structured as follows it contains two sections: atom descriptions, and molecule descriptions.
The atom descriptions describe the different atom types included in the topology.
Currently, this part of the topology description is redefined in the force field file for energy calculations, but
the topology file version is used for programmes that do not need a force field. This will be corrected in the 
future.

Atom types control the interactions between molecules, which are all atom based.
The atom description section starts with a line giving the number of atom types.
This is followed by a series of lines defining the atom types, each line giving a name and a size (radius).
The name is currently unused, and the size is only for drawing and has no impact on the interactions of the atoms.
The first atom has index 0, the second 1 and so on.

The second section describes a series of molecules (each made out of atoms).
The first line gives the number of molecules to expect.
This is followed by for each molecule:
* A line with the label for the molecule
* A line with the number of atoms in the molecule
* and then for each atom a line with an atom type (refering to the atoms above),
* a position x and y coordinates relative to the object center, and a
* color to use for drawing.

Currently several colors are predefined these are red, green, orange, blue and 
black. These colors will eventually be extended to give a larger palette.

It is an error if the molecule descriptions refer to an atom that is not defined
in the first part of the file.

Blank lines in the topology file are ignored.
A comment starts with a hash '#' and anything from there to the end of the line is
ignored.

