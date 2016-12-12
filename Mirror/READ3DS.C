/*
±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

 3D STUDIO SCENE CONVERTER - MAIN CODE
 PROGRAMMING BY MORPH/PULP

±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "read3ds.h"
#include "scale.h"

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void main();
void PR_CalcNormal();
void PR_MulFace();
S32 PR_FindChunk(U16 ID);
void PR_ReadMatName();
void PR_ReadMatType();
void PR_ReadMatFile();
void PR_ObjBlock();
void PR_ReadVert();
void PR_ReadFace();
void PR_ReadFaceMat();
void PR_ReadMapUV();
void PR_ReadMatrix();
void PR_CopyASCII(char *Dst);

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

U8 *Src;
HEADER *h;

VERTEX *Vertex;
VERTEX *Normal;
MAPUV *MapUV;
FACE *Face;
OBJECT *Object;
MATERIAL *Material;
TEXTURE *Texture;
U32 *N_Len;

struct {
   U32 SCN_Mat;
   U32 SCN_Obj;
   U32 SCN_Vertex;
   U32 SCN_Face;
} Scene={0,0,0,0};

S32 Len;
U32 p,my,mylen;

FILE *TR;

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

struct {
   U16 ID;
   void (*FPTR)();
} CHUNKS[]={{0x3d3d,0},
            {0xafff,0},
            {0xa000,PR_ReadMatName},
            {0xa100,PR_ReadMatType},
            {0xa200,0},
            {0xa300,PR_ReadMatFile},
            {0x4000,PR_ObjBlock},
            {0x4100,0},
            {0x4110,PR_ReadVert},
            {0x4120,PR_ReadFace},
            {0x4130,PR_ReadFaceMat},
            {0x4140,PR_ReadMapUV},
            {0x4160,PR_ReadMatrix}};

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void main(int argc,char** argv)
{
  FILE *Dst;
  S32 n;
  U32 i;

  Vertex=malloc(SCN_MAX_VERTEX*sizeof(VERTEX));
  Normal=malloc(SCN_MAX_VERTEX*sizeof(VERTEX));
  MapUV=malloc(SCN_MAX_VERTEX*sizeof(MAPUV));
  Face=malloc(SCN_MAX_FACE*sizeof(FACE));
  Object=malloc(SCN_MAX_OBJ*sizeof(OBJECT));
  Material=malloc(SCN_MAX_MAT*sizeof(MATERIAL));
  Texture=malloc(SCN_MAX_MAT*sizeof(TEXTURE));
  N_Len=malloc(SCN_MAX_VERTEX*sizeof(U32));

  Dst=fopen("test.3ds","rb");
  fseek(Dst,0L,SEEK_END);
  i=ftell(Dst);
  fseek(Dst,0L,SEEK_SET);
  Src=malloc(i);
  fread(Src,i,1,Dst);
  fclose(Dst);

  Dst=fopen("test.wld","wb");
  TR=fopen("test.obj","wt");

  printf("\nConverting...\n\n");

  h=(HEADER *)Src;

  if (h->ID!=0x4d4d)
  {
    printf("It isn't a 3DS file !\a");
    exit (-1);
  }

  Len=(h->LEN)-6;
  Src+=6;

  while (Len>0)
  {
    h=(HEADER *)Src;
    n=PR_FindChunk(h->ID);

    //printf("CHUNK: 0x%4X\n",h->ID);

    if (n<0)
    {
      Src+=h->LEN;
      Len-=h->LEN;
    }
    else {
           if (CHUNKS[n].FPTR!=0) CHUNKS[n].FPTR();
           else {
                  Src+=6;
                  Len-=6;
                }
         }
  }

  printf("\nDone.\n");

  if (Scene.SCN_Mat==0)
  {
    printf("No materials !\a");
    exit(-1);
  }

  PR_CalcNormal();
  PR_MulFace();

  fwrite(&Scene,sizeof(Scene),1,Dst);

  for (i=0;i<Scene.SCN_Mat;i++)
    fwrite(&Texture[i],strlen((U8 *)&Texture[i])+1,1,Dst);

  fwrite(Object,Scene.SCN_Obj*sizeof(OBJECT),1,Dst);
  fwrite(Vertex,Scene.SCN_Vertex*sizeof(VERTEX),1,Dst);
  fwrite(Normal,Scene.SCN_Vertex*sizeof(VERTEX),1,Dst);
  fwrite(MapUV,Scene.SCN_Vertex*sizeof(MAPUV),1,Dst);
  fwrite(Face,Scene.SCN_Face*sizeof(FACE),1,Dst);

  free(Vertex);
  free(Normal);
  free(MapUV);
  free(Face);
  free(Object);
  free(Material);
  free(Texture);
  free(N_Len);

  fclose(Dst);
  fclose(TR);
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_CalcNormal()
{
  F32 x1,y1,z1;
  F32 x2,y2,z2;
  F32 x3,y3,z3;

  F32 nx,ny,nz;
  F32 nl;

  U32 i;

  for (i=0;i<Scene.SCN_Vertex;i++)
  {
    Normal[i].x=0;
    Normal[i].y=0;
    Normal[i].z=0;
    N_Len[i]=0;
  }

  for (i=0;i<Scene.SCN_Face;i++)
  {
    x1=Vertex[Face[i].v1].x;
    y1=Vertex[Face[i].v1].y;
    z1=Vertex[Face[i].v1].z;

    x2=Vertex[Face[i].v2].x;
    y2=Vertex[Face[i].v2].y;
    z2=Vertex[Face[i].v2].z;

    x3=Vertex[Face[i].v3].x;
    y3=Vertex[Face[i].v3].y;
    z3=Vertex[Face[i].v3].z;

    nx=(y2-y1)*(z3-z1)-(z2-z1)*(y3-y1);
    ny=(z2-z1)*(x3-x1)-(x2-x1)*(z3-z1);
    nz=(x2-x1)*(y3-y1)-(y2-y1)*(x3-x1);

    nl=sqrt(nx*nx+ny*ny+nz*nz);

    nx=(nx*ns)/nl;
    ny=(ny*ns)/nl;
    nz=(nz*ns)/nl;

    Normal[Face[i].v1].x+=nx;
    Normal[Face[i].v1].y+=ny;
    Normal[Face[i].v1].z+=nz;

    N_Len[Face[i].v1]++;

    Normal[Face[i].v2].x+=nx;
    Normal[Face[i].v2].y+=ny;
    Normal[Face[i].v2].z+=nz;

    N_Len[Face[i].v2]++;

    Normal[Face[i].v3].x+=nx;
    Normal[Face[i].v3].y+=ny;
    Normal[Face[i].v3].z+=nz;

    N_Len[Face[i].v3]++;
  }

  for (i=0;i<Scene.SCN_Vertex;i++)
  {
    Normal[i].x/=N_Len[i];
    Normal[i].y/=N_Len[i];
    Normal[i].z/=N_Len[i];
  }
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_MulFace()
{
  U32 i;

  for (i=0;i<Scene.SCN_Face;i++)
  {
    Face[i].v1*=4;
    Face[i].v2*=4;
    Face[i].v3*=4;
    Face[i].v4*=4;
  }
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

S32 PR_FindChunk(U16 ID)
{
  S32 i;

  for (i=0;i<sizeof(CHUNKS)/sizeof(CHUNKS[0]);i++)
    if (ID==CHUNKS[i].ID) return (i);

  return (-1);
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_ReadMatName()
{
  PR_CopyASCII(Material[Scene.SCN_Mat].MatName);
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_ReadMatType()
{
  Src+=h->LEN;
  Len-=h->LEN;

  h++;

  Material[Scene.SCN_Mat].Type=h->ID;
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_ReadMatFile()
{
  PR_CopyASCII(Texture[Scene.SCN_Mat++].MatFile);
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_ObjBlock()
{
  int sw = 1;

  Src+=6;
  Len-=6;

  if ((*Src == 'L') || (*Src == 'C')) sw = 0;

  printf("Object: ");

  while (*Src!=0)
  {
    putchar(*Src);
    if (sw) fputc(*Src,TR);
    Src++;
    Len--;
  }

  if (sw) fprintf(TR,"\n");

  printf("\n");

  Src++;
  Len--;
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_ReadVert()
{
  VERTEX *Temp;
  U16 i;

  Temp=(VERTEX *)(Src+8);

  Src+=h->LEN;
  Len-=h->LEN;

  h++;

  p=Scene.SCN_Vertex;
  my=p;

  Scene.SCN_Vertex+=h->ID;
  Object[Scene.SCN_Obj].OBJ_Vertex=h->ID;
  mylen=h->ID;

  for (i=0;i<h->ID;i++)
  {
    Vertex[p+i].x=Temp[i].x*scale;
    Vertex[p+i].z=Temp[i].y*scale;
    Vertex[p+i].y=Temp[i].z*scale;
  }
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_ReadFace()
{
  FACE3DS *Temp;
  U16 i;

  Temp=(FACE3DS *)(Src+8);

  h++;

  Object[Scene.SCN_Obj++].OBJ_Face=h->ID;

  for (i=0;i<h->ID;i++)
  {
    Face[Scene.SCN_Face+i].v1=Temp[i].v1+p;
    Face[Scene.SCN_Face+i].v2=Temp[i].v2+p;
    Face[Scene.SCN_Face+i].v3=Temp[i].v3+p;
    Face[Scene.SCN_Face+i].v4=Temp[i].v1+p;
  }

  Scene.SCN_Face+=h->ID;

  Src+=8+(h->ID*sizeof(FACE3DS));
  Len-=8+(h->ID*sizeof(FACE3DS));
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_ReadMatrix()
{
  MTX3DS *T;
  MTX N;
  U32 lop;

  float wyznacznik;
  float sc;
  float d[3][3];

  F32 xx,yy,zz;

  T=(MTX3DS *)(Src+6);

  Src+=h->LEN;
  Len-=h->LEN;

  printf("%.6f %.6f %.6f\n",T->a,T->b,T->c);
  printf("%.6f %.6f %.6f\n",T->d,T->e,T->f);
  printf("%.6f %.6f %.6f\n",T->g,T->h,T->i);
  printf("%.6f %.6f %.6f\n",T->x,T->y,T->z);

  N[0][0] = T->a;
  N[0][2] = T->b;
  N[0][1] = T->c;

  N[2][0] = T->d;
  N[2][2] = T->e;
  N[2][1] = T->f;

  N[1][0] = T->g;
  N[1][2] = T->h;
  N[1][1] = T->i;

  N[3][0] = T->x*scale;
  N[3][2] = T->y*scale;
  N[3][1] = T->z*scale;

  wyznacznik = N[0][0]*N[1][1]*N[2][2]+
               N[0][1]*N[1][2]*N[2][0]+
               N[0][2]*N[1][0]*N[2][1]-
               N[0][2]*N[1][1]*N[2][0]-
               N[0][1]*N[1][0]*N[2][2]-
               N[0][0]*N[1][2]*N[2][1];

  sc = 1/wyznacznik;

  d[0][0] = (N[1][1]*N[2][2]-N[1][2]*N[2][1])*sc;
  d[1][0] = (N[1][2]*N[2][0]-N[1][0]*N[2][2])*sc;
  d[2][0] = (N[1][0]*N[2][1]-N[1][1]*N[2][0])*sc;
  d[0][1] = (N[2][1]*N[0][2]-N[2][2]*N[0][1])*sc;
  d[1][1] = (N[2][2]*N[0][0]-N[2][0]*N[0][2])*sc;
  d[2][1] = (N[2][0]*N[0][1]-N[2][1]*N[0][0])*sc;
  d[0][2] = (N[0][1]*N[1][2]-N[0][2]*N[1][1])*sc;
  d[1][2] = (N[0][2]*N[1][0]-N[0][0]*N[1][2])*sc;
  d[2][2] = (N[0][0]*N[1][1]-N[0][1]*N[1][0])*sc;


  for (lop=0;lop<mylen;lop++)
  {
    xx = Vertex[my+lop].x;
    yy = Vertex[my+lop].y;
    zz = Vertex[my+lop].z;

    xx-=N[3][0];
    yy-=N[3][1];
    zz-=N[3][2];

    Vertex[my+lop].x=xx*d[0][0]+yy*d[1][0]+zz*d[2][0];
    Vertex[my+lop].y=xx*d[0][1]+yy*d[1][1]+zz*d[2][1];
    Vertex[my+lop].z=xx*d[0][2]+yy*d[1][2]+zz*d[2][2];
  }
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_ReadFaceMat()
{
  U8 *Help;
  char Temp[17];
  U32 i=0;

  Help=Src;
  PR_CopyASCII(Temp);
  Src=Help;

  Src+=h->LEN;
  Len-=h->LEN;

  while (strcmp(Material[i].MatName,Temp)!=0)
    i++;

  Object[Scene.SCN_Obj-1].OBJ_Type=Material[i].Type-1;
  Object[Scene.SCN_Obj-1].OBJ_Fill=i;
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_ReadMapUV()
{
  MAPUV *Temp;
  U16 i;

  Temp=(MAPUV *)(Src+8);

  Src+=h->LEN;
  Len-=h->LEN;

  h++;

  for (i=0;i<h->ID;i++)
  {
    MapUV[p+i].u=Temp[i].u*255.0*256.0;
    MapUV[p+i].v=Temp[i].v*255.0*256.0;
  }
}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void PR_CopyASCII(char *Dst)
{
  U32 i=0;

  Src+=6;
  Len-=6;

  while (*Src!=0)
  {
    Dst[i++]=*Src;
    Src++;
    Len--;
  }

  Dst[i]=0;

  Src++;
  Len--;
}