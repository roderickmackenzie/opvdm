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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "gui_hooks.h"
#include "util.h"

#ifdef dbus
#include <dbus/dbus.h>
#endif

#ifdef windows
#include <windows.h>
#endif

int gui_send_data(char *tx_data_in)
{
	char tx_data[1024];
	char temp[1024];
	string_to_hex(temp, tx_data_in);
	sprintf(tx_data, "hex%s", temp);

#ifdef dbus
	DBusConnection *connection;
	DBusError error;
	dbus_error_init(&error);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);

	if (!connection) {
		printf("Failed to connect to the D-BUS daemon: %s",
		       error.message);
		dbus_error_free(&error);
		return 1;
	}

	DBusMessage *message;
	message =
	    dbus_message_new_signal("/org/my/test", "org.my.opvdm", tx_data);
	/* Send the signal */
	dbus_connection_send(connection, message, NULL);
	dbus_connection_flush(connection);
	dbus_message_unref(message);

#endif

#ifdef windows

	HANDLE pipe =
	    CreateFile("\\\\.\\pipe\\opvdm_pipe", GENERIC_WRITE,
		       FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		       FILE_ATTRIBUTE_NORMAL, NULL);

	if (pipe == INVALID_HANDLE_VALUE) {
		printf("Failed to connect to pipe.");
		return 1;
	}

	DWORD numBytesWritten = 0;

	WriteFile(pipe, (const wchar_t *)tx_data,
		  strlen(tx_data) * sizeof(char), &numBytesWritten, NULL);

	CloseHandle(pipe);

#endif
	return 0;
}

int dbus_init()
{
#ifdef dbus

#endif
	return 0;
}

void gui_start()
{
	gui_send_data("start");
}
