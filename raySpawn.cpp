//
// Created by felix on 14/02/2020.
//

#include <cstdlib>
#include <iostream>
using namespace std;
#include <math.h>
#include "raySpawn.h"

RAY *DebugSpawner(int count, float x, float y, float z, float xr, float yr, float zr, float offset) {
    RAY *raystack = (RAY *) malloc(sizeof(RAY)*count);
    RAY tempray;
    tempray.Zdir = zr;
    tempray.Xorigin = x;
    tempray.Yorigin = y;
    tempray.Zorigin = z;
    int sq =(int) sqrt(count);
    int hsq = sq/2;



    for (int i = 0; i < count; i++) {
        float xof =xr + (((i/sq)-(hsq))*offset); //generate ofset grid
        float yof =yr + (((i%sq)-(hsq))*offset);

        cout << xof << " | " << yof << "\n";

        tempray.Xdir = xof;
        tempray.Ydir = yof;

        tempray.rayid = i;

        raystack[i]=tempray;
        cout<<"rays x x"<<tempray.Xorigin<<" "<<tempray.Yorigin<<" "<<tempray.Zorigin<<" "<<tempray.Xdir<<" "<<tempray.Ydir<<" "<<tempray.Zdir<<"\n";
    }
    return raystack;
}
