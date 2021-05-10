#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gmath.h"
#include "matrix.h"
#include "ml6.h"


//vector functions
//normalize vetor, should modify the parameter
void normalize( double *vector ) {

	vector[0] = 0;
	vector[1] = 0;
	vector[2] = 1;
}

//Return the dot porduct of a . b
double dot_product( double *a, double *b ) {

	double answer;
	answer = (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
	return answer;
}


//Calculate the surface normal for the triangle whose first
//point is located at index i in polygons
double *calculate_normal(struct matrix *polygons, int i) {
	
	double x0 = polygons->m[0][i];
	double y0 = polygons->m[1][i];
	double z0 = polygons->m[2][i];

	double x1 = polygons->m[0][i+1];
	double y1 = polygons->m[1][i+1];
	double z1 = polygons->m[2][i+1];

	double x2 = polygons->m[0][i+2];
	double y2 = polygons->m[1][i+2];
	double z2 = polygons->m[2][i+2];

	double A[] = {x1 - x0, y1 - y0, z1 - z0};
	double B[] = {x2 - x0, y2 - y0, z2 - z0};

	double * N = malloc(sizeof(double) * 3);
	N[0] = A[1]*B[2] - A[2]*B[1];
	N[1] = A[2]*B[0] - A[0]*B[2];
	N[2] = A[0]*B[1] - A[1]*B[0];

	return N;
}
