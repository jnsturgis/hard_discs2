/**
 * \file    topology.cpp
 * \author  James Sturgis
 * \date    April 11, 2018
 * \version 1.0
 * \brief   Implementation of the topology class.
 *
 */
#include <malloc.h>
#include "topology.h"
#include "common.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace std;

#include <boost/format.hpp>
using boost::format;

topology::topology() {                          // An empty topology has no molecules or atoms.
    n_atom_types = 0;
    n_molecules  = 0;
    check();
}

topology::topology(topology* orig){		// Make a deep copy
    n_atom_types = orig->n_atom_types;
    atom_names.resize(n_atom_types);
    atom_sizes.resize(n_atom_types);
    for( size_t i =0; i < n_atom_types; i++ ){
        atom_names(i).assign(orig->atom_names(i));
        atom_sizes(i) = orig->atom_sizes(i); 
    }
    n_molecules = orig->n_molecules;
    for( size_t i =0; i < n_atom_types; i++ ){
        molecules(i) = new molecule( orig->molecules(i));
    }
}

topology::topology(const char *filename) {      // Read the topology from a named file.
    FILE *source;
    if(( source = fopen( filename, "r" )) != NULL ){
        read_topology( source );
        fclose( source );
    } else {
        throw std::runtime_error("Error opening file");
    }
}

topology::topology(FILE *source) {              // Read the topology from an open file.
    read_topology( source );
}

topology::topology(float size) {
    atom *an_atom = new atom(0, 0.0, 0.0, "red" );

    n_atom_types = 1;
    atom_names.resize( n_atom_types );
    atom_sizes.resize( n_atom_types );
    atom_names(0) = "Simple";
    atom_sizes(0) = size;
    n_molecules  = 1;
    molecules.resize( n_molecules );
    molecules[0].rename( "Hard disk" );
    molecules[0].add_atom( an_atom );
}

topology::~topology() {
    molecules.resize(0);			// Destroy the molecules
    n_molecules = 0;
    atom_sizes.resize(0);
    atom_names.resize(0);			// Destroy the atom names
    n_atom_types = 0;
}

bool topology::check() {			// Should also check all molecules and names!
    return(
        ( n_molecules == molecules.size() ) &&
        ( n_atom_types == atom_names.size() ) &&
        ( n_atom_types == atom_sizes.size() ) );
}

void
topology::read_topology(FILE *source) {	        /// @todo Error handling and comments
    size_t	i;
    char	name[64];

    fscanf( source, "%lu\n", &n_atom_types );
    atom_names.resize(n_atom_types);
    atom_sizes.resize(n_atom_types);
    for( i = 0; i < n_atom_types; i++ ){
        fscanf( source, "%s %lf\n", name, &(atom_sizes(i)) );
	atom_names(i).assign(name);
    }
    fscanf( source, "%lu\n", &n_molecules );
    molecules.resize(n_molecules);
    for( i = 0; i < n_molecules; i++ )
        molecules(i).read( source );
}

int
topology::write(FILE *dest){                  /// @todo Error handling and comments
    size_t	        i;

    fprintf( dest, "%lu\n", n_atom_types );
    for( i = 0; i < n_atom_types; i++ ){
        fprintf( dest, "%s %lf\n", atom_names(i).c_str(), atom_sizes(i) );
    }
    fprintf( dest, "%lu\n", n_molecules );
    for( i = 0; i < n_molecules; i++ )
        molecules(i).write( dest );
    return true;
}

int
topology::write(ostream& dest){            /// @todo Error handling and comments
    size_t	        i;

    dest << boost::format("%lu\n") % n_atom_types;
    for( i = 0; i < n_atom_types; i++ ){
        dest << boost::format("%s %lf\n") % atom_names(i).c_str() % atom_sizes(i);
    }
    dest << boost::format("%lu\n") % n_molecules;
    for( i = 0; i < n_molecules; i++ )
        molecules(i).write( dest );
    return true;
}

