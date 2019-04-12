/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   o_list.cpp
 * Author: james
 *
 * Created on April 10, 2018, 4:16 PM
 */

#include "o_list.h"
#include "common.h"

#include <malloc.h>

#define     GRAIN       128

o_list::o_list() {
    n_full = 0;
    n_alloc = 0;
    space = (object **)NULL;
}

o_list::o_list(o_list& orig) {
    int     i;
    object  *my_obj;
    object  *old_obj;

    n_full = 0;
    n_alloc = 0;
    for(i=0;i<orig.n_full;i++){
        old_obj = orig.get(i);
        my_obj = new object(*old_obj);
        this->add(my_obj);
    }
}

o_list::~o_list() {
    if(n_alloc){
        empty();
        free(space);
    }
}

int o_list::add(object *my_obj){
    if( n_full >= n_alloc ){
        if(n_alloc){
            space = (object **)realloc( space, (GRAIN+n_alloc )* sizeof(void *) );
        } else {
            space = (object **)malloc( GRAIN * sizeof(void *) );
        }
        n_alloc += GRAIN;
    }

    space[n_full++]=my_obj;
    return n_full;
}

object *o_list::get(int i){
    assert(i>=0);
    assert(i<n_full);
    return space[i];
}

int o_list::size(){
    return n_full;
}

void o_list::empty(){
    int i;

    if(n_alloc>0){
        for(i=0; i<n_full; i++ ){
            delete(this->get(i));
            space[i] = (object *)NULL;
        }
        n_full = 0;
    }
}
