//    Organic Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for organic solar cells. 
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#define PATHLEN	512
#define epsilon0 8.85418782e-12	// m-3 kg-1 s4 A2;
#define hp 6.62606896e-34	//J S Wikipeda
#define PI 3.14159265358979323846
#define hbar (6.62606896e-34/(2.0*PI))	//Calculated
#define kb 1.3806504e-23	//J K-1 Wiki
#define Q 1.602176487e-19	//C Wikipeda
#define m0 9.10938215e-31	//Kg Wikipeda
#define cl  2.99792458e8	//m/s Wikipieda
