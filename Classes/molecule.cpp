
#include "molecule.h"

molecule::molecule(){
    mol_name = "";
    n_atoms = 0;
}

molecule::molecule(molecule *orig){
//    int     i;
    mol_name = orig->mol_name;
    n_atoms = orig->n_atoms;
    the_atoms.resize( n_atoms );
//    for( i=0; i< n_atoms; i++ )
//        the_atoms(i).copy( orig->the_atoms(i) );
    assert( false );
}

molecule::molecule(FILE *source){
    read(source);
}

molecule::~molecule(){
    the_atoms.resize(0);
}

void
molecule::add_atom( atom *an_atom ){
    n_atoms++;
    the_atoms.resize( n_atoms );
    the_atoms(n_atoms - 1).copy( *an_atom );
}

void
molecule::rename( std::string a_name ){
    mol_name = a_name;
}

int
molecule::write(FILE *dest){
    int     i;

    fprintf(dest, "%s\n", mol_name.c_str() );
    fprintf(dest, "%d\n", n_atoms );
    for(i=0; i<n_atoms; i++)
        the_atoms(i).write( dest );
    return true;
}

int
molecule::write(std::ostream& dest){
    int     i;

    dest << mol_name.c_str() << "\n" << std::to_string(n_atoms) << "\n";
    for(i=0; i<n_atoms; i++)
        the_atoms(i).write( dest );
    return true;
}

void
molecule::read(FILE *source){
    int     i;
    char    name[64];

    assert( n_atoms == 0 );
    fscanf( source, "%s\n", (char *)(&name) );
    mol_name.assign(name);
    fscanf( source, "%d\n", &n_atoms );
    assert( n_atoms > 0 );
    the_atoms.resize(n_atoms);
    for( i=0; i< n_atoms; i++ )
        the_atoms(i).read(source);
}
