#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "CDT2d.h"

/**
	@function: EmptyCDT2d
	---------------------
**/
int EmptyCDT2d(CDT2d *cdt, unsigned long size){

	cdt->size = size;
	
	cdt->nSize = Malloc(2, unsigned long);
	cdt->nSize[0] = 1024; cdt->nSize[1] = 1024;
	
	cdt->Polygon = Malloc(MAX_NUMBER_OF_POLYGONS, POLYGON);
	cdt->I_Segment = Malloc(MAX_NUMBER_OF_I_SEGMENTS, I_SEGMENT);
	
	cdt->numberOfPolygons = 0;
	cdt->numberOfI_Segments = 0;
	
	cdt->Phi = Calloc(NUMBER_OF_INT, double);
	cdt->PhiCum = Malloc(NUMBER_OF_INT, double);
	
	return 0;
}

/** 
	@function: NumberSegmentsCDT2d
	------------------------------
**/
void NumberSegmentsCDT2d(CDT2d *cdt, unsigned long iCelda, unsigned long nVertices, double *vertX, double *vertY){
	unsigned long i,j; 	//Seg,Vert
	double restX,restY;
	double *segX, *segY;

	segX = Malloc(3, double);
	segY = Malloc(3, double);

	//2do...n poligonos
	
	if(iCelda != 0){
		for(i = 0; i < nVertices; i++){		//Vertices index
			j = 0;	//I_Segment index
			
			while(1){
				segX[0] = cdt->I_Segment[j].x0; segY[0] = cdt->I_Segment[j].y0;
				segX[1] = cdt->I_Segment[j].x1; segY[1] = cdt->I_Segment[j].y1;

				if(segX[0] != 0.000000 && segY[0] != 0.000000 && segX[1] != 0.000000 && segY[1] != 0.000000){
					if((vertX[i] == segX[1] && vertY[i] == segY[1]) && (vertX[(i+1)%nVertices] == segX[0] && vertY[(i+1)%nVertices] == segY[0])){
						break;
					}
					else if((vertX[i] == segX[0] && vertY[i] == segY[0]) && (vertX[(i+1)%nVertices] == segX[1] && vertY[(i+1)%nVertices] == segY[1])){
						break;
					}
					else{
						j++;
					}
				}

				else{
					cdt->I_Segment[j].x0 = vertX[i], cdt->I_Segment[j].y0 = vertY[i];
					cdt->I_Segment[j].x1 = vertX[(i+1)%nVertices], cdt->I_Segment[j].y1 = vertY[(i+1)%nVertices];

					restX = cdt->I_Segment[j].x1 - cdt->I_Segment[j].x0;
					restY = cdt->I_Segment[j].y1 - cdt->I_Segment[j].y0;

					cdt->I_Segment[j].beta = 0.0;
					cdt->I_Segment[j].length = sqrt((restX*restX) + (restY*restY));

					cdt->numberOfI_Segments++;

					cdt->I_Segment[j+1].x0 = 0, cdt->I_Segment[j+1].y0 = 0;
					cdt->I_Segment[j+1].x1 = 0, cdt->I_Segment[j+1].y1 = 0;

					break;
				}
			}
		}
	}
	
	//1er poligono
	else{
		for(i = 0; i < nVertices; i++){
			cdt->I_Segment[i].x0 = vertX[i], cdt->I_Segment[i].y0 = vertY[i];
			cdt->I_Segment[i].x1 = vertX[(i+1)%nVertices], cdt->I_Segment[i].y1 = vertY[(i+1)%nVertices];
			
			restX = cdt->I_Segment[i].x1 - cdt->I_Segment[i].x0;
			restY = cdt->I_Segment[i].y1 - cdt->I_Segment[i].y0;

			cdt->I_Segment[i].beta = 0.0;
			cdt->I_Segment[i].length = sqrt((restX*restX) + (restY*restY));
			cdt->numberOfI_Segments++;
		}
		cdt->I_Segment[i].x0 = 0, cdt->I_Segment[i].y0 = 0;
		cdt->I_Segment[i].x1 = 0, cdt->I_Segment[i].y1 = 0;
	}
}


/** 
 	@function: ReadFile
	-------------------
**/
void ReadFile(CDT2d *cdt, double *vertX, double *vertY){
	FILE *ptr_file;
	char buffer[500], vert[10];
	int i, j, k;
	unsigned long iCelda,nVertices, nCeldas;

	ptr_file = fopen("Vertices.txt","r");
	fgets(buffer, 500, ptr_file);
	nCeldas = atoi(buffer);
	cdt->numberOfPolygons = nCeldas;

	for(iCelda = 0; iCelda < nCeldas; iCelda++){
		fgets(buffer, 500, ptr_file);
		vert[0] = buffer[0];
		nVertices = atoi(vert);

		i = 0; j = 2; 		
		while(1){
			k = 0;
			while(buffer[j] != ' '){
				vert[k] = buffer[j];
				j++; k++;
			}
			vertX[i] = atof(vert);

			j++; k = 0;
			while(buffer[j] != ' '){
				vert[k] = buffer[j];
				j++; k++;
			}
			vertY[i] = atof(vert);

			if(buffer[j+2] == '\0'){

				break;
			}
			else{
				i++; j++;
			}
				
		}
		NumberSegmentsCDT2d(cdt,iCelda,nVertices,vertX,vertY);
		InitCDT2d(cdt,iCelda,nVertices,vertX,vertY);
	}	

	fclose(ptr_file);
}

/** 
	@function: InitCDT2d
	--------------------
**/
void InitCDT2d(CDT2d *cdt, unsigned long iCelda, unsigned long nVertices, double *vertX, double *vertY){
	unsigned long i;
	
	// random seed is initialized.
 	srandom(time(NULL));
 	
	cdt->Polygon[iCelda].numberOfVertices = nVertices;
	cdt->Polygon[iCelda].V = (double **)calloc2d(nVertices, 2, sizeof(double));

	for (i = 0; i < nVertices; i++){
		cdt->Polygon[iCelda].V[i][0] = vertX[i]; cdt->Polygon[iCelda].V[i][1] = vertY[i];
	}
	
	WidthFunction(cdt, (cdt->Polygon) + iCelda);
	PerimeterPolygon((cdt->Polygon) + iCelda);
	AreaPolygon((cdt->Polygon) + iCelda);
	RoundnessPolygon((cdt->Polygon) + iCelda);
}

/**
	@function: FreeCDT2d
	--------------------
**/
int FreeCDT2d(CDT2d *cdt){
	unsigned long i;
	
	Free(cdt->nSize);
	
	for(i = 0; i < cdt->numberOfPolygons; i++){
		Free2d(((cdt->Polygon) + i)->V);
		Free2d(((cdt->Polygon) + i)->H);
		Free(((cdt->Polygon) + i)->B);
	}
	Free(cdt->Polygon);
	
	Free(cdt->I_Segment);
	Free(cdt->Phi);
	Free(cdt->PhiCum);
	
	for(i = 0; i < cdt->numberOfGaussPolygons; i++){
		Free2d(((cdt->GaussPolygon) + i)->V);
		Free2d(((cdt->GaussPolygon) + i)->H);
		Free(((cdt->GaussPolygon) + i)->B);
	}
	Free(cdt->GaussPolygon);
	
	//Free2d(cdt->LinCD);
	//Free(cdt->SphereCD);
	
	return 0;
}

/**
	@function SetPhiIso
	-------------------
	Sets the phi distribution function and phi cumulative 
	distribution function for isotropic STIT.
**/
int SetPhiIso(CDT2d *cdt){
	unsigned long j;
	
	for(j = 0; j < NUMBER_OF_INT; j++){
		cdt->Phi[j] = 1.0 / NUMBER_OF_INT;
		cdt->PhiCum[j] = (double)(j + 1) / NUMBER_OF_INT;		
	}
	
	return 0;
}

/**
	@function SetPhiAniso
	-------------------
	Sets the phi distribution function and phi cumulative 
	distribution function for anisotropic STIT.
**/
int SetPhiAniso(CDT2d *cdt, double *angleDir, double *probDir, unsigned long nDir){
	unsigned long j, iDir;
	double sumProb = 0.;
	
	for(iDir = 0; iDir < nDir; iDir++){
		// checking if the angle is negative or greater than 1.
		if  ((angleDir[iDir] < 0.0) || (angleDir[iDir] >= 1.0)){
			fprintf(stderr,"The angle of the normal direction must be in the interval [0,pi[\n");
			exit(EXIT_FAILURE);
		}
		// checking if the probability is less or equal than 0 or greater than 1.
		if  ((probDir[iDir] <= 0.0) || (probDir[iDir] >= 1.0)){
			fprintf(stderr,"The probability of the normal direction must be in the interval ]0,1[\n");
			exit(EXIT_FAILURE);
		}
	}
	#define EPS 1.0E-16
	// checking if the sum of the probabilities is 1.
	for(iDir = 0; iDir < nDir; iDir++)
		sumProb += probDir[iDir];
	if (fabs(1 - sumProb) > EPS){
		fprintf(stderr,"The sum of probabilities is not 1.\n");
		exit(EXIT_FAILURE);
	}
	#undef EPS
	
	//computing phi discrete
	for(iDir = 0; iDir < nDir; iDir++){
		// the index j of the normal direction is computed ==> j = floor(angle*NUM_OF_INT) 
		j = floor(angleDir[iDir]*NUMBER_OF_INT);
		//the probability in the position j is updated.
		cdt->Phi[j] += probDir[iDir];
	}
	/* 
	the cummulative distribution function is computed, where 
	cumm_phi[0] = phi[0], and
	cumm_phi[j] = cumm_phi[j - 1] + phi[j] for all j = 1,...,N_OF_INT 
	*/
	cdt->PhiCum[0] = cdt->Phi[0];
	for(j = 1; j < NUMBER_OF_INT; j++){
		cdt->PhiCum[j] = cdt->PhiCum[j - 1] + cdt->Phi[j];
	}
	
	return 0;
}

/**
	@function SetPhiAnisoEllip
	-------------------
	Sets the phi distribution function and phi cumulative 
	distribution function for anisotropic elliptic STIT.
**/
int SetPhiAnisoEllip(CDT2d *cdt, double bEllip){
	unsigned long j;
	int N2;
	double angle, area;
	
	N2 = NUMBER_OF_INT / 2;
	for(j = 0; j < N2 - 1; j++){
		// angle for the index j.
		angle = (j + 1) * M_PI / NUMBER_OF_INT;
		/* computing the area of the ellipse sector [0,angle_j] 
		   divided by the half area of the ellipse (cummulative distribution function).*/
		area = (1.0 / M_PI) * atan(tan(angle) / bEllip);
		/* assign to position j of the discretisation.*/
		cdt->PhiCum[j] = area;
	}
	
	cdt->PhiCum[N2 - 1] = 0.5;
	for(j = 0; j < N2 - 1; j++){
		/* assign to position j of the discretisation. */
		cdt->PhiCum[N2 + j] = 1.0 - cdt->PhiCum[N2 - j - 2];
	}
	cdt->PhiCum[NUMBER_OF_INT - 1] = 1.0;
	
	cdt->Phi[0] = cdt->PhiCum[0];
	for(j = 1; j < NUMBER_OF_INT; j++){
		cdt->Phi[j] = cdt->PhiCum[j] - cdt->PhiCum[j - 1];
	}
	
	return 0;
}

/**
	@function: InitTime
	-------------------
**/
void InitTime(CDT2d *cdt, unsigned long lOption){
	unsigned long i,j;
	double bPhi = 0.0;
	// The lifetime of the polygon is determined. The lifetime
	// is exponentially distribuited with parameter bPhi, which is
	// the weighted sum of the width and the weights are the probabilities
	// Phi[j].
	
	
	for(i = 0; i < cdt->numberOfPolygons; i++){
		// Perimeter
		if(lOption == 0){
		for(j = 0; j < NUMBER_OF_INT; j++){
			bPhi += cdt->Polygon[i].B[j]*cdt->Phi[j];
		}
		cdt->Polygon[i].tau = RandomExponential(bPhi);
		}
		// Area
		else{
			cdt->Polygon[i].tau = RandomExponential(cdt->Polygon[i].area);
		}
		cdt->Polygon[i].beta = 0.0;
		cdt->Polygon[i].time = cdt->Polygon[i].beta + cdt->Polygon[i].tau;
	}
}

/**
	@function RandomDirection
	-------------------------
	Computes a random direction with the distribution phi_discrete.
	
	[IN]
	@param cdt: CDT2d.
	@param randNum: random number uniformly distributed.
	
	[OUT]
	@param j: index of the normal direction.
**/
int RandomDirection(CDT2d *cdt, double randNum){
	unsigned long j;
	
	for(j = 0; j < NUMBER_OF_INT; j++){
		if (randNum < cdt->PhiCum[j])
			break;
	}
	return j;
}

/**
	@function RandomLinePolygon
	------------------------------
	Isotropic, Anisotropic discrete
**/
LINE RandomLinePolygon(CDT2d *cdt, unsigned long i){
	unsigned long j;
	double d, randNum;
	LINE dirLine;
	
	do{
		randNum = RandomUniform();
		// random direction index.
		j = RandomDirection(cdt, randNum);
		// printf("j: %ld\n",j);
		// the signed distance dist is computed.
		d = cdt->Polygon[i].maxWidth * RandomUniform() + cdt->Polygon[i].H[j][0];
	// checking if the line is accepted.		
	}while(d > cdt->Polygon[i].H[j][1]); 

	// if the line is accepted, angle alpha is computed.
	dirLine.alpha = M_PI * (double)j / (double)NUMBER_OF_INT;
	dirLine.dist = d;
	dirLine.index = j;

	return dirLine;
}

/**
	@function RandomLinePolygonDisturbed
	------------------------------
	Anisotropic discrete disturbed, Anisotropic Ellipse
**/
LINE RandomLinePolygonDisturbed(CDT2d *cdt, unsigned long i, double bEllip){
	unsigned long j, jGamma;
	double d, randNum, gamma;
	LINE dirLine;
	
	do{
		randNum = RandomUniform();
		// random direction index.
		j = RandomDirection(cdt, randNum);
		// printf("j: %ld\n",j);
		gamma = RandomElliptic(bEllip);
		jGamma = floor(gamma * NUMBER_OF_INT / M_PI);
		j = (j + jGamma) % (NUMBER_OF_INT);
		// the signed distance dist is computed.
		d = cdt->Polygon[i].maxWidth * RandomUniform() + cdt->Polygon[i].H[j][0];
	// checking if the line is accepted.		
	}while(d > cdt->Polygon[i].H[j][1]); 

	// if the line is accepted, angle alpha is computed.
	dirLine.alpha = M_PI * (double)j / (double)NUMBER_OF_INT;
	dirLine.dist = d;
	dirLine.index = j;

	return dirLine;
}

/**
**/
#define EPS 1.0E-32
void RandomIntersectionPolygon(CDT2d *cdt, unsigned long i, unsigned long ip, LINE dirLine, unsigned long lOption, double omg){
	unsigned long j, k, k1, k2;
	// k0[1] - k0[0] is the number of vertices between the first and the second intersection point.
	unsigned long k0[2]; 
	// ind means index of intersection points (0 and 1).
	unsigned long ind = 0;
	double cosAlpha, det, deltaX, deltaY, sinAlpha, t, alpha, d, bPhi;
	double newVertex[2][2];
	double A[2], B[2], C[2], magA, magB, magC, cosAB, cosCD, omg1, omg2;

	// auxiliary variable for checking if the angle of the intersection between I_Segment and
	// the side of the polygon where the intersection point lies, is not close to zero. 
	// DETcheck = 1 (angle is ok) and DETcheck = 0 (angle is close to zero or equal to zero for
	// all the side of the polygons where an intersection point could be). Then, if DETcheck = 0
	// a new random line must be generated.
	int DETcheck = 1;

	POLYGON P;
  
	// copy of Polygon in to polygon P.
	P.numberOfVertices = cdt->Polygon[i].numberOfVertices;
	P.V = (double **)calloc2d(P.numberOfVertices, 2, sizeof(double));
	for(k = 0; k < P.numberOfVertices; k++) {
		P.V[k][0] = cdt->Polygon[i].V[k][0];
		P.V[k][1] = cdt->Polygon[i].V[k][1];
	}
	
	P.time = cdt->Polygon[i].time;
	
	alpha = dirLine.alpha;
	d = dirLine.dist;
	
	// computing omega as factor*PI
	omg = M_PI*omg;
  
	do{
		if (DETcheck == 0){	
			dirLine = RandomLinePolygon(cdt, i);
			alpha = dirLine.alpha;
			d = dirLine.dist;
			ind = 0;
			DETcheck = 1;
		}
		cosAlpha = cos(alpha);
		sinAlpha = sin(alpha);
	
		for(k = 0; k < P.numberOfVertices; k++) {
			k1 = (k + 1) % P.numberOfVertices;
			deltaX = P.V[k1][0] - P.V[k][0];
			deltaY = P.V[k1][1] - P.V[k][1];
			det = deltaX * cosAlpha + deltaY * sinAlpha;
			if(fabs(det) > EPS) {
				// the segment between vertices k and (k + 1) is parametrized 
				// with parameter t in [0,1]. The parameter value of the 
				// intersection point is computed.
				t = (d - P.V[k][0] * cosAlpha - P.V[k][1] * sinAlpha) / det;
			  	if(t >= 0.0 && t < 1.0){
					newVertex[ind][0] = P.V[k][0] + t * deltaX;
					newVertex[ind][1] = P.V[k][1] + t * deltaY;
					k0[ind] = k;
					ind++; 
				}				
			}
		}
		if (ind < 2)
			// the line is rejected, because there are less than 2 intersection points. 
			DETcheck = 0;
		// checking small angle 
		else{
			A[0] = P.V[k0[0]][0] - newVertex[0][0];
			A[1] = P.V[k0[0]][1] - newVertex[0][1];
			B[0] = newVertex[1][0] - newVertex[0][0];
			B[1] = newVertex[1][1] - newVertex[0][1];
			C[0] = P.V[k0[1]][0] - newVertex[1][0];
			C[1] = P.V[k0[1]][1] - newVertex[1][1];
			magA = sqrt(A[0]*A[0] + A[1]*A[1]);
			magB = sqrt(B[0]*B[0] + B[1]*B[1]);
			magC = sqrt(C[0]*C[0] + C[1]*C[1]);
			cosAB = (A[0]*B[0] + A[1]*B[1]) / (magA*magB);
			cosCD = -(B[0]*C[0] + B[1]*C[1]) / (magB*magC);
			// computing angle between A and B vector
			omg1 = acos(cosAB);
			// computing angle between C and D vector
			omg2 = acos(cosCD);
			//printf("OMG: %.9f OMG1: %.9f OMG2: %.9f\n",omg,omg1,omg2);
			// checking if omg1 < omg and PI - omg1 < omg
			if ((omg1 < omg) || (M_PI - omg1) < omg){
				DETcheck = 0;
			}
			// checking if omg2 < omg and PI - omg2 < omg
			if ((omg2 < omg) || (M_PI - omg2) < omg){
				DETcheck = 0;
			}
		}
			
		if (DETcheck == 1) break;
		
	}while(1);

	Free2d(cdt->Polygon[i].V);
	Free2d(cdt->Polygon[i].H);
	Free(cdt->Polygon[i].B);

	// vertices of the new two polygons are determined.
	cdt->Polygon[i].V = (double **)calloc2d(k0[1] - k0[0] + 2, 2, sizeof(double));
	cdt->Polygon[i].V[0][0] = newVertex[1][0];
	cdt->Polygon[i].V[0][1] = newVertex[1][1];
	cdt->Polygon[i].V[1][0] = newVertex[0][0];
	cdt->Polygon[i].V[1][1] = newVertex[0][1];
	cdt->Polygon[i].numberOfVertices = 2;

	cdt->Polygon[ip].V = (double **)calloc2d(P.numberOfVertices - (k0[1] - k0[0]) + 2, 2, sizeof(double));
	cdt->Polygon[ip].V[0][0] = newVertex[0][0];
	cdt->Polygon[ip].V[0][1] = newVertex[0][1];
	cdt->Polygon[ip].V[1][0] = newVertex[1][0];
	cdt->Polygon[ip].V[1][1] = newVertex[1][1];
	cdt->Polygon[ip].numberOfVertices = 2;
  
	for(k = 0; k < P.numberOfVertices; k++) {
		k1 = (k + k0[0] + 1) % P.numberOfVertices;
		k2 = (k + k0[1] + 1) % P.numberOfVertices;
		if(cdt->Polygon[i].numberOfVertices < k0[1] - k0[0] + 2){
			cdt->Polygon[i].V[cdt->Polygon[i].numberOfVertices][0] = P.V[k1][0];
			cdt->Polygon[i].V[cdt->Polygon[i].numberOfVertices][1] = P.V[k1][1];
			cdt->Polygon[i].numberOfVertices++;
		}
		if(cdt->Polygon[ip].numberOfVertices < P.numberOfVertices - (k0[1] - k0[0]) + 2){
			cdt->Polygon[ip].V[cdt->Polygon[ip].numberOfVertices][0] = P.V[k2][0];
			cdt->Polygon[ip].V[cdt->Polygon[ip].numberOfVertices][1] = P.V[k2][1];
			cdt->Polygon[ip].numberOfVertices++; 
		}
	}
  
	// The width function of Polygon i is computed.
	WidthFunction(cdt, (cdt->Polygon) + i);
	PerimeterPolygon((cdt->Polygon) + i);
	AreaPolygon((cdt->Polygon) + i);
	RoundnessPolygon((cdt->Polygon) + i);
    
    // The width function of Polygon ip is computed.
	WidthFunction(cdt, (cdt->Polygon) + ip);
	PerimeterPolygon((cdt->Polygon) + ip);
	AreaPolygon((cdt->Polygon) + ip);
	RoundnessPolygon((cdt->Polygon) + ip);
	
	// The lifetime of the polygon is determined. The lifetime
	// is exponentially distribuited with parameter bPhi, which is
	// the weighted sum of the width and the weights are the probabilities
	// Phi[j].
	
	// Lifetime of Polygon i.
	// Perimeter
	if(lOption == 0){
		bPhi = 0.;
		for(j = 0; j < NUMBER_OF_INT; j++){
			bPhi += (cdt->Polygon[i].B[j]) * (cdt->Phi[j]);
		}
		cdt->Polygon[i].tau = RandomExponential(bPhi);
	}
	// Area
	else{
		cdt->Polygon[i].tau = RandomExponential(cdt->Polygon[i].area);
	}
	cdt->Polygon[i].beta = P.time;
	cdt->Polygon[i].time = cdt->Polygon[i].beta + cdt->Polygon[i].tau;
	
	// Lifetime of Polygon ip.
	// Perimeter
	if(lOption == 0){
		bPhi = 0.;
		for(j = 0; j < NUMBER_OF_INT; j++){
			bPhi += (cdt->Polygon[ip].B[j]) * (cdt->Phi[j]);
		}
	
		cdt->Polygon[ip].tau = RandomExponential(bPhi);
	}
	// Area
	else{
		cdt->Polygon[ip].tau = RandomExponential(cdt->Polygon[ip].area);
	}
	cdt->Polygon[ip].beta = P.time;
	cdt->Polygon[ip].time = cdt->Polygon[ip].beta + cdt->Polygon[ip].tau;
	
	// The two vertices of the I_Segment are assgined.
	cdt->I_Segment[cdt->numberOfI_Segments].x0 = newVertex[0][0];
	cdt->I_Segment[cdt->numberOfI_Segments].y0 = newVertex[0][1]; 
	cdt->I_Segment[cdt->numberOfI_Segments].x1 = newVertex[1][0]; 
	cdt->I_Segment[cdt->numberOfI_Segments].y1 = newVertex[1][1];
	// The length of the I_Segment is computed.
	deltaX = newVertex[1][0] - newVertex[0][0];
	deltaY = newVertex[1][1] - newVertex[0][1];
	cdt->I_Segment[cdt->numberOfI_Segments].length = sqrt(deltaX*deltaX + deltaY*deltaY);
	// The birthtime of the I_Segment is assigned.
	cdt->I_Segment[cdt->numberOfI_Segments].beta = P.time;

	Free2d(P.V);
}
#undef EPS

/*
*/
#define EPS 1.0E-32
void RandomIntersectionPolygonDisturbed(CDT2d *cdt,unsigned long i,unsigned long ip, LINE dirLine, double bEllip, unsigned long lOption, double omg){
	unsigned long j, k, k1, k2;
	// k0[1] - k0[0] is the number of vertices between the first and the second intersection point., 
	unsigned long k0[2]; 
	// ind means index of intersection points (0 and 1).
	unsigned long ind = 0;
	double cosAlpha, det, deltaX, deltaY, sinAlpha, t, alpha, d, bPhi;
	double newVertex[2][2];
	double A[2], B[2], C[2], magA, magB, magC, cosAB, cosCD, omg1, omg2;

	// auxiliary variable for checking if the angle of the intersection between I_Segment and
	// the side of the polygon where the intersection point lies, is not close to zero. 
	// DETcheck = 1 (angle is ok) and DETcheck = 0 (angle is close to zero or equal to zero for
	// all the side of the polygons where an intersection point could be). Then, if DETcheck = 0
	// a new random line must be generated.
	int DETcheck = 1;

	POLYGON P;
  
	// copy of Polygon in to polygon P.
	P.numberOfVertices = cdt->Polygon[i].numberOfVertices;
	P.V = (double **)calloc2d(P.numberOfVertices, 2, sizeof(double));
	for(k = 0; k < P.numberOfVertices; k++) {
		P.V[k][0] = cdt->Polygon[i].V[k][0];
		P.V[k][1] = cdt->Polygon[i].V[k][1];
	}
	
	P.time = cdt->Polygon[i].time;
	
	alpha = dirLine.alpha;
	d = dirLine.dist;
	
	// computing omega as factor*PI
	omg = M_PI*omg;
  
	do{
		if (DETcheck == 0){	
			dirLine = RandomLinePolygonDisturbed(cdt, i, bEllip);
			alpha = dirLine.alpha;
			d = dirLine.dist;
			ind = 0;
			DETcheck = 1;
		}
		cosAlpha = cos(alpha);
		sinAlpha = sin(alpha);
	
		for(k = 0; k < P.numberOfVertices; k++) {
			k1 = (k + 1) % P.numberOfVertices;
			deltaX = P.V[k1][0] - P.V[k][0];
			deltaY = P.V[k1][1] - P.V[k][1];
			det = deltaX * cosAlpha + deltaY * sinAlpha;
			if(fabs(det) > EPS) {
				// the segment between vertices k and (k + 1) is parametrized 
				// with parameter t in [0,1]. The parameter value of the 
				// intersection point is computed.
				t = (d - P.V[k][0] * cosAlpha - P.V[k][1] * sinAlpha) / det;
			  	if(t >= 0.0 && t < 1.0){
					newVertex[ind][0] = P.V[k][0] + t * deltaX;
					newVertex[ind][1] = P.V[k][1] + t * deltaY;
					k0[ind] = k;
					ind++; 
				}				
			}
		}
		if (ind < 2)
			// the line is rejected, because there are less than 2 intersection points. 
			DETcheck = 0;
		// checking small angle 
		else{
			A[0] = P.V[k0[0]][0] - newVertex[0][0];
			A[1] = P.V[k0[0]][1] - newVertex[0][1];
			B[0] = newVertex[1][0] - newVertex[0][0];
			B[1] = newVertex[1][1] - newVertex[0][1];
			C[0] = P.V[k0[1]][0] - newVertex[1][0];
			C[1] = P.V[k0[1]][1] - newVertex[1][1];
			magA = sqrt(A[0]*A[0] + A[1]*A[1]);
			magB = sqrt(B[0]*B[0] + B[1]*B[1]);
			magC = sqrt(C[0]*C[0] + C[1]*C[1]);
			cosAB = (A[0]*B[0] + A[1]*B[1]) / (magA*magB);
			cosCD = -(B[0]*C[0] + B[1]*C[1]) / (magB*magC);
			// computing angle between A and B vector
			omg1 = acos(cosAB);
			// computing angle between C and D vector
			omg2 = acos(cosCD);
			//printf("OMG: %.9f OMG1: %.9f OMG2: %.9f\n",omg,omg1,omg2);
			// checking if omg1 < omg and PI - omg1 < omg
			if ((omg1 < omg) || (M_PI - omg1) < omg){
				DETcheck = 0;
			}
			// checking if omg2 < omg and PI - omg2 < omg
			if ((omg2 < omg) || (M_PI - omg2) < omg){
				DETcheck = 0;
			}
		}
			
		if (DETcheck == 1) break;
		
	}while(1);

	Free2d(cdt->Polygon[i].V);
	Free2d(cdt->Polygon[i].H);
	Free(cdt->Polygon[i].B);

	// vertices of the new two polygons are determined.
	cdt->Polygon[i].V = (double **)calloc2d(k0[1] - k0[0] + 2, 2, sizeof(double));
	cdt->Polygon[i].V[0][0] = newVertex[1][0];
	cdt->Polygon[i].V[0][1] = newVertex[1][1];
	cdt->Polygon[i].V[1][0] = newVertex[0][0];
	cdt->Polygon[i].V[1][1] = newVertex[0][1];
	cdt->Polygon[i].numberOfVertices = 2;

	cdt->Polygon[ip].V = (double **)calloc2d(P.numberOfVertices - (k0[1] - k0[0]) + 2, 2, sizeof(double));
	cdt->Polygon[ip].V[0][0] = newVertex[0][0];
	cdt->Polygon[ip].V[0][1] = newVertex[0][1];
	cdt->Polygon[ip].V[1][0] = newVertex[1][0];
	cdt->Polygon[ip].V[1][1] = newVertex[1][1];
	cdt->Polygon[ip].numberOfVertices = 2;
  
	for(k = 0; k < P.numberOfVertices; k++) {
		k1 = (k + k0[0] + 1) % P.numberOfVertices;
		k2 = (k + k0[1] + 1) % P.numberOfVertices;
		if(cdt->Polygon[i].numberOfVertices < k0[1] - k0[0] + 2){
			cdt->Polygon[i].V[cdt->Polygon[i].numberOfVertices][0] = P.V[k1][0];
			cdt->Polygon[i].V[cdt->Polygon[i].numberOfVertices][1] = P.V[k1][1];
			cdt->Polygon[i].numberOfVertices++;
		}
		if(cdt->Polygon[ip].numberOfVertices < P.numberOfVertices - (k0[1] - k0[0]) + 2){
			cdt->Polygon[ip].V[cdt->Polygon[ip].numberOfVertices][0] = P.V[k2][0];
			cdt->Polygon[ip].V[cdt->Polygon[ip].numberOfVertices][1] = P.V[k2][1];
			cdt->Polygon[ip].numberOfVertices++; 
		}
	}
  
	// The width function of Polygon i is computed.
	WidthFunction(cdt, (cdt->Polygon) + i);
	PerimeterPolygon((cdt->Polygon) + i);
	AreaPolygon((cdt->Polygon) + i);
	RoundnessPolygon((cdt->Polygon) + i);	
    
    // The width function of Polygon ip is computed.
	WidthFunction(cdt, (cdt->Polygon) + ip);
	PerimeterPolygon((cdt->Polygon) + ip);
	AreaPolygon((cdt->Polygon) + ip);
	RoundnessPolygon((cdt->Polygon) + ip);	
	
	// The lifetime of the polygon is determined. The lifetime
	// is exponentially distribuited with parameter bPhi, which is
	// the weighted sum of the width and the weights are the probabilities
	// Phi[j].
	
	// Lifetime of Polygon i.
	// Perimeter
	if(lOption == 0){
		bPhi = 0.;
		for(j = 0; j < NUMBER_OF_INT; j++){
			bPhi += (cdt->Polygon[i].B[j]) * (cdt->Phi[j]);
		}
		cdt->Polygon[i].tau = RandomExponential(bPhi);
	}
	// Area
	else{
		cdt->Polygon[i].tau = RandomExponential(cdt->Polygon[i].area);
	}
	cdt->Polygon[i].beta = P.time;
	cdt->Polygon[i].time = cdt->Polygon[i].beta + cdt->Polygon[i].tau;

	
	// Lifetime of Polygon ip.
	// Perimeter
	if(lOption == 0){
		bPhi = 0.;
		for(j = 0; j < NUMBER_OF_INT; j++){
			bPhi += (cdt->Polygon[ip].B[j]) * (cdt->Phi[j]);
		}
	
		cdt->Polygon[ip].tau = RandomExponential(bPhi);
	}
	// Area
	else{
		cdt->Polygon[ip].tau = RandomExponential(cdt->Polygon[ip].area);
	}
	cdt->Polygon[ip].beta = P.time;
	cdt->Polygon[ip].time = cdt->Polygon[ip].beta + cdt->Polygon[ip].tau;
	
	// The two vertices of the I_Segment are assgined.
	cdt->I_Segment[cdt->numberOfI_Segments].x0 = newVertex[0][0];
	cdt->I_Segment[cdt->numberOfI_Segments].y0 = newVertex[0][1]; 
	cdt->I_Segment[cdt->numberOfI_Segments].x1 = newVertex[1][0]; 
	cdt->I_Segment[cdt->numberOfI_Segments].y1 = newVertex[1][1];
	// The length of the I_Segment is computed.
	deltaX = newVertex[1][0] - newVertex[0][0];
	deltaY = newVertex[1][1] - newVertex[0][1];
	cdt->I_Segment[cdt->numberOfI_Segments].length = sqrt(deltaX*deltaX + deltaY*deltaY);
	// The birthtime of the I_Segment is assigned.
	cdt->I_Segment[cdt->numberOfI_Segments].beta = P.time;

	Free2d(P.V);
}
#undef EPS


/**
	@function STIT2dIso
	-------------------
	Isotropic STIT2d
**/
int STIT2dIso(CDT2d *cdt, double timeStop, unsigned long lOption, double omg){
	
	unsigned long i;
	
	// state variable, 0: stit process stops, 1: stit process continues.
	unsigned int stitStop;
	
	// number of polygons in the current level of the branching tree, which are 
	// divided before time_stop. The branching tree describes the cell division 
	// process. Also q is an auxiliary variable for storing new polygons.
	unsigned int q;
	
	// current number of polygons.
	unsigned int numPol;
	
	// current line candidate in the rejection process.	
	LINE dirLine;
	
	// phi and cumulative distribution function are computed.
	SetPhiIso(cdt);
		
	//lifetime of the first polygon (window) is computed.
	InitTime(cdt, lOption);
	for(i = 0; i < cdt->numberOfPolygons; i++){
		
	}
	
	// it assumes that the process continues.
	stitStop = 1;
	
	do{
		numPol = cdt->numberOfPolygons;
		
    	q = 0;
    	for(i = 0; i < numPol; i++){
			
    		// checking if the polygon i must be divided.
      		if(cdt->Polygon[i].time < timeStop){
				dirLine = RandomLinePolygon(cdt, i);
				
				if(cdt->numberOfPolygons == MAX_NUMBER_OF_POLYGONS){
					printf("Number of Polygons exceeds %dld\n", MAX_NUMBER_OF_POLYGONS);
					return 1;
				}
				if(cdt->numberOfI_Segments == MAX_NUMBER_OF_I_SEGMENTS) {
					printf("Number of I_Segments exceeds %dld\n", MAX_NUMBER_OF_I_SEGMENTS);
					return 1;
				}
				RandomIntersectionPolygon(cdt, i, numPol + q, dirLine, lOption, omg);
				q++;
				(cdt->numberOfPolygons)++;
				(cdt->numberOfI_Segments)++;
			}
			//printf("%f %f %f %f %f %ld %f %f %f %f\n",cdt->Polygon[i].area,cdt->Polygon[i].aspectRatio,cdt->Polygon[i].beta,cdt->Polygon[i].maxWidth,cdt->Polygon[i].minWidth,cdt->Polygon[i].numberOfVertices,cdt->Polygon[i].perimeter,cdt->Polygon[i].roundness,cdt->Polygon[i].tau,cdt->Polygon[i].wML);
		}
		// the proccess stops.
    	if(q == 0) stitStop = 0;
  	}while(stitStop);
  	
	return 0;
}

/**
	@function STIT2dAniso
	-------------------
	Anisotropic STIT2d
**/
int STIT2dAniso(CDT2d *cdt, double timeStop, double *angleDir, double *probDir, unsigned long nDir, unsigned long lOption, double omg){
	unsigned long i;
	
	// state variable, 0: stit process stops, 1: stit process continues.
	unsigned int stitStop;
	
	// number of polygons in the current level of the branching tree, which are 
	// divided before time_stop. The branching tree describes the cell division 
	// process. Also q is an auxiliary variable for storing new polygons.
	unsigned int q;
	
	// current number of polygons.
	unsigned int numPol;
	
	// current line candidate in the rejection process.	
	LINE dirLine;
	
	// phi and cumulative distribution function are computed.
	SetPhiAniso(cdt, angleDir, probDir, nDir);
	
	//lifetime of the first polygon (window) is computed.
	InitTime(cdt, lOption);
	
	// it assumes that the process continues.
	stitStop = 1;
	do{
		numPol = cdt->numberOfPolygons;
    	q = 0;
    	for(i = 0; i < numPol; i++){
    		// checking if the polygon i must be divided.
      		if(cdt->Polygon[i].time < timeStop){
				dirLine = RandomLinePolygon(cdt, i);
				
				if(cdt->numberOfPolygons == MAX_NUMBER_OF_POLYGONS){
					printf("Number of Polygons exceeds %dld\n", MAX_NUMBER_OF_POLYGONS);
					return 1;
				}
				if(cdt->numberOfI_Segments == MAX_NUMBER_OF_I_SEGMENTS) {
					printf("Number of I_Segments exceeds %dld\n", MAX_NUMBER_OF_I_SEGMENTS);
					return 1;
				}
				
				RandomIntersectionPolygon(cdt, i, numPol + q, dirLine, lOption, omg);
				q++;
				(cdt->numberOfPolygons)++;
				(cdt->numberOfI_Segments)++;
			}
		}
		// the proccess stops.
    	if(q == 0) stitStop = 0;
  	}while(stitStop);
  	
	return 0;
}

/**
	@function STIT2dAnisoDisturbed
	-------------------

	Anisotropic Disturbed STIT2d
**/
int STIT2dAnisoDisturbed(CDT2d *cdt, double timeStop, double *angleDir, double *probDir, unsigned long nDir, double bEllip, unsigned long lOption, double omg){
	unsigned long i;
	
	// state variable, 0: stit process stops, 1: stit process continues.
	unsigned int stitStop;
	
	// number of polygons in the current level of the branching tree, which are 
	// divided before time_stop. The branching tree describes the cell division 
	// process. Also q is an auxiliary variable for storing new polygons.
	unsigned int q;
	
	// current number of polygons.
	unsigned int numPol;
	
	// current line candidate in the rejection process.	
	LINE dirLine;
	
	// phi and cumulative distribution function are computed.
	SetPhiAniso(cdt, angleDir, probDir, nDir);
	
	//lifetime of the first polygon (window) is computed.
	InitTime(cdt, lOption);
	
	// it assumes that the process continues.
	stitStop = 1;
	do{
		numPol = cdt->numberOfPolygons;
    	q = 0;
    	for(i = 0; i < numPol; i++){
    		// checking if the polygon i must be divided.
      		if(cdt->Polygon[i].time < timeStop){
				dirLine = RandomLinePolygonDisturbed(cdt, i, bEllip);
				
				if(cdt->numberOfPolygons == MAX_NUMBER_OF_POLYGONS){
					printf("Number of Polygons exceeds %dld\n", MAX_NUMBER_OF_POLYGONS);
					return 1;
				}
				if(cdt->numberOfI_Segments == MAX_NUMBER_OF_I_SEGMENTS) {
					printf("Number of I_Segments exceeds %dld\n", MAX_NUMBER_OF_I_SEGMENTS);
					return 1;
				}
				
				RandomIntersectionPolygonDisturbed(cdt, i, numPol + q, dirLine, bEllip, lOption, omg);
				q++;
				(cdt->numberOfPolygons)++;
				(cdt->numberOfI_Segments)++;
			}
		}
		// the proccess stops.
    	if(q == 0) stitStop = 0;
  	}while(stitStop);
  	
	return 0;
}

/**
	@function STIT2dAnisoEllip
	-------------------

	Anisotropic Elliptic STIT2d

**/
int STIT2dAnisoEllip(CDT2d *cdt, double timeStop, double bEllip, unsigned long lOption, double omg){
	unsigned long i;
	
	// state variable, 0: stit process stops, 1: stit process continues.
	unsigned int stitStop;
	
	// number of polygons in the current level of the branching tree, which are 
	// divided before time_stop. The branching tree describes the cell division 
	// process. Also q is an auxiliary variable for storing new polygons.
	unsigned int q;
	
	// current number of polygons.
	unsigned int numPol;
	
	// current line candidate in the rejection process.	
	LINE dirLine;
	
	// phi and cumulative distribution function are computed.
	SetPhiAnisoEllip(cdt, bEllip);
	
	//lifetime of the first polygon (window) is computed.
	InitTime(cdt, lOption);
	
	// it assumes that the process continues.
	stitStop = 1;
	do{
		numPol = cdt->numberOfPolygons;
    	q = 0;
    	for(i = 0; i < numPol; i++){
    		// checking if the polygon i must be divided.
      		if(cdt->Polygon[i].time < timeStop){
				dirLine = RandomLinePolygonDisturbed(cdt, i, bEllip);
				
				if(cdt->numberOfPolygons == MAX_NUMBER_OF_POLYGONS){
					printf("Number of Polygons exceeds %dld\n", MAX_NUMBER_OF_POLYGONS);
					return 1;
				}
				if(cdt->numberOfI_Segments == MAX_NUMBER_OF_I_SEGMENTS) {
					printf("Number of I_Segments exceeds %dld\n", MAX_NUMBER_OF_I_SEGMENTS);
					return 1;
				}
				
				RandomIntersectionPolygonDisturbed(cdt, i, numPol + q, dirLine, bEllip, lOption, omg);
				q++;
				(cdt->numberOfPolygons)++;
				(cdt->numberOfI_Segments)++;
			}
		}
		// the proccess stops.
    	if(q == 0) stitStop = 0;
  	}while(stitStop);
  	
	return 0;
}

/**	
	@function: WidthFunction 
	-------------------------
	Computes the width function of a polygon.
	
	[IN]
    @param Polygon: polygon for which the width function is computed.
    
    [OUT]
    @param Polygon->H: support function (fixed direction)
    @param Polygon->B: width of each fixed direction.
    @param Polygon->maxWidth: maximal width of the polygon = max(b)
    @param Polygon->minWidth: minimal width of the polygon = min(b)
**/
int WidthFunction(CDT2d *cdt, POLYGON *Polygon){
	unsigned long j, k;
	double cosDir, sinDir, s, sMin, sMax, bMax, bMin, angDir;
  
	Polygon->H = (double **)calloc2d(NUMBER_OF_INT, 2, sizeof(double));
	Polygon->B = (double *)calloc(NUMBER_OF_INT,sizeof(double));
  
	for(j = 0; j < NUMBER_OF_INT; j++){
		angDir = (double)j * M_PI / (double)NUMBER_OF_INT;
		cosDir = cos(angDir);
		sinDir = sin(angDir);
		sMin = DOUBLE_MAX;
		sMax = DOUBLE_MIN;

		for(k = 0; k < Polygon->numberOfVertices; k++){
			s = Polygon->V[k][0] * cosDir + Polygon->V[k][1] * sinDir;
			if(s < sMin) sMin = s;
			if(s > sMax) sMax = s;
		}
		Polygon->H[j][0] = sMin;
		Polygon->H[j][1] = sMax;
	}

	bMax = DOUBLE_MIN;
	bMin = DOUBLE_MAX;
	for(j = 0; j < NUMBER_OF_INT; j++) {
		Polygon->B[j] = Polygon->H[j][1] - Polygon->H[j][0];
		if(Polygon->B[j] > bMax) bMax = Polygon->B[j];
		if(Polygon->B[j] < bMin) bMin = Polygon->B[j];
	}
	Polygon->maxWidth = bMax;
	Polygon->minWidth = bMin;
	Polygon->aspectRatio = bMin / bMax;
	
	Polygon->wML = 1. / ((cdt->size - Polygon->B[0]) * (cdt->size - Polygon->B[64]));
  
  return 0;
}

/**
	@function: PerimeterPolygon
	----------------------
**/
int PerimeterPolygon(POLYGON *Polygon){
	unsigned long k, k1;
	double deltaX, deltaY;
	
	Polygon->perimeter = 0.0;
	for(k = 0; k < Polygon->numberOfVertices; k++){
		k1 = (k + 1) % Polygon->numberOfVertices;
		deltaX = Polygon->V[k1][0] - Polygon->V[k][0];
		deltaY = Polygon->V[k1][1] - Polygon->V[k][1];
		Polygon->perimeter += sqrt(deltaX*deltaX + deltaY*deltaY); 
	}
	
	return 0;
}

/**
	@function: AreaPolygon
	----------------------
**/
int AreaPolygon(POLYGON *Polygon){
	unsigned long k, k1;
	
	Polygon->area = 0.0;
	for(k = 0; k < Polygon->numberOfVertices; k++){
		k1 = (k + 1) % Polygon->numberOfVertices;
		Polygon->area += (Polygon->V[k][0]*Polygon->V[k1][1]) - (Polygon->V[k][1]*Polygon->V[k1][0]); 
	}
	Polygon->area = 0.5*(Polygon->area);
	return 0;
}

/**
	@function: RoundnessPolygon
	----------------------
**/
int RoundnessPolygon(POLYGON *Polygon){
	
	Polygon->roundness = 4*M_PI*Polygon->area / (Polygon->perimeter * Polygon->perimeter);
	return 0;
}

/**
	@function RandomExponential
	---------------------------
	Simulates an exponentially distributed random number.
	
	[IN]
	@param lambda: the parameter of the exponential distribution.
	
	[OUT]
	@return a real-valued pseudorandom number.
	
	@warning should be initialized using InitRandomizer() or srandom()  
**/
double RandomExponential(double lambda){
	return -log(RandomUniform()) / lambda;
}

/**
	@function RandomElliptic
	---------------------------
	Generates a random number for the elliptic distribution
	with the cumulative distribution function 
	phi_cum(alpha) = area of ellipse sector [0,alpha] divided by half 
	area of the ellipse with half axes 1 and bEllip (less than 1)
	and alpha in [0,pi[.
	
	[IN]
	@param bEllip: minor half axis (vertical and less than 1).
	[OUT]
	@return random number.
**/
double RandomElliptic(double bEllip){
	double as, c, r, s, tas, x, phi;

	as = asin(bEllip);
	tas = tan(as/2.0);

	do{
		x = (as - bEllip*log(tas)) * RandomUniform();
		r = RandomUniform();
		phi = x;
		if(phi > as) {
			phi = 2.0 * (atan(tas * exp((x-as) / bEllip)));
			r = bEllip * ((1.0/sin(phi) -1.0) * r + 1.0);
		}
		else 
			r = (1.0 - bEllip) * r + bEllip;
	
		c = cos(phi);
		s = sin(phi);
	}while(r <= bEllip / sqrt(bEllip*bEllip*c*c + s*s));

	if(random() % 2)
		phi = M_PI - phi;

	return phi;
}

/**
	@function: StatSTIT
	--------------------
	Computes statistics of a STIT modification with Gauss distribution.
**/
void StatSTIT(CDT2d *cdt){
	unsigned long i;
	double sumWeights = 0., sumValues, mean, var, std, cv;
	FILE *statFile;
		
	//Writing stats in a file.
	statFile = fopen("statSTIT.txt","w");
	fprintf(statFile,"%ld\n",cdt->numberOfGaussPolygons);
	
	//Computing sum(wML)
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumWeights += cdt->GaussPolygon[i].wML;
	
	//Area
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += /*cdt->GaussPolygon[i].wML * */cdt->GaussPolygon[i].area;
	mean = sumValues / /*sumWeights */ cdt->numberOfGaussPolygons;
	
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * (cdt->GaussPolygon[i].area - mean) * (cdt->GaussPolygon[i].area - mean);
		
	var = sumValues / sumWeights;
	std = sqrt(var);
	cv = std / mean;
		
	fprintf(statFile,"%.12f,%.12f,%.12f\n",mean,std,cv);
	
	//Perimeter
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * cdt->GaussPolygon[i].perimeter;
	mean = sumValues / sumWeights;
	
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * (cdt->GaussPolygon[i].perimeter - mean) * (cdt->GaussPolygon[i].perimeter - mean);
		
	var = sumValues / sumWeights;
	std = sqrt(var);
	cv = std / mean;
		
	fprintf(statFile,"%.12f,%.12f,%.12f\n",mean,std,cv);
	
	//Roundness
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * cdt->GaussPolygon[i].roundness;
	mean = sumValues / sumWeights;
	
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * (cdt->GaussPolygon[i].roundness - mean) * (cdt->GaussPolygon[i].roundness - mean);
		
	var = sumValues / sumWeights;
	std = sqrt(var);
	cv = std / mean;
		
	fprintf(statFile,"%.12f,%.12f,%.12f\n",mean,std,cv);

	//Max width
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * cdt->GaussPolygon[i].maxWidth;
	mean = sumValues / sumWeights;
	
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * (cdt->GaussPolygon[i].maxWidth - mean) * (cdt->GaussPolygon[i].maxWidth - mean);
		
	var = sumValues / sumWeights;
	std = sqrt(var);
	cv = std / mean;
		
	fprintf(statFile,"%.12f,%.12f,%.12f\n",mean,std,cv);

	//Min width
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * cdt->GaussPolygon[i].minWidth;
	mean = sumValues / sumWeights;
	
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * (cdt->GaussPolygon[i].minWidth - mean) * (cdt->GaussPolygon[i].minWidth - mean);
		
	var = sumValues / sumWeights;
	std = sqrt(var);
	cv = std / mean;
		
	fprintf(statFile,"%.12f,%.12f,%.12f\n",mean,std,cv);

	//Aspect ratio
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * cdt->GaussPolygon[i].aspectRatio;
	mean = sumValues / sumWeights;
	
	sumValues = 0.;
	for(i = 0; i < cdt->numberOfGaussPolygons; i++)
		sumValues += cdt->GaussPolygon[i].wML * (cdt->GaussPolygon[i].aspectRatio - mean) * (cdt->GaussPolygon[i].aspectRatio - mean);
		
	var = sumValues / sumWeights;
	std = sqrt(var);
	cv = std / mean;
		
	fprintf(statFile,"%.12f,%.12f,%.12f\n",mean,std,cv);

	fclose(statFile);
}

/**
@function NoBoundary
**/
void NoBoundary(CDT2d *cdt){
	unsigned int i, k, state;
	
	cdt->GaussPolygon = Malloc(MAX_NUMBER_OF_POLYGONS, POLYGON);
	cdt->numberOfGaussPolygons = 0;
	
	for(i = 0; i < cdt->numberOfPolygons; i++){
		state = 1;
		for(k = 0; k < cdt->Polygon[i].numberOfVertices; k++){
if ((cdt->Polygon[i].V[k][0] == 0.0 || cdt->Polygon[i].V[k][0] == cdt->size) || (cdt->Polygon[i].V[k][1] == 0.0 || cdt->Polygon[i].V[k][1] == cdt->size)){
				state = 0;
				break;
			}
		}
		if (state){
			CopyPolygon(cdt->GaussPolygon + cdt->numberOfGaussPolygons, cdt->Polygon + i);
			(cdt->numberOfGaussPolygons)++;
		}
	}
}

/**
@function CopyPolygon
**/
void CopyPolygon(POLYGON *NewPolygon, POLYGON *Polygon){
	unsigned int j, k;

	NewPolygon->numberOfVertices = Polygon->numberOfVertices;
	NewPolygon->V = (double **)calloc2d(NewPolygon->numberOfVertices, 2, sizeof(double));
	
	for(k = 0; k < NewPolygon->numberOfVertices; k++){
		NewPolygon->V[k][0] = Polygon->V[k][0];
		NewPolygon->V[k][1] = Polygon->V[k][1];
	}
	
	NewPolygon->H = (double **)calloc2d(NUMBER_OF_INT, 2, sizeof(double));
	for(j = 0; j < NUMBER_OF_INT; j++){
		NewPolygon->H[j][0] = Polygon->H[j][0];
		NewPolygon->H[j][1] = Polygon->H[j][1];
	}
	
	NewPolygon->B = (double *)calloc(NUMBER_OF_INT,sizeof(double));
	for(j = 0; j < NUMBER_OF_INT; j++){
		NewPolygon->B[j] = Polygon->B[j];
	}
	
	NewPolygon->aspectRatio = Polygon->aspectRatio;
	NewPolygon->perimeter = Polygon->perimeter;
	NewPolygon->area = Polygon->area;
	NewPolygon->roundness = Polygon->roundness;
	NewPolygon->beta = Polygon->beta;
	NewPolygon->tau = Polygon->tau;
	NewPolygon->time = Polygon->time;
	NewPolygon->maxWidth = Polygon->maxWidth;
	NewPolygon->minWidth = Polygon->minWidth;
	NewPolygon->wML = Polygon->wML;
}

/**
	@function: GaussLinePolygon
	----------------------------
	Generates a Gaussian distributed line with normal direction alpha and distance d. 
	
	[IN]
	@param Polygon: polygon where the line is generated.
	@param sigma:
	@param r: 
	
	[OUT]
	@param dir_line: line with normal direction alpha and distance d. 
**/
LINE GaussLinePolygon(CDT2d *cdt, unsigned long i, double sigma, gsl_rng *r){
	unsigned long j; // direction index 
	double d, randNum, zGauss;
	LINE dirLine;

	/** 
	The direction is fix. The location of the sectional	line is choose 
	with Gaussian distribution, standard deviation sigma.
	**/	

	do{
		randNum = RandomUniform();
		// random direction index.
		j = RandomDirection(cdt, randNum);
		d = cdt->GaussPolygon[i].maxWidth * RandomUniform() + cdt->GaussPolygon[i].H[j][0];
	}while(d > cdt->GaussPolygon[i].H[j][1]);
	
	/**
	Simulation of a random variable with truncated standard Gaussian 
	distribution.
	**/
	do{
		double z = gsl_ran_gaussian(r, 1.0);
		zGauss = z;
	}while(zGauss < -1./(2*sigma) || zGauss > 1./(2*sigma));
	
	d = cdt->GaussPolygon[i].B[j]*sigma*zGauss + (cdt->GaussPolygon[i].H[j][0] + cdt->GaussPolygon[i].H[j][1])/2.;
	
	dirLine.alpha = M_PI * (double)j / (double)NUMBER_OF_INT;
	dirLine.dist = d;
	dirLine.index = j;
	
	return dirLine;
}

/**

	@function: GaussLinePolygonDisturbed
	----------------------------
	Generates a Gaussian distributed line with normal direction alpha and distance d. 
	
	[IN]
	@param Polygon: polygon where the line is generated.
	@param sigma:
	@param r: 
	
	[OUT]
	@param dir_line: line with normal direction alpha and distance d. 

**/
LINE GaussLinePolygonDisturbed(CDT2d *cdt, unsigned long i, double sigma, gsl_rng *r, double bEllip){
	unsigned long j, jGamma;
	double d, randNum, zGauss, gamma;
	LINE dirLine;

	do{
		randNum = RandomUniform();
		// random direction index.
		j = RandomDirection(cdt, randNum);
		gamma = RandomElliptic(bEllip);
		jGamma = floor(gamma * NUMBER_OF_INT / M_PI);
		j = (j + jGamma) % (NUMBER_OF_INT);
		d = cdt->GaussPolygon[i].maxWidth * RandomUniform() + cdt->GaussPolygon[i].H[j][0];
	}while(d > cdt->GaussPolygon[i].H[j][1]);
	
	/**
	Simulation of a random variable with truncated standard Gaussian 
	distribution.
	**/
	do{
		double z = gsl_ran_gaussian(r, 1.0);
		zGauss = z;
	}while(zGauss < -1./(2*sigma) || zGauss > 1./(2*sigma));
	
	d = cdt->GaussPolygon[i].B[j]*sigma*zGauss + (cdt->GaussPolygon[i].H[j][0] + cdt->GaussPolygon[i].H[j][1])/2.;
	
	dirLine.alpha = M_PI * (double)j / (double)NUMBER_OF_INT;
	dirLine.dist = d;
	dirLine.index = j;
	
	return dirLine;
}

/** 
	@function GaussIntersectionPolygon
	-----------------------------------
	Intersects a convex polygon (cell) with a random straight line (dir_line) of normal 
	direction alpha distributed according to directional distribution phi (isotropic: phi 
	is the uniform distribution on [0,pi[). The probability for choosing direction j is 
	proportional to phi[j]*b[j].
	
	[IN]
	@param dir_line: random straight line.
	@param Polygon: polygon to be intersected. 
	
	[OUT] 
    @param Polygon: one of the resulting polygons.
    @param Polygon1: the other resulting polygon.   
    @param I_Segment: the intersection segment     
**/
#define EPS 1.0E-32
void GaussIntersectionPolygon(CDT2d *cdt,unsigned long i,unsigned long ip,LINE dirLine,double sigma,gsl_rng *r, unsigned long lOption, double omg){
	unsigned long j, k, k1, k2;
	// k0[1] - k0[0] is the number of vertices between the first and the second intersection point.
	unsigned long k0[2]; 
	// ind means index of intersection points (0 and 1).
	unsigned long ind = 0;
	double cosAlpha, det, deltaX, deltaY, sinAlpha, t, alpha, d, bPhi;
	double newVertex[2][2];
	double A[2], B[2], C[2], magA, magB, magC, cosAB, cosCD, omg1, omg2;

	// auxiliary variable for checking if the angle of the intersection between I_Segment and
	// the side of the polygon where the intersection point lies, is not close to zero. 
	// DETcheck = 1 (angle is ok) and DETcheck = 0 (angle is close to zero or equal to zero for
	// all the side of the polygons where an intersection point could be). Then, if DETcheck = 0
	// a new random line must be generated.
	int DETcheck = 1;

	POLYGON P;
	  
	// copy of Polygon in to polygon P.
	P.numberOfVertices = cdt->GaussPolygon[i].numberOfVertices;
	P.V = (double **)calloc2d(P.numberOfVertices, 2, sizeof(double));
	for(k = 0; k < P.numberOfVertices; k++){
		P.V[k][0] = cdt->GaussPolygon[i].V[k][0];
		P.V[k][1] = cdt->GaussPolygon[i].V[k][1];
	}
	
	P.perimeter = cdt->GaussPolygon[i].perimeter;
	P.time = cdt->GaussPolygon[i].time;
	
	alpha = dirLine.alpha;
	d = dirLine.dist;
	
	// computing omega as factor*PI
	omg = M_PI*omg;
	
	do{
		if (DETcheck == 0){			
			dirLine = GaussLinePolygon(cdt, i, sigma, r);
			alpha = dirLine.alpha;
			d = dirLine.dist;
			ind = 0;
			DETcheck = 1;
		}
		cosAlpha = cos(alpha);
		sinAlpha = sin(alpha);
	
		for(k = 0; k < P.numberOfVertices; k++) {
			k1 = (k + 1) % P.numberOfVertices;
			deltaX = P.V[k1][0] - P.V[k][0];
			deltaY = P.V[k1][1] - P.V[k][1];
			det = deltaX * cosAlpha + deltaY * sinAlpha;
			if(fabs(det) > EPS) {
				// the segment between vertices k and (k + 1) is parametrized 
				// with parameter t in [0,1]. The parameter value of the 
				// intersection point is computed.
				t = (d - P.V[k][0] * cosAlpha - P.V[k][1] * sinAlpha) / det;
			  	if(t >= 0.0 && t < 1.0){
					newVertex[ind][0] = P.V[k][0] + t * deltaX;
					newVertex[ind][1] = P.V[k][1] + t * deltaY;
					k0[ind] = k;
					ind++; 
				}
			}
		}
		if (ind < 2)
			// the line is rejected, because there are less than 2 intersection points. 
			DETcheck = 0;
		// checking small angle 
		else{
			A[0] = P.V[k0[0]][0] - newVertex[0][0];
			A[1] = P.V[k0[0]][1] - newVertex[0][1];
			B[0] = newVertex[1][0] - newVertex[0][0];
			B[1] = newVertex[1][1] - newVertex[0][1];
			C[0] = P.V[k0[1]][0] - newVertex[1][0];
			C[1] = P.V[k0[1]][1] - newVertex[1][1];
			magA = sqrt(A[0]*A[0] + A[1]*A[1]);
			magB = sqrt(B[0]*B[0] + B[1]*B[1]);
			magC = sqrt(C[0]*C[0] + C[1]*C[1]);
			cosAB = (A[0]*B[0] + A[1]*B[1]) / (magA*magB);
			cosCD = -(B[0]*C[0] + B[1]*C[1]) / (magB*magC);
			// computing angle between A and B vector
			omg1 = acos(cosAB);
			// computing angle between C and D vector
			omg2 = acos(cosCD);
			//printf("OMG: %.9f OMG1: %.9f OMG2: %.9f\n",omg,omg1,omg2);
			// checking if omg1 < omg and PI - omg1 < omg
			if ((omg1 < omg) || (M_PI - omg1) < omg){
				DETcheck = 0;
			}
			// checking if omg2 < omg and PI - omg2 < omg
			if ((omg2 < omg) || (M_PI - omg2) < omg){
				DETcheck = 0;
			}
		}
			
		if (DETcheck == 1) break;
	}while(1);

	Free2d(cdt->GaussPolygon[i].V);
	Free2d(cdt->GaussPolygon[i].H);
	Free(cdt->GaussPolygon[i].B);

	cdt->GaussPolygon[i].V = (double **)calloc2d(k0[1] - k0[0] + 2, 2, sizeof(double));
	cdt->GaussPolygon[i].V[0][0] = newVertex[1][0];
	cdt->GaussPolygon[i].V[0][1] = newVertex[1][1];
	cdt->GaussPolygon[i].V[1][0] = newVertex[0][0];
	cdt->GaussPolygon[i].V[1][1] = newVertex[0][1];
	cdt->GaussPolygon[i].numberOfVertices = 2;

	cdt->GaussPolygon[ip].V = (double **)calloc2d(P.numberOfVertices - (k0[1] - k0[0]) + 2, 2, sizeof(double));
	cdt->GaussPolygon[ip].V[0][0] = newVertex[0][0];
	cdt->GaussPolygon[ip].V[0][1] = newVertex[0][1];
	cdt->GaussPolygon[ip].V[1][0] = newVertex[1][0];
	cdt->GaussPolygon[ip].V[1][1] = newVertex[1][1];
	cdt->GaussPolygon[ip].numberOfVertices = 2;
  
	for(k = 0; k < P.numberOfVertices; k++) {
		k1 = (k + k0[0] + 1) % P.numberOfVertices;
		k2 = (k + k0[1] + 1) % P.numberOfVertices;
		if(cdt->GaussPolygon[i].numberOfVertices < k0[1] - k0[0] + 2){
			cdt->GaussPolygon[i].V[cdt->GaussPolygon[i].numberOfVertices][0] = P.V[k1][0];
			cdt->GaussPolygon[i].V[cdt->GaussPolygon[i].numberOfVertices][1] = P.V[k1][1];
			cdt->GaussPolygon[i].numberOfVertices++;
		}
		if(cdt->GaussPolygon[ip].numberOfVertices < P.numberOfVertices - (k0[1] - k0[0]) + 2){
			cdt->GaussPolygon[ip].V[cdt->GaussPolygon[ip].numberOfVertices][0] = P.V[k2][0];
			cdt->GaussPolygon[ip].V[cdt->GaussPolygon[ip].numberOfVertices][1] = P.V[k2][1];
			cdt->GaussPolygon[ip].numberOfVertices++; 
		}
	}
	  
	// The width function of Polygon i is computed.
	WidthFunction(cdt, (cdt->GaussPolygon) + i);
	PerimeterPolygon((cdt->GaussPolygon) + i);
	AreaPolygon((cdt->GaussPolygon) + i);
	RoundnessPolygon((cdt->GaussPolygon) + i);
	    
  // The width function of Polygon ip is computed.
	WidthFunction(cdt, (cdt->GaussPolygon) + ip);
	PerimeterPolygon((cdt->GaussPolygon) + ip);
	AreaPolygon((cdt->GaussPolygon) + ip);
	RoundnessPolygon((cdt->GaussPolygon) + ip);
		
	// The lifetime of the polygon is determined. The lifetime
	// is exponentially distribuited with parameter bPhi, which is
	// the weighted sum of the width and the weights are the probabilities
	// Phi[j].
	
	// Lifetime of Polygon i.
	//Perimeter
	if(lOption == 0){
		bPhi = 0.;
		for(j = 0; j < NUMBER_OF_INT; j++){
			bPhi += (cdt->GaussPolygon[i].B[j]) * (cdt->Phi[j]);
		}
		cdt->GaussPolygon[i].tau = RandomExponential(bPhi);
	}
	// Area
	else{
		cdt->GaussPolygon[i].tau = RandomExponential(cdt->GaussPolygon[i].area);
	}
	cdt->GaussPolygon[i].beta = P.time;
	cdt->GaussPolygon[i].time = cdt->GaussPolygon[i].beta + cdt->GaussPolygon[i].tau;
	
	// Lifetime of Polygon ip.
	// Perimeter
	if(lOption == 0){
		bPhi = 0.;
		for(j = 0; j < NUMBER_OF_INT; j++){
			bPhi += (cdt->GaussPolygon[ip].B[j]) * (cdt->Phi[j]);
		}
		cdt->GaussPolygon[ip].tau = RandomExponential(bPhi);
	}
	// Area
	else{
		cdt->GaussPolygon[ip].tau = RandomExponential(cdt->GaussPolygon[ip].area);
	}
	cdt->GaussPolygon[ip].beta = P.time;
	cdt->GaussPolygon[ip].time = cdt->GaussPolygon[ip].beta + cdt->GaussPolygon[ip].tau;
	
	// The two vertices of the I_Segment are assgined.
	cdt->I_Segment[cdt->numberOfI_Segments].x0 = newVertex[0][0];
	cdt->I_Segment[cdt->numberOfI_Segments].y0 = newVertex[0][1]; 
	cdt->I_Segment[cdt->numberOfI_Segments].x1 = newVertex[1][0]; 
	cdt->I_Segment[cdt->numberOfI_Segments].y1 = newVertex[1][1];
	// The length of the I_Segment is computed.
	deltaX = newVertex[1][0] - newVertex[0][0];
	deltaY = newVertex[1][1] - newVertex[0][1];
	cdt->I_Segment[cdt->numberOfI_Segments].length = sqrt(deltaX*deltaX + deltaY*deltaY);
	// The birthtime of the I_Segment is assigned.
	cdt->I_Segment[cdt->numberOfI_Segments].beta = P.time;

	Free2d(P.V);
}
#undef EPS

/**
**/
#define EPS 1.0E-32
void GaussIntersectionPolygonDisturbed(CDT2d *cdt,unsigned long i,unsigned long ip,LINE dirLine,double sigma,gsl_rng *r, double bEllip, unsigned long lOption, double omg){
	unsigned long j, k, k1, k2;
	// k0[1] - k0[0] is the number of vertices between the first and the second intersection point.
	unsigned long k0[2]; 
	// ind means index of intersection points (0 and 1).
	unsigned long ind = 0;
	double cosAlpha, det, deltaX, deltaY, sinAlpha, t, alpha, d, bPhi;
	double newVertex[2][2];
	double A[2], B[2], C[2], magA, magB, magC, cosAB, cosCD, omg1, omg2;

	// auxiliary variable for checking if the angle of the intersection between I_Segment and
	// the side of the polygon where the intersection point lies, is not close to zero. 
	// DETcheck = 1 (angle is ok) and DETcheck = 0 (angle is close to zero or equal to zero for
	// all the side of the polygons where an intersection point could be). Then, if DETcheck = 0
	// a new random line must be generated.
	int DETcheck = 1;

	POLYGON P;
	  
	// copy of Polygon in to polygon P.
	P.numberOfVertices = cdt->GaussPolygon[i].numberOfVertices;
	P.V = (double **)calloc2d(P.numberOfVertices, 2, sizeof(double));
	for(k = 0; k < P.numberOfVertices; k++){
		P.V[k][0] = cdt->GaussPolygon[i].V[k][0];
		P.V[k][1] = cdt->GaussPolygon[i].V[k][1];
	}
	
	P.perimeter = cdt->GaussPolygon[i].perimeter;
	P.time = cdt->GaussPolygon[i].time;
	
	alpha = dirLine.alpha;
	d = dirLine.dist;
	
	// computing omega as factor*PI
	omg = M_PI*omg;
	
	do{
		if (DETcheck == 0){			
			dirLine = GaussLinePolygonDisturbed(cdt, i, sigma, r, bEllip);
			alpha = dirLine.alpha;
			d = dirLine.dist;
			ind = 0;
			DETcheck = 1;
		}
		cosAlpha = cos(alpha);
		sinAlpha = sin(alpha);
	
		for(k = 0; k < P.numberOfVertices; k++) {
			k1 = (k + 1) % P.numberOfVertices;
			deltaX = P.V[k1][0] - P.V[k][0];
			deltaY = P.V[k1][1] - P.V[k][1];
			det = deltaX * cosAlpha + deltaY * sinAlpha;
			if(fabs(det) > EPS) {
				// the segment between vertices k and (k + 1) is parametrized 
				// with parameter t in [0,1]. The parameter value of the 
				// intersection point is computed.
				t = (d - P.V[k][0] * cosAlpha - P.V[k][1] * sinAlpha) / det;
			  	if(t >= 0.0 && t < 1.0){
					newVertex[ind][0] = P.V[k][0] + t * deltaX;
					newVertex[ind][1] = P.V[k][1] + t * deltaY;
					k0[ind] = k;
					ind++; 
				}
			}
		}
		if (ind < 2)
			// the line is rejected, because there are less than 2 intersection points. 
			DETcheck = 0;
		// checking small angle 
		else{
			A[0] = P.V[k0[0]][0] - newVertex[0][0];
			A[1] = P.V[k0[0]][1] - newVertex[0][1];
			B[0] = newVertex[1][0] - newVertex[0][0];
			B[1] = newVertex[1][1] - newVertex[0][1];
			C[0] = P.V[k0[1]][0] - newVertex[1][0];
			C[1] = P.V[k0[1]][1] - newVertex[1][1];
			magA = sqrt(A[0]*A[0] + A[1]*A[1]);
			magB = sqrt(B[0]*B[0] + B[1]*B[1]);
			magC = sqrt(C[0]*C[0] + C[1]*C[1]);
			cosAB = (A[0]*B[0] + A[1]*B[1]) / (magA*magB);
			cosCD = -(B[0]*C[0] + B[1]*C[1]) / (magB*magC);
			// computing angle between A and B vector
			omg1 = acos(cosAB);
			// computing angle between C and D vector
			omg2 = acos(cosCD);
			//printf("OMG: %.9f OMG1: %.9f OMG2: %.9f\n",omg,omg1,omg2);
			// checking if omg1 < omg and PI - omg1 < omg
			if ((omg1 < omg) || (M_PI - omg1) < omg){
				DETcheck = 0;
			}
			// checking if omg2 < omg and PI - omg2 < omg
			if ((omg2 < omg) || (M_PI - omg2) < omg){
				DETcheck = 0;
			}
		}
			
		if (DETcheck == 1) break;
	}while(1);

	Free2d(cdt->GaussPolygon[i].V);
	Free2d(cdt->GaussPolygon[i].H);
	Free(cdt->GaussPolygon[i].B);

	cdt->GaussPolygon[i].V = (double **)calloc2d(k0[1] - k0[0] + 2, 2, sizeof(double));
	cdt->GaussPolygon[i].V[0][0] = newVertex[1][0];
	cdt->GaussPolygon[i].V[0][1] = newVertex[1][1];
	cdt->GaussPolygon[i].V[1][0] = newVertex[0][0];
	cdt->GaussPolygon[i].V[1][1] = newVertex[0][1];
	cdt->GaussPolygon[i].numberOfVertices = 2;

	cdt->GaussPolygon[ip].V = (double **)calloc2d(P.numberOfVertices - (k0[1] - k0[0]) + 2, 2, sizeof(double));
	cdt->GaussPolygon[ip].V[0][0] = newVertex[0][0];
	cdt->GaussPolygon[ip].V[0][1] = newVertex[0][1];
	cdt->GaussPolygon[ip].V[1][0] = newVertex[1][0];
	cdt->GaussPolygon[ip].V[1][1] = newVertex[1][1];
	cdt->GaussPolygon[ip].numberOfVertices = 2;
  
	for(k = 0; k < P.numberOfVertices; k++) {
		k1 = (k + k0[0] + 1) % P.numberOfVertices;
		k2 = (k + k0[1] + 1) % P.numberOfVertices;
		if(cdt->GaussPolygon[i].numberOfVertices < k0[1] - k0[0] + 2){
			cdt->GaussPolygon[i].V[cdt->GaussPolygon[i].numberOfVertices][0] = P.V[k1][0];
			cdt->GaussPolygon[i].V[cdt->GaussPolygon[i].numberOfVertices][1] = P.V[k1][1];
			cdt->GaussPolygon[i].numberOfVertices++;
		}
		if(cdt->GaussPolygon[ip].numberOfVertices < P.numberOfVertices - (k0[1] - k0[0]) + 2){
			cdt->GaussPolygon[ip].V[cdt->GaussPolygon[ip].numberOfVertices][0] = P.V[k2][0];
			cdt->GaussPolygon[ip].V[cdt->GaussPolygon[ip].numberOfVertices][1] = P.V[k2][1];
			cdt->GaussPolygon[ip].numberOfVertices++; 
		}
	}
	  
	// The width function of Polygon i is computed.
	WidthFunction(cdt, (cdt->GaussPolygon) + i);
	PerimeterPolygon((cdt->GaussPolygon) + i);
	AreaPolygon((cdt->GaussPolygon) + i);
	RoundnessPolygon((cdt->GaussPolygon) + i);	
    
  // The width function of Polygon ip is computed.
	WidthFunction(cdt, (cdt->GaussPolygon) + ip);
	PerimeterPolygon((cdt->GaussPolygon) + ip);
	AreaPolygon((cdt->GaussPolygon) + ip);
	RoundnessPolygon((cdt->GaussPolygon) + ip);	
	
	// The lifetime of the polygon is determined. The lifetime
	// is exponentially distribuited with parameter bPhi, which is
	// the weighted sum of the width and the weights are the probabilities
	// Phi[j].
	
	// Lifetime of Polygon i.
	//Perimeter
	if(lOption == 0){
		bPhi = 0.;
		for(j = 0; j < NUMBER_OF_INT; j++){
			bPhi += (cdt->GaussPolygon[i].B[j]) * (cdt->Phi[j]);
		}
		cdt->GaussPolygon[i].tau = RandomExponential(bPhi);
	}
	// Area
	else{
		cdt->GaussPolygon[i].tau = RandomExponential(cdt->GaussPolygon[i].area);
	}
	cdt->GaussPolygon[i].beta = P.time;
	cdt->GaussPolygon[i].time = cdt->GaussPolygon[i].beta + cdt->GaussPolygon[i].tau;
	
	// Lifetime of Polygon ip.
	// Perimeter
	if(lOption == 0){
		bPhi = 0.;
		for(j = 0; j < NUMBER_OF_INT; j++){
			bPhi += (cdt->GaussPolygon[ip].B[j]) * (cdt->Phi[j]);
		}
		cdt->GaussPolygon[ip].tau = RandomExponential(bPhi);
	}
	// Area
	else{
		cdt->GaussPolygon[ip].tau = RandomExponential(cdt->GaussPolygon[ip].area);
	}
	cdt->GaussPolygon[ip].beta = P.time;
	cdt->GaussPolygon[ip].time = cdt->GaussPolygon[ip].beta + cdt->GaussPolygon[ip].tau;
	
	// The two vertices of the I_Segment are assgined.
	cdt->I_Segment[cdt->numberOfI_Segments].x0 = newVertex[0][0];
	cdt->I_Segment[cdt->numberOfI_Segments].y0 = newVertex[0][1]; 
	cdt->I_Segment[cdt->numberOfI_Segments].x1 = newVertex[1][0]; 
	cdt->I_Segment[cdt->numberOfI_Segments].y1 = newVertex[1][1];
	// The length of the I_Segment is computed.
	deltaX = newVertex[1][0] - newVertex[0][0];
	deltaY = newVertex[1][1] - newVertex[0][1];
	cdt->I_Segment[cdt->numberOfI_Segments].length = sqrt(deltaX*deltaX + deltaY*deltaY);
	// The birthtime of the I_Segment is assigned.
	cdt->I_Segment[cdt->numberOfI_Segments].beta = P.time;

	Free2d(P.V);
}
#undef EPS

/**
	@function STIT2dIsoGauss
	----------------------
	Generates a 2D Gaussian modified STIT in a quadratic window of side length a.

	[IN]
	@param a: side length of the window.
	@param time_stop: the stopping time for the STIT generation (the parameter 
	of the STIT)
	@param sigma: Gauss standard deviation.
	
	[OUT]
	@param Polygon: list of the polygons (cells) of the STIT.
	@param NumberOfPolygons: total number of polygons (cells) of the STIT.
	@param I_Segment: list of the I-segments. Notice that the first four segments
	contained in this list are the edges of the square window, i.e the first 
	I-segment is pointed at I_Segment + 4.
	@param a: side length of the window.
	@param time_stop: stopping time of the STIT process.	 
**/  
int STIT2dGauss(CDT2d *cdt, double timeGaussStop, double sigma, unsigned long lOption, double omg){
	unsigned long i;
	
	// state variable, 0: stit process stops, 1: stit process continues.
	unsigned int stitStop;
	
	// number of polygons in the current level of the branching tree, which are 
	// divided before time_stop. The branching tree describes the cell division 
	// process. Also q is an auxiliary variable for storing new polygons.
	unsigned int q;
	
	// current number of polygons.
	unsigned int numPol;
	
	// current line candidate in the rejection process.	
	LINE dirLine;
	
	gsl_rng *r;
  
	// create random number generator 
	r = gsl_rng_alloc(gsl_rng_mt19937);

	stitStop = 1;
	do{
		numPol = cdt->numberOfGaussPolygons;
		q = 0;
		for(i = 0; i < numPol; i++){
			// checking if the polygon i must be divided.
			if(cdt->GaussPolygon[i].time < timeGaussStop){
				dirLine = GaussLinePolygon(cdt, i, sigma, r);
				
				if(cdt->numberOfGaussPolygons == MAX_NUMBER_OF_POLYGONS){
					printf("Number of Polygons exceeds %dld\n", MAX_NUMBER_OF_POLYGONS);
					return 1;
				}
				if(cdt->numberOfI_Segments == MAX_NUMBER_OF_I_SEGMENTS) {
					printf("Number of I_Segments exceeds %dld\n", MAX_NUMBER_OF_I_SEGMENTS);
					return 1;
				}
				GaussIntersectionPolygon(cdt, i, numPol + q, dirLine, sigma, r, lOption, omg);
				q++;
				(cdt->numberOfGaussPolygons)++;
				(cdt->numberOfI_Segments)++;
			}
		}
    	// the proccess stops.
    	if(q == 0) stitStop = 0;
  	}while(stitStop);
	
	gsl_rng_free(r);

	return 0;
}

/*
*/
int STIT2dGaussDisturbed(CDT2d *cdt, double timeGaussStop, double sigma, double bEllip, unsigned long lOption, double omg){
	unsigned long i;
	
	// state variable, 0: stit process stops, 1: stit process continues.
	unsigned int stitStop;
	
	// number of polygons in the current level of the branching tree, which are 
	// divided before time_stop. The branching tree describes the cell division 
	// process. Also q is an auxiliary variable for storing new polygons.
	unsigned int q;
	
	// current number of polygons.
	unsigned int numPol;
	
	// current line candidate in the rejection process.	
	LINE dirLine;
	
	gsl_rng *r;
  
	// create random number generator 
	r = gsl_rng_alloc(gsl_rng_mt19937);

	stitStop = 1;
	do{
		numPol = cdt->numberOfGaussPolygons;
		q = 0;
		for(i = 0; i < numPol; i++){
			// checking if the polygon i must be divided.
			if(cdt->GaussPolygon[i].time < timeGaussStop){
				dirLine = GaussLinePolygonDisturbed(cdt, i, sigma, r, bEllip);
				
				if(cdt->numberOfGaussPolygons == MAX_NUMBER_OF_POLYGONS){
					printf("Number of Polygons exceeds %dld\n", MAX_NUMBER_OF_POLYGONS);
					return 1;
				}
				if(cdt->numberOfI_Segments == MAX_NUMBER_OF_I_SEGMENTS) {
					printf("Number of I_Segments exceeds %dld\n", MAX_NUMBER_OF_I_SEGMENTS);
					return 1;
				}
				GaussIntersectionPolygonDisturbed(cdt, i, numPol + q, dirLine, sigma, r, bEllip, lOption, omg);
				q++;
				(cdt->numberOfGaussPolygons)++;
				(cdt->numberOfI_Segments)++;
			}
		}
    	// the proccess stops.
    	if(q == 0) stitStop = 0;
  	}while(stitStop);
	
	gsl_rng_free(r);

	return 0;
}

/**
@function PlotCDT2d
**/
void PlotCDT2d(CDT2d *cdt){
	unsigned long i;
	char buffer[15];
	FILE *file;
	
	sprintf(buffer,"CDT2d.tex");
	file = fopen(buffer, "w");
	
	fprintf(file, "\\documentclass[10pt]{article}\n");
	fprintf(file, "\\usepackage{tikz}\n");
	fprintf(file,"\\usepackage[active,pdftex,tightpage]{preview}\n");
	fprintf(file,"\\PreviewEnvironment[{[]}]{tikzpicture}\n");
	fprintf(file,"\\pagestyle{empty}\n");
	fprintf(file, "\\begin{document}\n");
	fprintf(file, "\\begin\{tikzpicture}[x = 10cm,y = 10cm]\n");
   
	for(i = 0; i < cdt->numberOfI_Segments; i++)
		fprintf(file, "\\draw[thick](%lf,%lf) -- (%lf,%lf);\n", cdt->I_Segment[i].x0, cdt->I_Segment[i].y0, cdt->I_Segment[i].x1, cdt->I_Segment[i].y1);

	fprintf(file, "\\end\{tikzpicture}\n");
	fprintf(file, "\\end\{document}\n");

	fclose(file);
}

/** Allocation of a 2d array. 
    Allocates a continuous area in memory for a 2d array x[0..m-1][0..n-1] 
    of items of size ny. The area is not initialized.   
    @param m [IN] number of rows  
    @param n [IN] number of collumns  
    @param ny [IN] size of an item (element)  
    @return either a null pointer if unable to allocate, or a void pointer
            to the allocated area
***/
void **malloc2d(unsigned long m, unsigned long n, size_t ny) {
  unsigned long i;
  unsigned char *x;
  void* *y;
   
  /* sanity check */
  if( m==0 || n==0 || ny==0) {
    assert( !(m==0 || n==0 || ny==0) );
    return NULL;
  }
  if(m>ULONG_MAX/n) { 
    assert(m<=ULONG_MAX/n); 
    return NULL; 
  }
  if(m*n>ULONG_MAX/ny) { 
    assert(m*n<=ULONG_MAX/ny); 
    return NULL; 
  }

  /* work part */
  if((x=(unsigned char *)malloc(m*n*ny))==NULL) {
    assert(x!=NULL); 
    return NULL; 
  }
  if((y=(void *)malloc(m*sizeof(void *)))==NULL) {
    assert(y!=NULL); 
    free((char *)x); 
    return NULL; 
  }

  for(i=0;i<m;i++) 
    y[i]=&x[i*n*ny];
  
  return (void **)y;
}

/** Allocation and initialization of a 2d array. 
    Allocates a continuous area in memory for a 2d array x[0..m-1][0..n-1] 
    of items of size ny. The area is initialized to all bits zero.   
    @param m [IN] number of rows  
    @param n [IN] number of collumns  
    @param ny [IN] size of an item (element)  
    @return either a null pointer if unable to allocate, or a void pointer to the allocated area
**/
void **calloc2d(unsigned long m, unsigned long n, size_t ny) {
  unsigned long i;
  unsigned char *x;
  void* *y;

  /* sanity check */
  if( m==0 || n==0 || ny==0) {
    assert( !(m==0 || n==0 || ny==0) );
    return NULL;
  }
  if(m>ULONG_MAX/n) { 
    assert(m<=ULONG_MAX/n); 
    return NULL; 
  }
  if(m*n>ULONG_MAX/ny) { 
    assert(m*n<=ULONG_MAX/ny); 
    return NULL; 
  }

  /* work part */
  if((x=(unsigned char *)calloc(m*n,ny))==NULL) {
    assert(x!=NULL); 
    return NULL;
  }
  if((y=(void *)malloc(m*sizeof(void *)))==NULL) {
    assert(y!=NULL); 
    free((char *)x); 
    return NULL; 
  }
  for(i=0;i<m;i++) 
     y[i]=&x[i*n*ny];

  return (void **)y;
}

/** Delallocation of a 2d array.
    Deallocates the memory area pointed to by x, that was allocated
    by a previous malloc2d or calloc2d. If x is null, no action occurs.
    @param x [IN] the 2d array to be allocated    
**/
void free2d(
  char **x
  )
{
  /* sanity check */
  if(x==NULL) return;

  free((char *)&(x[0][0])); 
  free((char *)x); 
  x = NULL;
} 

double RandomUniform(void)
{  
  return (double)random()/(double)RAND_MAX; 
}
