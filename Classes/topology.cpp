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
    molecules.resize(n_molecules);
    for( size_t i =0; i < n_molecules; i++ ){
        molecules(i).mol_name.assign(orig->molecules(i).mol_name);
        molecules(i).n_atoms = orig->molecules(i).n_atoms;
        molecules(i).the_atoms.resize(molecules(i).n_atoms);
        for(int j=0; j< molecules(i).n_atoms; j++ ){
            molecules(i).the_atoms(j) = orig->molecules(i).the_atoms(j);
        }
    }
}

topology::topology(const char *filename) {      // Read the topology from a named file.
    ifstream ff(filename);
    read_topology( ff );
    ff.close();
}

topology::topology(std::istream& source) {      // Read the topology from an open file.
    read_topology( source );
}

topology::topology(float size) {
    n_atom_types = 0;
    n_molecules  = 0;
    add_molecule( size );
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

/**
 * Helper function to strip blank lines and comments from an input stream.
 * This should really be declared in common.h and defined in common.cpp
 * TODO Create and organise common.cpp
 */
bool
my_getline(std::istream& ff, string *line){
    while(getline(ff, *line)){		        // Read a new line
        if(line->find('#')<line->npos)		// Remove comments
            line->erase(line->find('#'));
        while(line->size() && isspace(line->front())) 
            line->erase(line->begin());
        while(line->size() && isspace(line->back())) 
            line->pop_back();
        if( !line->empty() ) return true;
    }
    return false;
}


/**
 * Read a topology from a FILE
 * @param source a FILE pointer for an input stream.
 *
 * This should deal sensibly with errors and be able to ignore blank lines
 * and comments that run from # to the end of the line.
 * Various errors either from the file reading or the file structure will
 * cause a runtime_error to be thrown.
 */
void
topology::read_topology(std::istream& ff ) {
    size_t	i;
    double	size;
    std::string	line;
    std::string	name;

    // Check if the file exists
    if(ff.fail())                     throw runtime_error("Could not open topology file\n");
    if(!my_getline(ff, &line))        throw runtime_error("Found no content in the topology file\n");
    istringstream iss(line);
    if (!(iss >> n_atom_types))       throw runtime_error("First line of the topology file should have number of atom types, exiting ...\n");
    atom_names.resize(n_atom_types);
    atom_sizes.resize(n_atom_types);
    for( i = 0; i < n_atom_types; i++ ){
        if(!my_getline(ff, &line))    throw runtime_error("Error reading atom list unexpected end..\n");
        iss.clear();
        iss.str(line);
        if( !(iss >> name >> size ))  throw runtime_error("Error reading atom list unexpected line...\n");
        atom_sizes[i] = size;
	atom_names(i).assign(name);
    }

    if(!my_getline(ff, &line))        throw runtime_error("File ended before molecule descriptions., exiting... \n");
    iss.clear();
    iss.str(line);
    if (!(iss >> n_molecules))        throw runtime_error("Failed to read number of molecules, exiting ...\n");
    molecules.resize(n_molecules);
    for( i = 0; i < n_molecules; i++ ){
        if(!my_getline(ff, &line))    throw runtime_error("File ended unexpectedly in molecule descriptions., exiting... \n");
        iss.clear();
        iss.str(line);
        if (!(iss >> name))           throw runtime_error("File ended unexpectedly in molecule descriptions., exiting... \n");
        molecules[i].mol_name.assign(name);
        if(!my_getline(ff, &line))    throw runtime_error("File ended unexpectedly in molecule descriptions., exiting... \n");
        iss.clear();
        iss.str(line);
        if (!(iss >> molecules[i].n_atoms)) 
                                      throw runtime_error("File ended unexpectedly in molecule descriptions., exiting... \n");
        assert( molecules[i].n_atoms > 0 );
        molecules[i].the_atoms.resize(molecules[i].n_atoms);
        for(int j=0; j< molecules[i].n_atoms; j++ ){
            if(!my_getline(ff, &line ))
                                      throw runtime_error("File ended unexpectedly in molecule descriptions., exiting... \n");
            iss.clear();
            iss.str(line);
            if (!(iss >> molecules[i].the_atoms[j].type
                      >> molecules[i].the_atoms[j].x_pos
                      >> molecules[i].the_atoms[j].y_pos
                      >> molecules[i].the_atoms[j].color ))
                                      throw runtime_error("File ended unexpectedly in molecule descriptions., exiting... \n");
            if( molecules[i].the_atoms[j].type >= (int) n_atom_types )
                                      throw runtime_error("Undefined atom type in molecule\n");
        }
    }
}

/**
 * Write the topology to an ostream.
 * @param the address of the output stream.
 * @return true for success.
 *
 * TODO Error handling
 */
int
topology::write(ostream& dest){
    size_t	        i;

    dest << n_atom_types << "\n";
    for( i = 0; i < n_atom_types; i++ ){
        dest << atom_names(i) << "\t" << atom_sizes(i) << "\n";
    }
    dest << n_molecules << "\n";
    for( i = 0; i < n_molecules; i++ ){
        dest << molecules(i).mol_name.c_str() << "\n" 
             << std::to_string(molecules(i).n_atoms) << "\n";
        for(int j = 0; j < molecules(i).n_atoms; j++)
            dest << molecules(i).the_atoms(j).type << "\t" 
                 << molecules(i).the_atoms(j).x_pos << "\t" 
                 << molecules(i).the_atoms(j).y_pos << "\t" 
                 << molecules(i).the_atoms(j).color << "\n";
    }
    return true;
}

/**
 * Add a simple molecules (1 central atom) to the topology.
 * @param r the radius of the atom to add for this molecule.
 *
 * This is used as for the default topology file.
 */
void
topology::add_molecule( float r ){
    int  i;
    atom *an_atom = NULL;

    i = n_atom_types;
    n_atom_types++;
    atom_names.resize( n_atom_types );
    atom_sizes.resize( n_atom_types );
    atom_names(i) = "Simple";
    atom_sizes(i) = r;

    an_atom = new atom(i, 0.0, 0.0, "Red" );

    i = n_molecules;
    n_molecules++;
    molecules.resize( n_molecules );
    molecules[i].rename( "Hard disk" );
    molecules[i].add_atom( an_atom );
    delete an_atom;
}

