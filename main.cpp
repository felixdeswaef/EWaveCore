#include <iostream>
#include "main.h"
#include "raySpawn.h"
#include "CLFUNC.h"


int main(int argc, char *argv[] ) {

    std::cout << "Hello, World! arc "<<argc <<argv[0] << std::endl;
    if (argc > 1){
        initCL(argv[1]);
    } else {
        initCL();
    }
    //initGL();
    std::cout << "goodbye, World!" << std::endl;
    return 0;
}
void initGL(){

}
void initCL(){
    CLFUNC clf = CLFUNC(false) ;
    clf.trymap(2.0f, sizeof(char)*10000000,1);
    int raystacksize =900000;
    //todo Watchdog kills kernel above n seconds kernel runtime
    clf.meminfRAY(raystacksize);
    RAY * Raystack;// = DebugSpawner(raystacksize ,1.1f,1.1f,1.1f,0.0f,10.0f,10.0f,0.0f);
    clf.runcolisions(raystacksize);
}
void initCL(char * path ){
    CLFUNC clf = CLFUNC(false) ;

    int raystacksize =clf.handleinputfile(path);
    clf.path = path;
    //clf.trymap(2.0f, sizeof(char)*10000000,1);
    //todo Watchdog kills kernel above n seconds kernel runtime


    clf.meminfRAY(raystacksize);

    clf.runcolisions(raystacksize);
}