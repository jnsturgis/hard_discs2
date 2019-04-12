/**
 * @file        config.h
 * @author      James Sturgis
 * @date        April 6, 2018
 * @version     1.0
 * \brief       Header file for the config class.
 *
 * @class       config config.h
 * @brief       The configuration class.
 *
 * A configuration of objects on a surface. Currently the configuration
 * contains:
 * * a boundary, for the moment a bounding box, that can be interpreted
 *   as used for periodic boundary conditions or not (depending on the value of
 *   the flag 'is_periodic' ).
 * * a list of objects stored in the 'obj_list'.
 * * a topology that contains the description of the different objects and how
 *   they are made up of atoms.
 * * Also there is a saved_energy and a recalculate flag to increase the
 *   efficiency of energy calculations.
 *
 * The constructors for the class allow for creation of a minimal valid but empty
 * configurations. Constructors for copying either with a conformation or a pointer
 * to a conformation. (This is unnecessary and just reflects my lack of c++
 * experience.) Finaly there is a constructor for reading the configuration from
 * a file. These are complemented by a destructor that frees up any allocated space.
 *
 * There are methods for associating objects with the configuration.
 * * add_topology(tp) Associates the topology tp with the configuration.
 *
 * There are three output methods:
 * * write(fp) that writes the configuration to the file pointer fp, that should be
 *              open for writing, in a format that can be used to recreate
 *              the configuration using the file based constructor.
 * * ps_atoms(ff, fp) that produces a postscript snippet containing a representation
 *              of the different atoms.
 * * ps_box(fp) that produces a postscript path of the boundaries.
 *
 * Methods that return information on the configuration.
 * * area() returns the surface are enclosed by the bounding box.
 * * n_objects() returns the number of objects in the configuration.
 * * object_types() returns the number of different types of object (not very useful)
 * * energy(ff) returns the energy of the configuration using the forcefield
 *              ff for the calculation.
 *
 * Methods that modify the configuration.
 * * expand(dl) change the area of the configuration by an isometric expansion
 *              using the multiplicative factor dl for all coordinates.
 *              (Identity operation if dl = 1).
 * * move( no, dl ) move object number 'no' by a random amount controlled by
 *              the scaling factor dl. (Identity operation if dl = 0)
 * * rotate( no, dth ) rotate object number 'no' by a random angle controlled
 *              by the scaling factor dth. (Identity operation if dth = 0).
 * * invalidate_within( r, no ) This marks the energies associated with objects
 *              less than the distance 'r' from object number 'no' as needing
 *              recalculation.
 *
 * Methods that operate on a pair of configurations
 * * rms( ref ) compare the configuration with that a reference configuration 'ref'
 *              and return the rms distance between atoms in the two configurations.
 *
 * @todo Implement non-box boundaries as used for pcf calculations from experimental
 *       configurations measured by AFM.
 * @todo Forcefield should be associated with the configuration so can detect changes
 *       that will invalidate the saved_energy, also will avoid sending forcefield info
 *       for writing postscript which is illogical.
 * @todo Implement ps_box().
 * @todo Add functions to the interface for manipulating a configuration
 *       that will make the empty constructor usefull and allow other
 *       types of ensemble than NVT etc.
 * @todo Common interface for modification methods, so they can be used
 *       interchangeably in the integrators.
*/

#ifndef CONFIG_H
#define CONFIG_H

#include "o_list.h"

using namespace std;

class config {
public:
    config();                       ///< Create a new empty conformation.
    config(config& orig);           ///< Copy an existing conformation.
    config(config *orig);           ///< Copy an existing conformation bis.
    config(std::string in_file);    ///< Create by reading a named file.
    config(std::istream& source);   ///< Create from an input source.

    virtual ~config();              ///< Destroy a conformation

/* TODO Class interface needs cleaning */

    void    add_topology(topology *a_topology
                                 ); ///< Attach a topology to the configuration
    void    add_object(object *orig
                                 ); ///< Insert an object in the configuration
    int     write(std::ostream& dest
                              );    ///< Write the conformation to a stream.
    int     write(FILE *dest);      ///< Write the conformation to a 'c' file.
    void    ps_atoms(std::ostream& dest
                               );   ///< Write the postscript part for the atoms.

    double  energy(force_field *&the_force
                               );   ///< Calculate the energy of a conformation using the given force field.
    double  area();                 ///< Return the total area of the configuation.
    int     object_types();         ///< The number of different object types.
    int     n_objects();            ///< The number of objects in configuration.

    void    expand( double dl );    ///< Expand the surface area by a factor dl.
    void    move(int obj_number, double dl_max 
                                );  ///< Move an object in the configuration.
    void    rotate(int obj_number, double theta_max
                                 ); ///< Rotate an object in the configuration.
    void    invalidate_within(double distance, int index 
                                 ); ///< Mark energies for recalculation.
    bool    test_clash( object *new_object 
                                 ); ///< Check if there is a clash to insert new object.
    double  rms(const config& ref); ///< Calculate rms difference from a second conformation.

    /* Variables should be more private - the copy function is the main problem */
    double      x_size;             ///< The width of the configuration
    double      y_size;             ///< The height of the configuration
    bool        unchanged;          ///< Is the configuration unchanged since the last evaluation of energy.
    bool        is_periodic;        ///< Use periodic boundary conditions

private:
    void	config_read(std::istream& src
                                 ); ///< Helper function reading from a stream.

    double      saved_energy;       ///< The last result of energy evaluation.
    o_list      obj_list;           ///< The objects in the configuration
    topology    *the_topology;      ///< The object topology file.
    bool        check();            ///< Is the current configuration valid?
};

#endif /* CONFIG_H */
