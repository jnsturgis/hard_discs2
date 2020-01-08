/**
 * @file        polygon.h
 * @author      James Sturgis
 * @date        December 30, 2019
 * @version     1.0
 * \brief       Header file for the polygon class.
 *
 * @class       polygon polygon.h
 * @brief       The polygon class.
 */

#ifndef POLYGON_H
#define POLYGON_H

#include <iostream>
#include <stdio.h>

using namespace std;

typedef struct Point {
    double   x, y;
    Point()
    { x = 0.0; y = 0.0; }
    Point(double x_val, double y_val )
    { x = x_val; y = y_val; }
} Point;

class polygon {
public:
    polygon();
    polygon( int n_vertex );
    polygon( polygon* orig );

    virtual ~polygon();

    void    add_vertex( double x, double y );
    void    expand( double scale );

    double  area();
    double  max_dist();		///< Longest vertex to vertex distance (JS 8/1/20)
    double  x_min();		///< Points on bounding rectangle (JS 8/1/20)
    double  x_max();
    double  y_min();
    double  y_max();

    bool    is_inside( double x, double y );
    bool    is_inside( double x, double y, double radius );
    bool    is_inside( polygon* other );

    int     write( FILE *dest );
    int     write( std::ostream& dest );

    int     n_vertex;
private:
    int     n_alloc;
    Point*  _vertices;
};

#endif
