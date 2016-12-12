/*
±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

 3D STUDIO ANIMATION CONVERTER - MAIN CODE
 PROGRAMMING BY MORPH/PULP

±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "readvue.h"
#include "scale.h"

typedef struct
{
  float x,y,z;
  int r,g,b;
} LIGHT;

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void main();
void mul_matrix(MATRIX mat1,MATRIX mat2,MATRIX mat3);
void pr_change_name(char* data,char* pat);

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void main(int argc,char** argv)
{
  FILE *Src,*Dst,*DCAM,*OBJ,*LIG,*Pom;

  char Temp[256];
  char ONAME[64];
  U32 FrameNo=0;

  MATRIX Cam,Tra;
  MATR *TraAddr,*TPOM;

  LIGHT* Light;

  F32 cx,cy,cz;
  F32 tx,ty,tz;
  F32 vx,vy,vz;

  F32 p1,p2;

  F32 SinX,CosX;
  F32 SinY,CosY;
  F32 SinZ,CosZ;

  F32 a,b,c;
  F32 d,e,f;
  F32 g,h,i;
  F32 j,k,l;

  float lx,ly,lz;
  float lr,lg,lb;

  int lcount = 0;

  int ltotal;

  U32 objnum = 0,p;

  if (argc != 2)
  {
    printf("Bad arguments!\n");
    exit(-1);
  }

  Src=fopen(argv[1],"rt");

  memcpy(argv[1]+strlen(argv[1])-3,"trs",3);
  Dst=fopen(argv[1],"wb");

  memcpy(argv[1]+strlen(argv[1])-3,"cam",3);
  DCAM=fopen(argv[1],"wb");

  memcpy(argv[1]+strlen(argv[1])-3,"lig",3);
  LIG=fopen(argv[1],"wb");

  memcpy(argv[1]+strlen(argv[1])-3,"lnu",3);
  Pom=fopen(argv[1],"wb");

  memcpy(argv[1]+strlen(argv[1])-3,"obj",3);
  OBJ=fopen(argv[1],"rt");

  TraAddr = malloc(64*sizeof(MATR));
  Light = malloc(64*sizeof(LIGHT));

  fseek(Dst,4L,SEEK_SET);

  printf("\n");

//----------------------------------------------------------

  while (fscanf(OBJ,"%s",ONAME)==1)
    if (strncmp("Camera",ONAME,6) != 0) objnum++;

  printf("objnum: %d\n",objnum);

//----------------------------------------------------------

  while (fscanf(Src,"%s",Temp)==1)
  {
    if (strcmp(Temp,"frame")==0)
    {
      fscanf(Src,"%s",Temp);
      printf("Converting Frame... %s\r",Temp);
      FrameNo++;
    }

    if (strcmp(Temp,"transform")==0)
    {
      fseek(OBJ,0L,SEEK_SET);

      fscanf(Src,"%s",Temp);
      fscanf(Src,"%f%f%f%f%f%f%f%f%f%f%f%f",&a,&b,&c,&d,&e,&f,&g,&h,&i,&j,&k,&l);

      p = 1;
      while (Temp[p++] != '"');
      Temp[p-1] = 0;

      p = 0;
      while (fscanf(OBJ,"%s",ONAME)==1)
      {
        //printf("Temp:%s\nONAME:%s\n",Temp+1,ONAME);

        if (strcmp(Temp+1,ONAME) == 0)
        {
        //printf("pe: %d\n",p);
        break;
        }

        p++;
      }

      TPOM = TraAddr;
      TPOM += p;

      TPOM->a0 = a;
      TPOM->c0 = d;
      TPOM->b0 = g;

      TPOM->a2 = b;
      TPOM->c2 = e;
      TPOM->b2 = h;

      TPOM->a1 = c;
      TPOM->c1 = f;
      TPOM->b1 = i;

      TPOM->a3 = j*scale;
      TPOM->c3 = k*scale;
      TPOM->b3 = l*scale;
    }

    if (strcmp(Temp,"light")==0)
    {
      fscanf(Src,"%s",Temp);
      fscanf(Src,"%f%f%f%f%f%f",&lx,&ly,&lz,&lr,&lg,&lb);

      Light[lcount].x = lx*scale;
      Light[lcount].y = lz*scale;
      Light[lcount].z = ly*scale;

      Light[lcount].r = lr*32;
      Light[lcount].g = lg*32;
      Light[lcount].b = lb*32;

      lcount++;
    }

    if (strcmp(Temp,"spotlight")==0)
    {
      fscanf(Src,"%s",Temp);
      fscanf(Src,"%f%f%f%f%f%f",&cx,&cz,&cy,&tx,&tz,&ty);

      vx=tx-cx;
      vy=ty-cy;
      vz=tz-cz;

      p1=sqrt(vx*vx+vy*vy+vz*vz);
      p2=sqrt(vx*vx+vz*vz);

      SinX=vy/p1;
      CosX=p2/p1;
      SinY=-vx/p2;
      CosY=vz/p2;

      Cam[0][0]=CosY;
      Cam[0][1]=0.0;
      Cam[0][2]=SinY;

      Cam[1][0]=SinY*SinX;
      Cam[1][1]=CosX;
      Cam[1][2]=-CosY*SinX;

      Cam[2][0]=-SinY*CosX;
      Cam[2][1]=SinX;
      Cam[2][2]=CosY*CosX;

      Cam[3][0]=-cx*scale;
      Cam[3][1]=-cy*scale;
      Cam[3][2]=-cz*scale;

      fwrite(&Cam,48,1,DCAM);
    }

    if (strcmp(Temp,"camera")==0)
    {
      fwrite(TraAddr,objnum*sizeof(MATR),1,Dst);

      fwrite(Light,lcount*sizeof(LIGHT),1,LIG);
      ltotal = lcount;
      lcount = 0;

      fscanf(Src,"%f%f%f%f%f%f",&cx,&cz,&cy,&tx,&tz,&ty);

      vx=tx-cx;
      vy=ty-cy;
      vz=tz-cz;

      p1=sqrt(vx*vx+vy*vy+vz*vz);
      p2=sqrt(vx*vx+vz*vz);

      SinX=vy/p1;
      CosX=p2/p1;
      SinY=-vx/p2;
      CosY=vz/p2;

      Cam[0][0]=CosY;
      Cam[0][1]=0.0;
      Cam[0][2]=SinY;

      Cam[1][0]=SinY*SinX;
      Cam[1][1]=CosX;
      Cam[1][2]=-CosY*SinX;

      Cam[2][0]=-SinY*CosX;
      Cam[2][1]=SinX;
      Cam[2][2]=CosY*CosX;

      Cam[3][0]=-cx*scale;
      Cam[3][1]=-cy*scale;
      Cam[3][2]=-cz*scale;

      fwrite(&Cam,48,1,DCAM);
    }
  }

  printf("\n");

  fseek(Dst,0L,SEEK_SET);
  fwrite(&FrameNo,4,1,Dst);

  fwrite(&ltotal,sizeof(ltotal),1,Pom);

  fclose(Src);
  fclose(Dst);
  fclose(DCAM);
  fclose(OBJ);
  fclose(LIG);
  fclose(Pom);

  free(TraAddr);
  free(Light);
}

void pr_change_name(char* data,char* pat)
{
  int i;

  for(i=0;data[i]!='.';i++);

}

//°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

void mul_matrix(MATRIX mat1,MATRIX mat2,MATRIX mat3)
{
        int i,j,k;
        MATRIX result;

        for (i=0;i<4;i++)
         for (j=0;j<4;j++)
          result[i][j]=mat2[i][0]*mat3[0][j]+mat2[i][1]*mat3[1][j]+mat2[i][2]*mat3[2][j]+mat2[i][3]*mat3[3][j];

        for (i=0;i<4;i++)
         for (j=0;j<4;j++)
          mat1[i][j]=result[i][j];
}