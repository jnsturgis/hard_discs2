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

#define MAX_ATOMS   16
#define MAX_TOPO     8

class topology {
public:
    topology();
    topology(int i);
    topology(topology *orig);
    topology(const topology& orig);
    virtual ~topology();
    int     n_atom(int type);           ///< Number of atoms in this topology
    atom    *atoms(int type, int i);    ///< Function to read data
    int     write(FILE *dest);          ///< Write the topology info.
private:
    int     check();                    ///< Verify all is well with the topology.
    int     len[MAX_TOPO];               ///< Number of atoms of different types. (JS 16/4)
    atom    **data[MAX_TOPO];           ///< The atoms in the topology
};

#endif /* TOPOLOGY_H */

