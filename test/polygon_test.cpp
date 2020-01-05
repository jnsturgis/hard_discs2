
#include "../Classes/polygon.h"
#include <cassert>

int main()
{
    printf("Starting tests for Class polygon\n\n");
    // Test constructors
    polygon* poly1 = new polygon();
    poly1->add_vertex( 0.00, 0.00 );
    poly1->add_vertex( 1.00, 0.00 );
    poly1->add_vertex( 1.00, 1.00 );
    poly1->add_vertex( 0.00, 1.00 );

    printf( "Created unit square..." );
    assert( poly1->n_vertex == 4 );
    printf( "Has 4 sides... " );
    assert( poly1->area() == 1.00 );
    printf( "Area OK\n" );

    poly1->expand(2.00);
    assert( poly1->area() == 4.00 );
    printf( "Expansion works as expected.\n" );

    polygon* poly2 = new polygon(4);
    poly2->add_vertex( 0.50, 0.50 );
    poly2->add_vertex( 1.50, 0.50 );
    poly2->add_vertex( 1.50, 1.50 );
    poly2->add_vertex( 0.50, 1.50 );

    polygon* poly3 = new polygon(poly1);

    poly1->expand(0.5);
    assert( poly1->area() == 1.00 );
    printf( "Shrinking works as expected.\n" );

    assert( poly2->is_inside( 1.0, 1.0 ));
    assert( !poly2->is_inside( 5.0, 1.0 ));
    assert( poly2->is_inside( 1.0, 1.5 ));
    assert( poly2->is_inside( 1.5, 1.0 ));
    assert( poly2->is_inside( 1.5, 1.5 ));
    assert( poly2->is_inside( 0.5, 0.5 ));
    assert( poly2->is_inside( 1.1, 1.1, 0.3 ));
    assert( ! poly2->is_inside( 1.1, 1.1, 0.4 )); // This is not what I want !! maths errors (10^-16)
    assert( ! poly2->is_inside( 1.1, 1.1, 0.41 ));
    printf( "Points inside test OK\n" );

    assert( poly3->is_inside( poly2 ));
    assert( poly3->is_inside( poly1 ));
    assert( ! poly2->is_inside( poly1 ));
    printf( "Polygon inside tests OK\n");

    poly2->write(stdout);

    delete poly1;
    delete poly2;
    delete poly3;				// Valgrind is happy there are no leaks (31/12/19)

    printf("Finished tests for Class polygon\n");
    return EXIT_SUCCESS;
};

