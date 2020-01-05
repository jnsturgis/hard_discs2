
#include "../Classes/config.h"
#include <cassert>
#include <exception>

#define EPSILON 1e-15

int main()
{
    printf("-------------------------------\n");
    printf("Starting tests for Class config\n\n");

    config* config0 = new config();			// An empty configuration
    config* config1 = new config("test1.config");	// Read from a file
    config* config2 = new config( *config1 );		// Clone config1 passing configuration
    config* config3 = new config( config1 );		// Clone config1 passing pointer
    config* config4 = new config("test2.config");	// Read from a file multiple object types and non-rectangle

    printf("Constructors tested for Class config\n");

    assert( config1->rms( *config2 ) <= EPSILON );
    assert(( config1->area()-1e4) < EPSILON );
    assert( ! config2->expand(2.0));			// No associated topology
    assert(( config2->area()-4e4) < EPSILON );

    config1->write( stdout );				// This should be the same as test1.config


    assert( ! config4->is_periodic );
    assert( ! config4->is_rectangle );
    assert( config4->n_vertex == 4 );
    double  value = config4->area();
    printf( "Area is %f\n", value);
    assert( ! config4->expand(2.0));			// No associated topology
    assert(( config4->area()-4*value) < EPSILON );

    printf("Testing errors on badly formed files for Class config\n");

    try {
        config* config5 = new config("test3.config");	// Read from a poorly formed file
        delete config5;
    }
    catch(exception &e) {
        cout << e.what() << "\n";
    }

    try {
        config* config6 = new config("no_name");        // Read from a non-existant file
        delete config6;
    }
    catch(exception &e) {
        cout << e.what() << "\n";
    }

    printf("Running destructors\n");

    delete config0;
    delete config1;
    delete config2;
    delete config3;
    delete config4;

    printf("Finished tests for Class config\n");
    printf("-------------------------------\n");
    return EXIT_SUCCESS;
};

