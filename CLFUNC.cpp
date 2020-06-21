//
// Created by felix on 14/02/2020.
//

#include "CLFUNC.h"
#include "envSpawn.h"
#include "formatDefinitions.h"


 CLFUNC::CLFUNC(bool manual) {
        strcpy(chart1.name, "chart1");
        if (!manual) {
            setupAuto();
        } else {

        }
    }

    void CLFUNC::setupAuto() {
        setup(&CTXT, true); //does all necesary setup
        loadkernel("../kernels/F_REFV1.cl",&CTXT.context,&CTXT.device,&CTXT.program,&fibRayGen, "fibRayGen");
        loadkernel("../kernels/F_REFV1.cl",&CTXT.context,&CTXT.device,&CTXT.program,&FACEPREP, "FaceNorm"); //load kernel
        loadkernel("../kernels/F_REFV1.cl",&CTXT.context,&CTXT.device,&CTXT.program,&MAIN, "RayColGen"); //load kernel
        loadkernel("../kernels/F_REFV1.cl",&CTXT.context,&CTXT.device,&CTXT.program,&REFLECT, "RayRef"); //load kernel
        loadkernel("../kernels/F_REFV1.cl",&CTXT.context,&CTXT.device,&CTXT.program,&resND, "resND"); //load kernel
        loadkernel("../kernels/F_REFV1.cl",&CTXT.context,&CTXT.device,&CTXT.program,&ASSOC, "ASSOC"); //load kernel



    }

    void CLFUNC::runcolisions(int raystacksize) {

        //CL_Rayinfo * nstack = convert(raystack,raystacksize);



        CL_Rayinfo * outstack =(CL_Rayinfo *) malloc(raystacksize * sizeof(CL_Rayinfo));
        float * outfield =(float *) malloc(sizeof(float)*((resd==3)?resdc*resdc*resdc:resdc*resdc) *steps* 3);



        cl_float3 **trisstack = gettrisstack(Xsize,Ysize,Zsize);




        int stacksize = 12;
        bool verbose = false;
        int err;
        /* points
         * faces
         * rays
         *
         *
         */

        cl_mem APoints = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, stacksize * sizeof(cl_float3), nullptr,&err);
        std::cout<<err;
        cl_mem BPoints = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, stacksize * sizeof(cl_float3), nullptr,&err);
        std::cout<<err;
        cl_mem CPoints = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, stacksize * sizeof(cl_float3), nullptr,&err);
        std::cout<<err;
        cl_mem COLIDER = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, raystacksize*stacksize * sizeof(cl_float4), nullptr,&err);
        std::cout<<err;
        cl_mem Faceinfo = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, stacksize * sizeof(CL_Faceinfo), nullptr,&err);
        std::cout<<err;
        cl_mem raysA = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, raystacksize * sizeof(CL_Rayinfo), nullptr,&err);
        std::cout<<err;
        cl_mem field = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, sizeof(cl_float)*((resd==3)?resdc*resdc*resdc:resdc*resdc) *steps* 3, nullptr,&err);
        std::cout<<err;
        cl_mem assoca = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, raystacksize * sizeof(cl_int), nullptr,&err);
        std::cout<<err;
        cl_mem raysB = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, raystacksize * sizeof(CL_Rayinfo), nullptr,&err);
        cl_mem raysOverflow = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, raystacksize * sizeof(CL_Rayinfo), nullptr,&err);
        cl_mem raysStore = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, raystacksize * sizeof(CL_Rayinfo), nullptr,&err);


        cl_float nul = 0.0f;
        cl_event filwzero;
        err = clEnqueueFillBuffer(CTXT.commandQue, field,&nul, sizeof(cl_float),0 , sizeof(cl_float)*((resd==3)?resdc*resdc*resdc:resdc*resdc) *steps* 3,0,NULL,&filwzero);
        if (err!=0||verbose)std::cout<<"\nfilwzero enqued filling"<<resdc<<";"<<sizeof(cl_float)*((resd==3)?resdc*resdc*resdc:resdc*resdc) *steps* 3/1000000.0f<<"MB err:"<<err;
        clWaitForEvents(1,&filwzero);


        /* ARGUMENT ASSIGNMENT FOR ALL KERNEL
         * SUBJECT TO CHANGE DURING CYCLES
         *
         *
        */
        cl_event els[3];
        err = clEnqueueWriteBuffer(CTXT.commandQue, APoints, CL_TRUE, 0, stacksize * sizeof(cl_float3), trisstack[0], 0,
                NULL, &els[0]);

        if (err!=0||verbose)std::cout<<err;
        err = clEnqueueWriteBuffer(CTXT.commandQue, BPoints, CL_TRUE, 0, stacksize * sizeof(cl_float3), trisstack[1], 0,
                                   NULL, &els[1]);
        if (err!=0||verbose)std::cout<<err;
        err = clEnqueueWriteBuffer(CTXT.commandQue, CPoints, CL_TRUE, 0, stacksize * sizeof(cl_float3), trisstack[2], 0,
                                   NULL, &els[2]);
        if (err!=0||verbose)std::cout<<err;
        clWaitForEvents(3,els);




        //global
        size_t global_item_size[3], local_item_size[3]; // Process the entire lists
        local_item_size[0] = 32;
        local_item_size[1] = 32;

        antennaorigin.x = 2.5f;
        antennaorigin.y = 2.5f;
        antennaorigin.z = 2.5f;
        polarisation.x = 1.0f;
        polarisation.y = 0.0f;
        polarisation.z = 0.0f;
        testcubeorigin.x = 0.2f;
        testcubeorigin.y = 0.2f;
        testcubeorigin.z = 0.2f;

        long stz = (long) stacksize;

        int cachecount = 8;

        //faceprep
        err = clSetKernelArg(FACEPREP, 0, sizeof(cl_mem), &APoints);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(FACEPREP, 1, sizeof(cl_mem), &BPoints);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(FACEPREP, 2, sizeof(cl_mem), &CPoints);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(FACEPREP, 3, sizeof(cl_mem), &Faceinfo);
        if (err!=0||verbose)std::cout<<err<<" assigned Faceprep parameters\n";

        err = clSetKernelArg(fibRayGen, 0, sizeof(cl_mem), &raysA); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(fibRayGen, 1, sizeof(cl_float3), &antennaorigin); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(fibRayGen, 2, sizeof(cl_float3), &polarisation); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err<<" assigned Raygen parameters\n";

        err = clSetKernelArg(MAIN, 0, sizeof(cl_mem), &APoints);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(MAIN, 1, sizeof(cl_mem), &BPoints);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(MAIN, 2, sizeof(cl_mem), &CPoints);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(MAIN, 3, sizeof(cl_mem), &raysA);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(MAIN, 4, sizeof(cl_mem), &COLIDER);
        if (err!=0||verbose)std::cout<<err<<" assigned Collision parameters\n";

        err = clSetKernelArg(REFLECT, 0, sizeof(cl_mem), &raysA);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(REFLECT, 1, sizeof(cl_mem), &Faceinfo);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(REFLECT, 2, sizeof(cl_mem), &COLIDER);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(REFLECT, 3, sizeof(cl_long), &stz);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(REFLECT, 4, sizeof(cl_mem), &raysB);
        if (err!=0||verbose)std::cout<<err<<" assigned Reflection parameters\n";

        err = clSetKernelArg(ASSOC, 0, sizeof(cl_mem), &raysA);
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(ASSOC, 1, sizeof(cl_mem), &assoca);
        if (err!=0||verbose)std::cout<<err<<" assigned Asociation parameters\n";

        err = clSetKernelArg(resND, 0, sizeof(cl_mem), &field); //TODO arg pos change
        if (err!=0||verbose)std::cout<<"resnd"<<err<<"\n";
        err = clSetKernelArg(resND, 1, sizeof(cl_mem), &raysA); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err<<"\n";
        err = clSetKernelArg(resND, 2, sizeof(cl_float3), &testcubeorigin); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err<<"\n";

        err = clSetKernelArg(resND, 3, sizeof(cl_float), &celsizeAct); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(resND, 4, sizeof(cl_int), &resdc); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(resND, 5, sizeof(cl_float), &angleofset); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(resND, 6, sizeof(cl_int), &steps); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(resND, 7, sizeof(cl_int), &raystacksize); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(resND, 8, sizeof(cl_mem), &assoca); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err;
        err = clSetKernelArg(resND, 9, sizeof(cl_int), &cachecount); //TODO arg pos change
        if (err!=0||verbose)std::cout<<err<<" assigned Result kernel parameters\n";

        cl_event finished,readoutcomplete,associated,gen,col,ref;

        global_item_size[0] = stacksize;
        global_item_size[1] = stacksize;
        err = clEnqueueNDRangeKernel(CTXT.commandQue,FACEPREP,1, nullptr, global_item_size, nullptr, NULL, nullptr,nullptr);
        if (err!=0||verbose)std::cout<<err<<"enq faceprep \n";


        global_item_size[0] = raystacksize;
        global_item_size[1] = raystacksize;
        err = clEnqueueNDRangeKernel(CTXT.commandQue,fibRayGen,1, nullptr, global_item_size, nullptr, NULL, nullptr,&gen);
        if (err!=0||verbose)std::cout<<err<<"enq ray gen \n";
        err = clEnqueueReadBuffer(CTXT.commandQue, raysA, CL_TRUE, 0, raystacksize * sizeof(CL_Rayinfo), outstack, 0, NULL, &readoutcomplete);

        for (int R = 0; R <2;R++){ //reflection cycle loops
            cl_mem Primary,Secondary;
            if (R%2==0){
                Primary = raysA;
                Secondary = raysB;
            } else{
                Primary = raysB;
                Secondary = raysA;
            }
                err = clSetKernelArg(REFLECT, 0, sizeof(cl_mem), &Primary);
                if (err!=0||verbose)std::cout<<err;
                err = clSetKernelArg(REFLECT, 4, sizeof(cl_mem), &Secondary);
                if (err!=0||verbose)std::cout<<err;

                err = clSetKernelArg(ASSOC, 0, sizeof(cl_mem), &Primary);
                if (err!=0||verbose)std::cout<<err;

                err = clSetKernelArg(resND, 1, sizeof(cl_mem), &Primary);
                if (err!=0||verbose)std::cout<<err<<"\n";

                err = clSetKernelArg(MAIN, 3, sizeof(cl_mem), &Primary);
                if (err!=0||verbose)std::cout<<err;





            global_item_size[0] = raystacksize;
            global_item_size[1] = stacksize;
            err = clEnqueueNDRangeKernel(CTXT.commandQue,MAIN,2, nullptr, global_item_size, nullptr, 1, &gen,&col);
            if (err!=0||verbose)std::cout<<err<<"enq ray col \n";
            err = clEnqueueReadBuffer(CTXT.commandQue, Primary, CL_TRUE, 0, raystacksize * sizeof(CL_Rayinfo), outstack, 0, NULL, &readoutcomplete);


            global_item_size[0] = raystacksize;
            global_item_size[1] = stacksize;
            err = clEnqueueNDRangeKernel(CTXT.commandQue,REFLECT,1, nullptr, global_item_size, nullptr, 1, &col,&finished);
            if (err!=0||verbose)std::cout<<err<<"end ray ref \n";

            err = clEnqueueReadBuffer(CTXT.commandQue, Primary, CL_TRUE, 0, raystacksize * sizeof(CL_Rayinfo), outstack, 0, NULL, &readoutcomplete);


            global_item_size[0] = raystacksize;
            err = clEnqueueNDRangeKernel(CTXT.commandQue,ASSOC,1, nullptr, global_item_size, nullptr, 0, nullptr,&associated);
            if (err!=0||verbose)std::cout<<err<<"end association \n";

            global_item_size[0] = resdc;
            global_item_size[1] = resdc;
            global_item_size[2] = resdc;
            cl_event finishedres;
            err = clEnqueueNDRangeKernel(CTXT.commandQue,resND,resd, nullptr, global_item_size, nullptr, 1, &associated,&finishedres);
            if (err!=0||verbose)std::cout<<err<<"end result \n";



            clWaitForEvents(1,&finishedres);
            if (err!=0||verbose)std::cout<<err;
            cl_ulong time_start;
            cl_ulong time_end;

            clGetEventProfilingInfo(finishedres, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
            clGetEventProfilingInfo(finishedres, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);

            double nanoSeconds =(double) time_end-time_start;
            float milliSeconds = ((time_end-time_start)/ 1000000.0f);
            printf("Result kernel Execution time is: %0.3f milliseconds \n",milliSeconds );


        }



        err = clEnqueueReadBuffer(CTXT.commandQue, field, CL_TRUE, 0,  sizeof(cl_float)*((resd==3)?resdc*resdc*resdc:resdc*resdc) *steps* 3, outfield, 0, NULL, &readoutcomplete);
        //err = clEnqueueReadBuffer(CTXT.commandQue, raysA, CL_TRUE, 0, raystacksize * sizeof(CL_Rayinfo), outstack, 0, NULL, &readoutcomplete);
        if (err!=0||verbose)std::cout<<"readout"<<err;



        clWaitForEvents(1,&readoutcomplete);





        clReleaseMemObject(APoints);
        clReleaseMemObject(BPoints);
        clReleaseMemObject(CPoints);
        clReleaseMemObject(COLIDER);
        clReleaseMemObject(Faceinfo);
        clReleaseMemObject(raysA);
        clReleaseMemObject(raysB);
        clReleaseMemObject(raysOverflow);
        clReleaseMemObject(raysStore);




        clFlush(CTXT.commandQue);
        err = clFinish(CTXT.commandQue);


        clReleaseKernel(REFLECT);
        clReleaseKernel(MAIN);
        clReleaseKernel(FACEPREP);

        err = clReleaseProgram(CTXT.program);
        err = clReleaseCommandQueue(CTXT.commandQue);
        err = clReleaseContext(CTXT.context);






        formatDefinitions file = formatDefinitions(false,path);
        cout<<"storing in"<<path;
        //file.store(outstack,raystacksize);

        file.storeField(outfield,resd,resdc,steps);
        cl_float3 roomsize,testvolsize;
        roomsize.x = Xsize;
        roomsize.y = Ysize;
        roomsize.z = Zsize;

        testvolsize.x = resdsx;
        testvolsize.y = resdsx;
        testvolsize.z = resdsx;

        file.storemeta(roomsize,antennaorigin,testcubeorigin,testvolsize);
        file.close();
        //formatDefinitions file2 = formatDefinitions(true,"cATA.SDAT");

        //file2.close();







        //cleanup(&CTXT);
        return;

    }

bool CLFUNC::trymap(float wdim,size_t maxsize,int phasesteps) {
    cl_ulong memsizealoc;
    clGetDeviceInfo(CTXT.device,CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(memsizealoc),&memsizealoc,NULL);
    cl_ulong localsize;
    clGetDeviceInfo(CTXT.device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &localsize, 0);


    cout<<"\nMax alloc :"<<memsizealoc/1000000.0f<<"MB, private mem:"<<localsize/1000.0f<<"KB\n";
    float diag = (Xsize*Xsize+Ysize*Ysize+Zsize*Zsize);
    //float c = 299792458.0f;
    float c = 300000000.0f;
    float wavelength = c/simhz;
    float celsize = wavelength*0.1;
    size_t celbytes = sizeof(float)*3;//Directional
    //size_t celbytes = sizeof(float);

    resdc = trunc(wdim/celsize)+((fmod(wdim ,celsize)>0)?1:0);
    celsizeAct = wdim / resdc;
    steps = phasesteps;
    cout<<"frequency "<<simhz/1000000000<<"GHZ testcube dimensions"<<wdim<<" Meter, wavelength "<<wavelength<<" Meter " <<"subdivisons in volume "<<resdc ;
    if (!((resdc*resdc*resdc*celbytes*phasesteps) > memsizealoc))
    {
        cout<<"\nacceptable parrameters for 3d map\n";
        cout<<"\nresultmap 3d size in MB "<<(resdc*resdc*resdc*celbytes*phasesteps)/1000000.0f;
        resd = 3;
        resdsx = wdim;

    }
    else if (!((resdc*resdc*celbytes*phasesteps) > memsizealoc))
    {
        cout<<"\nacceptable parrameters for 2d map\n";
        cout<<"\nresultmap 2d size in MB "<<(resdc*resdc*celbytes*phasesteps)/1000000.0f;
        resd = 2;
        resdsx = wdim;

    }
    else {
        cout<<"\nunacceptable parrameters for result map , exeeds device memory limitations\nrequirements 2dmap in MB "<<(resdc*resdc*celbytes*phasesteps)/1000000.0f<<"\nExess of MB "<<(resdc*resdc*celbytes*phasesteps-memsizealoc)/1000000.0f;

    }

    return false;
}

bool CLFUNC::meminfRAY(int rays) {
    cl_ulong memsizealoc;
    clGetDeviceInfo(CTXT.device,CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(memsizealoc),&memsizealoc,NULL);
    cl_ulong globalsize;
    clGetDeviceInfo(CTXT.device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &globalsize, 0);

    long raystacksize = sizeof(CL_Rayinfo)*rays;


    cout<<"\nMax alloc usage by raystack :"<<(float)raystacksize/memsizealoc*100<<"%, raystack size:"<<raystacksize/1000000.0f<<"MB combined global usage "<<raystacksize*2.0f/globalsize*100.0f<<"\n";
    return true;
}

int CLFUNC::handleinputfile(char * path) {
    cl_float3 roomsize,testvolsize;
    string antpath ;
    int rayc,steps;
    strtrd(path,&antpath, &roomsize,&antennaorigin,&testcubeorigin,&testvolsize,&simhz,&rayc,&steps);


    Xsize = roomsize.x;
    Ysize = roomsize.y;
    Zsize = roomsize.z;


    resdsx = testvolsize.x ;
    //resdsy = testvolsize.y ;
    //resdsz = testvolsize.z ;

    cout<<rayc << " : "<< simhz;
    trymap(resdsx, sizeof(char)*10000000,steps);

    return  rayc;
}
