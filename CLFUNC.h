//
// Created by felix on 14/02/2020.
//

#ifndef RAYSIM_CLFUNC_H
#define RAYSIM_CLFUNC_H

#include <cmath>
#include <library.h>
#include "formatDefinitions.h"

class CLFUNC {
    CLCNTXT CTXT;
    chartable chart1;
    cl_kernel fibRayGen;
    cl_kernel FACEPREP;
    cl_kernel REFLECT;
    cl_kernel MAIN;
    cl_kernel resND;
    cl_kernel ASSOC;
    cl_kernel STORE;
    cl_kernel DEBUGSTORE;

    //room data
    float Xsize = 5.0f;
    float Ysize = 5.0f;
    float Zsize = 5.0f;

    float simhz = 90000000;
    //float simhz = 2000000000;

    int resdc; //count per dimension
    float resdsx; //size per dimension
    float celsizeAct ;
    int resd; //dimensions
    int steps; //frames in time
    float angleofset; //angle ofset

    cl_float3 testcubeorigin,antennaorigin,polarisation ;



public:
    char * path = "ydat.SDAT";
    CLFUNC(bool = false);

    void setupAuto();

    void runcolisions(RAY *, int);

    void runcolisions( int);

    int handleinputfile(char *);



    bool trymap( float wdim, size_t maxsize,int phasesteps);

    bool meminfRAY(int rays);
};


#endif //RAYSIM_CLFUNC_H
