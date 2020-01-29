/** 
 * @file        polygon.cpp
 * @author      James Sturgis
 * @date        December 30, 2019
 * @version     1.0
 * \brief       Implementation file for the polygon class.
 *
 * @class       polygon polygon.h
 * @brief       The polygon class.
 *
 * The unit test programme is in ../tests/polygon_test.cpp to
 * test the various functions behave as expected.
 */

#include "polygon.h"
#include <boost/format.hpp>
#include <math.h>

// TODO it would be nice if with points we could do some other
// operations like: +, -, dot(), and other vector operators
// Polygon intersections - bounding boxes - postscript paths...
// TODO dynamically allocate space for points if there is n_alloc
// is too small rather than just dying.

double
distance2( Point A, Point B){
    return ((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y));
}

double
distance( Point A, Point B){
    return (sqrt(distance2(A,B)));
}

polygon::polygon(){
    n_alloc = 8;
    n_vertex = 0;
    _vertices = new Point[n_alloc];
}

polygon::polygon(int n_sides){
    n_alloc = n_sides;
    n_vertex = 0;
    _vertices = new Point[n_alloc];
}

polygon::polygon( polygon *orig ){
    n_alloc = orig->n_alloc;
    _vertices = new Point[ n_alloc ];
    n_vertex = orig->n_vertex;
    for( int i=0; i < n_vertex; i++ ){
        _vertices[i].x = orig->_vertices[i].x;
        _vertices[i].y = orig->_vertices[i].y;
    }
}

polygon::~polygon(){
    delete[] _vertices;
}

void
polygon::add_vertex( double x_val, double y_val ){
	if( n_vertex == n_alloc ){
		n_alloc += n_alloc;
		Point *new_vertices = new Point[ n_alloc ];
		for( int i = 0; i < n_vertex; i++ ){
			new_vertices[i].x = _vertices[i].x;
			new_vertices[i].y = _vertices[i].y;
		}
		delete _vertices;
		_vertices = new_vertices;
	}
    _vertices[n_vertex].x = x_val;
    _vertices[n_vertex].y = y_val;
    n_vertex++;
}

void
polygon::expand( double scale ){
    for( int i=0; i < n_vertex; i++ ){
        _vertices[i].x *= scale;
        _vertices[i].y *= scale;
    }
}

double
polygon::area(){
    double Area = 0.0;
    for(int i = 0; i < n_vertex; i++){
        Point curr = _vertices[i];
        Point next = _vertices[(i + 1)%n_vertex];
        Area += curr.x * next.y - next.x * curr.y;
    }
    return abs(Area / 2.0);
}

double
polygon::max_dist()			// Calculate maximum length in polygon.
{
    double result = 0.0;
    double dist;
    Point  A;
    Point  B;

    for(int i = 0; i < n_vertex; i++ ){
        A = _vertices[i];
        for(int j=i; j < n_vertex; j++ ){
            B = _vertices[j];
            dist = distance( A, B );
            if( dist > result ) result = dist;
        }
    }
    return result;
}

double
polygon::x_min()
{
    double	result = _vertices[0].x;
    for( int i=1; i<n_vertex; i++ )
        if( _vertices[i].x < result ) result = _vertices[i].x;
    return result;
}

double
polygon::y_min()
{
    double	result = _vertices[0].y;
    for( int i=1; i<n_vertex; i++ )
        if( _vertices[i].y < result ) result = _vertices[i].y;
    return result;
}

double
polygon::x_max()
{
    double	result = _vertices[0].x;
    for( int i=1; i<n_vertex; i++ )
        if( _vertices[i].x > result ) result = _vertices[i].x;
    return result;
}

double
polygon::y_max()
{
    double	result = _vertices[0].y;
    for( int i=1; i<n_vertex; i++ )
        if( _vertices[i].y > result ) result = _vertices[i].y;
    return result;
}

double
polygon::center_x()
{
    double result = _vertices[0].x;
    for( int i=1; i<n_vertex; i++ )
        result += _vertices[i].x;
    return result/n_vertex;
}

double
polygon::center_y()
{
    double result = _vertices[0].y;
    for( int i=1; i<n_vertex; i++ )
        result += _vertices[i].y;
    return result/n_vertex;
}

bool
polygon::is_inside( double x, double y )
{
    bool    l_test = false;		// Test both left and right rays to resolve
    bool    r_test = false;		// Edge issues... also need to handle 
					// horizontal and vertical edges.
    Point   fixed = Point(x, y);

    for( int i=0; i<n_vertex; i++ ){
        Point curr = _vertices[i];
        Point next = _vertices[(i + 1)%n_vertex];
        if(((curr.y >= fixed.y ) && ( next.y <= fixed.y ))          // could left and right rays cross segment ?
            || ((curr.y <= fixed.y ) && ( next.y >= fixed.y ))) {
            if( next.y != curr.y ){				    // point not colinear with a horizontal edge
                double slope = (next.x - curr.x) / (next.y - curr.y);
                double x_test = curr.x + ( fixed.y - curr.y ) * slope;
                if( fixed.x < x_test ) r_test = ! r_test;           // x to left of current edge
                if( fixed.x > x_test ) l_test = ! l_test;           // x to right of current edge
            }
        }
    }
    return ( l_test || r_test );
}

bool
polygon::is_inside( double x, double y, double radius )
{
    bool     test = is_inside( x, y );
    int      i = 0;
    double   t;
    double   dist;
    Point    fixed = Point(x, y);
    Point    projection = Point();

    while( test && (i < n_vertex)){
        Point curr = _vertices[i];
        Point next = _vertices[(i + 1)%n_vertex];
        float l2 = distance2( next, curr);
        if( 0.0 == l2 ){ 
            dist = distance( fixed, curr );
        } else {
            t = ((fixed.x - curr.x) * (next.x - curr.x) + (fixed.y - curr.y) * (next.y - curr.y)) / l2;
            t = (t<0.0)?0.0:((t>1.0)?1.0:t);
            projection.x = curr.x + t * (next.x - curr.x);
            projection.y = curr.y + t * (next.y - curr.y);
            dist = distance( fixed, projection );
        }
        if( dist < radius ) test = false;
        i++;
    }
    return test;
}

bool
polygon::is_inside( polygon* other )
{
    for(int i = 0; i < other->n_vertex; i++){
        if( ! is_inside( other->_vertices[i].x, other->_vertices[i].y ))
            return false;
    }
    return true;
}

int
polygon::write( FILE *dest ){
    fprintf( dest, "%d\n", n_vertex );
    for( int i=0; i < n_vertex; i++ ){
        Point curr = _vertices[i];
        fprintf( dest, "%9f %9f \n", curr.x, curr.y );
    }
    return EXIT_SUCCESS;
}

int
polygon::write( std::ostream& dest ){
    dest << boost::format("%d\n") % n_vertex;
    for( int i=0; i < n_vertex; i++ ){
        Point curr = _vertices[i];
        dest << boost::format("%9f %9f \n") % curr.x % curr.y ;
    }
    return EXIT_SUCCESS;
}

void
polygon::ps_draw( std::ostream& dest ){
    Point curr = _vertices[0];
    dest << curr.x << " " << curr.y << " moveto\n";
    for( int i=1; i < n_vertex; i++ ){
        Point curr = _vertices[i];
        dest << curr.x << " " << curr.y << " lineto\n";
    }
}

int
polygon::winding(){			/// Return +ve if CW winding -ve cor CCW
    double sum = (_vertices[0].x - _vertices[n_vertex-1].x)*(_vertices[0].y - _vertices[n_vertex-1].y);
    for( int i=1; i < n_vertex; i++ ){
        sum += (_vertices[i].x - _vertices[i-1].x)*(_vertices[i].y - _vertices[i-1].y);
    }
    return ((sum>0.0)?+1:-1);
}

void
polygon::order_vertices(){

    int    index = 0;
    int	   i;
    double min_y = _vertices[0].y;
    double x_val = _vertices[0].x;

    for( i=1; i < n_vertex; i++ ){
        if(( _vertices[i].y < min_y ) || (( _vertices[i].y == min_y ) && ( _vertices[i].x < x_val ))) {
            index = i;
            min_y = _vertices[i].y;
            x_val = _vertices[i].x;
        }
    }
    while( index != 0){
        Point save = _vertices[0];
        for( i=1; i < n_vertex; i++ ){
            _vertices[i-1] = _vertices[i];
        }
        _vertices[i-1] = save;
        index--;
    }
    if( winding() < 0.0 ){	/// Reverse order
       int j = 1;
       int k = n_vertex - j;
       while( j < k ){
           Point      p = _vertices[j];
           _vertices[j] = _vertices[k];
           _vertices[k] = p;
           j++;
           k--;
       }
    }

    assert( winding() > 0.0 );
    assert( _vertices[0].y == y_min() );
    assert( _vertices[1].y >  y_min() );

}

void
polygon::translate( double dx, double dy ){
    for(int i=0; i < n_vertex; i++ ){
        _vertices[i].x += dx;
        _vertices[i].y += dy;
    }
}

void
polygon::rotate( double angle ){	/// Do a clockwise rotation of angle.
    double c = cos(-angle);
    double s = sin(-angle);

    for(int i=0; i < n_vertex; i++ ){ /// Rotate all points
        double x_new = _vertices[i].x * c - _vertices[i].y * s;
        double y_new = _vertices[i].x * s + _vertices[i].y * c;
        _vertices[i].x = x_new;
        _vertices[i].y = y_new;
    }
}

const Point
polygon::get_vertex( int i ){
    return _vertices[i];
}

bool
polygon::is_parallelogram(){
    if( n_vertex != 4 ) return false;
    if( (_vertices[1].x - _vertices[0].x) != (_vertices[3].x - _vertices[2].x)) return false;
    if( (_vertices[1].y - _vertices[0].y) != (_vertices[3].y - _vertices[2].y)) return false;
    if( (_vertices[2].x - _vertices[1].x) != (_vertices[3].x - _vertices[0].x)) return false;
    if( (_vertices[2].y - _vertices[1].y) != (_vertices[3].y - _vertices[0].y)) return false;
    return true;
}


