/**
 * @file        config.cpp  Implementation of the configuration class.
 * @brief       Implementation of the configuration class to handle molecular organizations.
 * @author      James Sturgis
 * @date        April 6, 2018
 * @version     1.0
 */

#include <float.h>
#include <math.h>
#include <iostream>
#include "config.h"
#include <boost/format.hpp>

using boost::format;

/**
 * Constructor that produces an empty basic configuration. This is not
 * currently much use as there are not all the necessary functions for
 * manipulating the configuration.
 */
config::config() {
    x_size       = 1.0;
    y_size       = 1.0;
    unchanged    = true;
    saved_energy = 0.0;
    obj_list     = o_list();
    the_topology = (topology *)NULL;
    is_periodic  = false;
}

/**
 * Constructor that reads the configuration from a file. The format of this
 * file is described in the class description.
 *
 * @param src   An file descriptor open for reading that contains the
 *              configuration to be read.
 *
 * @todo        Define file format for configuration. Currently:
 *              x_size, y_size \n
 *              n_objects \n
 *              o_type x_pos y_pos rotation \n one line per object.
 *              Would like to:
 *              - Include comments
 *              - Be space tolerant
 *              - Have a default rotation for backward compatibility.
 * @todo        Handle errors in the input file or file reading in a sensible
 *              way. If there is an error this should be apparent even if the
 *              structure is still valid.
 * @todo        Read from file if periodic conditions or not.
 * @todo        Integrate an object constructor from a file saves 4 variables
 *              and a couple of lines of code.
 */
config::config(std::istream& source ){
    config_read(source);
}

config::config(string in_file) {
    // Open a stream of the file
    ifstream ff(in_file.c_str());
    config_read( ff );
    ff.close();
}

void
config::config_read(std::istream& ff){
    int         n_obj;
    int         o_type;
    double      x_pos, y_pos, angle;
    object      *my_obj;
    // Plus a string for each line
    string      line;
    
    // Check if the file exists
    if(ff.fail()) {
        throw runtime_error("Could not open configuration file\n");
    }
    
    // We get each line with getline(in_file, line)
    // Here we want stuff from the first two lines so
    // Get the line
    getline(ff, line);
    istringstream iss(line);
    
    // From this, populate other variables - x_size and y_size are doubles
    // Can also check if the line has the right number of fields
    if (!(iss >> x_size >> y_size)) {
        throw range_error("First line of the configuration file should be x_size y_size, exiting ...\n");
    }
    
    // Clear iss
    iss.clear();
    
    // Next line
    getline(ff, line);
    iss.str(line);
    if (!(iss >> n_obj)) {
        throw range_error("Second line of the configuration file should be the number of objects, exiting ...\n");
    }
    
    // Clear iss again
    iss.clear();
    
    // Now loop through the remaining lines
    while (getline(ff, line)) {
        
        // The line
        iss.str(line);
        
        // Bead type, x, y and angle at each line
        if (!(iss >> o_type >> x_pos >> y_pos >> angle)) {
            throw range_error("Problem in the coordinates, exiting ...\n");
        }
        
        // Populate a new object and add it to the list
        my_obj = new object(o_type, x_pos, y_pos, angle );
        obj_list.add(my_obj); 
        
        // Clear iss
        iss.clear();
    }
    
    unchanged = false;                              // Set up so will calculate energy.
    saved_energy = 0.0;
    the_topology = (topology *)NULL;                // Topologies are not included in
                                                    // the file.
    is_periodic = true;                             // This should be read from file.

    assert(n_obj == n_objects() );                  // Should check the configuration
                                                    // is alright.
}

/**
 * Copy constructor
 * @param orig the original configuration to be copied.
 */
config::config(config& orig) {
    object  *my_obj1;
    object  *my_obj2;

    x_size         = orig.x_size;
    y_size         = orig.y_size;
    saved_energy   = orig.saved_energy;
    unchanged      = orig.unchanged;
    the_topology   = orig.the_topology; // THIS CHEAT
    is_periodic    = orig.is_periodic;
    obj_list.empty();
    for(int i = 0; i < orig.n_objects(); i++){
        my_obj1 = orig.obj_list.get(i);
        my_obj2 = new object( my_obj1->o_type, my_obj1->pos_x,
                my_obj1->pos_y, my_obj1->orientation);
        obj_list.add(my_obj2);
    }
}

/**
 * Destructor. Destroy the configuration releasing memory. As constructor
 * uses new probably need explicit destroy.
 */
config::~config() {
    //~ if(the_topology) delete(the_topology);
}
/*
int config::get_n_top(){
    return the_topology->n_top;
}
*/

/**
 * @return The area of the configuration.
 *
 */
double config::area() { return x_size * y_size;}
/**
 * This function calculates the energy of a configuration by comparing using
 * the force field interaction function to measure the energy between pairs of
 * objects. As both indexes run from 0 to the end all interactions are counted
 * twice. This is done so that neighbour lists, if implemented, will work more
 * easilly. To increase the energy the object recalculate flag, and the
 * configuration unchanged flag are checked to reduce unnecessary evaluations
 * as long as these flags are correctly and efficiently updated.
 *
 * @param  the_force the force field to use for the energy calculation.
 * @return the total interaction energy between all object pairs.
 * @todo   Handle periodic conditions.
 */
double config::energy(force_field *&the_force) {
    int     i1, i2;                         // Two counters
    double  value = 0.0;                    // An accumulator that starts at 0.0
    object  *my_obj1, *my_obj2;                       // Two object pointers

    if (! unchanged) {                      // Only if necessary
        saved_energy = 0.0;                 // Loop over the objects
                                            // This code needs optimizing.
        for(i1 = 0; i1 < obj_list.size(); i1++ ){
            my_obj1 = obj_list.get(i1);
            if( my_obj1->recalculate ){
                value = 0.0;
                for(i2 = 0; i2<obj_list.size(); i2++ ){
                    if(i1 != i2){            // If periodic then
                        double r, r2;
                        double dx = 0.0;
                        double dy = 0.0;

                        my_obj2 = obj_list.get(i2);
                        if(is_periodic){     // Move my_obj2 to closest image
                                             // Check this code...
                            r  = my_obj2->pos_x - my_obj1->pos_x;
                            dx = (r<0)?x_size:-x_size;
                            r2 = r + dx;
                            dx = (abs(r2)<abs(r))?dx:0.0;
                            my_obj2->pos_x += dx;

                            r  = my_obj2->pos_y - my_obj1->pos_y;
                            dy = (r<0)?y_size:-y_size;
                            r2 = r + dy;
                            dy = (abs(r2)<abs(r))?dy:0.0;
                            my_obj2->pos_y += dy;
                        }
                        value += my_obj1->interaction( the_force,
                                the_topology, my_obj2 );
                        if(is_periodic){    // And move back again.
                            my_obj2->pos_x -= dx;
                            my_obj2->pos_y -= dy;
                        }
                    }
                }                           // Calculate interaction with wall
                if(! is_periodic ){         // if not periodic conditions.
                    value += my_obj1->box_energy( the_force, the_topology,
                            x_size, y_size );
                }
                my_obj1->set_energy(value); // Set the energy of the object
            }                               // End of the recalculation.
            value = my_obj1->get_energy();  // Get object energy
            saved_energy += value;          // Add into the sum
        }                                   // End of loop over objects
        unchanged = true;                   // Value is correct mark as unchanged.
    }

    return saved_energy/2.0;                // All interactions are counted twice.
}

/**
 * Write the current configuration to a file in a format that can be used to
 * reinitialize a configuration with the file based constructor. See the class
 * description for the file format.
 *
 * @param dest  This is a file descriptor that should be open for writing.
 * @return      Should return exit status (currently always OK).
 *
 * @todo        Incorporate error handling and exit status return that is
 *              correct.
 */

int config::write( FILE *dest ){
    int     i;
    object  *this_obj;

    fprintf( dest, "%9f2 %9f2 \n", x_size, y_size );
    fprintf( dest, "%d\n", obj_list.size());
    for(i = 0; i< obj_list.size(); i++){    // For each object in configuration
        this_obj = obj_list.get(i);         // Get the object and
        this_obj->write(dest);              // Write it to the file -- Pass the stream
    }
    return EXIT_SUCCESS;                    // Return all well
}

/**
 * Write the current configuration to a file in a format that can be used to
 * reinitialize a configuration with the file based constructor. See the class
 * description for the file format.
 *
 * @param dest  This is an output file stream.
 * @return      Should return exit status (currently always OK).
 *
 * @todo        Incorporate error handling and exit status return that is
 *              correct.
 */

int config::write(std::ostream& dest){
    int     i;
    object  *this_obj;
    
    // Put the header and number of objects inside
    // Using boost for formatting, which seems the proper way in c++
    dest << format("%9f2 %9f2 \n") % x_size % y_size;
    dest << format("%d\n") % obj_list.size();

    for(i = 0; i< obj_list.size(); i++){    // For each object in configuration
        this_obj = obj_list.get(i);         // Get the object and
        this_obj->write(dest);              // Write it to the file -- Pass the stream
    }
    return EXIT_SUCCESS;                    // Return all well
}

/**
 * @return The number of objects found in the configuration.
 */
int config::n_objects(){
    return obj_list.size();                 // Get size of object list.
}

/**
 * This function tests if there are any clashes the configuration using the
 * topology file.
 *
 * @return true if there is a clash otherwise false.
 *
 * @todo Write code.
 */
bool
config::test_clash(){
    int	i;
    for(i=0;i<obj_list.size();i++);
    return false;
}

/**
 * This function tests if the new_object can be inserted into the configuration
 * without generating a clash (as determined by the topology file).
 *
 * @param new_object a pointer to a valid object to test for insertion.
 * @return true if there is a clash otherwise false.
 *
 * @todo handle periodic conditions or wall collisions as appropriate.
 */
bool
config::test_clash( object *new_object ){
    object *my_obj;
    int max_o_type = the_topology->n_atom_types - 1 ;
    int o_type1 = simple_min( new_object->o_type, max_o_type );
    int o_type2;
    double theta1 = new_object->orientation;
    double theta2;
    double t1, dx1, dy1, r1, x1, y1;
    double t2, dx2, dy2, r2, x2, y2;
    double r, dx, dy;

    if( !is_periodic ){                     // Check clash with walls.
        for(int i = 0; i < the_topology->molecules(o_type1).n_atoms; i++ ){
            t1  =  the_topology->molecules(o_type1).the_atoms(i).type;
            dx1 =  the_topology->molecules(o_type1).the_atoms(i).x_pos;
            dy1 =  the_topology->molecules(o_type1).the_atoms(i).y_pos;
            r1  =  the_topology->atom_sizes(t1);  // Get radius
                                            // Calculate atom position
            x1  =  new_object->pos_x + dx1 * cos(theta1) - dy1 * sin(theta1);
            y1  =  new_object->pos_y + dx1 * sin(theta1) + dy1 * cos(theta1);
            if(( x1 < r1 ) || ( (x1 + r1) > x_size ) || ( y1 < r1 ) || 
                ( (y1 + r1) > y_size ))
                return true;
        }
    }
                                            // Loop over the objects.
    for(int i = 0; i < obj_list.size(); i++){
        my_obj  = obj_list.get(i);
        theta2  = my_obj->orientation;
        o_type2 = simple_min( my_obj->o_type, max_o_type );

                                            // Loop over the atoms my_obj
        for(int j = 0; j < the_topology->molecules(o_type2).n_atoms; j++ ){
                                            // Get atom information
            t2  =  the_topology->molecules(o_type2).the_atoms(j).type;
            dx2 =  the_topology->molecules(o_type2).the_atoms(j).x_pos;
            dy2 =  the_topology->molecules(o_type2).the_atoms(j).y_pos;
            r2  =  the_topology->atom_sizes(t2);      // Get radius
                                            // Calculate atom position
            x2  =  my_obj->pos_x + dx2 * cos(theta2) - dy2 * sin(theta2);
            y2  =  my_obj->pos_y + dx2 * sin(theta2) + dy2 * cos(theta2);

            for( int k = 0; k < the_topology->molecules(o_type1).n_atoms; k++ ){
                                            // Get atom information
                t1  =  the_topology->molecules(o_type1).the_atoms(j).type;
                dx1 =  the_topology->molecules(o_type1).the_atoms(j).x_pos;
                dy1 =  the_topology->molecules(o_type1).the_atoms(j).y_pos;
                r1  =  the_topology->atom_sizes(t1);  // Get radius
                                            // Calculate atom position
                x1  =  new_object->pos_x + dx1 * cos(theta1) - dy1 * sin(theta1);
                y1  =  new_object->pos_y + dx1 * sin(theta1) + dy1 * cos(theta1);

                dx = (x2-x1);
                dy = (y2-y1);
                                            // Handle periodic conditions
                if( dx > (x_size - r1 - r2 )) dx -= x_size;
                if( dx < (r1 + r2 - x_size )) dx += x_size;
                if( dy > (y_size - r1 - r2 )) dy -= y_size;
                if( dy < (r1 + r2 - y_size )) dy += y_size;

                r = dx*dx+dy*dy;
                if( r < ((r1+r2)*(r1+r2))) return (true);
            }
        }
    }
    return (false);
}

/**
 * Count the number of different types of object are found in the current
 * configuration. Actually it just returns the highest object type number found.
 *
 * @todo    Clean up semantics. What is actually needed and return this.
 * @return  as an integer the total number of different object types found.
 *
 */
int config::object_types(){
    int     i;
    int     max_type = -1;

    assert( check() );

    for(i = 0; i< obj_list.size(); i++ ){
        max_type = simple_max(max_type, obj_list.get(i)->o_type);
    }
    assert(max_type>=0);
//  assert( check() );
    return max_type;
}

/**
 * Private member function to verify that the config structure is internally
 * valid.
 *
 * Internal validations (should) include:
 * - the object_list is a valid list.
 * - all objects in the list are valid.
 * - all objects are within the area of the configuration.
 * - if 'unchanged' is true then saved_energy is the energy obtained by
 *   calculation. This is hard to check as usually the force field is not
 *   available to the check function.
 *
 * On entry to and on exit from all of the functions in the config class
 *  this->check() should return 'true'.
 *
 * @return true or false depending on evaluation.
 *
 * @todo  Implement this function.
 */
bool config::check(){
    return true;
}

/**
 * This function compares atom by atom the current configuration and the
 * reference configuration and calculates the root mean square distance between
 * the atoms.
 *
 * @param ref   a reference configuration
 * @return      as a double the rms distance
 *
 * @todo        Currently just a stub, returns 1.0
 */
double config::rms(const config& ref){
    return 1.0;
}

/**
 * This function changes the size of a configuration by an isometric expansion
 * moving all the objects apart. It does not change the orientations of the
 * various objects.
 *
 * @param dl    The multiplicative factor to apply to the size and the object
 *              coordinates.
 * @return      Return if there are clashes.
 */
bool config::expand(double dl){
    int     i;

    x_size *= dl;                           // Expand boundary
    y_size *= dl;
    unchanged = false;                      // The energies will be different
    for(i=0;i<obj_list.size();i++){
        obj_list.get(i)->recalculate = true;// Also for the objects
        obj_list.get(i)->expand(dl);        // Move objects in rescaled box
    }
    return this->test_clash();
}

/**
 * This function changes the size of a configuration by an isometric expansion
 * moving all the objects apart. It does not change the orientations of the
 * various objects.
 *
 * @param dl    The multiplicative factor to apply to the size and the object
 *              coordinates.
 * @param max_try Number of attempted object displacements to remove clashes.
 * @return      Return if there are clashes.
 * @todo        Jiggle and jolt (move and twist to remove clashes) steepest descent
 */
bool config::expand(double dl, int max_try ){
    int     i;

    x_size *= dl;                           // Expand boundary
    y_size *= dl;
    unchanged = false;                      // The energies will be different
    for(i=0;i<obj_list.size();i++){
        obj_list.get(i)->recalculate = true;// Also for the objects
        obj_list.get(i)->expand(dl);        // Move objects in rescaled box
    }
    for(i=0;i<max_try;i++){
      if(this->test_clash()) /* jiggle and jolt */;
    }
    return this->test_clash();
}

/**
 * Move a given object to a new place using the scaling factor dl_max
 * to control the distance distribution.
 *
 * @param obj_number the index of the object to move
 * @param dl_max the scaling parameter.
 */
void config::move(int obj_number, double dl_max){
    obj_list.get(obj_number)->move(dl_max, x_size, y_size, is_periodic );
    obj_list.get(obj_number)->rotate(M_2PI);// Mix for the moment move and rotate
}

/**
 * Rotate an object designated by the obj_number a random angle
 * scaled by theta_max.
 *
 * @param obj_number The index of the object to move
 * @param theta_max The scaling parameter.
 */
void config::rotate(int obj_number, double theta_max){
    obj_list.get(obj_number)->rotate(theta_max);
}

/**
 * Mark as needing recalculation of energies all objects within a certain
 * distance of a reference object.
 *
 * @param distance the cut-off distance to use.
 * @param index the number of the reference object.
 */
void    config::invalidate_within(double distance, int index){
    object  *obj1;
    object  *obj2;

    obj1 = obj_list.get(index);
    for(int i=0; i< n_objects(); i++)       // For each object in the cnfiguration
      if (i!= index){                       // That is difference
        obj2 = obj_list.get(i);             // Check distance
        if( obj1->distance(obj2, x_size, y_size, is_periodic) < distance )
            obj2->recalculate = true;       // and set flag if necessary
    }
}

/** \brief Associate a topology with the configuration
 *
 * \param a_topology a pointer to the topology.
 *
 */

void    config::add_topology(topology* a_topology){
    if( the_topology )                      // If there is already one
        delete( the_topology );             // Get rid of it
    the_topology = a_topology;              // Make the new association
}

/** \brief Insert an object into the configuration.
 *
 * \param orig the object to add
 * \return nothing
 *
 */
void    config::add_object(object* orig ){
    obj_list.add(orig);
}

/** \brief Output a postscript snippet to draw the configuration
 *
 * \param the_forces forcefield, needed for atom sizes and colors.
 * \param dest the file for the output.
 * \return no return value
 *
 * \todo use return value for error handling.
 */

void    
config::ps_atoms( std::ostream& dest ){
/*  Need to reorganize for topology with molecules...
    should incorporate atom draw radius in topology (so no FF needed)... */

    object  *my_obj;
    double  theta, dx, dy, r, x, y;
    int     t, lr, tb;
    char    *my_color;
    int     max_o_type, o_type;

    
    if ( !the_topology ){
         std::cerr << "Generating postscript images from a configuration "
            "requires setting a topology.";
         exit(1);
    }

    max_o_type = the_topology->n_atom_types -1 ;

                                            // Loop over the objects.
    for(int i = 0; i < obj_list.size(); i++){
        my_obj = obj_list.get(i);
        theta  = my_obj->orientation;
        o_type = my_obj->o_type;
        if( o_type > max_o_type ){          // Use object type 0 if not defined
            o_type = 0;
        }
                                            // Loop over the atoms
        for(int j = 0; j < the_topology->molecules(o_type).n_atoms; j++ ){
                                            // Get atom information
            t  =  the_topology->molecules(o_type).the_atoms(j).type;
            dx =  the_topology->molecules(o_type).the_atoms(j).x_pos;
            dy =  the_topology->molecules(o_type).the_atoms(j).y_pos;
            r  =  the_topology->atom_sizes(t);      // Get radius
                                            // Calculate atom position
            x  =  my_obj->pos_x + dx * cos(theta) - dy * sin(theta);
            y  =  my_obj->pos_y + dx * sin(theta) + dy * cos(theta);
            my_color = the_topology->molecules(o_type).the_atoms(j).color;
                                            // Write postscript snippet for atom.
            dest << format("newpath %g %g moveto %g %s fcircle \n") 
                    % x % y % r % (my_color);

                                            // Handle intersections with the border
            lr = 0; tb = 0;                 // need up to 4 copies.
            if ( x < r ) lr = -1;
            if ( x > x_size - r ) lr = +1;
            if ( y < r ) tb = -1;
            if ( y > y_size - r ) tb = +1;
            if (lr != 0 ){                  // Copy on other side
                dest << format("newpath %g %g moveto %g %s fcircle \n") 
                    % (x-lr*x_size) % y % r % (my_color);
            }
            if (tb != 0 ){                  // Vertical copy
                dest << format("newpath %g %g moveto %g %s fcircle \n") 
                    % x % (y-tb*y_size) % r % (my_color);
            }
            if ((lr != 0 )&&(tb != 0)){     // In the corner!
                dest << format("newpath %g %g moveto %g %s fcircle \n") 
                    % (x-lr*x_size) % (y-tb*y_size)
                    % r % ( my_color );
            }
        }
    }
}
