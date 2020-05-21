typedef struct Rayinfo{
    float3 S; //source
    float3 D; //destination //length is accurate but relative to startpoint
    float3 polarisation; //polarisation //normalised vector for now
    float totaltravel;
    uint faceseed;


} Rayinfo ;

typedef struct Faceinfo{
    uint faceseed;
    float3 normal;
    float ref;
    float trans;

} Faceinfo ;
#pragma OPENCL EXTENSION cl_intel_printf : enable
#pragma OPENCL EXTENSION cl_amd_printf : enable

float vecDet(float3 a, float3 b, float3 c){




    return ((a.x*(b.y*c.z-c.y*b.z))-(b.x*(a.y*c.z-c.y*a.z))+(c.x*(a.y*b.z-b.y*a.z)));


}
uint wang_hash(uint seed)
{
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
        __global Faceinfo * faceinfo



){
int face = (int) get_global_id(0);
float3 Apoint = Apoints[face];
float3 ABvector = Bpoints[face] - Apoint ;
float3 ACvector = Cpoints[face] - Apoint ;
float3 Normal = cross(ABvector,ACvector);
faceinfo[face].faceseed = wang_hash(face);
faceinfo[face].normal = normalize (Normal);


}
__kernel void RayRef(
        __global Rayinfo * RAYS,
        __global Faceinfo * faceinfo,
        __global float4 * Coliders,
         long faces

){
int ray = (int) get_global_id(0);
int facecount = faces;
float mindist = 100;
int face = -1;
//printf("%d %d %d pos\n",facecount,faces,ray);
for (int i = 0 ; i<facecount ; i++){
    //printf("%d %d %d %f pos\n",ray*facecount+i,i,ray,Coliders[ray*facecount+i].w);
    float curdist = Coliders[ray*facecount+i].w;
    if (curdist < mindist && curdist > 0){
        //suplement with validity check
        //otherwise keep this result
        mindist = curdist;
        face = i;
    }
}
//operate on face selected
printf("ray %d collided with %d at %f pos\n",ray,face,mindist);
float3 incoming = RAYS[ray].D;
float3 normal = faceinfo[face].normal;
float3 reflected = incoming - 2* dot(incoming,normal) * normal;
uint Hash = wang_hash(faceinfo[face].faceseed + faceinfo[face].faceseed );

printf( "\n%d,%d, H:%d\ninc %f %f %f \nnorm %f %f %f\nout %f %f %f\n",ray,face,Hash,incoming.x,incoming.y,incoming.z,normal.x,normal.y,normal.z,reflected.x,reflected.y,reflected.z);





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
int raycount =(int) get_global_size(0);
int facecount =(int) get_global_size(1);


Rayinfo Rayin = RAYS[ray];
float3 rayorigin = Rayin.S;
float3 Apoint = Apoints[face];
float3 rayvector =Rayin.D ;
float3 ABvector = Bpoints[face] - Apoint ;
float3 ACvector = Cpoints[face] - Apoint ;

float determinant = ((-1*rayvector)*cross(ABvector , ACvector) ).z;

float3 result = (float3)(0.0f,0.0f,0.0f);
float3 rest,resu,resv; //todo also check inside plane
float3 subcal = (rayorigin - Apoint );
if (determinant!=0) {   //ray crosses plane

rest = (cross(ABvector,ACvector)*subcal)/determinant;
resu = (cross(ABvector , rayvector)*subcal)/determinant;
resv = (cross(rayvector , ACvector)*subcal)/determinant;

result = rayorigin + rayvector *rest.z;
// we do care if its in range becauze it cant be behind the startpoit
// also we need to scale our rays to be vallid
// so t between 0 and 1 for inside the line (ray) bounds
// and u + v <= 1 and u v both between 0 and 1
if (rest.z<=1 && rest.z>=0 && resu.z<=1 && resu.z>=0 && resv.z<=1 && resv.z>=0 && (resu.z+resv.z)<=1 ){

//printf("a%d,%f,%f,%f b,%f,%f,%f, c ,%f,%f,%f\n",face,Apoint.x,Apoint.y,Apoint.z,Bpoints[face].x,Bpoints[face].y,Bpoints[face].z,Cpoints[face].x,Cpoints[face].y,Cpoints[face].z);
//printf("got hit %d face %d,%f,%f,%f to,%f,%f,%f\n face %d,ray %d , result,%f,%f,%f dist %f \n",ray,face,Rayin.XS,Rayin.YS,Rayin.ZS,Rayin.XD,Rayin.YD,Rayin.ZD,face,ray,result.x,result.y,result.z,distance(rayorigin,result));
float4 colres = (float4)(result.x,result.y,result.z,distance(rayorigin,result));
Coliders[ray*facecount+face]= colres;
//printf("%d %d pos %d",face,ray,ray*facecount+face);
//printf("%f%f",Coliders[ray*facecount+face].w,Coliders[ray*facecount+face].x);

}
}
//printf("from,%f,%f,%f to,%f,%f,%f\n",rayorigin.x,rayorigin.y,rayorigin.z,raydir.x,raydir.y,raydir.z);
//printf("to,%f,%f,%f\n",raydir.x,raydir.y,raydir.z);


//printf("face %d,ray %d , result,%f,%f,%f \n",face,ray,result.x,result.y,result.z);
//printf("result,%f,%f,%f,%f\n",rest.z,resu.z,resv.z,0.7f);
//printf("result,%f,%f,%f,%f\n",result.x,result.y,result.z,0.7f);
//printf("ABxACtwas,%f,%f,%f,%f\n",(ABvector * ACvector).x,(ABvector * ACvector).y,(ABvector * ACvector).z,0.7f);
//printf("detwas,%f,%f,%f,%f\n",determinant,determinant,determinant,0.8f);
//else next








}



















