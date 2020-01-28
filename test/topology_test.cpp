
#include "../Classes/topology.h"
#include <cassert>
#include <exception>
#include <iostream>

#define EPSILON 1e-15

int main(int argc, char **argv )
{
    printf("-------------------------------\n");
    printf("Starting tests for topology config\n\n");

    std::cerr << "0";
    topology* topo0 = new topology();			// An empty configuration
    std::cerr << "1";
    topology* topo1 = new topology("test1.topo");	// Read from a named file
    std::cerr << "2";
    topology* topo2 = new topology( topo1 );		// Clone topo1 passing pointer
    std::cerr << "3";
    topology* topo3 = new topology(0.5);		// Simple topology radius 0.5 disc
    std::cerr << "4";
    topology* topo4 = new topology(argv[1]);		// From argument named file

    printf("Constructors tested for topology config\n");

    std::cerr << "================0==============\n";
    topo0->write( std::cerr );
    std::cerr << "================1==============\n";
    topo1->write( std::cerr );
    std::cerr << "================2==============\n";
    topo2->write( std::cerr );
//    std::cerr << "Atoms    :" << topo2->n_atom_types << "\n";
//    std::cerr << "Molecules:" << topo2->n_molecules << "\n";
    std::cerr << "================3==============\n";
    topo3->write( std::cerr );
    std::cerr << "===add a second 1 atom mol====\n";
    topo3->add_molecule(1.0);
    topo3->write( std::cerr );
    std::cerr << "================4==============\n";
    topo4->write( std::cerr );
    std::cerr << "==============================\n";
    printf("Running destructors\n");

    delete topo0;
    delete topo1;
    delete topo2;
    delete topo3;
    delete topo4;

    printf("Finished tests for Class topology\n");
    printf("---------------------------------\n");
    return EXIT_SUCCESS;
};

