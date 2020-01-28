/**
 * \file    topology.h
 * \author  James Sturgis
 * \date    April 11, 2018
 * \version 1.0
 * \brief   Header file for the topology class.
 *
 * \class   topology topology.h
 * \brief   A class describing the structure of several molecules.
 *
 */

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "molecule.h"
#include <string>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;   // For vector

#define MAX_ATOMS   16
#define MAX_TOPO    16

class topology {
public:
    topology();				 ///< Constructor empty topology
    topology(topology *orig);            ///< Constructor with copy from pointer
    topology(const topology& orig);      ///< Constructor with copy from object
    topology(const char *filename);	 ///< Constructor read from named file
    topology(std::istream& source); 	 ///< Constructor from input stream
    topology(float r);			 ///< Constructor default simple topology

    virtual ~topology();                 ///< Destructor

                                         /* This is really a bit of a mess */
                                         /* TODO Tidy up topology interface */
/*    void     fill_topology(vector<double> radius, std::string topology_file); NVT
    int     n_atom(int type);            ///< Number of atoms in this topology
    atom    *atoms(int type, int i);     ///< Function to read data
    int     n_top;                       ///< how many topologies
*/
    int     write(const char *filename); ///< Write the topology to a named file. (Absent)
///    int     write(FILE *dest);  	 ///< Write the topology to a file.
    int     write(std::ostream& dest );  ///< Write the topology to c++ ofstream.

    void    add_molecule( float r );     ///< Add a new molecule type to the topology circle radius r.

    size_t  n_atom_types;                ///< Total number of different atom types.
    vector<std::string>    atom_names;   ///< Labels for the different types of atoms.
    vector<double>         atom_sizes;   ///< Atom sizes for drawing.
    size_t  n_molecules;                 ///< Number of different molecules in topology.
    vector<molecule>       molecules;    ///< List/Vector of the different molecule types.
private:
    void    read_topology(std::istream& source); ///< Helper routine for reading a topology file.
    bool    check();                     ///< Helper routine verify that the topology is good.

/*  bool    check_topology();            ///< Verify all is well with the topology.
    int     len[MAX_TOPO];               ///< Number of atoms of different types. (JS 16/4)
    atom    **data[MAX_TOPO];            ///< The atoms in the topology
    vector<std::string>    data_type;       // To keep track of the type (single, square, etc)
                                         // Probably needs another class for a sub-topology or smth
*/
};

#endif /* TOPOLOGY_H */

