#ifndef BIN2ASCII
#define BIN2ASCII

#include <iostream>
#include <gdal.h>
#include <gdal_priv.h>
#include <fstream>
#include <iomanip>

using namespace std;


//double readValueB2A ( void *data, GDALDataType type, int index );

void bin2ascii(char *binFile, char *asciiFile);

#endif
