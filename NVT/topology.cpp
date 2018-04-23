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
    // Now lets put in the first topology with 1 central atom
    data[0] = (atom **)malloc(MAX_ATOMS*sizeof(atom*));
    for(int j = 0; j < MAX_ATOMS; j++) data[0][j] = (atom *)NULL;
    data[0][0] = new atom(0, 0.0, 0.0);  // Memory losses here
    len[0]=1;           // JS 16/4

    // Now more complex a squarish object
    data[1] = (atom **)malloc(MAX_ATOMS*sizeof(atom*));
    for(int j = 0; j < MAX_ATOMS; j++) data[1][j] = (atom *)NULL;
    data[1][0] = new atom(1, 1.0, 1.0);
    data[1][1] = new atom(1, 1.0,-1.0);
    data[1][2] = new atom(1,-1.0, 1.0);
    data[1][3] = new atom(1,-1.0,-1.0);
    len[1]= 4;          // JS 16/4
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

int     topology::write(FILE *dest){
    int     i;
    int     rc;
    for(i = 0; i < MAX_TOPO; i++){
        if (data[i] == (atom **)NULL ) break;
    }
    rc = fprintf(dest, "Topology %d types\n", i );
    for(i = 0; i < MAX_TOPO; i++){
        if (data[i] == (atom **)NULL ) break;
        for(int j = 0; j < MAX_ATOMS; j++){
            if(!atoms(i,j)) break;
            rc = fprintf(dest, "%3d %3d ", i, j );
            atoms(i,j)->write(dest);
        }
    }
    return rc;
}
