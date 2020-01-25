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

