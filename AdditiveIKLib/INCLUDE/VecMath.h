#ifndef VECMATHH
#define VECMATHH

#include <d3dx10.h>

float ChaVector3Dot( ChaVector3* vec1, ChaVector3* vec2 );
int ChaVector3Cross( ChaVector3* outvec, ChaVector3* vec1, ChaVector3* vec2 );
float ChaVector3Length( ChaVector3* v );
int ChaVector3Normalize( ChaVector3* outv, ChaVector3* inv );

#endif