/**
 * \file    molecule.h
 * \author  James Sturgis
 * \date    April 11, 2019
 * \version 1.0
 * \brief   Header file for the molecule class.
 *
 * \class   molecule molecule.h
 * \brief   A class describing the structure of molecules.
 *
 */

#ifndef MOLECULE_H
#define MOLECULE_H

#include <string>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>

#include "atom.h"

using namespace boost::numeric::ublas;   // For vector

class molecule {
public:
    molecule();				///< Constructor for an empty molecule
    molecule(molecule *orig);		///< Constructor copying original

    virtual ~molecule();		///< A destructor to remove the molecule

    void add_atom( atom *an_atom );	///< Add an atom to the molecule
    void rename( std::string a_name );  ///< Change the name of a molecule

///    int	write(FILE *dest);		///< Write the molecule to an opened file
    int	write(std::ostream& dest);	///< Write the molecule to a stream

    std::string		mol_name;	///< A name for the molecule
    int         	n_atoms;	///< The number of atoms in the molecule
    vector<atom>	the_atoms;	///< The individual atoms in a vector
};

#endif /* MOLECULE_H */
