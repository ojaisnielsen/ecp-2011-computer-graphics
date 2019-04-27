/*! \file */
#pragma once

#include <windows.h> 
#include <math.h>

#define packXY(x, y, w) ((x) + (y) * (w))
#define xFromXY(xy, w) ((xy) %  (w))
#define yFromXY(xy, w) ((xy) /  (w))
#define packXYZ(x, y, z, w, h) ((x) + (y) * (w) + (z) * (w) * (h))
#define xFromXYZ(xyz, w, h) ((xyz) % (w))
#define yFromXYZ(xyz, w, h) (((xyz) / (w)) % (h))
#define zFromXYZ(xyz, w, h) ((xyz) / ((w) * (h)))
