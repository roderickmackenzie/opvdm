//    Organic Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for organic solar cells. 
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//	roderick.mackenzie@nottingham.ac.uk
//	www.roderickmackenzie.eu
//	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim.h"
#include <math.h>

static int frames;
void render1d_reset()
{
	frames = 0;
}

void redner1d_slice_dump(struct device *in, int i)
{
#ifdef dump1d
	FILE *out;
	int band = 0;
	char outpath[200];
	char temp[400];
	struct istruct dump1;
	inter_init(&dump1);

	struct istruct dump2;
	inter_init(&dump2);

	struct istruct dump3;
	inter_init(&dump3);

	struct istruct dump4;
	inter_init(&dump4);

	struct istruct dump5;
	inter_init(&dump5);

	struct istruct dump6;
	inter_init(&dump6);

	for (band = 0; band < in->srh_bands; band++) {

		inter_append(&dump1, get_dos_E_n(band, in->imat[i]),
			     in->n[i] * in->srh_n_r1[i][band] -
			     in->srh_n_r2[i][band]);
		inter_append(&dump2, get_dos_E_p(band, in->imat[i]),
			     in->p[i] * in->srh_p_r1[i][band] -
			     in->srh_p_r2[i][band]);
		inter_append(&dump3, get_dos_E_n(band, in->imat[i]),
			     in->nt[i][band] - in->ntbinit[i][band]);
		inter_append(&dump4, get_dos_E_p(band, in->imat[i]),
			     in->pt[i][band] - in->ptbinit[i][band]);
		inter_append(&dump5, get_dos_E_n(band, in->imat[i]),
			     in->p[i] * in->srh_n_r3[i][band] -
			     in->srh_n_r4[i][band]);
		inter_append(&dump6, get_dos_E_p(band, in->imat[i]),
			     in->n[i] * in->srh_p_r3[i][band] -
			     in->srh_p_r4[i][band]);

	}

	sprintf(outpath, "%s%s", in->outputpath, "dump1.dat");
	inter_save(&dump1, outpath);
	inter_free(&dump1);

	sprintf(outpath, "%s%s", in->outputpath, "dump2.dat");
	inter_save(&dump2, outpath);
	inter_free(&dump2);

	sprintf(outpath, "%s%s", in->outputpath, "dump3.dat");
	inter_save(&dump3, outpath);
	inter_free(&dump3);

	sprintf(outpath, "%s%s", in->outputpath, "dump4.dat");
	inter_save(&dump4, outpath);
	inter_free(&dump4);

	sprintf(outpath, "%s%s", in->outputpath, "dump5.dat");
	inter_save(&dump5, outpath);
	inter_free(&dump5);

	sprintf(outpath, "%s%s", in->outputpath, "dump6.dat");
	inter_save(&dump6, outpath);
	inter_free(&dump6);

	out = fopen("./frame.plot", "w");
	fprintf(out, "set title 'V=%.3lf (V) time=%le (s)'\n", in->Vapplied,
		in->time);
	fprintf(out, "set terminal jpeg large \n");
	fprintf(out, "set multiplot\n");
	fprintf(out, "set origin 0,0\n");
	fprintf(out, "set size 1.0,0.33\n");

	fprintf(out, "set xlabel 'Energy (eV)'\n");
	fprintf(out, "set ylabel 'log10(T)'\n");

	fprintf(out,
		"plot 'dump1.dat' using (-$1):($2) title 'T_n' with l lw 3,'dump2.dat' using (-$1):($2) title 'T_p' with l lw 3\n");
	fprintf(out, "set origin 0,0.66\n");
	fprintf(out, "unset logscale y\n");
	fprintf(out, "set yrange [*:*]\n");
	fprintf(out, "set ylabel 'Charge density (m^{-3})'\n");
	fprintf(out,
		"plot 'dump3.dat' using (-$1):($2) title 'n' with l lw 3,'dump4.dat' using (-$1):($2) title 'p' with l lw 3\n");
	fprintf(out, "set origin 0,0.33\n");
	fprintf(out, "set logscale y\n");
	fprintf(out, "set yrange [*:*]\n");
	fprintf(out, "set ylabel 'Charge density (m^{-3})'\n");
	fprintf(out,
		"plot 'dump5.dat' using (-$1):($2) title 'R_n' with l lw 3,'dump6.dat' using (-$1):($2) title 'R_p' with l lw 3\n");
	fclose(out);

	system("gnuplot frame.plot >frame.jpg");
	sprintf(temp, "mv frame.jpg %d.jpg", frames);
	printf("running '%s'\n", temp);
	system(temp);
	frames++;
#endif
}
