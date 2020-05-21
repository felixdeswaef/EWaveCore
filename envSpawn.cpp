//
// Created by felix on 20/02/2020.
//

#include <CL/cl_platform.h>
#include "envSpawn.h"
 // MASSIVE PROBLEM POSSIBLE MEMORY LEAK CAUSES HEAP CORRUPTION AND FAILS UPCOMMING MALLOCS
cl_float3 ** gettrisstack() {

    cl_float3 ** floater = (cl_float3 **) malloc(sizeof(cl_float3 *) * 3 );



    cl_float3 *trisA = (cl_float3 *) malloc(sizeof(cl_float3) * 12);
    cl_float3 *trisB = (cl_float3 *) malloc(sizeof(cl_float3) * 12);
    cl_float3 *trisC = (cl_float3 *) malloc(sizeof(cl_float3) * 12);

    floater[0] = trisA;
    floater[1] = trisB;
    floater[2] = trisC;


    float h = (float) 5;
    float d = (float) 5;
    float w = (float) 5;
    float o = (float) 0;

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


    }

    return (floater);
}