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
topology::topology() {
    int i;

    for(i=0; i<MAX_TOPO; i++){
        data[i]=(atom **)NULL;
        len[i] = 0;     // JS 16/4
    }
    
    // If we don't create the atoms here, we can create them later at the right place
    
    // Now lets put in the first topology with 1 central atom
    //~ data[0] = (atom **)malloc(MAX_ATOMS*sizeof(atom*));
    //~ for(int j = 0; j < MAX_ATOMS; j++) data[0][j] = (atom *)NULL;
    //~ data[0][0] = new atom(0, 0.0, 0.0);  // Memory losses here
    //~ len[0]=1;           // JS 16/4

    // Now more complex a squarish object
    //~ data[1] = (atom **)malloc(MAX_ATOMS*sizeof(atom*));
    //~ for(int j = 0; j < MAX_ATOMS; j++) data[1][j] = (atom *)NULL;
    //~ data[1][0] = new atom(1, 1.0, 1.0);
    //~ data[1][1] = new atom(1, 1.0,-1.0);
    //~ data[1][2] = new atom(1,-1.0, 1.0);
    //~ data[1][3] = new atom(1,-1.0,-1.0);
    //~ len[1]= 4;          // JS 16/4
    
    // And a triangle one
    //~ data[2] = (atom **)malloc(MAX_ATOMS*sizeof(atom*));
    //~ for(int j = 0; j < MAX_ATOMS; j++) data[1][j] = (atom *)NULL;
    //~ len[2] = 3;
}

topology::topology(topology* orig){
    topology();                             // Cheat
    assert(true);
}

topology::topology(const topology& orig) {
    assert(false);                  // Stub not ready to be used
}

topology::~topology() {
    for(int i = 0; i < MAX_TOPO; i++ ){
        if(data[i]!=(atom **)NULL){
            for(int j = 0; j< MAX_ATOMS; j++)
                if(data[i][j] != (atom *)NULL)
                    delete data[i][j];
            free(data[i]);
        }
    }
}

int topology::fill_topology(boost::numeric::ublas::vector<double> radius, std::string topology_file) {
    
    /* Read the atom types from the topology file
     * Then create atoms at the right spot for each pre-defined topology
     * Currently only 3 types - single, square and triangle 
     * Basically, read one line (single, square or triangle)
     * And add the corresponding atoms in the next line */
    
    // Declare what will handle our lines
    istringstream iss;
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
                    data[data_count][0] = new atom(a1, 0.0, 0.0);  // Memory losses here
                    len[data_count]=1;           
                    
                    data_count ++;
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
                    data[data_count][0] = new atom(a1, radius(a1), radius(a1));
                    data[data_count][1] = new atom(a2, radius(a2),-radius(a2));
                    data[data_count][2] = new atom(a3,-radius(a3), radius(a3));
                    data[data_count][3] = new atom(a4,-radius(a4),-radius(a4));
                    len[data_count]= 4;       

                    data_count ++;
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
                    /* For an equilateral triangle with the point 0,0 at its center of geometry
                     * And the upper point at 0, 1
                     * The y of the two other points is 0.5
                     * The x (or -x) is sin(30)*1 
                     * Of course multiply all these by the radius */
                    data[data_count][0] = new atom(a1,                  0, radius(a1));
                    data[data_count][1] = new atom(a2, sin(30)*radius(a2),-0.5*radius(a2));
                    data[data_count][2] = new atom(a3,-sin(30)*radius(a3),-0.5*radius(a3));
                    len[data_count]= 3;       

                    data_count ++;
                }
            }
        }
        
        iss.clear();
    }
    
    return 1;
}

/**
 * How many atoms are there in objects of the given type?
 * @param type  The type of object concerned.
 * @return      The number.
 */
int     topology::n_atom(int type){
    return len[type];       // JS 16/4
//    int     i;
//    assert(data[type] != (atom **)NULL );    ///< There is some data
//    for(i=0; i< MAX_ATOMS; i++){
//if (atoms(type,i) == (atom *)NULL ) break;
//}
//return i;
}

/**
 *
 * @param type
 * @param i
 * @return
 */
atom*   topology::atoms(int type, int i) {
    assert(data[type] != (atom **)NULL );
    return data[type][i];
}

int     topology::write(std::ofstream& _log){
    int     i;
    for(i = 0; i < MAX_TOPO; i++){
        if (data[i] == (atom **)NULL ) break;
    }
    _log << format("\nTopology summary\n%d different topologies\n") % i;
    for(i = 0; i < MAX_TOPO; i++){
        if (data[i] == (atom **)NULL ) break;
        
        // Now for each type of topology ...
        _log << "Topology " << data_type[i] << " number " << i << " has these bead types: ";
        
        for(int j = 0; j < MAX_ATOMS; j++){
            if( !data[i][j] ) break;
            _log << format("%g ") % data[i][j]->type;
        }
        _log << "\n";
    }
    _log << "\n";
    return 1;
}
