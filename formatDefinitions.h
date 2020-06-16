//
// Created by felix on 14/02/2020.
//

#ifndef RAYSIM_FORMATDEFINITIONS_H
#define RAYSIM_FORMATDEFINITIONS_H

#include <vector_types.h>
#include <CL/cl_platform.h>
#include <iostream>
#include <fstream>
using namespace std;

/* FORMAT AND TYPE DEFENITIONS FOR SAVEFILES AND OPERATIONS
 *
 *
 *      Structs and Types
 *          - Simstate
 *          - SpatialDefenitions
 *          - anntenas
 *          - Extris
 *          - Rays
 *
 *
 *
 *
 *
 *
 */
enum TID{
    //version based id for eventual backwards compatibility
    SimstateV1 = 0,
    SpatialDeffinitionsV1 = 1,
    antennasV1 = 2,
    ExtrisV1 = 3,
    RaysV1 = 4,


    //Current id for

    Simstate = 0,
    SpatialDeffinitions = 1,
    antennas = 2,
    Extris = 3,
    Rays = 4,

};

typedef struct Vertex{
    unsigned long A;
    unsigned long B;
    unsigned long C;
    unsigned long Facehash;
    float reflectivity;
    float transparency;
} Vertex;

typedef struct SimState{ //simstate v1 simulation state info

//Simulation flags
    bool Blank;
    bool startstackCreated;
    bool startstackRunning;
    bool startstackHitBounceLimmit;
    bool RanWithError ;
    bool Completed ;

    bool resulthandled;
    bool debugRaystore;
    bool debugRayshow;

} SimState;

typedef struct SpatialDefinitions{ //spatialdefinitions v1 simulation room size & type
    bool squareRoom;
    float Ox;
    float Oy;
    float Oz;

    float Ex;
    float Ey;
    float Ez;

    int count;
    int saveid;
    Vertex * vertStack;


}SpatialDefinitions;

typedef struct Antenna{ //Antenna v1 simulation
    bool std;
    //position
    float Ox;
    float Oy;
    float Oz;
    //front direction
    float Ex;
    float Ey;
    float Ez;

    int tfcount; //floats in list
    float * top; //topview clockwise increments 360/n
    int rfcount;
    float * right; //idem right side
    int ffcount;
    float * front; //idem front




}Antenna;




typedef struct RAY{
    float Xorigin; //
    float Yorigin; //
    float Zorigin; // Spatial position at start

    float wStart; // traveldistance at begin point


    float Xdir;
    float Ydir;
    float Zdir;
    float startphase; //phase at traveldistance 0

    float xEnd; //
    float yEnd; //
    float zEnd; // Spatial position at end
    float wEnd; // traveldistance at end point

    bool final = false; // is fully calculated
    long rayid;     // id of origin ray
    int refcount; // reflectioncount

} RAY ;
typedef struct CL_Rayinfo{
    cl_float3 S; //source
    cl_float3 D; //destination //length is accurate but relative to startpoint
    cl_float3 polarisation; //polarisation //normalised vector for now
    cl_float totaltravel;
    cl_uint faceseed;
    cl_int lastface;

}CL_Rayinfo ;
typedef struct CL_Faceinfo{
    cl_uint faceseed;
    cl_float3 normal;
    cl_float ref;
    cl_float trans;

} CL_Faceinfo ;




CL_Rayinfo convert (RAY);
CL_Rayinfo * convert (RAY *, int);

float** tristackgen(float*,int skibef,int skiaft,int tricount);



class formatDefinitions {


    bool READ;
    ofstream myfileout;
    ifstream myfilein;

    string startblock = std::string("RAYSIMSTORE.");
    string BlockCLRAYS = std::string("CL_RAY_REV1.");
    string BlockMETAROOM = std::string("METASTORE.");
    string BlockFIELDND3D = std::string("FIELDMULTIDIM.");
    string BlockEND = std::string("ENDRAYSIMSTORE.");



public:
    CL_Rayinfo * R_rayinfo;
    int R_rayinfo_L = 0;



    formatDefinitions(bool read, const char *filename);
    void store(CL_Rayinfo * , int);
    void storeField(float * field, int dims, int size,int steps);
    void storemeta(cl_float3 roomsize,cl_float3 antenaorigin,cl_float3 testvolorigin,cl_float3 testvolsize);
    void read(CL_Rayinfo * , int);
    void close();

};


#endif //RAYSIM_FORMATDEFINITIONS_H
