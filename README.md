<!---
This is the initial page for the documentation tree beyond the files and classes
that are generated automatically.
-->

The suite of programmes desribed here are designed to provide a simple system
for investigating very coarse grained models of membrane proteins. The programmes
operate with several different types of file that describe the system being modelled.

# Files and file formats

The programmes have in common that they all operate on configurations.
A configuration can be thought of as a series of objects organized on 
a surface. 
These objects might, for example, be proteins and the surface
a flat two dimensional representation of a membrane. 
There can be several different types of object in the configuration.
Configurations can be stored in human readable files and their structure 
is described [here](@ref config_file).

The various objects referenced in the configuration are made up of one
or more beads (or atoms). Atoms have various properties, such as size 
and color and determine how the various objects interact.
The organization of the various atoms to make up objects is described by a 
topology file that is described in detail [here](@ref topology_file).

The interactions between the different atoms is defined by a force field.
This data structure defines the properties of the different atom types, and
teir pairwise interactions. The force field is saved in a file that is
described [here](@ref force_field_file).

# Programmes and utilities

The programmes:
* [NVT](@ref NVT) - perform a NVT monte-carlo simulation
* [makeconfig](@ref makeconfig) - create a new configuration file
* [config2eps](@ref config2eps) - create a postscript file from a configuration file

# Compilation, installation and testing

* Need the BOOST library

# Tutorial

