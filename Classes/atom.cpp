/**
 * @file    atom.cpp  Implementation file for the atom class.
 * @author  James Sturgis
 * @date    April 6, 2018
 * @class   atom atom.h "atom.h"
 *
 * @todo    Replace c strings with c++ strings.
 * @todo    Replace FILE * with istream and ostream.
 * @todo    Error management and exceptions.
 * @todo    Improve documentation and add class methods (see documentation)
 */

#include <string.h> // for strcpy()
#include "atom.h"

#include <boost/format.hpp>
using boost::format;

/**
 * \brief  Constructor creating an empty (but valid) atom.
 */
atom::atom() {
    type =
    x_pos =
    y_pos = 0;
    color[0] = '\0';
}

/** \brief Constructor for an atom of given type and position
 *
 *  \param  t the atom type.
 *  \param  x the atoms x position relative to the object center
 *  \param  y the atoms y position relative to the object center
 *  \param  col pointer to a c_type string containing a postscript acceptable representation of the color.
 */
atom::atom(int t, double x, double y, const char *col){
    type  = t;
    x_pos = x;
    y_pos = y;
    strcpy( color, col );
}

/** \brief Constructor reproducing an original atom via a copy.
 *
 *  @param orig The atom to copy in position and type.
 *  @return the copied atom.
 */
atom::atom(const atom& orig) {
    copy(orig);
}

/** \brief Destructor for atoms (virtual). No complex parts.
 */
atom::~atom() {
}

/** \brief Write the atom to a file.
 *
 *  \param dest FILE*   The file pointer to the destination
 *                      open for writing.
 *  \return int         The return value (error status) from the
 *                      print call.
 */
int     
atom::write(FILE* dest){
    return fprintf(dest, "%3d %9g %9g %s\n", type, x_pos, y_pos, color);
}

/** \brief Write the atom to a stream.
 *
 *  \param dest ofstream&   The destination output stream.
 *  \return int         The return value (error status).
 */
int     
atom::write(std::ostream& dest){
    dest << boost::format("%3d %9g %9g %s\n") % type % x_pos % y_pos % color;
    return EXIT_SUCCESS;
}

/** \brief Copy an atom description to this atom.
 *
 *  \param orig const atom&   Pointer to the original atom.
 *  On exit the values of the atom descriptors are changed.
 */
void
atom::copy(const atom& orig ){
    type  = orig.type;
    x_pos = orig.x_pos;
    y_pos = orig.y_pos;
    strcpy( color, orig.color );
}

