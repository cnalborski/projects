#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "3d.h"
#include "scena.h"
#include "read3ds.h"

//------------

#define scale 8.0

extern HWND hOkno;

//------------

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
} Scene;

S32 Len;
U32 p,my,mylen;

//------------

struct {
   U16 ID;
   void (*FPTR)();
} CHUNKS[]={{0x3d3d,0},
            //{0xafff,0},
            //{0xa000,PR_ReadMatName},
            //{0xa100,PR_ReadMatType},
            //{0xa200,0},
            //{0xa300,PR_ReadMatFile},
            {0x4000,PR_ObjBlock},
            {0x4100,0},
            {0x4110,PR_ReadVert},
            {0x4120,PR_ReadFace},
            //{0x4130,PR_ReadFaceMat},
            {0x4140,PR_ReadMapUV},
            //{0x4160,PR_ReadMatrix}
			};

//------------

int read3ds(char* plik,Scena_t& Scena)
{
  FILE *Dst;
  S32 n;
  U32 i;

  Scene.SCN_Mat = 0;
  Scene.SCN_Obj = 0;
  Scene.SCN_Vertex = 0;
  Scene.SCN_Face = 0;

  Vertex=new VERTEX[SCN_MAX_VERTEX];
  Normal=new VERTEX[SCN_MAX_VERTEX];
  MapUV=new MAPUV[SCN_MAX_VERTEX];
  Face=new FACE[SCN_MAX_FACE];
  Object=new OBJECT[SCN_MAX_OBJ];
  Material=new MATERIAL[SCN_MAX_MAT];
  Texture=new TEXTURE[SCN_MAX_MAT];
  N_Len=new U32[SCN_MAX_VERTEX];

  Dst=fopen(plik,"rb");
  
  if (Dst == NULL)
  {
	MessageBox(hOkno,"B³¹d otwierania pliku 3DS !!!","Komunikat",MB_ICONEXCLAMATION | MB_OK);
	return -1;
  }

  fseek(Dst,0L,SEEK_END);
  i=ftell(Dst);
  fseek(Dst,0L,SEEK_SET);
  Src=new U8[i];
  fread(Src,i,1,Dst);
  fclose(Dst);

  h=(HEADER *)Src;

  if (h->ID!=0x4d4d)
  {
	MessageBox(hOkno,"To nie jest plik 3DS !!!","Komunikat",MB_ICONEXCLAMATION | MB_OK);
	return -1;
  }

  Len=(h->LEN)-6;
  Src+=6;

  while (Len>0)
  {
    h=(HEADER *)Src;
    n=PR_FindChunk(h->ID);

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

  PR_CalcNormal();

  Scena.vertices = Scene.SCN_Vertex;
  Scena.faces = Scene.SCN_Face;

  Scena.vptr = new VERTEX[Scene.SCN_Vertex];
  memcpy(Scena.vptr,Vertex,Scene.SCN_Vertex*sizeof(VERTEX));

  Scena.nptr = new VERTEX[Scene.SCN_Vertex];
  memcpy(Scena.nptr,Normal,Scene.SCN_Vertex*sizeof(VERTEX));

  Scena.fptr = new FACE[Scene.SCN_Face];
  memcpy(Scena.fptr,Face,Scene.SCN_Face*sizeof(FACE));

  delete[] Vertex;
  delete[] Normal;
  delete[] MapUV;
  delete[] Face;
  delete[] Object;
  delete[] Material;
  delete[] Texture;
  delete[] N_Len;


  FILE *f = fopen("c:\\3dobject.txt","wt");

  /*
    VERTEX obj1_vertex[8] = {
        { -1000.0, 1000.0, -1000.0 },
        { 1000.0, 1000.0, -1000.0 },
        { 1000.0, -1000.0, -1000.0 },
        { -1000.0, -1000.0, -1000.0 },
        { -1000.0, 1000.0, 1000.0 },
        { 1000.0, 1000.0, 1000.0 },
        { 1000.0, -1000.0, 1000.0 },
        { -1000.0, -1000.0, 1000.0 }
    };
    
    FACE obj1_faces[6] = {
        { 0,1,2,3 },
        { 4,5,6,7 },
        { 0,4,5,1 },
        { 3,7,6,2 },
        { 0,3,7,4 },
        { 1,2,6,5 }
    };
  
	*/

  fprintf(f,"int obj2_vertex_len = %d;\n",Scene.SCN_Vertex);
  fprintf(f,"int obj2_faces_len = %d;\n",Scene.SCN_Face);
  fprintf(f,"int obj2_faces_type = 3;\n");
  fprintf(f,"VERTEX obj2_vertex[%d] = {\n",Scene.SCN_Vertex);
  for( i=0; i<Scene.SCN_Vertex; i++ ) {
		if( i==Scene.SCN_Vertex-1 )
		  fprintf(f,"{ %.2f,%.2f,%.2f }\n",Scena.vptr[i].x,Scena.vptr[i].y,Scena.vptr[i].z);		
		else
		  fprintf(f,"{ %.2f,%.2f,%.2f },\n",Scena.vptr[i].x,Scena.vptr[i].y,Scena.vptr[i].z);		
  }
  fprintf(f,"};\n\n");
  fprintf(f,"FACE obj2_faces[%d] = {\n",Scene.SCN_Face);
  for( i=0; i<Scene.SCN_Face; i++ ) {
		if( i==Scene.SCN_Face-1 )
		  fprintf(f,"{ %d,%d,%d,%d }\n",Scena.fptr[i].v1,Scena.fptr[i].v2,Scena.fptr[i].v3,Scena.fptr[i].v4);		
		else
		  fprintf(f,"{ %d,%d,%d,%d },\n",Scena.fptr[i].v1,Scena.fptr[i].v2,Scena.fptr[i].v3,Scena.fptr[i].v4);		
  }
  fprintf(f,"};\n");

  fclose(f);

  return 0;
}

//------------

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

//------------

S32 PR_FindChunk(U16 ID)
{
  S32 i;

  for (i=0;i<sizeof(CHUNKS)/sizeof(CHUNKS[0]);i++)
    if (ID==CHUNKS[i].ID) return (i);

  return (-1);
}

//------------

void PR_ReadMatName()
{
  PR_CopyASCII(Material[Scene.SCN_Mat].MatName);
}

//------------

void PR_ReadMatType()
{
  Src+=h->LEN;
  Len-=h->LEN;

  h++;

  Material[Scene.SCN_Mat].Type=h->ID;
}

//------------

void PR_ReadMatFile()
{
  PR_CopyASCII(Texture[Scene.SCN_Mat++].MatFile);
}

//------------

void PR_ObjBlock()
{
  Src+=6;
  Len-=6;

  while (*Src!=0)
  {
    //putchar(*Src);
    //fputc(*Src,TR);
    Src++;
    Len--;
  }

  Src++;
  Len--;
}

//------------

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

//------------

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

//------------

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

//------------

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

//------------

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
    MapUV[p+i].v=-Temp[i].v*255.0*256.0;
  }
}

//------------

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