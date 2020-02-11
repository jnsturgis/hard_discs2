# The analysis programs

There are several programs that are designed to manipulate and analyse configurations and trajectories. Either to modify the configuration in some way, to produce various figures and diagrammes or perform calculations.

* map2eps - produce a postscript file figure from the output matrix of 2DOrder (or other such functions)
* pcf - calculate a pair correlation function / radial distribution function
* 2DOrder - calculate positional and rotational organization around objects in a configuration or trajectory
* wrap - calculate a convex polygon enclosing the objects in the configuration

Usage: map2eps [-c color_name] [-a] < map_file > eps_file

Usage: pcf [-v] [-o output] [-r dist] [-t type1] [-u type2] file1...
* -v verbose output to stderr,
* -o output send output to file output (default stdout),
* -r dist set the integration bin size to dist (default 1.0),
* -t type1 look at distances between objects of this type and type2 (default 0),
* -u type2 look at distances between objects of this type and type1 (default 0),
* file1... series of configuration files to read, if none are given use stdin.

Usage: 2DOrder [-v] [-z] [-o map_file] [-d dist] [-r rotation][-t type1] [-u type2] file1...
* -v verbose output to stderr,
* -z the input files are compressed trajectory files,
* -o output send output to file output (default stdout),
* -d dist set the integration bin size to dist (default 1.0),
* -r rotation, symmetry to apply for organization of orientation (default 1),
* -t type1 look at distances between objects of this type and type2 (default 0),
* -u type2 look at distances between objects of this type and type1 (default 0),
* file1... series of configuration or trajectory files to read, if none are given use stdin.



