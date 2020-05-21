//
// Created by felix on 14/02/2020.
//

#ifndef RAYSIM_CLFUNC_H
#define RAYSIM_CLFUNC_H


#include <library.h>
#include "formatDefinitions.h"

class CLFUNC {
    CLCNTXT CTXT;
    chartable chart1;
    cl_kernel FACEPREP;
    cl_kernel REFLECT;
    cl_kernel MAIN;
    cl_kernel STORE;
    cl_kernel DEBUGSTORE;

public: CLFUNC(bool = false);

    void setupAuto();
    void runcolisions(RAY *,int );
};


#endif //RAYSIM_CLFUNC_H
