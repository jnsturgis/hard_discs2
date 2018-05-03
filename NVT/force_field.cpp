/**
 * \file   force_field.cpp
 * \author James Sturgis
 * \date   April 6, 2018
 */

#include "common.h"
#include "force_field.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#define  BIGVALUE   10E6

#include <boost/format.hpp>
using boost::format;

using namespace std;

force_field::force_field() {

    // Empty force-field !
    cut_off    = 0;
    // Except length
    length     = 1.0;
    type_max   = 0;
    big_energy = BIGVALUE;
    
    //~ for( i=0; i< type_max; i++ ){
        //~ radius[i] = my_radius[i];
        //~ color[i]  = my_color[i];
        //~ for( j = 0; j < type_max; j++ ) energy[i][j] = my_energy[i][j];
    //~ }
}

force_field::force_field(const force_field& orig) {
    int i, j;

    cut_off    = orig.cut_off;
    length     = orig.length;
    type_max   = orig.type_max;
    big_energy = orig.big_energy;
    for( i=0; i< type_max; i++ ){
        radius(i) = orig.radius(i);
        color[i]  = orig.color[i];
        for( j = 0; j < type_max; j++ ) energy(i,j) = orig.energy(i, j);
    }
}

force_field::~force_field() {
}

// Update a force field (empty or not) with a file
void force_field::update(std::string ff_filename) {
    int         i, j;
    string      line;
    double      temp_number;
    string      temp_char;
    int         n_check = 0;
    
    // Open a stream of the file
    ifstream ff(ff_filename.c_str());
    
    // Get the first line
    getline(ff, line);
    istringstream iss;
    iss.str(line);
    
    /* For the matrix, three possibilities
     * Using Boost, using std::vector or just building the matrix from base types
     * Going for Boost, even though it might get tricky later
     * As it seems to be the best, with both vector and matrix types */
     
    // The first line is the number of beads, so
    if (!(iss >> type_max)) {
        throw range_error("First line of the force-field file should only be the number of beads ...\n");
    }
    
    // Clear iss
    iss.clear();
    
    // Next line
    getline(ff, line);
    
    if (!(line.empty())) {
        iss.str(line);
    } else {
        throw runtime_error("Second line was empty ?\n");
    }

    // Since we know the number of beads ...
    // Now the radius
    radius.resize(type_max);   
    // Loop to fill the vector
    for ( i=0; iss >> temp_number; i++ ) {
        
        // If we have too many things in this line
        if (i >= type_max) {
            throw range_error("Too many radiuses ! Exiting ...\n");
        }
        
        // Fill it
        radius (i) = temp_number;
        
    }
    
    //~ cout << "We have " << type_max << " radiuses\n";
    
    // Next line
    iss.clear();
    getline(ff, line);
    iss.str(line);
    
    // The colors, kinda hard coded so please don't modify them
    // Keep red green blue orange please
    // Still loop
    color.resize(type_max);
    for ( i=0; iss >> temp_char; i++ ) {
        
        // If we have too many things in this line
        if (i >= type_max) {
            //~ cout << "There are " << i << " colors there \n";
            throw range_error("Too many colors ! Exiting ...\n");
        }
        
        // Fill it
        color (i) = temp_char;
        
        n_check ++;
    }
    
    // Again, we should have type_max beads == n_check here
    if (!(type_max == n_check)) {
        throw range_error("Third line of the force-field file should be the colors of the beads ...\n");
    }
    
    // Now the cutoff and length - still questioning whether we need it in the object or somewhere else
    iss.clear();
    getline(ff, line);
    iss.str(line);
    if (!(iss >> cut_off >> length)) {
        throw range_error("Fourth line of the force-field file should be the cutoff and the length ...\n");
    }
    
    // And the matrix of interaction, and using type_max
    // Not checking mistakes here, please don't do any
    // Since we know type_max, we know the size of the matrix, we can initialize it 
    energy.resize(type_max, type_max);
    // And we can then just loop through the type_max lines and fill it
    for( i=0; i<type_max; i++) {
        iss.clear();
        getline(ff, line);
        iss.str(line);
        
        for( j=0; iss >> temp_number; j++) {
            energy(i, j) = temp_number;
        }
    }
    
    // Close the stream
    ff.close();

}

double
force_field::interaction(int t1, int t2, double r) {
    double value = 0.0;
    double hard;

    if(r < cut_off){

        assert( t1 < type_max );
        assert( t2 < type_max );

        // Implementation of triangle potential with repulsive core
        hard  = radius(t1) + radius(t2);
        r    -= hard;
        if( r < 0 ){
            value = big_energy * (1 - r/hard);
        } else if (r< length) {
            r /= length;                        /// r between 0.0 and 1.0
            value = energy(t1, t2) * (1.0-r);
        }
    }
    return value;
}

double  force_field::size(int t1){
    return radius[t1];
}

void force_field::write(std::ofstream& _log){
    int i,j;

    _log << "\nSummary of the force-field\n";
    _log << "Cut off is " << cut_off << "\n";
    _log << "Length scale is " << length << "\n";
    _log << "Number of atom types is " << type_max << "\n";
    _log << "Colors are  [";
    for( i=0; i< type_max; i++) {
        _log << format("%g ,") % color(i);
    }
    _log << "]\nRadius array is  [";
    for( i=0; i< type_max; i++ ) {
        _log << format("%7.3g ,") % radius(i);
    }
    _log << "]\nEnergy array is [";
    for( i=0; i< type_max; i++ ){
        if( i > 0) {
            _log << "\n                 ";
        }
        _log << "[";
        for(j=0;j<type_max; j++ ){
            _log << format("%7.3g ,") % energy(i, j);
        }
        _log << "]";
    }
    _log << "]\n\n";

}

const char  *force_field::get_color(int t){
    return color[t].c_str();
}
