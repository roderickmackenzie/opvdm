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
#include <math.h>
#include "util.h"
#include "true_false.h"
#include "../../sim_modes.h"
#include "../../dump_ctrl.h"

int check_int(char *in)
{
	int i = 0;
	int numeric = TRUE;
	for (i = 0; i < strlen(in); i++) {
		if ((in[i] < 48) || (in[i] > 57)) {
			numeric = FALSE;
			break;
		}
	}
	return numeric;
}

static int unused __attribute__ ((unused));

int english_to_bin(char *in)
{
	int ret = 0;
	if (check_int(in) == TRUE) {
		sscanf(in, "%d", &ret);
		return ret;
	}

	if (strcmp(in, "true") == 0) {
		return TRUE;
	} else if (strcmp(in, "false") == 0) {
		return FALSE;
	} else if (strcmp(in, "left") == 0) {
		return LEFT;
	} else if (strcmp(in, "right") == 0) {
		return RIGHT;
	} else if (strcmp(in, "gaus") == 0) {
		return 0;
	} else if (strcmp(in, "exp") == 0) {
		return 1;
	} else if (strcmp(in, "tpv_set_light") == 0) {
		return tpv_set_light;
	} else if (strcmp(in, "tpv_set_voltage") == 0) {
		return tpv_set_voltage;
	} else if (strcmp(in, "tpv_mode_laser") == 0) {
		return tpv_mode_laser;
	} else if (strcmp(in, "tpv_mode_sun") == 0) {
		return tpv_mode_sun;
	} else if (strcmp(in, "jv") == 0) {
		return sim_mode_jv;
	} else if (strcmp(in, "tpv") == 0) {
		return sim_mode_tpv;
	} else if (strcmp(in, "ce") == 0) {
		return sim_mode_ce;
	} else if (strcmp(in, "tpc") == 0) {
		return sim_mode_tpc;
	} else if (strcmp(in, "tof") == 0) {
		return sim_mode_tof;
	} else if (strcmp(in, "otrace") == 0) {
		return sim_mode_otrace;
	} else if (strcmp(in, "celiv") == 0) {
		return sim_mode_celiv;
	}

	printf("I don't understand the command %s\n", in);
	exit(0);
	return 0;
}

double read_value(char *file, int skip, int line)
{
	FILE *in;
	char buf[1000];
	double value;
	in = fopen(file, "r");
	if (in == NULL) {
		printf("Can not read file %s\n", file);
		exit(0);
	}
	int l = 0;

	do {
		l++;
		unused = fscanf(in, "%s", buf);

		if (l == line) {
			sscanf((buf + skip), "%le\n", &value);
			break;
		}

	} while (!feof(in));

	fclose(in);

	return value;
}

void safe_file(char *name)
{
	FILE *file;
	file = fopen(name, "rb");

	if (!file) {
		printf("File %s not found\n", name);
		getchar();
		exit(0);
	}

	fclose(file);
}

int cmpstr_min(char *in1, char *in2)
{
	int i;
	int max = strlen(in1);
	if (strlen(in2) < max)
		max = strlen(in2);
	for (i = 0; i < max; i++) {
		if (in1[i] != in2[i])
			return 1;
	}
	return 0;
}

int strcmp_end(char *str, char *end)
{

	if (strlen(str) < strlen(end))
		return 1;
	int pos = strlen(str) - strlen(end);

	return strcmp((char *)(str + pos), end);
}

int extract_str_number(char *in, char *cut)
{
	int out;
	int len = strlen(cut);
	sscanf((in + len), "%d", &out);
	return out;
}

void waveprint(char *in, double wavelength)
{
	if (wavelength < 400.0) {
		textcolor(fg_purple);
	} else if (wavelength < 500.0) {
		textcolor(fg_blue);
	} else if (wavelength < 575.0) {
		textcolor(fg_green);
	} else if (wavelength < 600.0) {
		textcolor(fg_yellow);
	} else {
		textcolor(fg_red);
	}

	printf("%s", in);
	textcolor(fg_reset);

}

void randomprint(char *in)
{
	int i;
	for (i = 0; i < strlen(in); i++) {
		int rnd = (float)6.0 * rand() / (float)RAND_MAX;
		if (rnd == 0)
			textcolor(fg_reset);
		if (rnd == 1)
			textcolor(fg_red);
		if (rnd == 2)
			textcolor(fg_green);
		if (rnd == 3)
			textcolor(fg_yellow);
		if (rnd == 4)
			textcolor(fg_blue);
		if (rnd == 5)
			textcolor(fg_purple);
		printf("%c", in[i]);
	}

}

void textcolor(int color)
{
	char command[13];
	sprintf(command, "\e[%dm", color);
	printf("%s", command);
}

FILE *fopena(char *path, char *name, const char *mode)
{
	char wholename[200];
	strcpy(wholename, path);
	strcat(wholename, name);
	FILE *pointer;
	pointer = fopen(wholename, mode);

	return pointer;
}

FILE *fopenaa(char *path, char *add0, char *name, const char *mode)
{
	char wholename[200];
	strcpy(wholename, path);
	strcat(wholename, add0);
	strcat(wholename, name);
	FILE *pointer;
	pointer = fopen(wholename, mode);

	return pointer;
}

FILE *fopenaaa(char *path, char *add0, char *add1, char *name, const char *mode)
{
	char wholename[200];
	strcpy(wholename, path);
	strcat(wholename, add0);
	strcat(wholename, add1);
	strcat(wholename, name);
	FILE *pointer;
	pointer = fopen(wholename, mode);

	return pointer;
}

int scanarg(char *in[], int count, char *find)
{
	int i;
	for (i = 0; i < count; i++) {
		if (strcmp(in[i], find) == 0)
			return TRUE;
	}
	return FALSE;
}

int get_arg_plusone_pos(char *in[], int count, char *find)
{
	int i;
	for (i = 0; i < count; i++) {
		if (strcmp(in[i], find) == 0) {
			if ((i + 1) < count) {
				return i + 1;
			} else {
				return FALSE;
			}
		}
	}
	return FALSE;
}

char *get_arg_plusone(char *in[], int count, char *find)
{
	int i;
	static char no[] = "";
	for (i = 0; i < count; i++) {

		if (strcmp(in[i], find) == 0) {
			if ((i + 1) < count) {

				return in[i + 1];
			} else {
				return no;
			}
		}
	}

	return no;
}

void edit_file_int(char *in_name, char *front, int line, int value)
{
	if (get_dump_status(dump_iodump) == TRUE)
		printf("edit_file_int %s %d %d\n", in_name, line, value);
	FILE *in;
	FILE *out;
	char buf[400];
	in = fopen(in_name, "r");
	if (in == NULL) {
		printf("edit_file_int: error opening file %s\n", in_name);
		exit(0);
	}
	out = fopen("temp.tmp", "w");
	if (out == NULL) {
		printf("edit_file_int: error opening file temp.tmp\n");
		exit(0);
	}
	int l = 0;

	do {
		l++;
		unused = fscanf(in, "%s", buf);
		if ((!feof(in))) {
			if (l != line) {
				fprintf(out, "%s\n", buf);
			} else {
				fprintf(out, "%s%d\n", front, value);
			}
		}
	} while (!feof(in));

	fclose(in);
	fclose(out);
	char command[1000];
	sprintf(command, "mv temp.tmp %s", in_name);
	unused = system(command);
	if (get_dump_status(dump_iodump) == TRUE)
		printf("Exit edit_file_int %s\n", in_name);
}

void edit_file(char *in_name, char *front, int line, double value)
{
	if (get_dump_status(dump_iodump) == TRUE)
		printf("edit_file %s %d %le\n", in_name, line, value);
	FILE *in;
	FILE *out;
	char buf[400];
	in = fopen(in_name, "r");
	if (in == NULL) {
		printf("edit_file_int: error opening file %s\n", in_name);
		exit(0);
	}
	out = fopen("temp.tmp", "w");
	if (out == NULL) {
		printf("edit_file_int: error opening file temp.tmp\n");
		exit(0);
	}
	int l = 0;

	do {

		l++;
		unused = fscanf(in, "%s", buf);
		if ((!feof(in))) {
			if (l != line) {
				fprintf(out, "%s\n", buf);
			} else {
				fprintf(out, "%s%le\n", front, value);
			}
		}
		if (l > 1000)
			exit(0);
	} while (!feof(in));

	if (get_dump_status(dump_iodump) == TRUE)
		printf("Exit edit_file_int loop %s\n", in_name);

	fclose(in);
	fclose(out);

	char command[1000];
	sprintf(command, "mv temp.tmp %s", in_name);
	unused = system(command);
	if (get_dump_status(dump_iodump) == TRUE)
		printf("Exit edit_file %s\n", in_name);
}

void copy_file(char *out_name, char *in_name, char *front, int line,
	       double value)
{
	FILE *in;
	FILE *out;
	char buf[400];
	in = fopen(in_name, "r");
	out = fopen(out_name, "w");
	int l = 0;

	do {
		l++;
		unused = fscanf(in, "%s", buf);
		if ((!feof(in))) {
			if (l != line) {
				fprintf(out, "%s\n", buf);
			} else {
				fprintf(out, "%s%le\n", front, value);
			}
		}
	} while (!feof(in));

	fclose(in);
	fclose(out);
}

void edit_file_by_var(char *in_name, char *token, char *newtext)
{
	FILE *in;
	FILE *out;
	char buf[400];
	in = fopen(in_name, "r");
	out = fopen("temp.tmp", "w");
	int l = 0;
	int found = FALSE;
	do {
		l++;
		unused = fscanf(in, "%s", buf);
		if ((!feof(in))) {
			if (strcmp(buf, token) != 0) {
				fprintf(out, "%s\n", buf);
			} else {
				fprintf(out, "%s\n", buf);
				fprintf(out, "%s\n", newtext);
				unused = fscanf(in, "%s", buf);
				found = TRUE;
			}
		}
	} while (!feof(in));

	fclose(in);
	fclose(out);
	if (found == FALSE) {
		printf("Token not found in file %s\n", token);
		exit(0);
	}
	char command[1000];
	sprintf(command, "mv temp.tmp %s", in_name);
	unused = system(command);
}
