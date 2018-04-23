/**
 * \file    force_field.h
 * \author  James Sturgis
 * \version 1.0
 * \date    April 6, 2018
 * \brief   Header file for the force_field class
 *
 * \class force_field force_field.h
 * \brief The force_field class.
 *
 * The force field allows the determination of the potential energy
 * associated with a configuration. The fundamental function of the
 * class is interaction(t1, t2, r ) that returns the interaction energy
 * associated with two particles separated by a distance r, one of
 * type t1 the other of type t2.
 *
 * Internally the force field contains the following information and
 * tables.
 * * The number of different atom types (type_max).
 * * The interaction length scale (currently all interactions the same).
 * * A vector of type_max radii, the hard core sizes of each atom
 *   type in the force field.
 * * A vector of type_max colors, for the postscript representation of
 *   the different
 * * A matrix of well depths (this should be symmetric).
 * * A cut_off distance beyond which all interactions are zero.
 * * A big_number that is a stand_in for infinity but avoids the numerical
 *   problems associated with infinity.
 *
 * Constructor methods are defined for a a default force_field and a
 * copy constructor, and a destructor method.
 *
 * There are methods for:
 * * writing the forcefield to a file descriptor.
 * * obtaining the hard core size of an atom.
 * * obtaining a postscript string setting the color of an atom
 *
 * \todo Constructor from a reading a file
 * \todo Pair dependent length scale.
 */

#ifndef FORCE_FIELD_H
#define FORCE_FIELD_H

#include <stdio.h>

// There must be an efficient beter way
#define  MAX_TYPE   4
// The forcefield is currently hard coded... should read from a file.

class force_field {
public:
    force_field();                          ///< Constructor default
    force_field(const force_field& orig);   ///< Constructor with copy
    virtual ~force_field();                 ///< Destructor
    double      interaction(int t1, int t2, double r); ///< Calculate interaction energy
    double      size(int t1);               ///< The hard core size of an atom type t1.
    int         write( FILE *dest );        ///< Write the forcefield to file
    const char  *get_color(int t);          ///< Color for plot output
    double      cut_off;                    ///< Distance cutoff between objects
    double      big_energy;                 ///< Large value less than infinity.
private:
    int         type_max;                   ///< The number of different atom types
    double      length;                     ///< Interaction length (probably should be array)
    double      radius[MAX_TYPE];           ///< Atom radii
    const char *color [MAX_TYPE];           ///< Atom colors for postscript
    double      energy[MAX_TYPE][MAX_TYPE]; ///< Pairwise interaction well depths.
};

#endif /* FORCE_FIELD_H */

