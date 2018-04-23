/**
 * \file    config2eps.cpp
 * \author  James Sturgis
 * \date    April 12, 2018
 * \version 1.0
 * \brief   Convert a configuration to an eps figure.
 *
 * This file contains the main routine for the config2eps program that is part of
 * the Very Coarse Grained disc simulation programmes.
 *
 * The program converts a configuration file, read from the input, to an encapuslated
 * postscript file containing a representation of the configuration in which each atom
 * of each object is drawn as a circle of the radius given in the force field and
 * the color given in the force field. The area of the forcefield is scaled to fit into
 * the output area.
 *
 * Usage:
 *          config2eps < config_file > eps_file.
 *
 * \todo        Non square areas scaled correctly.
 * \todo        More control on preamble and ending of output.
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include "../NVT/config.h"

using namespace std;

string preamble = ""
"%!PS-Adobe-2.0 EPSF-1.2 \n"
"%%Title: Postscript figure for configuration \n"
"%%Creator: James \n"
"%%CreationDate: Today \n"
"%%DocumentFonts: (atend) \n"
"%%BoundingBox: 0 0 227 227 \n"
"%%EndComments \n"
" \n"
"/mydict 128 dict def \n"
"mydict begin \n"
"/Color true def \n"
"/Solid true def \n"
"/Rounded false def \n"
" \n"
"/gnulinewidth 5.000 def \n"
"/userlinewidth gnulinewidth def \n"
"/vshift 0 def \n"
"/dl {10.0 mul} def \n"
"/hpt_ 1.0 def \n"
"/vpt_ 1.0 def \n"
"/hpt hpt_ def \n"
"/vpt vpt_ def \n"
"/symbolsize 0.5 def \n"
" \n"
"/vpt2 vpt 2 mul def \n"
"/hpt2 hpt 2 mul def \n"
" \n"
"/Lshow { currentpoint stroke moveto 0 vshift rmoveto show } def \n"
"/Rshow { currentpoint stroke moveto dup stringwidth pop neg vshift rmoveto show } def \n"
"/Cshow { currentpoint stroke moveto dup stringwidth pop -2 div vshift rmoveto show } def \n"
" \n"
"/UP { dup vpt_ mul /vpt exch def hpt_ mul /hpt exch def \n"
"  /hpt2 hpt 2 mul def /vpt2 vpt 2 mul def } def \n"
"/DL { Color {setrgbcolor Solid {pop []} if 0 setdash } \n"
" {pop pop pop 0 setgray Solid {pop []} if 0 setdash} ifelse } def \n"
"/BL { stroke userlinewidth 2 mul setlinewidth \n"
"      Rounded { 1 setlinejoin 1 setlinecap } if } def \n"
"/AL { stroke userlinewidth 2 div setlinewidth \n"
"      Rounded { 1 setlinejoin 1 setlinecap } if } def \n"
"/UL { dup gnulinewidth mul /userlinewidth exch def \n"
"      dup 1 lt {pop 1} if 10 mul /udl exch def } def \n"
"/PL { stroke userlinewidth setlinewidth \n"
"      Rounded { 1 setlinejoin 1 setlinecap } if } def \n"
" \n"
"% Plotting stuff \n"
"/minorticksize 0.015 def \n"
"/majorticksize minorticksize 2.0 mul def \n"
" \n"
"/xminortick { 0 moveto 0.0 minorticksize rlineto  \n"
"	0.0 1.0 2.0 minorticksize mul sub rmoveto  \n"
"	0.0 minorticksize rlineto } def \n"
"/xmajortick { 0 moveto 0.0 majorticksize rlineto  \n"
"	0.0 1.0 2.0 majorticksize mul sub rmoveto  \n"
"	0.0 majorticksize rlineto 0.0 -1.0 rmoveto } def \n"
"/yminortick { 0 exch moveto minorticksize 0.0 rlineto \n"
"	1.0 2.0 minorticksize mul sub 0.0 rmoveto \n"
"	minorticksize 0.0 rlineto -1.0 0.0 rmoveto } def \n"
"/ymajortick { 0 exch moveto majorticksize 0.0 rlineto \n"
"	1.0 2.0 majorticksize mul sub 0.0 rmoveto \n"
"	majorticksize 0.0 rlineto -1.0 0.0 rmoveto } def \n"
" \n"
"% Linetypes \n"
"/LTw { PL [] 1 setgray } def \n"
"/LTb { BL [] 0 0 0 DL } def \n"
"/LTa { AL [1 udl mul 2 udl mul] 0 setdash 0 0 0 setrgbcolor } def \n"
"/LT0 { PL [] 0.7 0 0 DL } def \n"
"/LT1 { PL [40. 20.] 0 0.6 0 DL } def \n"
"/LT2 { PL [20. 30.] 0 0 0.7 DL } def \n"
"/LT3 { PL [10. 15.] 1 0 1 DL } def \n"
"/LT4 { PL [50. 20. 10. 20.] 0 1 1 DL } def \n"
"/LT5 { PL [40. 30. 10. 30.] 1 1 0 DL } def \n"
"/LT6 { PL [20. 20. 20. 40.] 0 0 0 DL } def \n"
"/LT7 { PL [20. 20. 20. 20. 20. 40.] 1 0.3 0 DL } def \n"
"/LT8 { PL [20. 20. 20. 20. 20. 20. 20. 40.] 0.5 0.5 0.5 DL } def \n"
" \n"
"/green { 0 1.0 0 setrgbcolor } def \n"
"/red   { 1.0 0 0 setrgbcolor } def \n"
"/orange {0.7 0.35 0 setrgbcolor } def \n"
"/blue  { 0 0 1.0 setrgbcolor } def \n"
"/black { 0 setgray} def \n"
" \n"
"% Symbols \n"
"/CircE { currentpoint symbolsize dup 0 rmoveto 0 360 arc stroke } def \n"
"/circle {currentpoint 3 -1 roll dup 0 rmoveto 0 360 arc stroke } def \n"
"/fcircle {currentpoint 3 -1 roll dup 0 rmoveto 0 360 arc fill } def \n"
"/BoxE  { currentpoint symbolsize dup dup rmoveto -2.0 mul dup 0  \n"
"	rlineto dup 0 exch rlineto -1 mul 0 rlineto closepath stroke } def \n"
" \n"
"/Symbol-Oblique /Symbol findfont [1 0 .167 1 0 0] makefont \n"
"dup length dict begin {1 index /FID eq {pop pop} {def} ifelse} forall \n"
"currentdict end definefont pop \n"
"end \n"
"%%EndProlog \n"
"%% \n"
"%%Page: 1 \n"
"mydict begin \n"
" \n"
"gsave			% Original \n"
"28.3 28.3 scale		% Use cm as units \n"
" \n"
"gsave			% cm scaling page \n"
"			% Set up for panel 1 box 0,0 to 5,5 \n"
"0 setgray \n"
" \n"
"newpath \n"
" \n"
"0.005 UL		% set standard line width \n"
"LTb			% set line colour, width and dash. \n"
" 0 0   moveto		% draw bounding box \n"
" 8 0  lineto \n"
" 8 8  lineto \n"
" 0 8  lineto \n"
"closepath \n"
"stroke \n"
" \n"
"newpath \n"
"0  0  moveto		% and again bounding box \n"
"8  0  lineto \n"
"8  8  lineto \n"
"0  8  lineto \n"
"closepath \n"
"clip \n"
" \n"
"gsave \n";

string ending = ""
"grestore		% leave data scaling \n"
"grestore		% End of panel \n"
"grestore		% End of Figure \n"
"end \n"
"showpage \n"
"%%Trailer \n"
"%%DocumentFonts: Helvetica \n"
"%%Pages: 1 \n";

/**
 *
 */
int main(int argc, char** argv) {
    topology    *a_topology    = new topology();
    config      *current_state = new config(stdin);
    force_field *the_forces    = new force_field();
    double      scale;

    current_state->add_topology(a_topology);
    scale = 8.0/max(current_state->x_size,current_state->y_size);

    cout << preamble;
    cout << scale;
    cout << " dup scale \n";
    cout << 0.5/scale;
    cout << " UL\n";
    current_state->ps_atoms(the_forces, stdout);
    cout << ending;

    delete the_forces;
    delete current_state;

    return 0;
}
