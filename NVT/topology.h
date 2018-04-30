/**
 * \file    topology.h
 * \author  James Sturgis
 * \date    April 11, 2018
 * \version 1.0
 * \brief   Header file for the topology class.
 *
 * \class   topology topology.h
 * \brief   A class describing the structure of objects.
 *
 */

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "atom.h"
#include <string>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;

#define MAX_ATOMS   16
#define MAX_TOPO     8

class topology {
public:
    topology();
    topology(int i);
    topology(topology *orig);
    topology(const topology& orig);
    virtual ~topology();
    int     fill_topology(vector<double> radius, std::string topology_file);
    int     n_atom(int type);           ///< Number of atoms in this topology
    atom    *atoms(int type, int i);    ///< Function to read data
    int     write(std::ofstream& _log);          ///< Write the topology info.
private:
    int     check();                    ///< Verify all is well with the topology.
    int     len[MAX_TOPO];               ///< Number of atoms of different types. (JS 16/4)
    atom    **data[MAX_TOPO];           ///< The atoms in the topology
    std::vector<std::string>    data_type;       // To keep track of the type (single, square, etc)
                                        // Probably needs another class for a sub-topology or smth
};

#endif /* TOPOLOGY_H */

