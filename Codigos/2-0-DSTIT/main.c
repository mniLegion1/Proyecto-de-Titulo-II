#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>

#include "CDT2d.h"

/**
STIT Tessellation is simulated until time t = time_stop, 
restricted to a window W of side length a (integer greater than 0).
**/
/*
argc: amount of arguments.
argv[]: array with the parameters.
argv[0]: ./main
argv[1]: lifetime option (0: perimeter, 1: area)
argv[2]: STIT option (0: Iso, 1: Aniso, 2: AnisoDisturbed, 3: AnisoEllip)

All STIT:
	argv[3]: timeStop
	argv[4]: omega

STIT Anisotropic / Anisotropic Disturbed:
	argv[5]: nDir (number of directions)
	argv[6 + 2*i]: angle dir[i]
	argv[6 + 2*i + 1]: probability dir[i]
	for each i = 0,...,nDir - 1
	
Anisotropic Disturbed:
	argv[6 + 2*nDir]: ellipse half axis b

Anisotropic Elliptic:
	argv[5]: ellipse half axis b
*/
int main(int argc, char *argv[]){
	
	unsigned long lifeOption = atoi(argv[1]);	//0
	unsigned long option = atoi(argv[2]);		//0
	double timeStop = atof(argv[3]);			//200
	double omg = atof(argv[4]);					//0.3
	
	printf("%ld %ld %.3f %.3f ",lifeOption,option,timeStop,omg);

	CDT2d cdt;
	
	double *vertX, *vertY;
	vertX = Malloc(MAX_NUMBER_OF_DIR, double);
	vertY = Malloc(MAX_NUMBER_OF_DIR, double);

	EmptyCDT2d(&cdt,1);
	ReadFile(&cdt,vertX,vertY);

	// STIT Isotropic
	if (option == 0){
		STIT2dIso(&cdt, timeStop, lifeOption, omg);
	}	
	// STIT Anisotropic
	else if ((option == 1) || (option == 2)){
		unsigned long nDir = atoi(argv[5]);
		printf("%ld ",nDir);
		double *angleDir, *probDir;
		if (nDir > MAX_NUMBER_OF_DIR){
			fprintf(stderr,"The number of directions must be less or equal than %d.\n",MAX_NUMBER_OF_DIR);
			exit(EXIT_FAILURE);		
		}
		angleDir = Malloc(MAX_NUMBER_OF_DIR, double);
		probDir = Malloc(MAX_NUMBER_OF_DIR, double);
		int iDir = 0;
		while (iDir < nDir){
			angleDir[iDir] = atof(argv[6 + 2*iDir]);
			probDir[iDir] = atof(argv[6 + 2*iDir + 1]);
			printf("%.3f %.3f ",angleDir[iDir],probDir[iDir]);
			iDir++;
		}
		if (option == 1){
			STIT2dAniso(&cdt, timeStop, angleDir, probDir, nDir, lifeOption, omg);
		}
		else{
			double bEllip = atof(argv[6 + 2*nDir]);
			printf("%.3f ",bEllip);
			STIT2dAnisoDisturbed(&cdt, timeStop, angleDir, probDir, nDir, bEllip, lifeOption, omg);		
		}

		Free(angleDir);
		Free(probDir);
	}
	else{
		double bEllip = atof(argv[5]);
		printf("%.3f ",bEllip);
		STIT2dAnisoEllip(&cdt, timeStop, bEllip, lifeOption, omg);
	}
	printf("\n");
	//ImageCDT2d(&cdt, &image, "CDT2d");
	NoBoundary(&cdt);
	StatSTIT(&cdt);
	PlotCDT2d(&cdt);
	FreeCDT2d(&cdt);
	
	return 0;
}
