#include <QtCore/QCoreApplication>
#include <math.h>
#include <QTime>
#include <cstdio>
#include <cstdlib>
#include "ripper.h"

// constructor?
Ripper::Ripper()
{
}


/*

	this is like the old int main() :)

*/
void Ripper::RunMe(int argc, char *argv[])
{
	// just get the time
	QTime t;
	t.start();

	printf("at ripper::runme!\n");

	Open_Files(argc, argv);

	Read_WRP_Info(map);

	Read_Elevations(map, output_xyz, MapSize);

	Read_Texture_Indexes(map, output_xyz, MapSize);

	Read_Texture_Names(map, output);

	Read_Objects(map, output);

	Close_Files(map, output, t);
}


// gets the scale of the object
float Ripper::GetScale()
{
	return sqrt(R11 * R11 + R21 * R21 + R31 * R31);
}


// returns the objects rotation / direction
float Ripper::GetDirection()
{
	float Angle;
	float Scale = GetScale();

	if (fabs(R11/Scale) > 1)
	{
		if (fabs(R31/Scale) > 1)
		return -1;
		Angle = asin(R31/Scale)/M_PI * 180.;
		if (R11/Scale>=0 && R31/Scale < 0) //IV
		Angle += 360;
		else
		if (R11/Scale <0 && R31/Scale < 0) //III
		Angle = 180 - Angle;
		else //II
		Angle = 270 + Angle;
	}
	else
	{
		Angle = acos(R11/Scale)/M_PI * 180.;
		if (R31/Scale < 0)
		Angle = 360 - Angle;
	}
	return Angle;
}


// check parameters and if ok... open files.
void Ripper::Open_Files(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Incorrect parameters supplied.\nUsage:\n\nWRP_Ripper SOURCE.wrp DESTINATION_objects.txt DESTINATION_terrain.xyz\n");
		exit(1);
	}

	map = fopen (argv[1], "rb");
	if (!map)
	{
		printf ("error in %s\n", argv[1]);
		exit (1);
	}

	printf ("Opened %s\n", argv[1]);

	output = fopen (argv[2], "wt");
	if (!output)
	{
		printf ("error at %s file\n", argv[2]);
		exit (1);
	}

	output_xyz = fopen (argv[3], "wt");
	if (!output_xyz)
	{
		printf ("error at %s file\n", argv[3]);
		exit (1);
	}

	printf ("Opened %s\n", argv[2]);
}


// read signature and map size, then check they are ok.
void Ripper::Read_WRP_Info(FILE *map)
{
	fread(sig, 4, 1, map);
	sig[4] = 0;
	
	// if its not correct file format, exit.
	if (strcmp (sig, "4WVR") != 0)
	{
		printf("This is not 4WVR format WRP, exiting...\n");
		exit(1);
	}
	fread(&MapSize, 4, 1, map);
	fread(&MapSize, 4, 1, map);

	if (MapSize > 4096)
	{
		printf("MapSize %i is too large! exiting!\n", MapSize);
		exit(1);
	};

	printf("Signature: %s\nMapSize: %i\nReading elevations...", sig, MapSize);
}


// read elevations
void Ripper::Read_Elevations(FILE *map, FILE *output_xyz, int MapSize)
{
	short Elevation = 0;
	int x = 0, z = 0;
	for (int zx = 0; zx < MapSize * MapSize; zx++)
	{
		fread(&Elevation,sizeof(Elevation),1,map);
		// earlier this was just Elevation / 22
		// was %i %i %d before
		fprintf(output_xyz, "%i %i %f\n", x, z, Elevation / 22.222);

		x++;
		if (x == MapSize)
		{
			z++;
			x = 0;
		}
		if (z == MapSize)
		{
			z = 0;
		}
	}

	printf(" Done\nReading Textures...");
}


// read textures IDs
void Ripper::Read_Texture_Indexes(FILE *map, FILE *output_xyz, int MapSize)
{
	short TexIndex = 0;
	int x = 0, z = 0;

	for (int tx = 0; tx < MapSize * MapSize; tx++)
	{
		fread(&TexIndex,sizeof(TexIndex),1,map);

		x++;
		if (x == MapSize)
		{
			z++;
			x = 0;
		}
		if (z == MapSize)
		{
			z = 0;
		}
	}

	printf(" Done\nReading Texture names...");
}


// textures 32 char length and total of 512
void Ripper::Read_Texture_Names(FILE *map, FILE *output_xyz)
{
	for (int ix = 0; ix < 512; ix++)
	{
		sig[0] = 0;
		fread(sig, 32, 1, map);
	}

	printf(" Done\nReading and writing 3dObjects...");
}


// read objects and write objects file.
void Ripper::Read_Objects(FILE *map, FILE *output)
{
	while (!feof(map))
	{
		// R11
		fread(&R11,4,1,map);
		// R12
		fread(&R12,4,1,map);
		// R13
		fread(&R13,4,1,map);
		// R21
		fread(&R21,4,1,map);
		// R22
		fread(&R22,4,1,map);
		// R23
		fread(&R23,4,1,map);
		// R31
		fread(&R31,4,1,map);
		// R32
		fread(&R32,4,1,map);
		// R33
		fread(&R33,4,1,map);

		// X coord
		fread(&dDirX,4,1,map);

		// elevation
		fread(&dElevation,4,1,map);

		// Z coord
		fread(&dDirZ,4,1,map);

		// calculate the elevation
		//dElevation = GetElevationByPoint(dDirX, dDirZ);

		dObjIndex++;
		fread(&dObjIndex,4,1,map);

		// clear the objname variable
		for (int i = 0; i < 76; i++)
		dObjName[i] = 0;
		fread(dObjName,76,1,map);

		// function to calculate rotation
		dRotation = GetDirection();

		char tmpName[76];
		strcpy (tmpName,"");
		strncat (tmpName, dObjName, strlen(dObjName) - 4);

		if (strlen (dObjName) > 1)
		{
			fprintf(output,"\"%s\";%f;%f;%f;%f;\n", tmpName, dDirX, dDirZ, dElevation, dRotation);
		}
	};

	printf(" Done, %ld Object Indexes read\n",dObjIndex);
}


// close all open files, display time used.
void Ripper::Close_Files(FILE *map, FILE *output, QTime t)
{
	fclose(map);
	fclose(output);
	fclose(output_xyz);

	qDebug("Time elapsed: %d ms", t.elapsed());
}
