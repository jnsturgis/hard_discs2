# File formats {#file_formats}

The programs use several file formats to exchange information. These include:
* the configuration file that describes an organisation of objects on a 2 dimensional patch,
* a topology file that describes the structure of these objects and how to represent them,
* a force field file that describes the interactions between objects and of objects with their environment.
* plot files that describe 1 dimensional data sets,
* map files that describe 2 dimensional data sets.

This section decribes the format of these different files as produced and used by the different programs.

# The configuration file format {#config_file}

\brief   Description of the structure of a configuration file. 
A conforming file should be read correctly by all programes in the suite.
The configuration file describes where the different objects are located in a membrane patch, and the
shape of that membrane patch.

 * Authors James Sturgis
 * Date    17 Jan 2020 (last revision of the format)
 * Version 1.1

## File structure
The configuration file contains two parts. 
First a description of a bounded area, and second a description of the objects in that area.

The description of the area can take 2 forms: 
*either* a **width** and a **height** on a line defining a bounding rectangle 
*or* a **width** and **height** both of zero on a line followed by a description of a polygonal area.
A polygonal area is described by first the **number of vertices** on a line, 
followed by a series of **x**, **y** coordinates for the verticies, 
the polygon is closed by connecting the last vertex to the first.

The description of the objects takes the form of:
 * A line with the number of objects in the configuration
 * A series of lines with one object described on each line.
Each object in the configuration is then described by a single line composed of:
an interger describing the object type (**type**), 
two real numbers describing the position (**x_pos**, **y_pos**) on the surface,
a real number describing the rotational angle of the object (**rotation**).

The different object types are described in a topology file {#topology_file} which will be used with the configuration.
Size and position are measured in arbitrary length units and rotation is measured in degrees.

It is an **error** if:
 * The number of vertices in the polygon is not as declared.
 * The number of objects is not as declared.
 * The objects are not inside the boundary (polygon or rectangle).

### V1.0
The V1.0 configuration file is a simple text file structured as follows:
 * line 1: x_size y_size
 * line 2: nobject
 * line 3...: type x_pos y_pos rotation
**x_size**, **y_size** are two real numbers describing the size of the rectangular patch of membrane.
**nobject** is an integer that gives the number of object descriptions to expect.
Each object in the configuration is then described by a single line composed of:
an interger describing the object type (**type**), 
two real numbers describing the position (**x_pos**, **y_pos**) on the surface,
a real number describing the rotational angle of the object (**rotation**).

The different object types are described in a topology file {#topology_file} which will be used with the configuration.
Size and position are measured in arbitrary length units and rotation is measured in degrees.

### V1.1
The V1.1 configuration file is backwards compatible with V1.0 it retains a simple text format.
The new features introduced by format 1.1 are:
 * Comments and blank lines in configuration files.
 * Support for non-rectangular regions.

For comments all content after a # on an input line are ignored, and blank (white space only) lines are ignored.

To include arbitrary shapes if x_size and y_size are both zero (0.0) then before the line describing the number of objects
is a description of a polygon if the form
 * line 1: n_vertices
 * line 2...: x_coord, y_coord...

# The topology file format. {#topology_format}

Description of the structure, requirements and use of topology files.

* Author  James Sturgis
* Date    9 April 2019
* Version 1.0

The topology file is structured as follows it contains two sections: atom descriptions, and molecule descriptions.
The atom descriptions describe the different atom types included in the topology.
Blank lines in the topology file are ignored.
A comment starts with a hash '#' and anything from there to the end of the line is
ignored.

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

It is an error if the molecule descriptions refer to an atom that is not defined
in the first part of the file.

The colors are predefined these are the html colors these are:
IndianRed 
LightCoral 
Salmon 
DarkSalmon 
LightSalmon 
Crimson 
Red 
FireBrick 
DarkRed 
Pink 
LightPink 
HotPink 
DeepPink 
MediumVioletRed 
PaleVioletRed 
LightSalmon 
Coral 
Tomato 
OrangeRed 
DarkOrange 
Orange 
Gold 
Yellow 
LightYellow 
LemonChiffon 
LightGoldenrodYellow 
PapayaWhip 
Moccasin 
PeachPuff 
PaleGoldenrod 
Khaki 
DarkKhaki 
Lavender 
Thistle 
Plum 
Violet 
Orchid 
Fuchsia 
Magenta 
MediumOrchid 
MediumPurple 
RebeccaPurple 
BlueViolet 
DarkViolet 
DarkOrchid 
DarkMagenta 
Purple 
Indigo 
SlateBlue 
DarkSlateBlue 
MediumSlateBlue 
GreenYellow 
Chartreuse 
LawnGreen 
Lime 
LimeGreen 
PaleGreen 
LightGreen 
MediumSpringGreen 
SpringGreen 
MediumSeaGreen 
SeaGreen 
ForestGreen 
Green 
DarkGreen 
YellowGreen 
OliveDrab 
Olive 
DarkOliveGreen 
MediumAquamarine 
DarkSeaGreen 
LightSeaGreen 
DarkCyan 
Teal 
Aqua 
Cyan 
LightCyan 
PaleTurquoise 
Aquamarine 
Turquoise 
MediumTurquoise 
DarkTurquoise 
CadetBlue 
SteelBlue 
LightSteelBlue 
PowderBlue 
LightBlue 
SkyBlue 
LightSkyBlue 
DeepSkyBlue 
DodgerBlue 
CornflowerBlue 
MediumSlateBlue 
RoyalBlue 
Blue 
MediumBlue 
DarkBlue 
Navy 
MidnightBlue 
Cornsilk 
BlanchedAlmond 
Bisque 
NavajoWhite 
Wheat 
BurlyWood 
Tan 
RosyBrown 
SandyBrown 
Goldenrod 
DarkGoldenrod 
Peru 
Chocolate 
SaddleBrown 
Sienna 
Brown 
Maroon 
White 
Snow 
HoneyDew 
MintCream 
Azure 
AliceBlue 
GhostWhite 
WhiteSmoke 
SeaShell 
Beige 
OldLace 
FloralWhite 
Ivory 
AntiqueWhite 
Linen 
LavenderBlush 
MistyRose 
Gainsboro 
LightGray 
Silver 
DarkGray 
Gray 
DimGray 
LightSlateGray 
SlateGray 
DarkSlateGray 
and
Black 
.

# The force field file format {#force_field_file}

\brief   Description of the requirements for a force field file

 * \author  James Sturgis
 * \date    9 April 2019
 * \version 1.0

The force field file is structured as follows:

 * line 1: the number of different types of atom
 * line 2: the radius of each bead type (float)
 * line 3: the color for each bead type
 * line 4: two numbers, the interaction cut-off for calculations and the length scale.
 * line 5..: an n_atom by n_atom matrix of interaction strengths.

Colors are taken from the ist of defined colors which is currently: red, green, blue, orange...

## Comments
Blank lines are ignored and lines with a # as the first printing character are considered comments.

Continuation lines?

This file format is used exclusively in the force_field class.

# Plot file format {#Plot_file}

# Map file format {#Map_file}

