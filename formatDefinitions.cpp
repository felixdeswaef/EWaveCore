//
// Created by felix on 14/02/2020.
//

#include <cstdlib>
#include "formatDefinitions.h"

CL_Rayinfo convert(RAY R) {
    CL_Rayinfo ret;
    ret.S.x = R.Xorigin;
    ret.S.y = R.Yorigin;
    ret.S.z = R.Zorigin;
    ret.D.x = R.Xdir;
    ret.D.y = R.Ydir;
    ret.D.z = R.Zdir;



    return ret;
}
//create points from normal float array


CL_Rayinfo *convert(RAY * ostack, int size) {
    CL_Rayinfo * nstack =(CL_Rayinfo *) malloc(sizeof(CL_Rayinfo)*size);
    for(int i = 0 ; i< size ;i++){
        nstack[i] = convert(ostack[i]);
    }


    return nstack;
}

float **tristackgen(float * verts, int skibef, int skiaft,int tricount) {
    float** fltr =(float **) malloc(sizeof(float *) *3);
    fltr[0] = (float*) malloc(sizeof(float)*tricount);
    fltr[1] = (float*) malloc(sizeof(float)*tricount);
    fltr[2] = (float*) malloc(sizeof(float)*tricount);
    for (int i = 0 ; i<tricount ; i++){
        int it = (skibef+skiaft+3*i)+skibef;
        fltr[0][i] = verts[it];
        fltr[1][i] = verts[it+1];
        fltr[2][i] = verts[it+2];

    }


    return fltr;
}

formatDefinitions::formatDefinitions(bool rd,const char* filename) {

    READ = rd;
    if (READ) {
        myfilein.open(filename,std::ofstream::in);
        string val ;
        myfilein >> val;
        if (val != startblock) return;
        while(!myfilein.eof()){
            int count;
            myfilein >> val;
            if (val==BlockCLRAYS){
                myfilein >> count;
                //TODO MALLOC
                R_rayinfo_L =count;
                R_rayinfo = (CL_Rayinfo*) malloc(sizeof(CL_Rayinfo)*R_rayinfo_L);
                read(R_rayinfo,R_rayinfo_L);


            }
        }

    } else{

        //SIGTRAP FROM KERNEL ? nope
        myfileout.open(filename,std::ofstream::out | std::ofstream::trunc);
        if (!myfileout.is_open()) return ;

        myfileout << startblock<<std::endl;
    }
}

void formatDefinitions::store(CL_Rayinfo * info, int count) {
    myfileout << BlockCLRAYS<<std::endl;
    myfileout << count<<std::endl;
    for (int i ; i< count ;i++){
        myfileout << info[i].S.x<<std::endl<< info[i].S.y<<std::endl<< info[i].S.z<<std::endl;


        myfileout << info[i].D.x<<std::endl<< info[i].D.y<<std::endl<< info[i].D.z<<std::endl;


        myfileout << info[i].polarisation.x<<std::endl<< info[i].polarisation.y<<std::endl<< info[i].polarisation.z<<std::endl;


        myfileout << info[i].faceseed<<std::endl;
        myfileout << info[i].totaltravel<<std::endl;
    }

}

void formatDefinitions::read(CL_Rayinfo * info, int count) {
    char disc;

    for (int i ; i< count ;i++){
        myfilein >> info[i].S.x;
        myfilein >> info[i].S.y;
        myfilein >> info[i].S.z;

        myfilein >> info[i].D.x;
        myfilein >> info[i].D.y;
        myfilein >> info[i].D.z;

        myfilein >> info[i].polarisation.x;
        myfilein >> info[i].polarisation.y;
        myfilein >> info[i].polarisation.z;

        myfilein >> info[i].faceseed;
        myfilein >> info[i].totaltravel;

    }

}

void formatDefinitions::close() {

    if (READ) myfilein.close();
    if (!READ) {
        myfileout<<BlockEND;
        myfileout.close();
    }
}

void formatDefinitions::storeField(float *field, int dims, int size,int steps) {
    myfileout << BlockFIELDND3D<<std::endl;
    myfileout << dims<<std::endl;
    myfileout << size<<std::endl;
    myfileout << steps<<std::endl;
    int maxpos =size*size*(dims>2? size : 1 )*steps;

    myfileout << maxpos<<std::endl;
    for (int i=0 ; i<maxpos ;i++){
        //myfileout <<std::endl;
        myfileout << field[i*3]<<std::endl;
        myfileout << field[i*3+1]<<std::endl;
        myfileout << field[i*3+2]<<std::endl;

    }
}

void formatDefinitions::storemeta(cl_float3 roomsize, cl_float3 antenaorigin, cl_float3 testvolorigin,
                                  cl_float3 testvolsize) {
    myfileout << BlockMETAROOM<<std::endl;
    myfileout << roomsize.x <<std::endl << roomsize.y<<std::endl << roomsize.z<<std::endl;
    myfileout << antenaorigin.x <<std::endl << antenaorigin.y<<std::endl << antenaorigin.z<<std::endl;
    myfileout << testvolorigin.x <<std::endl << testvolorigin.y<<std::endl << testvolorigin.z<<std::endl;
    myfileout << testvolsize.x <<std::endl << testvolsize.y<<std::endl << testvolsize.z<<std::endl;

}
