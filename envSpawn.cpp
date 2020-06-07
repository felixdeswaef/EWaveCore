//
// Created by felix on 20/02/2020.
//

#include <CL/cl_platform.h>
#include <iostream>
#include "envSpawn.h"
 // fixed memory leak
cl_float3 ** gettrisstack(float xsize,float ysize,float zsize) {

    cl_float3 ** floater = (cl_float3 **) malloc(sizeof(cl_float3 *) * 3 );



    cl_float3 *trisA = (cl_float3 *) malloc(sizeof(cl_float3) * 12);
    cl_float3 *trisB = (cl_float3 *) malloc(sizeof(cl_float3) * 12);
    cl_float3 *trisC = (cl_float3 *) malloc(sizeof(cl_float3) * 12);

    floater[0] = trisA;
    floater[1] = trisB;
    floater[2] = trisC;

     float verticesVolCube[] = {//for volumetric cube needs to be 0-1 range

             0.0f, 0.0f, 0.0f, //A
             1.0f, 0.0f, 0.0f, //B
             1.0f, 1.0f, 0.0f, //c
             0.0f, 1.0f, 0.0f, //D

             0.0f, 0.0f, 1.0f, //e
             1.0f, 0.0f, 1.0f,  //f
             1.0f, 1.0f, 1.0f,//g
             0.0f, 1.0f, 1.0f //H

     };
     unsigned int indicesVolCube[] = {
             0,3,1,
             1,3,2,

             4,0,1,
             1,5,4,

             0,4,7,
             7,3,0,

             4,5,6,
             6,7,4,

             5,1,2,
             2,6,5,

             3,7,6,
             6,2,3



     };
     float h = (float) xsize;
    float d = (float) ysize;
    float w = (float) zsize;
    float o = (float) 0;
     for (int i = 0; i < 12; i++) {
         trisA[i].x = verticesVolCube[indicesVolCube[i*3]*3]*xsize;
         trisA[i].y = verticesVolCube[indicesVolCube[i*3]*3+1]*ysize;
         trisA[i].z = verticesVolCube[indicesVolCube[i*3]*3+2]*zsize;

         trisB[i].x = verticesVolCube[indicesVolCube[i*3+1]*3]*xsize;
         trisB[i].y = verticesVolCube[indicesVolCube[i*3+1]*3+1]*ysize;
         trisB[i].z = verticesVolCube[indicesVolCube[i*3+1]*3+2]*zsize;

         trisC[i].x = verticesVolCube[indicesVolCube[i*3+2]*3]*xsize;
         trisC[i].y = verticesVolCube[indicesVolCube[i*3+2]*3+1]*ysize;
         trisC[i].z = verticesVolCube[indicesVolCube[i*3+2]*3+2]*zsize;

     }

/*
    float z = o;
    float z2 = o;
    for (int i = 0; i < 12; i++) {
        if (i < 2 ^ i > 9) {
            if (i > 2) {
                z = h;
            } else {
                z = o;
            }
            trisA[i].x = (i + 1) % 2 * w;
            trisA[i].y = i % 2 * d;
            trisA[i].z = z;

            trisB[i].x = o;
            trisB[i].y = o;
            trisB[i].z = z;

            trisC[i].x = w;
            trisC[i].y = d;
            trisC[i].z = z;
        } else {
            bool t;
            if (i > 6) {
                z = h;
                z2 = o;
                t = true;
            } else {
                z = o;
                z2 = h;
                t = true;
            }
            switch ((i - 1) % 4) {
                case 0:
                    trisA[i].x = t ? o : w;
                    trisA[i].y = o;
                    trisA[i].z = z;

                    trisB[i].x = w;
                    trisB[i].y = o;
                    trisB[i].z = z2;

                    trisC[i].x = o;
                    trisC[i].y = o;
                    trisC[i].z = z2;

                    break;

                case 1:
                    trisA[i].x = o;
                    trisA[i].y = t ? o : d;
                    trisA[i].z = z;

                    trisB[i].x = o;
                    trisB[i].y = d;
                    trisB[i].z = z2;

                    trisC[i].x = o;
                    trisC[i].y = o;
                    trisC[i].z = z2;

                    break;

                case 2:
                    trisA[i].x = t ? o : w;
                    trisA[i].y = d;
                    trisA[i].z = z;

                    trisB[i].x = w;
                    trisB[i].y = d;
                    trisB[i].z = z2;

                    trisC[i].x = o;
                    trisC[i].y = d;
                    trisC[i].z = z2;

                    break;
                case 3:
                    trisA[i].x = w;
                    trisA[i].y = t ? o : d;
                    trisA[i].z = z;

                    trisB[i].x = w;
                    trisB[i].y = d;
                    trisB[i].z = z2;

                    trisC[i].x = w;
                    trisC[i].y = o;
                    trisC[i].z = z2;

                    break;
            }


        }

        //std::cout<< "face "<<i<<"\n"<<trisA[i].x<<"   :   "<<trisA[i].y<<"   :   "<<trisA[i].z<<"::"<<trisB[i].x<<"   :   "<<trisB[i].y<<"   :   "<<trisB[i].z<<"::"<<trisC[i].x<<"   :   "<<trisC[i].y<<"   :   "<<trisC[i].z<<"\n";

    }*/

    return (floater);
}