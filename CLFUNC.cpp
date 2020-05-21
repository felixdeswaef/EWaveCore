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

        loadkernel("../kernels/F_REFV1.cl",&CTXT.context,&CTXT.device,&CTXT.program,&FACEPREP, "FaceNorm"); //load kernel
        loadkernel("../kernels/F_REFV1.cl",&CTXT.context,&CTXT.device,&CTXT.program,&MAIN, "RayColGen"); //load kernel
        loadkernel("../kernels/F_REFV1.cl",&CTXT.context,&CTXT.device,&CTXT.program,&REFLECT, "RayRef"); //load kernel



    }

    void CLFUNC::runcolisions(RAY * raystack,int raystacksize) {

        CL_Rayinfo * nstack = convert(raystack,raystacksize);



        CL_Rayinfo * outstack =(CL_Rayinfo *) malloc(raystacksize * sizeof(CL_Rayinfo));



        cl_float3 **trisstack = gettrisstack();




        nstack[3].S.x = 1.0f;
        nstack[3].S.y = 0.0f;
        nstack[3].S.z = 1.0f;
        nstack[3].D.x = 0.0f;
        nstack[3].D.y = 0.0f;
        nstack[3].D.z = 10.0f;

        nstack[2].S.x = 1.1f;
        nstack[2].S.y = 1.1f;
        nstack[2].S.z = 1.1f;
        nstack[2].D.x = 0.0f;
        nstack[2].D.y = 10.0f;
        nstack[2].D.z = 10.0f;

        int stacksize = 12;

        int err;
        /* points
         * faces
         * rays
         *
         *
         */

        cl_mem APoints = clCreateBuffer(CTXT.context, CL_MEM_READ_ONLY, stacksize * sizeof(cl_float3), nullptr,&err);
        std::cout<<err;
        cl_mem BPoints = clCreateBuffer(CTXT.context, CL_MEM_READ_ONLY, stacksize * sizeof(cl_float3), nullptr,&err);
        std::cout<<err;
        cl_mem CPoints = clCreateBuffer(CTXT.context, CL_MEM_READ_ONLY, stacksize * sizeof(cl_float3), nullptr,&err);
        std::cout<<err;
        cl_mem COLIDER = clCreateBuffer(CTXT.context, CL_MEM_READ_WRITE, raystacksize*stacksize * sizeof(cl_float4), nullptr,&err);
        std::cout<<err;
        cl_mem Faceinfo = clCreateBuffer(CTXT.context, CL_MEM_READ_ONLY, stacksize * sizeof(CL_Faceinfo), nullptr,&err);
        std::cout<<err;
        cl_mem raysA = clCreateBuffer(CTXT.context, CL_MEM_READ_ONLY, raystacksize * sizeof(CL_Rayinfo), nullptr,&err);
        std::cout<<err;
        cl_mem raysB = clCreateBuffer(CTXT.context, CL_MEM_READ_ONLY, raystacksize * sizeof(CL_Rayinfo), nullptr,&err);
        cl_mem raysOverflow = clCreateBuffer(CTXT.context, CL_MEM_READ_ONLY, raystacksize * sizeof(CL_Rayinfo), nullptr,&err);
        cl_mem raysStore = clCreateBuffer(CTXT.context, CL_MEM_READ_ONLY, raystacksize * sizeof(CL_Rayinfo), nullptr,&err);



        err = clEnqueueWriteBuffer(CTXT.commandQue, APoints, CL_TRUE, 0, stacksize * sizeof(cl_float3), trisstack[0], 0,
                NULL, NULL);

        std::cout<<err;
        err = clEnqueueWriteBuffer(CTXT.commandQue, BPoints, CL_TRUE, 0, stacksize * sizeof(cl_float3), trisstack[1], 0,
                                   NULL, NULL);
        std::cout<<err;
        err = clEnqueueWriteBuffer(CTXT.commandQue, CPoints, CL_TRUE, 0, stacksize * sizeof(cl_float3), trisstack[2], 0,
                                   NULL, NULL);
        std::cout<<err;
        err = clEnqueueWriteBuffer(CTXT.commandQue, raysA, CL_TRUE, 0, raystacksize * sizeof(CL_Rayinfo), nstack, 0,
                                   NULL, NULL);
        std::cout<<err;




        //run normal finder
        err = clSetKernelArg(FACEPREP, 0, sizeof(cl_mem), &APoints);
        std::cout<<err;
        err = clSetKernelArg(FACEPREP, 1, sizeof(cl_mem), &BPoints);
        std::cout<<err;
        err = clSetKernelArg(FACEPREP, 2, sizeof(cl_mem), &CPoints);
        std::cout<<err;
        err = clSetKernelArg(FACEPREP, 3, sizeof(cl_mem), &Faceinfo); //TODO arg pos change
        std::cout<<err<<"\n";


        size_t global_item_size[2], local_item_size[2]; // Process the entire lists
        local_item_size[0] = 32;
        local_item_size[1] = 32;
        global_item_size[0] = stacksize;
        global_item_size[1] = stacksize;

        err = clEnqueueNDRangeKernel(CTXT.commandQue,FACEPREP,1, nullptr, global_item_size, nullptr, NULL, nullptr,nullptr);
        std::cout<<err;



        //run ray colission

        err = clSetKernelArg(MAIN, 0, sizeof(cl_mem), &APoints);
        std::cout<<err;
        err = clSetKernelArg(MAIN, 1, sizeof(cl_mem), &BPoints);
        std::cout<<err;
        err = clSetKernelArg(MAIN, 2, sizeof(cl_mem), &CPoints);
        std::cout<<err;
        err = clSetKernelArg(MAIN, 3, sizeof(cl_mem), &raysA); //TODO arg pos change
        std::cout<<err<<"\n";
        err = clSetKernelArg(MAIN, 4, sizeof(cl_mem), &COLIDER); //TODO arg pos change
        std::cout<<err<<"\n";


        local_item_size[0] = 32;
        local_item_size[1] = 32;
        global_item_size[0] = raystacksize;
        global_item_size[1] = stacksize;

        err = clEnqueueNDRangeKernel(CTXT.commandQue,MAIN,2, nullptr, global_item_size, nullptr, NULL, nullptr,nullptr);
        std::cout<<err;



        //run ray reflection
        err = clSetKernelArg(REFLECT, 0, sizeof(cl_mem), &raysA); //TODO arg pos change
        std::cout<<err<<"\n";
        err = clSetKernelArg(REFLECT, 1, sizeof(cl_mem), &Faceinfo); //TODO arg pos change
        std::cout<<err<<"\n";
        err = clSetKernelArg(REFLECT, 2, sizeof(cl_mem), &COLIDER); //TODO arg pos change
        std::cout<<err<<"\n";
        long stz = (long) stacksize;
        err = clSetKernelArg(REFLECT, 3, sizeof(cl_long), &stz); //TODO arg pos change
        std::cout<<err<<"\n";


        local_item_size[0] = 32;
        local_item_size[1] = 32;
        global_item_size[0] = raystacksize;
        global_item_size[1] = stacksize;
        cl_event finished;

        err = clEnqueueNDRangeKernel(CTXT.commandQue,REFLECT,1, nullptr, global_item_size, nullptr, 0, nullptr,&finished);




        clWaitForEvents(1,&finished);
        std::cout<<err;

        cl_event readoutcomplete;
        err = clEnqueueReadBuffer(CTXT.commandQue, raysA, CL_TRUE, 0, raystacksize * sizeof(CL_Rayinfo), outstack, 0, NULL, &readoutcomplete);
        std::cout<<err;



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






        formatDefinitions file = formatDefinitions(false,"bATA.SDAT");
        file.store(outstack,raystacksize);
        file.close();
        formatDefinitions file2 = formatDefinitions(true,"bATA.SDAT");

        file2.close();







        //cleanup(&CTXT);
        return;

    };
