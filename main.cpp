#include <iostream>
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
    clf.trymap(2.0f, sizeof(char)*10000000,1);
    int raystacksize =3000;
    //todo Watchdog kills kernel above n seconds kernel runtime
    clf.meminfRAY(raystacksize);
    RAY * Raystack;// = DebugSpawner(raystacksize ,1.1f,1.1f,1.1f,0.0f,10.0f,10.0f,0.0f);
    clf.runcolisions(Raystack,raystacksize);
}
