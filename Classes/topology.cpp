/**
 * \file    topology.cpp
 * \author  James Sturgis
 * \date    April 11, 2018
 * \version 1.0
 * \brief   Implementation of the topology class.
 *
 * @todo Read a topology file
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

/**
 * Initialize hard coded topology should really be empty topology and then allow
 * the manipulation of the topology. It would be beter if there was no hard coded
 * maxima and the size could be obtained without counting, and there was less wasted
 * space. Plenty of room for improvement.
 */
topology::topology() {                          // An empty topology has no molecules or atoms.
    n_atom_types = 0;
    n_molecules  = 0;
    check();
}

topology::topology(topology* orig){		// Stub should copy
    assert(false);                  		// Stub not ready to be used
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
topology::read_topology(FILE *source) {	/// @todo Error handling and comments
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
topology::write(FILE *dest){
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
topology::write(ostream& dest){
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

/*
void topology::fill_topology(boost::numeric::ublas::vector<double> radius, std::string topology_file) {
    
     * Read the atom types from the topology file
     * Then create atoms at the right spot for each pre-defined topology
     * Currently only 3 types - single, square and triangle 
     * Basically, read one line (single, square or triangle)
     * And add the corresponding atoms in the next line 
    
    // Declare what will handle our lines
    istringstream      iss;
    std::string        line;
    std::string        temp_str;
    // And an int to count the number of topologies
    int           data_count = 0;
    // And ints for atom types, currently 8, should perhaps increase it
    // With more complicated topologies
    int           a1, a2, a3, a4;
    
    // First open a stream to read the file
    ifstream ff(topology_file.c_str());
    
    // While it's not eof ...
    while (getline(ff, line)) {
        
        // Load in the istrinstream
        iss.str(line);
        
        if( (iss >> temp_str) ) {
            if( !temp_str.compare("single") ) {
                
                // If it's a single atom, first go to the next line
                iss.clear();
                getline(ff, line);
                iss.str(line);
                
                data_type.push_back("single");
                
                // Get the atom type
                if( (iss >> a1) ) {
                    data[data_count] = (atom **)malloc(MAX_ATOMS*sizeof(atom*));
                    for(int j = 0; j < MAX_ATOMS; j++) data[data_count][j] = (atom *)NULL;
                    data[data_count][0] = new atom(a1, 0.0, 0.0, "red");  // Memory losses here
                    len[data_count]=1;           
                    
                    data_count ++;
                    n_top ++;
                }
            } else if( !temp_str.compare("square") ) {

                // If it's a square, first go to the next line
                iss.clear();
                getline(ff, line);
                iss.str(line);
                
                data_type.push_back("square");

                // Get the 4 atom types
                if( (iss >> a1 >> a2 >> a3 >> a4) ) {
                    data[data_count] = (atom **)malloc(MAX_ATOMS*sizeof(atom*));
                    for(int j = 0; j < MAX_ATOMS; j++) data[data_count][j] = (atom *)NULL;
                        
                    // We have the radius, we want to put the atoms at the right places, so ... 
                    // It's at the points (x, y) (radius, radius)                  
                    data[data_count][0] = new atom(a1, radius(a1), radius(a1), "red");
                    data[data_count][1] = new atom(a2, radius(a2),-radius(a2), "blue");
                    data[data_count][2] = new atom(a3,-radius(a3), radius(a3), "green");
                    data[data_count][3] = new atom(a4,-radius(a4),-radius(a4), "orange");
                    len[data_count]= 4;       

                    data_count ++;
                    n_top ++;
                }
            } else if( !temp_str.compare("triangle") ) {
                
                // If it's a triangle ..
                iss.clear();
                getline(ff, line);
                iss.str(line);
                
                data_type.push_back("triangle");

                // Get the 3 atom types
                if( (iss >> a1 >> a2 >> a3) ) {
                    data[data_count] = (atom **)malloc(MAX_ATOMS*sizeof(atom*));
                    for(int j = 0; j < MAX_ATOMS; j++) data[data_count][j] = (atom *)NULL;
                    
                    // A triangle, should get the right coordinates as this is not a triangle
                     * For an equilateral triangle with the point 0,0 at its center of geometry
                     * And the upper point at 0, 1
                     * The y of the two other points is 0.5
                     * The x (or -x) is sin(30)*1 
                     * Of course multiply all these by the radius *
                    data[data_count][0] = new atom(a1,                  0, radius(a1), "red");
                    data[data_count][1] = new atom(a2, sin(30)*radius(a2),-0.5*radius(a2), "blue");
                    data[data_count][2] = new atom(a3,-sin(30)*radius(a3),-0.5*radius(a3), "green");
                    len[data_count]= 3;       

                    data_count ++;
                    n_top ++;
                }
            }
        }
        
        iss.clear();
    }
    ff.close();
}
*/
/**
 * How many atoms are there in objects of the given type?
 * @param type  The type of object concerned.
 * @return      The number.
 *
int     topology::n_atom(int type){
    return len[type];       // JS 16/4
//    int     i;
//    assert(data[type] != (atom **)NULL );    ///< There is some data
//    for(i=0; i< MAX_ATOMS; i++){
//if (atoms(type,i) == (atom *)NULL ) break;
//}
//return i;
}
*/
/**
 *
 * @param type
 * @param i
 * @return
atom*   topology::atoms(int type, int i) {
    assert(data[type] != (atom **)NULL );
    return data[type][i];
}
*/
/*
int     topology::write(std::ofstream& _log){
    int     i;
    for(i = 0; i < MAX_TOPO; i++){
        if (data[i] == (atom **)NULL ) break;
    }
    _log << format("\nTopology summary\n%d different topologies\n") % i;
    for(i = 0; i < MAX_TOPO; i++){
        if (data[i] == (atom **)NULL ) break;
        
        // Now for each type of topology ...
        _log << "Topology " << data_type[i] << " number " << i << " has " << len[i] << " beads with these bead types: ";
        
        for(int j = 0; j < MAX_ATOMS; j++){
            if( !data[i][j] ) break;
            _log << format("%g ") % data[i][j]->type;
        }
        _log << "\n";
    }
    _log << "\n";
    return 1;
}
*/
