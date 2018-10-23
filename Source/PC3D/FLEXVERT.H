#ifndef  FLEXVERT_H
#define  FLEXVERT_H


// FLEXIBLE VERTEX FORMAT DEFINITIONS
//-----------------------------------


// The storage of each vertex follows a strict ordering. There is always a vector
// component to the vertex, but the remaining fields are optional (uv's, normals, user data).
// All fields occur in a predefined order in the "imaginary" vertex structure: 
//  
// struct FlexibleVertex 
// {
//    vertex position;
//    float rhw;
//    vertex normal;
//    DWORD diffuse;
//    DWORD specular;
//    float tu1,tv1;  // texture coordinates for up to eight textures
//    float tu2,tv2;
//    float tu3,tv3;
//    float tu4,tv4;
//    float tu5,tv5;
//    float tu6,tv6;
//    float tu7,tv7;
//    float tu8,tv8; 
// };


// flexible vertex type definitions

typedef D3DVERTEX flexVertex1; 
typedef D3DLVERTEX flexVertex2; 
typedef D3DTLVERTEX flexVertex3;

// untransformed vertex with two sets of texture coordinates
struct flexVertex4
{
  D3DVECTOR position;
  float     rhw;
  DWORD     diffuse;
  DWORD     specular;
  float     tu1,tv1;  
  float     tu2,tv2;  
};

// basic transformed vertex
struct flexVertex5
{
  D3DVECTOR position;
  float     rhw;
  DWORD     diffuse;
  DWORD     specular;
  float     tu1,tv1;
};

#endif
