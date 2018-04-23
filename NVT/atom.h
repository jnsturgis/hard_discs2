/**
 * @file    atom.h
 * @author  James Sturgis
 * @date    April 6, 2018
 * @version 1.0
 * \brief   Header file for the atom class.
 *
 * @class   atom atom.h
 * @brief   A class for sub-objects that have position and type.
 *
 * The atom type (an integer) should correspond to one of the
 * atom types present in the force field. The other values in the class
 * are the position, relative to its reference point, which should be
 * the center of mass. These values are all public.
 *
 * Class methods include methods for reading and writing atoms to a file,
 * creating atoms with explicit values for the type and position, creating
 * a copy of an existing atom and a virtual destructor.
 */

#ifndef ATOM_H
#define ATOM_H

#include <stdio.h>

class atom {
public:
    atom(FILE *src);            ///< Constructor reading an atom from a file.
    atom(int t,
         double x, double y);   ///< Constructor with type and position
    atom(const atom& orig);     ///< Constructor for the copy of an atom
    virtual ~atom();            ///< Destructor for atoms
    int     write(FILE *dest);  ///< Write the atom descriptor
    int     type;               ///< Integer atom type, controls interactions
    double  x_pos;              ///< Atom x position relative to the object origin.
    double  y_pos;              ///< Atom y position relative to the object origin.
private:
};

#endif /* ATOM_H */

