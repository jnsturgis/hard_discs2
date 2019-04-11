/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * @file    atom.cpp  Implementation file for the atom class.
 * @author  James Sturgis
 * @date    April 6, 2018
 * @class   atom atom.h "atom.h"
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include "common.h"
#include "atom.h"
#include <boost/format.hpp>
using boost::format;


/**
 * \brief  Constructor reading an atom from a file.
 * \param  src File descriptor to read the new atom from.
 * \return the new atom
 *
 * If there is an error during the read then the new atoms
 * type will be -1. Currently not implemented.
 *
 * \todo   Implement this constructor.
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
 *  \return the new atom
 */
atom::atom(int t, double x, double y, const char *col){
    type  = t;
    x_pos = x;
    y_pos = y;
    strcpy( color, col );
}

/** \brief Constructor reproducing an original atom and a copy.
 *
 *  @param orig The atom to copy in position and type.
 *  @return the copied atom.
 */
atom::atom(const atom& orig) {
    copy(orig);
}

/** Destructor for atoms (virtual).
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
int     atom::write(FILE* dest){
    return fprintf(dest, "%3d %9g %9g %s\n", type, x_pos, y_pos, color);
}

void
atom::read(FILE *source ){
    fscanf(source, "%d %lf %lf %s\n", &type, &x_pos, &y_pos, &color[0]);
}

void
atom::copy(const atom& orig ){
    type  = orig.type;
    x_pos = orig.x_pos;
    y_pos = orig.y_pos;
    strcpy( color, orig.color );
}
