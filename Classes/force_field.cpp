/**
 * \file   force_field.cpp
 * \author James Sturgis
 * \date   April 6, 2018
 */

#include "common.h"
#include "force_field.h"
#include <cstring>
#include <ctype.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#define  BIGVALUE   10E6

#include <boost/format.hpp>
using boost::format;

using namespace std;

force_field::force_field() {
    cut_off    = 2.0;
    length     = 1.0;
    type_max   = 0;
    big_energy = BIGVALUE;
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

force_field::force_field( const char *file_name ){
    FILE *source;
    cut_off    = 2.0;
    length     = 1.0;
    type_max   = 0;
    big_energy = BIGVALUE;

    if(( source = fopen( file_name, "r" )) != NULL ){
        read_force_field( source );
        fclose( source );
    } else {
        throw std::runtime_error("Error opening file");
    }
}

force_field::force_field( FILE *source ){
    cut_off    = 2.0;
    length     = 1.0;
    type_max   = 0;
    big_energy = BIGVALUE;

    read_force_field( source );
}

force_field::force_field( float r ){
    // Default repulsive force field
    cut_off    = 2*r;
    length     = 1.0;
    type_max   = 1;
    big_energy = BIGVALUE;
    
    radius.resize(1);
    color.resize(1);
    energy.resize(1,1);

    radius[0] = r;
    color(0) = "red" ;		// Should not be here but fix later
    energy(0,0) = 0.0;
}

#define	COMMENTCHAR	'#'

bool
force_field::is_comment( char *line ){
//  Comments are lines of whitespace or lines
//  That start (possibly after whitespace) with
//  COMMENTCHAR...
    int i;
    while( line[i] != '\0' ){
        if( isspace( line[i] )) i++;
        else
            return( line[i] == COMMENTCHAR );
    }
    return( true );
}

void
force_field::read_force_field( FILE *source ){
/*
**  Need to add more error handling and checking,
**  Need to add documentation.
**  Should relax formatting restrictions.
*/
    char	*line = NULL;
    size_t	len = 0;
    ssize_t     read;
    int		logical_line = 0;
    int         line_number = 0;
    int		i, j = 0;
    double       number;
    char	data[32];			        // 32 is maximum length of a color

    while(( read = getline( &line, &len, source )) != -1 ){
        line_number++;
        if( !is_comment( line )){
            switch( logical_line ){
            case 0: sscanf( line, "%d ", &type_max );	// Number of different atom types
                    if( type_max <= 0 )                 // Sanity check
                        throw std::runtime_error( "Error in force field file Line number "
                            + std::to_string(line_number)
                            + " : Invalid number of atom types "
                            + std::to_string(type_max) );
                    radius.resize( type_max );          // Resize arrays as necessary.
                    color.resize(type_max);
                    energy.resize(type_max, type_max);
                    logical_line++;                     // Move on
                    break;
            case 1: for( i = 0; i < type_max; i++ ){
                        sscanf( line, "%lf", &number );
                        radius[i] = number;
                    }
                    logical_line++;
                    break;
            case 2: for( i = 0; i < type_max; i++ ){
                        sscanf( line, "%s", &data );
                        color(i) = data;
                    }
                    logical_line++;
                    break;
            case 3: sscanf( line, "%lf %lf", &cut_off, & length );	// interaction cutoff distance and length scale.
                    logical_line++;
                    break;
            case 4: for( i = 0; i < type_max; i++ ){    // Read in energy matrix
                        sscanf( line, "%lf", &number );
                        energy(i,j) = number;
                    }
                    if( ++j == type_max )
                        logical_line++;
                    break;
            default:
                    break;
           }
        }
    }
}

force_field::~force_field() {                           // Probably need to get rid of arrays.
}

// Update a force field (empty or not) with a file
void force_field::update(std::string ff_filename) {
    int         i, j;
    string      line;
    double      temp_number;
    string      temp_char;
    int         n_check = 0;
    ifstream ff(ff_filename.c_str());
    
    // Open a stream of the file
    
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

void force_field::write(FILE *dest){
    assert( false );
}

void force_field::write(std::ostream& dest){ // This should be normal rereadable not logging...
    int i,j;

    dest << "\nSummary of the force-field\n";
    dest << "Cut off is " << cut_off << "\n";
    dest << "Length scale is " << length << "\n";
    dest << "Number of atom types is " << type_max << "\n";
    dest << "Colors are  [";
    for( i=0; i< type_max; i++) {
        dest << format("%g ,") % color(i);
    }
    dest << "]\nRadius array is  [";
    for( i=0; i< type_max; i++ ) {
        dest << format("%7.3g ,") % radius(i);
    }
    dest << "]\nEnergy array is [";
    for( i=0; i< type_max; i++ ){
        if( i > 0) {
            dest << "\n                 ";
        }
        dest << "[";
        for(j=0;j<type_max; j++ ){
            dest << format("%7.3g ,") % energy(i, j);
        }
        dest << "]";
    }
    dest << "]\n\n";

}

const char  *force_field::get_color(int t){
    return color[t].c_str();
}
