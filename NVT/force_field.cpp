/**
 * \file   force_field.cpp
 * \author James Sturgis
 * \date   April 6, 2018
 */

#include "common.h"
#include "force_field.h"
#include <string>

#define  BIGVALUE   10E6

// Hard coded boring force field copied into instances...
double my_radius[MAX_TYPE]           =  { 1.0, 1.0, 1.0,-1.0 };
const char *my_color [MAX_TYPE]    =  {"red","green","blue","orange"};
double my_energy[MAX_TYPE][MAX_TYPE] = {{-1.0,-1.0,-1.0,-1.0 },
                                        {-1.0,-1.0,-1.0,-1.0 },
                                        {-1.0,-1.0,-1.0,-1.0 },
                                        {-1.0,-1.0,-1.0,-1.0 }};
double my_cut_off = 5.0;
double my_length = 1.0;

force_field::force_field() {
    int i, j;

    cut_off    = my_cut_off;
    length     = my_length;
    type_max   = MAX_TYPE;
    big_energy = BIGVALUE;
    for( i=0; i< type_max; i++ ){
        radius[i] = my_radius[i];
        color[i]  = my_color[i];
        for( j = 0; j < type_max; j++ ) energy[i][j] = my_energy[i][j];
    }
}

force_field::force_field(const force_field& orig) {
    int i, j;

    cut_off    = orig.cut_off;
    length     = orig.length;
    type_max   = orig.type_max;
    big_energy = orig.big_energy;
    for( i=0; i< type_max; i++ ){
        radius[i] = orig.radius[i];
        color[i]  = orig.color[i];
        for( j = 0; j < type_max; j++ ) energy[i][j] = orig.energy[i][j];
    }
}

force_field::~force_field() {
}

double
force_field::interaction(int t1, int t2, double r) {
    double value = 0.0;
    double hard;

    if(r < cut_off){

        assert( t1 < type_max );
        assert( t2 < type_max );

        // Implementation of triangle potential with repulsive core
        hard  = radius[t1] + radius[t2];
        r    -= hard;
        if( r < 0 ){
            value = big_energy * (1 - r/hard);
        } else if (r< length) {
            r /= length;                        /// r between 0.0 and 1.0
            value = energy[t1][t2] * (1.0-r);
        }
    }
    return value;
}

double  force_field::size(int t1){
    return radius[t1];
}

int
force_field::write(FILE* dest){
    int i,j;

    fprintf(dest,"Cut off is %g\n", cut_off );
    fprintf(dest,"Length scale is %g\n", length );
    fprintf(dest,"Number of atom types is %d\n", type_max );
    fprintf(dest,"Radius array is  [");
    for( i=0; i< type_max; i++ )
            fprintf(dest,"%7.3g ,", radius[i]);
    fprintf(dest,"]\nEnergy array is [");
    for( i=0; i< type_max; i++ ){
        if( i > 0) fprintf(dest,"\n                 ");
        fprintf(dest,"[");
        for(j=0;j<type_max; j++ ){
            fprintf(dest,"%7.3g ,", energy[i][j]);
        }
        fprintf(dest,"]");
    }
    fprintf(dest,"]\n");

    return 0;
}

const char  *force_field::get_color(int t){
    return color[t];
}
