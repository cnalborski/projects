#include <windows.h>
#include <math.h>
#include "types.h"
#include "3d.h"
#include "scena.h"
#include "read3ds.h"
#include "resource.h"
#include "kolory.h"

//------------

#define PI 3.141592687

#define RADIAN(x) ((PI*x)/180.0)

//------------

typedef double MATRIX[3][4];

typedef struct
{
	int zet;
	int index;
} SDATA;

typedef struct
{
	int fcount;
	SDATA datas[SCN_MAX_FACE];
} SFACE;

//------------

void rotate_points(int angx,int angy);
void rotate_normals(int angx,int angy);
void prepare_faces(void);
void persp_calc(long cx,long cy);
void light_calc(void);
void sort_faces(void);
void draw_vect(HDC ekran);
void draw_drut(HDC ekran);
void draw_flat(HDC ekran);
void draw_phong(HDC ekran);

//------------

extern UINT Grafika;

SFACE* sfaces;

SDATA* ftab;
int ftablen;

VERTEX* object;
VERTEX* normals;
FACE* faces;

VERTEX* rcalc;
VERTEX* rnormal;
POINT* plane;

int* light;

int vert;
int face;

Scena_t Scena;

float zoom = 200.0;
float z_depth = 3200.0;

double tx = 0.0;
double ty = 0.0;
double tz = 0.0;

//------------

int init_vect(char* plik)
{
	if (read3ds(plik,Scena)) return -1;

	vert = Scena.vertices;
	face = Scena.faces;

	object = Scena.vptr;
	normals = Scena.nptr;
	faces = Scena.fptr;

	rcalc = new VERTEX[vert];
	rnormal = new VERTEX[vert];
	plane = new POINT[vert];

	sfaces = new SFACE[256];

	ftab = new SDATA[SCN_MAX_FACE];

	light = new int[SCN_MAX_FACE];

	return 0;
}

//------------

void reset_vect(void)
{
	delete[] object;
	delete[] normals;
	delete[] faces;
	delete[] rcalc;
	delete[] rnormal;
	delete[] plane;
	delete[] sfaces;
	delete[] ftab;
	delete[] light;
}

//------------

void display_vect(HDC ekran,int wsx,int wsy,int kx,int ky)
{
	rotate_points(kx,ky);
	rotate_normals(kx,ky);
	persp_calc(wsx,wsy);
	
	switch(Grafika)
	{
		case ID_WYPEL_FLAT:
			prepare_faces();
			sort_faces();
		break;

		case ID_WYPEL_PHONG:
			prepare_faces();
			sort_faces();
			light_calc();
		break;
	}

	draw_vect(ekran);
}

//------------

void rotate_points(int angx,int angy)
{
	MATRIX rot;
	int i;

	double SinX,SinY;
	double CosX,CosY;

	SinX = sin(RADIAN(angx));
	CosX = cos(RADIAN(angx));

	SinY = sin(RADIAN(angy));
	CosY = cos(RADIAN(angy));

    rot[0][0] = CosY;
    rot[0][1] = SinY*SinX;
    rot[0][2] = -SinY*CosX;
    rot[0][3] = tx;

    rot[1][0] = 0.0;
    rot[1][1] = CosX;
    rot[1][2] = SinX;
    rot[1][3] = ty;

    rot[2][0] = SinY;
    rot[2][1] = -CosY*SinX;
    rot[2][2] = CosY*CosX;
    rot[2][3] = tz;

	for (i=0; i<vert; i++)
	{
		rcalc[i].x = object[i].x*rot[0][0] + object[i].y*rot[0][1] + object[i].z*rot[0][2] + rot[0][3];
		rcalc[i].y = object[i].x*rot[1][0] + object[i].y*rot[1][1] + object[i].z*rot[1][2] + rot[1][3];
		rcalc[i].z = object[i].x*rot[2][0] + object[i].y*rot[2][1] + object[i].z*rot[2][2] + rot[2][3];

		object[i].x = rcalc[i].x;	
		object[i].y = rcalc[i].y;	
		object[i].z = rcalc[i].z;	
	}
}

void rotate_normals(int angx,int angy)
{
	MATRIX rot;
	int i;

	double SinX,SinY;
	double CosX,CosY;

	SinX = sin(RADIAN(angx));
	CosX = cos(RADIAN(angx));

	SinY = sin(RADIAN(angy));
	CosY = cos(RADIAN(angy));

    rot[0][0] = CosY;
    rot[0][1] = SinY*SinX;
    rot[0][2] = -SinY*CosX;

    rot[1][0] = 0.0;
    rot[1][1] = CosX;
    rot[1][2] = SinX;

    rot[2][0] = SinY;
    rot[2][1] = -CosY*SinX;
    rot[2][2] = CosY*CosX;

	for (i=0; i<vert; i++)
	{
		rnormal[i].x = normals[i].x*rot[0][0] + normals[i].y*rot[0][1] + normals[i].z*rot[0][2];
		rnormal[i].y = normals[i].x*rot[1][0] + normals[i].y*rot[1][1] + normals[i].z*rot[1][2];
		rnormal[i].z = normals[i].x*rot[2][0] + normals[i].y*rot[2][1] + normals[i].z*rot[2][2];

		normals[i].x = rnormal[i].x;	
		normals[i].y = rnormal[i].y;	
		normals[i].z = rnormal[i].z;	
	}
}

//------------

void prepare_faces(void)
{
	int i;

	long x1,y1;
	float z1;
	
	long x2,y2;
	float z2;
	
	long x3,y3;
	float z3;

	int w;

	for (i=0,ftablen=0; i<face; i++)
	{
		x1 = plane[faces[i].v1].x;
		y1 = plane[faces[i].v1].y;
		z1 = rcalc[faces[i].v1].z + z_depth;

		x2 = plane[faces[i].v2].x;
		y2 = plane[faces[i].v2].y;
		z2 = rcalc[faces[i].v2].z + z_depth;

		x3 = plane[faces[i].v3].x;
		y3 = plane[faces[i].v3].y;
		z3 = rcalc[faces[i].v3].z + z_depth;

		w = ((x1-x2)*(y3-y2)) - ((x2-x3)*(y2-y1));

		if (w > 0)
		{
			ftab[ftablen].zet = (int)((z1+z2+z3)*128.0);
			ftab[ftablen].index = i;
			ftablen++;
		}
	}
}

//------------

void persp_calc(long sx,long sy)
{
	int i;

	long cx = sx/2;
	long cy = (sy/2)-10;

	double px = (double)sx / 380.0;
	double py = (double)sy / 360.0;

	for (i=0; i<vert; i++)
	{
		plane[i].x = cx + ((rcalc[i].x*zoom*px) / (rcalc[i].z+z_depth));
		plane[i].y = cy - ((rcalc[i].y*zoom*py) / (rcalc[i].z+z_depth));
	}
}

//------------

void light_calc(void)
{
	int i;

	float x1,y1;
	float x2,y2;
	float x3,y3;
	
	int l1,l2,l3;
	int l;

	for (i=0; i<ftablen; i++)
	{
		x1 = rnormal[faces[ftab[i].index].v1].x;
		y1 = rnormal[faces[ftab[i].index].v1].y;

		l1 = (int)sqrt(x1*x1+y1*y1);

		x2 = rnormal[faces[ftab[i].index].v2].x;
		y2 = rnormal[faces[ftab[i].index].v2].y;

		l2 = (int)sqrt(x2*x2+y2*y2);

		x3 = rnormal[faces[ftab[i].index].v3].x;
		y3 = rnormal[faces[ftab[i].index].v3].y;

		l3 = (int)sqrt(x3*x3+y3*y3);

		l = (l1+l2+l3)/3;

		light[ftab[i].index] = l;
	}
}

//------------

void sort_faces(void)
{
	int i,j,k,r;
	int p;

	for (j=0; j < 8*4; j+=8)
	{
		for (i=0; i<256; i++) 
			sfaces[i].fcount = 0;

		for (i=0; i<ftablen; i++)
		{
			p = (ftab[i].zet >> j) & 0xff;
			sfaces[p].datas[sfaces[p].fcount] = ftab[i];			
			sfaces[p].fcount++;
		}

		for (i=0,k=0; i<256; i++)
		{
			if (sfaces[i].fcount)
			{
				for (r=0; r<sfaces[i].fcount; r++)  
					ftab[r+k] = sfaces[i].datas[r];
				k+=sfaces[i].fcount;
			}
		}
	}
}

//------------

void draw_vect(HDC ekran)
{
	switch(Grafika)
	{
		case ID_DRUT:
			draw_drut(ekran);
		break;

		case ID_WYPEL_FLAT:
			draw_flat(ekran);
		break;

		case ID_WYPEL_PHONG:
			draw_phong(ekran);
		break;
	}
}

void draw_drut(HDC ekran)
{
	int i;

	for (i=0; i<face; i++)
	{
		MoveToEx(ekran,plane[faces[i].v1].x,plane[faces[i].v1].y,NULL);
		LineTo(ekran,plane[faces[i].v2].x,plane[faces[i].v2].y);

		MoveToEx(ekran,plane[faces[i].v2].x,plane[faces[i].v2].y,NULL);
		LineTo(ekran,plane[faces[i].v3].x,plane[faces[i].v3].y);

		MoveToEx(ekran,plane[faces[i].v3].x,plane[faces[i].v3].y,NULL);
		LineTo(ekran,plane[faces[i].v4].x,plane[faces[i].v4].y);
	}
}

void draw_flat(HDC ekran)
{
	int i;
	POINT tri[3];

	HPEN oPen,nPen;
	HBRUSH oBrush,nBrush;

	nPen = CreatePen(PS_SOLID,1,RGB(0,0,0));
	oPen = (HPEN)SelectObject(ekran,nPen);

	nBrush = CreateSolidBrush(RGB(200,200,180));
	oBrush = (HBRUSH)SelectObject(ekran,nBrush);

	for (i=ftablen-1; i>=0; i--)
	{
		tri[0].x = plane[faces[ftab[i].index].v1].x;
		tri[0].y = plane[faces[ftab[i].index].v1].y;

		tri[1].x = plane[faces[ftab[i].index].v2].x;
		tri[1].y = plane[faces[ftab[i].index].v2].y;

		tri[2].x = plane[faces[ftab[i].index].v3].x;
		tri[2].y = plane[faces[ftab[i].index].v3].y;

		Polygon(ekran,tri,3);
	}

	SelectObject(ekran,oPen);
	SelectObject(ekran,oBrush);

	DeleteObject(nPen);
	DeleteObject(nBrush);
}

void draw_phong(HDC ekran)
{
	int i;
	POINT tri[3];

	HPEN oPen,nPen;
	HBRUSH oBrush,nBrush;

	int l;

	for (i=ftablen-1; i>=0; i--)
	{
		tri[0].x = plane[faces[ftab[i].index].v1].x;
		tri[0].y = plane[faces[ftab[i].index].v1].y;

		tri[1].x = plane[faces[ftab[i].index].v2].x;
		tri[1].y = plane[faces[ftab[i].index].v2].y;

		tri[2].x = plane[faces[ftab[i].index].v3].x;
		tri[2].y = plane[faces[ftab[i].index].v3].y;

		l = light[ftab[i].index];

		if (l < 0) l = -l;
		if (l > 255) l = 255;

		l = 255 - l;

		nPen = CreatePen(PS_SOLID,1,RGB(l,l,0));
		oPen = (HPEN)SelectObject(ekran,nPen);

		nBrush = CreateSolidBrush(RGB(l,l,0));
		oBrush = (HBRUSH)SelectObject(ekran,nBrush);
		
		Polygon(ekran,tri,3);

		SelectObject(ekran,oPen);
		SelectObject(ekran,oBrush);

		DeleteObject(nPen);
		DeleteObject(nBrush);
	}
}