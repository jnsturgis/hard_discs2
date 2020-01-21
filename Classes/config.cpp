/**
 * @file        config.cpp  Implementation of the configuration class.
 * @brief       Implementation of the configuration class to handle molecular organizations.
 * @author      James Sturgis
 * @date        April 6, 2018
 * @version     1.0
 *
 * 30 december 2019 - implement non-rectangular areas (necessarily aperiodic)
 * Signaled in file format by x_size == y_size == 0.0.
 * Configuration boundary is then given by a polygon in the following lines
 *	N_vertice: number of vertices in polygon (second line of file)
 *	x y: coordinates of vertices one per line.
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
    is_rectangle = true;
    n_vertex     = 0;
    poly         = (polygon *)NULL;
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

bool
my_getline(std::istream& ff, string *line){
    while(getline(ff, *line)){		        // Read a new line
        if(line->find('#')<line->npos)		// Remove comments
            line->erase(line->find('#'));
        while(line->size() && isspace(line->front())) 
            line->erase(line->begin());
        while(line->size() && isspace(line->back())) 
            line->pop_back();
        if( !line->empty() ) return true;
    }
    return false;
}

/*
 * Read a configuration file as a stream. 
 * Want to enhance this to:
 * 1. Deal sensibly with errors in the input file
 *    In case of errors it throws runtime_error() with a useful message!
 * 2. Ignore empty lines, and comments "# to end of line"
 *    These are removed by my_getline(ff, line)
 */
void
config::config_read(std::istream& ff){
    int         n_obj;
    int         o_type;
    double      x_pos, y_pos, angle;
    object      *my_obj;
    // Plus a string for each line
    string      line;
    
    is_periodic  = true;                             // Set up defaults.
    is_rectangle = true;
    n_vertex     = 0;
    poly         = (polygon *)NULL;

    // Check if the file exists
    if(ff.fail()) {
        throw runtime_error("Could not open configuration file\n");
    }
    
    // We get each line with my_getline(in_file, line)
    // This fills string with the content of the next line that is not all
    // whitespace or comment and returns true if it succeeded and false on failure.

    if(!my_getline(ff, &line)){			// Found end of file before content.
        throw runtime_error("Found no content in the configuration file\n");
    }
    istringstream iss(line);
    if (!(iss >> x_size >> y_size)) {
        throw runtime_error("First line of the configuration file should be x_size y_size, exiting ...\n");
    }
    iss.clear();

    // Here we handle the case of a non-rectangular configuration signalled
    // by 2 zeros in the first line (after any comments etc)... 
    if( x_size == 0.0 ){
        is_rectangle = false;
        is_periodic = false;

        if(! my_getline(ff, &line))
            throw runtime_error("Failed to read number of vertices..\n");
        iss.str(line);
        if( !(iss >> n_vertex )){
            throw runtime_error("Failed to read number of vertices...\n");
        }
        iss.clear();
        poly = new polygon( n_vertex );

        double x_coord, y_coord;
        for( int i = 0; i < n_vertex; i++){
            if(!my_getline(ff, &line))
        	throw runtime_error("Error reading bounding polygon coordinates..\n");
            iss.str(line);
            if( !(iss >> x_coord >> y_coord )){
        	throw runtime_error("Error reading bounding polygon coordinates...\n");
            }
            poly->add_vertex( x_coord, y_coord );
            iss.clear();
        }
    }
 
    // Next line the number of objects in the configuration
    if( !my_getline(ff, &line))
        throw range_error("Failed to read number of objects..\n");
    iss.str(line);
    if (!(iss >> n_obj))
        throw range_error("Failed to read number of objects...\n");
    iss.clear();
    
    // Now loop through the objects and remaining lines
    for( int i = 0; i < n_obj; i++){
        if( !my_getline(ff, &line))
            throw runtime_error("Problem in the coordinates..\n");
        iss.str(line);
        if (!(iss >> o_type >> x_pos >> y_pos >> angle))
            throw runtime_error("Problem in the coordinates...\n");
        
        my_obj = new object(o_type, x_pos, y_pos, angle );
        obj_list.add(my_obj); 
        iss.clear();
    }
    if( my_getline(ff, &line )){			// Should be at end of file
        throw runtime_error("Configuration ends before file.\n");
    }

    unchanged = false;                          // Set up so will calculate energy.
    saved_energy = 0.0;
    the_topology = (topology *)NULL;            // Topologies are not included
    assert(n_obj == n_objects() );              // Include some extra tests.
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
    if( orig.the_topology ) 
        the_topology   = new topology(orig.the_topology);
    else
        the_topology = NULL;
    is_periodic    = orig.is_periodic;
    obj_list.empty();
    for(int i = 0; i < orig.n_objects(); i++){
        my_obj1 = orig.obj_list.get(i);
        my_obj2 = new object( my_obj1->o_type, my_obj1->pos_x,
                my_obj1->pos_y, my_obj1->orientation);
        obj_list.add(my_obj2);
    }
    // Add non-periodic bits
    is_rectangle   = orig.is_rectangle;
    n_vertex       = orig.n_vertex;
    if( orig.poly )
        poly       = new polygon( orig.poly );
    else
        poly       = NULL;
}

/**
 * Copy constructor
 * @param orig pointer to the original configuration to be copied.
 */
config::config(config* orig) {
    object  *my_obj1;
    object  *my_obj2;

    x_size         = orig->x_size;
    y_size         = orig->y_size;
    saved_energy   = orig->saved_energy;
    unchanged      = orig->unchanged;
    if( orig->the_topology ) 
        the_topology   = new topology(orig->the_topology);
    else
        the_topology = NULL;
    is_periodic    = orig->is_periodic;
    obj_list.empty();
    for(int i = 0; i < orig->n_objects(); i++){
        my_obj1 = orig->obj_list.get(i);
        my_obj2 = new object( my_obj1->o_type, my_obj1->pos_x,
                my_obj1->pos_y, my_obj1->orientation);
        obj_list.add(my_obj2);
    }
    // Add non-periodic bits
    is_rectangle   = orig->is_rectangle;
    n_vertex       = orig->n_vertex;
    if( orig->poly )
        poly       = new polygon( orig->poly );
    else
        poly       = NULL;
}

/**
 * Destructor. Destroy the configuration releasing memory. As constructor
 * uses new probably need explicit destroy.
 */
config::~config() {
    if(the_topology) delete(the_topology);
    if(poly) delete(poly);
}

/**
 * @return The area of the configuration.
 *
 */
double config::area() {
    if( is_rectangle )
        return x_size * y_size;
    else
        return poly->area();
}

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
                    if( is_rectangle )
                        value += my_obj1->box_energy( the_force, the_topology,
                            x_size, y_size );
                    else
                        value += my_obj1->box_energy( the_force, the_topology,
                            poly );
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
 * @todo        More c++ style!
 */

int config::write( FILE *dest ){
    int     i;
    object  *this_obj;

    if( is_rectangle )
        fprintf( dest, "%9f %9f \n", x_size, y_size );
    else {
        fprintf( dest, "%9f %9f \n", 0.0, 0.0 );
        poly->write( dest );
    }
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
    if( is_rectangle )
        dest << format("%9f %9f \n") % x_size % y_size;
    else {
        dest << format("%9f %9f \n") % 0.0 % 0.0;
        poly->write( dest );
    }
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
 * This function tests if there is a clash between the 2 objects obj1 and obj2
 *
 * @param obj1 a pointer to the first object.
 * @param obj2 a pointer to the second object.
 * @return true if there is a clash otherwise false.
 */
bool
config::test_clash( object *obj1, object *obj2 ){

    double  theta1 = obj1->orientation;
    double  theta2 = obj2->orientation;
    int     o_type1 = obj1->o_type;
    int     o_type2 = obj2->o_type;

    double  t1, t2, dx1, dx2, dy1, dy2, r1, r2, x1, x2, y1, y2;
    double  dx, dy, r;

    if( ! the_topology ){		// No topology (so no size) just points.
        return(( obj1->pos_x == obj2->pos_x ) && ( obj1->pos_y == obj2->pos_y ));
    }

    for(int j = 0; j < the_topology->molecules(o_type2).n_atoms; j++ ){
                                        // Get atom information
        t2  =  the_topology->molecules(o_type2).the_atoms(j).type;
        dx2 =  the_topology->molecules(o_type2).the_atoms(j).x_pos;
        dy2 =  the_topology->molecules(o_type2).the_atoms(j).y_pos;
        r2  =  the_topology->atom_sizes(t2);      // Get radius
                                        // Calculate atom position
        x2  =  obj2->pos_x + dx2 * cos(theta2) - dy2 * sin(theta2);
        y2  =  obj2->pos_y + dx2 * sin(theta2) + dy2 * cos(theta2);

        for( int k = 0; k < the_topology->molecules(o_type1).n_atoms; k++ ){
                                        // Get atom information
            t1  =  the_topology->molecules(o_type1).the_atoms(k).type;
            dx1 =  the_topology->molecules(o_type1).the_atoms(k).x_pos;
            dy1 =  the_topology->molecules(o_type1).the_atoms(k).y_pos;
            r1  =  the_topology->atom_sizes(t1);  // Get radius
                                        // Calculate atom position
            x1  =  obj1->pos_x + dx1 * cos(theta1) - dy1 * sin(theta1);
            y1  =  obj1->pos_y + dx1 * sin(theta1) + dy1 * cos(theta1);

            dx = (x2-x1);
            dy = (y2-y1);
                                        // Handle periodic conditions
            if( is_periodic ){          // Find closest image
                if( dx > (x_size - r1 - r2 )) dx -= x_size;
                if( dx < (r1 + r2 - x_size )) dx += x_size;
                if( dy > (y_size - r1 - r2 )) dy -= y_size;
                if( dy < (r1 + r2 - y_size )) dy += y_size;
            }
            r = dx*dx+dy*dy;
            if( r < ((r1+r2)*(r1+r2))) return true;
        }
    }
    return false;
}

/**
 * This function tests if there are any clashes the configuration using the
 * topology file but not the forcefield
 *
 * @return true if there is a clash otherwise false.
 */
bool
config::test_clash(){
    int	i;
    object *obj1;
    object *obj2;

    for(i=0;i<obj_list.size();i++){
        obj1 = obj_list.get(i);
        for(int j=0; j<i; j++){
            obj2 = obj_list.get(j);
            if( test_clash( obj1, obj2 )) return true;
        }
    }
    return false;
}

/**
 * This function tests if the new_object can be inserted into the configuration
 * without generating a clash (as determined by the topology file).
 *
 * @param new_object a pointer to a valid object to test for insertion.
 * @return true if there is a clash otherwise false.
 */
bool
config::test_clash( object *new_object ){
    int max_o_type = the_topology->n_atom_types - 1 ;
    int o_type1 = simple_min( new_object->o_type, max_o_type );
    double theta1 = new_object->orientation;
    double t1, dx1, dy1, r1, x1, y1;
    object *obj1;

    if( !is_periodic ){                     // Check clash with walls.
        for(int i = 0; i < the_topology->molecules(o_type1).n_atoms; i++ ){
            t1  =  the_topology->molecules(o_type1).the_atoms(i).type;
            dx1 =  the_topology->molecules(o_type1).the_atoms(i).x_pos;
            dy1 =  the_topology->molecules(o_type1).the_atoms(i).y_pos;
            r1  =  the_topology->atom_sizes(t1);  // Get radius
                                            // Calculate atom position
            x1  =  new_object->pos_x + dx1 * cos(theta1) - dy1 * sin(theta1);
            y1  =  new_object->pos_y + dx1 * sin(theta1) + dy1 * cos(theta1);
            if( is_rectangle ){
                if(( x1 < r1 ) || ( (x1 + r1) > x_size ) || ( y1 < r1 ) ||
                    ( (y1 + r1) > y_size ))
                    return true;
            } else {
                if( ! poly->is_inside( x1, y1, r1 )) return true;
            }
        }
    }
                                            // Loop over the objects.
    for(int i = 0; i < n_objects(); i++){
        obj1 = obj_list.get(i);
        if(test_clash( obj1 ,new_object)) return true;
    }
    return false;
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
    return 0.0;
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

    if( is_rectangle ){
        x_size *= dl;                           // Expand boundary
        y_size *= dl;
    } else {
        poly->expand( dl );
    }
    unchanged = false;                      // The energies will be different
    for(i=0;i<obj_list.size();i++){
        obj_list.get(i)->recalculate = true;// Also for the objects
        obj_list.get(i)->expand(dl);        // Move objects in rescaled box
    }
    return (test_clash());
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

    if( is_rectangle ){
        x_size *= dl;                           // Expand boundary
        y_size *= dl;
    } else {
        poly->expand( dl );
    }
    unchanged = false;                      // The energies will be different
    for(i=0;i<obj_list.size();i++){
        obj_list.get(i)->recalculate = true;// Also for the objects
        obj_list.get(i)->expand(dl);        // Move objects in rescaled box
    }
    for(i=0;i<max_try;i++){
      if(test_clash()) jiggle();
    }
    return test_clash();
}

/**
 * Move a given object to a new place using the scaling factor dl_max
 * to control the distance distribution.
 *
 * @param obj_number the index of the object to move
 * @param dl_max the scaling parameter.
 */
void config::move(int obj_number, double dl_max){
    obj_list.get(obj_number)->move(dl_max, x_size, y_size, is_periodic ); // TODO: Need to fix this for non-rectangles
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
        if( obj1->distance(obj2, x_size, y_size, is_periodic) < distance ) // TODO: Need to check works for non-rectangles
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

/** \brief Fetch object from list by index
 *
 *  \param index the index of the object in the list
 *  \return the_object
 */
object	*config::get_object( int index ){
    assert( index < n_objects() );
    return obj_list.get(index);
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
    should incorporate atom draw radius in topology (so no FF needed)... 

    TODO handle is_periodic and is_rectangle correctly
*/

    object  *my_obj;
    double  theta, dx, dy, r, x, y;
    int     t, lr, tb;
    char    *my_color;
    int     max_o_type, o_type;
    
    if ( !the_topology ){
        throw runtime_error(
        "Generating postscript images from a configuration requires setting a topology.");
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

bool
config::has_clash( int i ){
    for(int j=0; j< obj_list.size(); j++ ){
        if (i!=j) {
            if( test_clash( obj_list.get(i), obj_list.get(j) )) return true;
        }
    }
    return false;
}

/***
 * @brief Shake objects a bit to try and remove bad contacts
 */
void
config::jiggle(){
    for( int i=0; i < obj_list.size(); i++){
        if( has_clash( i )) jiggle( obj_list.get(i) );
    }
}

/***
 * @brief Shake an object a little
 *
 * @param my_obj the object to jiggle
 */
void
config::jiggle(object *my_obj){
   my_obj->move(1.0, x_size, y_size, is_periodic);		// TODO: Need to check works for non-rectangles
   my_obj->rotate(M_PI);
}
