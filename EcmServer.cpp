/*
	Copyright 2014 Mark Briggs

	This file is part of ColorRing.

	ColorRing is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	ColorRing is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	There is a copy of the GNU General Public License in the
	accompanying COPYING file
*/

#include "EcmServer.h"

#define UDP_READ_BUFFER_SIZE 20

EcmServer::EcmServer(uint16_t port) : UDPServer(port) {
	isOutColorGrabbed = true;
	isInColorGrabbed = true;
}

bool EcmServer::handleNewColorPacket(PixelColor &newColor, bool isOutside) {
	// Check for New Color Packet from UDP port.
	// If none, return false.
	// If exists, set pixelColor & return true.
	
	if (available()) {
		// Read data
		char buffer[UDP_READ_BUFFER_SIZE];
		int n = readData(buffer, UDP_READ_BUFFER_SIZE);  // n contains # of bytes of buffer
		//Serial.print("n: "); Serial.println(n);
	
		if (n % 4 == 0) {  // ONLY expecting 4-byte packets
			byte r,g,b;
			bool currIsOutside;
			PixelColor currColor;
			for (int i = 0; i < n; ++i) {
				uint8_t v = buffer[i];
				//cout << "v: " << (int)v << endl;
				if (i % 4 == 0) {
					currIsOutside = ((v == 1) ? true : false);
					//cout << (int)currIsOutside << endl;
				} else if (i % 4 == 1) {
					r = v;
				} else if (i % 4 == 2) {
					g = v;
				} else {
					b = v;
				}	
		
				currColor.R = r;
				currColor.G = g;
				currColor.B = b;
				if (currIsOutside) {
					//cout << "OUTSIDE" << endl;
					_lastOutColor = currColor; //????? Do I need to override the "=" operator ???
					isOutColorGrabbed = false;
				} else {
					//cout << "INSIDE" << endl;
					_lastInColor = currColor; //????? Do I need to override the "=" operator ???
					isInColorGrabbed = false;
				}
			}
		}
	}
	
	if (!isOutside) {
		// Inside
		if (!isInColorGrabbed) {
			newColor = _lastInColor;
			isInColorGrabbed = true;
			//Serial.print("IN ");Serial.print(newColor.R);Serial.print(",");Serial.print(newColor.G);Serial.print(",");Serial.println(newColor.B);
			return true;
		}
	} else {
		// Outside
		if (!isOutColorGrabbed) {
			newColor = _lastOutColor;
			isOutColorGrabbed = true;
			//Serial.print("OUT ");Serial.print(newColor.R);Serial.print(",");Serial.print(newColor.G);Serial.print(",");Serial.println(newColor.B);
			return true;
		}
	}
	
	//if (isOutside) { Serial.print("OUT "); } else { Serial.print("IN "); }
	//Serial.print(r);Serial.print(",");Serial.print(g);Serial.print(",");Serial.print(b);Serial.print("  n:");Serial.println(n);
	//Serial.println("no new color packet.");
	
	return false;
}