/**
 * \file    o_list.h
 * \author  James Sturgis
 * \date    April 10, 2018
 * \version 1.0
 * \brief   Header file for the o_list class.
 *
 * \class   o_list o_list.h
 * \brief   A variable length list of objects
 *
 * This class implements an expandable variable length list of objects
 * that can be referenced by index number. The implementation contains
 * three class variables, a size of the allocated space, the number of
 * slots used (i.e. the length of the list) and the data space.
 *
 * The constructors can create a copy of an existing list (copying the
 * contained objects) or create a new empty list. The destructor will
 * destroy the elements in the list as well as the list itself.
 *
 * Class methods allow for:
 * * Addition of a new object to the end of the list.
 * * Returning a reference to an element of the list.
 * * Returning the current size of the list.
 * * Emptying all elements out of the list.
 *
 * \todo A number of other functions would seem useful to remove individual
 *       elements from the list, insert at other places etc. SHould be
 *       possible using the std library rather than writing my own code.
 */

#ifndef O_LIST_H
#define O_LIST_H

#include "object.h"

class o_list {
public:
    o_list();                               ///< Constructor for an empty list.
    o_list(o_list& orig);                   ///< Constructor for a deep copy of the list
    virtual ~o_list();                      ///< Destructor
    int     add(object *my_obj);            ///< Add an object to the list
    object *get(int i);                     ///< Retrieve a pointer to the indexed object.
    int     size();                         ///< Return the number of objects in the list.
    void    empty();                        ///< Clear the contents of the list.
private:
    int     n_full;                         ///< Number of full slots.
    int     n_alloc;                        ///< Size of allocated space.
    object  **space;                        ///< Allocated data space.
};

#endif /* O_LIST_H */

