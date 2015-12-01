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
#include <signal.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include "cal_path.h"
#include "util.h"

#ifdef windows
#include <windows.h>
#endif

void get_light_lib_path(char *out)
{
#ifdef windows
	DWORD dwType = REG_SZ;
	HKEY hKey = 0;
	char value[1024];
	DWORD value_length = 1024;
	const char *subkey = "SOFTWARE\\opvdm";
	if (RegOpenKey(HKEY_CURRENT_USER, subkey, &hKey) == ERROR_SUCCESS) {
		RegQueryValueEx(hKey, "installpath", NULL, &dwType,
				(LPBYTE) & value, &value_length);
		RegCloseKey(hKey);
		join_path(2, out, value, "light");
		printf("Key in registry: %s\n", out);
	} else {
		strcpy(out, "c:\\opvdm\\light");
		printf("No key found in registry: %s\n", out);

	}

#else
	strcpy(out, "/usr/lib64/opvdm/");
#endif
}
