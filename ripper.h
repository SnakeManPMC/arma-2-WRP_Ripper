#ifndef PMC_RIPPER_H
#define PMC_RIPPER_H

#include <QtCore/QCoreApplication>
#include <math.h>
#include <QTime>
#include <cstdio>
#include <cstdlib>

class Ripper
{
public:
	// constructor
	Ripper();

	// method
	void RunMe(int argc, char *argv[]);

private:

	// lets see if these work in private ;)

	FILE *map;
	FILE *output;
	FILE *output_xyz;

	char sig[33];
	int MapSize, CellSize;
	char dObjName[76];
	float dDirX, dElevation, dDirZ, dRotation;
	long dObjIndex;
	// doesnt work here?
	//const float ElevFactor = 9./200.;

	// roation matrix storing
	float R11, R12, R13, R21, R22, R23, R31, R32, R33;

	// methods
	float GetScale();
	float GetDirection();
	void Open_Files(int argc, char *argv[]);
	void Read_WRP_Info(FILE *map);
	void Read_Elevations(FILE *map, FILE *output_xyz, int MapSize);
	void Read_Texture_Indexes(FILE *map, FILE *output_xyz, int MapSize);
	void Read_Texture_Names(FILE *map, FILE *output_xyz);
	void Read_Objects(FILE *map, FILE *output);
	void Close_Files(FILE *map, FILE *output, QTime t);
};

#endif
