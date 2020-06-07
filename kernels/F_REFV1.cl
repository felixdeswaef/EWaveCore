#define debug1
#define cachecount 16
typedef struct Rayinfo {
    float3 S; //source
    float3 D; //destination //length is accurate but relative to startpoint
    float3 polarisation; //polarisation //normalised vector for now
    float totaltravel;
    uint faceseed;


} Rayinfo;

typedef struct Faceinfo {
    uint faceseed;
    float3 normal;
    float ref;
    float trans;

} Faceinfo;
#pragma OPENCL EXTENSION cl_intel_printf : enable
#pragma OPENCL EXTENSION cl_amd_printf : enable

float vecDet(float3 a, float3 b, float3 c) {


    return ((a.x * (b.y * c.z - c.y * b.z)) - (b.x * (a.y * c.z - c.y * a.z)) + (c.x * (a.y * b.z - b.y * a.z)));


}
/*
inline void AtomicAdd(volatile __global float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;
    do {
        prevVal.floatVal = *source;
        newVal.floatVal = prevVal.floatVal + operand;
    } while (atomic_cmpxchg((volatile __global unsigned int *) source, prevVal.intVal, newVal.intVal) !=
             prevVal.intVal);
}//http://suhorukov.blogspot.com/2011/12/opencl-11-atomic-operations-on-floating.html
inline void AtomicAddVector(volatile __global float3

*source,
const float operand
) {
union {
    unsigned int intVal;
    float3 floatVal;
} newVal;
union {
    unsigned int intVal;
    float3 floatVal;
} prevVal;
do {
prevVal.
floatVal = *source;
newVal.
floatVal = prevVal.floatVal + operand;
} while (atomic_cmpxchg((volatile __global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}//http://suhoruko
*/

uint wang_hash(uint seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

__kernel void FaceNorm(
        __global float3 * Apoints, //args
        __global float3 * Bpoints,
        __global float3 * Cpoints,
        __global Faceinfo* faceinfo



){
int face = (int) get_global_id(0);
float3 Apoint = Apoints[face];
float3 ABvector = Bpoints[face] - Apoint;
float3 ACvector = Cpoints[face] - Apoint;
float3 Normal = cross(ABvector, ACvector);
faceinfo[face].faceseed = wang_hash(face);
faceinfo[face].normal = normalize(Normal);


}

__kernel void RayRef(
        __global Rayinfo * RAYS,

        __global Faceinfo * faceinfo,
        __global float4 * Coliders,
        long faces,
        __global Rayinfo * RAYSR

){
int ray = (int) get_global_id(0);
int facecount = faces;
float mindist = 100;
int face = -1;

for (int i = 0;i<facecount;i++){

    float curdist = Coliders[ray * facecount + i].w;
    //printf("distance %f face %d",curdist,i);
    if (curdist<mindist && curdist> 0){
        //suplement with validity check
        //otherwise keep this result
        mindist = curdist;
        face = i;
    }
}
//operate on face selected
#ifdef debug0
printf("ray %d collided with %d at %f pos\n",ray,face,mindist);
#endif
printf("ray %d collided with %d at %f pos %f\n",ray,face,mindist,Coliders[ray * facecount + facecount].x);

float3 incoming = RAYS[ray].D;
float3 normal = faceinfo[face].normal;
float3 reflected = incoming - 2 * dot(incoming, normal) * normal;
uint Hash = wang_hash(RAYS[ray].faceseed + faceinfo[face].faceseed);
RAYSR[ray].D=reflected;
RAYSR[ray].faceseed=Hash;
RAYSR[ray].S=Coliders[ray * facecount + face].xyz - RAYS[ray].S;
RAYS[ray].D=Coliders[ray * facecount + face].xyz - RAYS[ray].S;
RAYSR[ray].polarisation = incoming - 2 * dot(incoming, normal) * normal;   //dirty formula see reflection chapter
RAYSR[ray].totaltravel = Coliders[ray * facecount + face].w + RAYS[ray].totaltravel;


/*
 * float3 S; //source
    float3 D; //destination //length is accurate but relative to startpoint
    float3 polarisation; //polarisation //normalised vector for now
    float totaltravel;
 * */

#ifdef debug0
printf( "\n%d,%d, H:%d\ninc %f %f %f \nnorm %f %f %f\nout %f %f %f\n",ray,face,Hash,incoming.x,incoming.y,incoming.z,normal.x,normal.y,normal.z,reflected.x,reflected.y,reflected.z);
#endif


}

__kernel void RayColGen(
        __global float3 * Apoints, //args
        __global float3 * Bpoints,
        __global float3 * Cpoints,
        __global Rayinfo * RAYS,
        __global float4 * Coliders
){
int ray = (int) get_global_id(0);
int face = (int) get_global_id(1);
int raycount = (int) get_global_size(0);
int facecount = (int) get_global_size(1);

Rayinfo Rayin = RAYS[ray];
float3 rayorigin = Rayin.S;
float3 Apoint = Apoints[face];
float3 rayvector = Rayin.D;
float3 ABvector = (Bpoints[face]) - (Apoint);
float3 ACvector = (Cpoints[face]) - (Apoint);

float determinant = length((-1 * rayvector) * cross(ABvector, ACvector));
printf("\ndet x %fy%f z%f ",((-1 * rayvector) * cross(ABvector, ACvector)).x,((-1 * rayvector) * cross(ABvector, ACvector)).y,((-1 * rayvector) * cross(ABvector, ACvector)).z );

float3 result = (float3)(0.0f, 0.0f, 0.0f);
float rest, resu, resv; //todo also check inside plane
float3 subcal = (rayorigin - Apoint);
float4 colres =(float4)(0.0f, 0.0f, 0.0f, -1.0f);
if (determinant!=0) {   //ray crosses plane

rest = length((cross(ABvector, ACvector) * subcal) / determinant);
resu = length((cross(ABvector, rayvector) * subcal) / determinant);
resv = length((cross(rayvector, ACvector) * subcal) / determinant);

result = rayorigin + rayvector * rest;
// we do care if its in range becauze it cant be behind the startpoit
// also we need to scale our rays to be vallid
// so t between 0 and 1 for inside the line (ray) bounds
// and u + v <= 1 and u v both between 0 and 1

//bu if (rest.z<=1 && rest.z>=0.0f && resu.z<=1 && resu.z>=0.0f && resv.z<=1 && resv.z>=-0.0f && (resu.z+resv.z)<=1 ){
if (rest<=1 && rest>=0.0f && resu<=1 && resu>=0.0f && resv<=1 && resv>=-0.0f && (resu+resv)<=1 ){
    colres = (float4)(result.x, result.y, result.z, distance(rayorigin, result));
    printf("\n gocol ray%d face %d x%f y%f z%f",ray,face,result.x,result.y,result.z);
}else{

    //colres = (float4)(2.0, 2.0, 2.0, -1.0f);
    //printf(" colcheck x ray%d face %d  R%f %f %f  A%f %f %f  B%f %f %f  C%f %f %f \n",ray,face, RAYS[ray].D.x,RAYS[ray].D.y,RAYS[ray].D.z,Apoints[face].x,Apoints[face].y,Apoints[face].z,Bpoints[face].x,Bpoints[face].y,Bpoints[face].z,Cpoints[face].x,Cpoints[face].y,Cpoints[face].z);

//printf("\nout of bounds ´%d %d  t%du%dv%dT%d %f %f %f \n%f %f %f d %f",ray,face,(rest.z<=1 && rest.z>=-0.0f )?0:1, (resu.z<=1 && resu.z>=-0.0f )?0:1, (resv.z<=1 && resv.z>=(-0.0f) )?0:1,((rest.z<=1 && rest.z>=0.0f && resu.z<=1 && resu.z>=0.0f && resv.z<=1 && resv.z>=-0.0f && (resu.z+resv.z)<=1 )?0:1), rest.z,resu.z,resv.z,result.x, result.y, result.z, distance(rayorigin, result));
}


//printf(" colcheck x ray%d face %d  R%f %f %f  A%f %f %f  B%f %f %f  C%f %f %f \n",ray,face, RAYS[ray].D.x,RAYS[ray].D.y,RAYS[ray].D.z,Apoints[face].x,Apoints[face].y,Apoints[face].z,Bpoints[face].x,Bpoints[face].y,Bpoints[face].z,Cpoints[face].x,Cpoints[face].y,Cpoints[face].z);

}
Coliders[ray *facecount +face]= colres;

//printf("\n col ray%d face %d X%fY%fZ%fW%f",ray,face,Coliders[ray *facecount +face].x,Coliders[ray *facecount +face].y,Coliders[ray *facecount +face].z,Coliders[ray *facecount +face].w);
//didn
//printf("from,%f,%f,%f to,%f,%f,%f\n",rayorigin.x,rayorigin.y,rayorigin.z,raydir.x,raydir.y,raydir.z);
//printf("to,%f,%f,%f\n",raydir.x,raydir.y,raydir.z);


//printf("face %d,ray %d , result,%f,%f,%f \n",face,ray,result.x,result.y,result.z);
//printf("result,%f,%f,%f,%f\n",rest.z,resu.z,resv.z,0.7f);
//printf("result,%f,%f,%f,%f\n",result.x,result.y,result.z,0.7f);
//printf("ABxACtwas,%f,%f,%f,%f\n",(ABvector * ACvector).x,(ABvector * ACvector).y,(ABvector * ACvector).z,0.7f);

//else next


//printf(" colcheck x ray%d face %d  A%f %f %f\n",ray,face, RAYS[ray].D.x,RAYS[ray].D.y,RAYS[ray].D.z);






}


__kernel void resND(
        __global float * vectors,
        __global Rayinfo* RAYS,
        float3 Origin,
        float sizecel,
        int subdivs,
        float angle,
        int frames,
        int rayc,
        __global int * ASSOCA,
        int cachecountDEP


){
//float3 Minpos = (float3)(get_global_id(0)*size,get_global_id(1)*size,get_global_id(2)*size) + Origin;
//float3 Maxpos = (float3)(get_global_id(0)*size+size,get_global_id(1)*size+size,get_global_id(2)*size+size)+ Origin;
//float3
//printf("kernelrun");
int x = get_global_id(0);
int y = get_global_id(1);
int z = get_global_id(2);
int d = get_work_dim();
float hcel = sizecel / 2;
float dcel = sqrt(sizecel * sizecel + sizecel * sizecel + sizecel * sizecel);
int count = 0;
int count2 = 0;
int count3 = 0;


//printf("|--  %d %d %d %d --|",d,x,y,z);

float3 Samplepoint = (float3)(x * sizecel + hcel + Origin.x, y * sizecel + hcel + Origin.y,
                              z * sizecel + hcel + Origin.z);

//printf("|--  %f %f %f %f %f --|\n",sizecel,hcel,Origin.x,x * sizecel + hcel + Origin.x,Samplepoint.x);

float3 Maxpoint = (float3)(x * sizecel + sizecel + Origin.x, y * sizecel + sizecel + Origin.y,
                           z * sizecel + sizecel + Origin.z);
float3 Minpoint = (float3)(x * sizecel + Origin.x, y * sizecel + Origin.y, z * sizecel + Origin.z);

uint cache[cachecount];//cache of done cycles of hashes
int cachec[cachecount]; //count in hash in case we double
int cacheusage = 0;
int count4 ;
for(int i = 0;i<rayc;i++){ //loop
    count4= i;
    float3 origin = RAYS[i].S;
    float3 direction = RAYS[i].D;
    float3 closest = dot((Samplepoint - origin), direction) + origin;
    float dista = distance(closest,Samplepoint);
    //printf("X1X);
    //printf("\nLoop run %d of %d",i,rayc);
    //if (dista<dcel) printf("\n%f m ** ",dista) ;
    //if (dista<0.5f) printf("\n%f m ",dista);
    //printf("closest,%f,%f,%f\nto ,%f,%f,%f",closest.x,closest.y,closest.z,Samplepoint.x,Samplepoint.y,Samplepoint.z);
    if (
            closest.x >= Minpoint.x
            &&
            closest.y >= Minpoint.y
            &&
            closest.z >= Minpoint.z
            &&
            closest.x <= Maxpoint.x
            &&
            closest.y <= Maxpoint.y
            &&
            closest.z <= Maxpoint.z
            )
    {
        //printf(" IS IN VOLUME ");
        count2++;
        bool wasincache = false;
        for (int x = 0; x<cacheusage;x++){
            if (cache[x]==RAYS[i].faceseed) wasincache = true;
            //printf("wasincache\n");
        }

        if (!wasincache)
        {
            //printf(" was not in cache");
            count++;
            int closestid = i;
            float closestdistance = dista;
            int j = i;
            int hc = 0;
            while(ASSOCA[j]!=i * -1 && ASSOCA[j]!=i ){//loop trough once
                int next = ASSOCA[j];
                hc++;
                j = ((next>=0)?next:(next*-1));
                float3 originsc = RAYS[j].S;
                float3 directionsc = RAYS[j].D;
                float distasubcal = distance(dot((Samplepoint - originsc), directionsc) + originsc,Samplepoint); //distance closest point & samplepoint
                if (distasubcal<closestdistance)
                {//beter result -> store
                    if(true) //cubecheck
                    {
                         closestdistance = distasubcal;
                         closestid = j;
                    }
                }

            } //end loop trough other
            //****ACTUAL RESULT ADDED TO RESULT ARRAY *****//
            //int cel = 0
            //vectors[(cel*((d<3)?subdivs*subdivs:subdivs*subdivs*subdivs*3)]
            int frameofset = ((d<3)?subdivs*subdivs:subdivs*subdivs*subdivs);
            int offset = z*subdivs*subdivs*3 + y*subdivs*3 + x*3 ;
            //int offset = 0*subdivs*subdivs*3 + 0*subdivs*3 + 1*3 ;
            vectors[0*frameofset + offset + 0] = RAYS[closestid].polarisation.x + 1; //vectors[0*frameofset + offset + 0];
            vectors[0*frameofset + offset + 1] = 1.0f;//RAYS[closestid].polarisation.y + vectors[0*frameofset + offset + 1];
            vectors[0*frameofset + offset + 2] = hc + 0.0f;//RAYS[closestid].polarisation.z + vectors[0*frameofset + offset + 2];
            //****ACTUAL RESULT ADDED TO RESULT ARRAY *****//
            //printf("went trough %d samehash rays",hc);
            if (cacheusage<cachecount){
                //printf("space left in cache");
                cachec[cacheusage] = hc;
                cache[cacheusage]=RAYS[i].faceseed;
                cacheusage++;
                //printf("added to cache");
            }
            else{
                int smol = -1;
                int smolI;
                bool gotspot = false;
                for (int z = 0; z<cachecount;z++){
                if (cachec[z]<hc) gotspot = true;
                if (cachec[z]<smol || smol == -1){
                    smolI = z;
                    smol = cachec[z];
                }
                }
                if (gotspot){
                    cachec[smolI] = hc;
                    cache[smolI]=RAYS[i].faceseed;
                    printf("exeeded cache size inaccurate results can occur");
                }
            }
        }else{
            //printf("skiped cached hash");
        }

    }

}

//printf("- %d /%d  -- %d,%d,%d -\n",count4+1,rayc,x,y,z);

}




__kernel void fibRayGen(
        __global Rayinfo * RAYS,
        float3 origin,
        float3 polar

        //textures for strength
        )
{
int n = get_global_size(0);
int i = get_global_id(0) + 1;
int ray = get_global_id(0);
float golden = 1.61803398875;
float v = 0.5 * i;
/*float phi = acos(1 - 2 * v / n);
float theta = 2 * M_PI * v / golden;
 //TEMP CHANGE BECAUSE OF DISTRIBUTION ERROR
 */
float arc = 2.0f* M_PI/n * i;///n * i;//M_PI*2.0f*1.0f;//0.5f * M_PI/n * i;
//float ele = 0.125f * M_PI;
//float ele = M_PI*(fmod(1.0f /(5) * i,1))+0.5*M_PI;
float ele = M_PI*(fmod(1.0f /(n/sqrt(n*1.0f)) * i,1))+0.5*M_PI;
//float ele = ((1.0f * M_PI)/(n * i))+0.5*M_PI;

float3 dir = (float3)(sin(arc)*fabs(cos(ele)), sin(ele), cos(arc)*fabs(cos(ele)));//fabs(cos(ele)));
// f u s
//float3 dir = (float3)(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
//float3 dir = (float3)(cos(theta)* sin(phi), sin(phi)*1, sin(theta)*cos(phi));

RAYS[ray].D = (dir * 1.0f);
printf("len %f %f",length(dir),length(RAYS[ray].D));
RAYS[ray].S = (origin);
RAYS[ray].faceseed = wang_hash(0);
RAYS[ray].totaltravel = 0.0f;
RAYS[ray].polarisation = polar;

//printf("heading,%f,%f,%f\n",RAYS[ray].D.x,RAYS[ray].D.y,RAYS[ray].D.z);

}


__kernel void ASSOC(
        __global Rayinfo* RAYS,
        __global int * ASSOCA

)
{
int M = get_global_size(0);
int I = get_global_id(0);
uint Hashtarget =  RAYS[I].faceseed;
for(int i=I+1;i<M;i++){
    if (RAYS[i].faceseed == Hashtarget){
        ASSOCA[I]=i;
        return ;
    }
}
for(int i=0;i<I;i++){
if (RAYS[i].faceseed == Hashtarget){
ASSOCA[I]=i*-1;
return ;
}

}
ASSOCA[I]=I;

}

















