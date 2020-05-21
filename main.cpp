#include <iostream>
#include <library.h>
#include "main.h"
#include "raySpawn.h"
#include "CLFUNC.h"


int main() {
    std::cout << "Hello, World!" << std::endl;
    initCL();
    //initGL();
    std::cout << "goodbye, World!" << std::endl;
    return 0;
}
void initGL(){

}
void initCL(){
    CLFUNC clf = CLFUNC(false) ;
    int raystacksize = 5;
    RAY * Raystack = DebugSpawner(raystacksize ,1.1f,1.1f,1.1f,0.0f,10.0f,10.0f,0.0f);
    clf.runcolisions(Raystack,raystacksize);
}
