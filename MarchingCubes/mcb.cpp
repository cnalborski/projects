// mcb.cpp : Defines the entry point for the application.
//
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <commctrl.h>
#include <commdlg.h>

#include "resource.h"

#define MAX_LOADSTRING 100

#define PI 3.141592687
#define RADIAN(x) ((PI*x)/180.0)

#define Window_X_Start 100
#define Window_Y_Start 100
#define Window_X_Size 800
#define Window_Y_Size 800

#define Window_Menu_Size 240

#define ns 300.0

#define ImageTypeRaw 1
#define ImageTypeBmp 2

typedef struct {
   float x,y,z;
} VERTEX;

typedef float MATRIX[3][4];

typedef struct {
	float x,y,z;
} XYZ;

typedef struct {
   XYZ p[3];
} TRIANGLE;

typedef struct {
   int p[3];
} TRIANGLEINDEX;

typedef struct {
   XYZ p[8];
   float val[8];
} GRIDCELL;

typedef struct {
	int zet;
	int index;
} SDATA;

typedef struct {
	int fcount;
	SDATA *datas;
} SFACE;

int draw_mode=1; /* 1: drut 2: flat */

int Grid_X_Size=0;
int Grid_Y_Size=0;
int Grid_Z_Size=0;

float Cell_Size = 1.0;

float ThresholdValue = 5.0;

GRIDCELL *Cells=NULL;
int Cells_len=Grid_X_Size*Grid_Y_Size*Grid_Z_Size;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

HWND hWnd;

HCURSOR hArrow,hHand1,hHand2,hZoom,hAxis;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK UstawieniaKolorow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Open(HWND okno,char* plik,int rozm,char *filtry);
BOOL Save(HWND okno,char* plik,int rozm,char *filtry);
BOOL CALLBACK Dlg1Prg(HWND hDlg,UINT komunikat,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK DlgI3DPrg(HWND hDlg,UINT komunikat,WPARAM wParam,LPARAM lParam);

void WriteBMPFile(HBITMAP bitmap, LPTSTR filename, HDC hDC);
void Write3DFile(LPTSTR filename);
void ChooseColorDialog(COLORREF &pcrColor);

bool CalculatePhongPal(void);

char DlgI3DTitle[100];

int drawFlag=0;

int FillCells(void);
int PolygoniseAll(void);
int init_vect(void);
int draw(HDC ekran,int wsx,int wsy,int kx,int ky);

bool I3DStat;

HBRUSH hBrush_KolorTla;
COLORREF KolorTla;

HBRUSH hBrush_Drut_KolorLinii;
COLORREF Drut_KolorLinii;

HBRUSH hBrush_Flat_KolorLinii;
COLORREF Flat_KolorLinii;
HBRUSH hBrush_Flat_KolorWypelnienia;
COLORREF Flat_KolorWypelnienia;

HBRUSH hBrush_Phong_KolorPoczatkowy;
COLORREF Phong_KolorPoczatkowy;
HBRUSH hBrush_Phong_KolorKoncowy;
COLORREF Phong_KolorKoncowy;

typedef struct {
	int r,g,b;
} Color_t;

Color_t PhongPal[256];

float zoom = 400.0;
float z_depth = 200.0;

float tx = 0.0;
float ty = 0.0;
float tz = 0.0;

bool PrzeliczFlag=false;

VERTEX* object=NULL;
VERTEX* normal=NULL;
VERTEX* rcalc=NULL;
VERTEX* rnormal=NULL;
int* n_len=NULL;
POINT* plane=NULL;
int* light=NULL;

int vert=0;

SDATA* ftab=NULL;
int ftablen;

SFACE* sfaces=NULL;

TRIANGLE *Triangles=NULL;
TRIANGLEINDEX *TrianglesIndex=NULL;
int Triangles_len;

int edgeTable[256]={
0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };

int triTable[256][16] =
{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

class I3D {
public:
	char **Warstwy;
	int IloscWarstw;
	int SzerokoscWarstwy;
	int WysokoscWarstwy;
	float WielkoscCeli;
	float WartoscProgowa;
	float Powiekszenie;
	float Obserwator;
	int ImageType;
 
	I3D();
	~I3D();

	bool CopyI3D(I3D *d);
	int DodajWarstwe(char *Warstwa);
	int ZmienWarstwe(char *Warstwa,int ind);
	int UsunWarstwe(int ind);
	int UsunWszystkieWarstwy(void);
	char *GetWarstwa(int i);
	int GetIloscWarstw(void);
	int GetImageType(void);
	bool LoadI3D(char *plik);
	bool SaveI3D(char *plik);
	bool SetParams(	int SzerokoscWarstwy,
					int WysokoscWarstwy,
					int IloscWarstw,
					float WielkoscCeli,
					float WartoscProgowa,
					float Powiekszenie,
					float Obserwator);
	bool GetParams(	int &SzerokoscWarstwy,
					int &WysokoscWarstwy,
					int &IloscWarstw,
					float &WielkoscCeli,
					float &WartoscProgowa,
					float &Powiekszenie,
					float &Obserwator);
};

I3D::I3D() {
	Warstwy=NULL;
	IloscWarstw=0;
	SzerokoscWarstwy=0;
	WysokoscWarstwy=0;
	WielkoscCeli=0.0;
	WartoscProgowa=0.0;
	Powiekszenie=0.0;
	Obserwator=0.0;
}

I3D::~I3D() {
	UsunWszystkieWarstwy();
}

bool I3D::CopyI3D(I3D *d) {
	for(int i=0;i<d->IloscWarstw;i++)
		DodajWarstwe(d->GetWarstwa(i));

	SzerokoscWarstwy=d->SzerokoscWarstwy;
	WysokoscWarstwy=d->WysokoscWarstwy;
	WielkoscCeli=d->WielkoscCeli;
	WartoscProgowa=d->WartoscProgowa;
	Powiekszenie=d->Powiekszenie;
	Obserwator=d->Obserwator;
	ImageType=d->ImageType;

	return true;
}

int I3D::DodajWarstwe(char *Warstwa) {
	char **tmpWarstwy = new char*[IloscWarstw+1];
	if( IloscWarstw ) {
		memcpy(tmpWarstwy,Warstwy,IloscWarstw*sizeof(char*));
	}
	delete[] Warstwy;
	Warstwy=tmpWarstwy;
	int l=strlen(Warstwa);
	Warstwy[IloscWarstw] = new char[l+1];
	memcpy(Warstwy[IloscWarstw],Warstwa,l+1);
	return IloscWarstw++;
}

int I3D::ZmienWarstwe(char *Warstwa,int ind) {
	if( ind<0 || ind>IloscWarstw-1 )
		return -1;

	delete[] Warstwy[ind];
	int l=strlen(Warstwa);
	Warstwy[ind] = new char[l+1];
	memcpy(Warstwy[ind],Warstwa,l+1);

	return ind;
}

int I3D::UsunWarstwe(int ind) {
	if( IloscWarstw ) {
		if( ind<0 || ind>IloscWarstw-1 )
			return -1;
		
		char **tmpWarstwy = new char*[IloscWarstw-1];
		for(int i=0,j=0;i<IloscWarstw; i++) {
			if( i!=ind )
				tmpWarstwy[j++]=Warstwy[i];
		}
		delete[] Warstwy;
		Warstwy=tmpWarstwy;
		IloscWarstw--;
	}
	return ind;
}

int I3D::UsunWszystkieWarstwy(void) {
	if( IloscWarstw ) {
		for(int i=0; i<IloscWarstw; i++) {
			delete[] Warstwy[i];
		}
		delete[] Warstwy;
	}
	Warstwy=NULL;
	IloscWarstw=0;
	return 1;
}

char *I3D::GetWarstwa(int i) {
	if( i<0 || i>IloscWarstw-1 )
		return NULL;
	return Warstwy[i];
}

int I3D::GetIloscWarstw(void) {
	return IloscWarstw;
}

int I3D::GetImageType(void) {
	return ImageType;
}

bool I3D::LoadI3D(char *plik) {
	FILE *f;
	char tmp[100];

	f = fopen(plik,"rt");
	if( f==NULL )
		return false;

	fgets(tmp,sizeof(tmp),f);
	tmp[strlen(tmp)-1]=0;
	SzerokoscWarstwy = atoi(tmp);

	fgets(tmp,sizeof(tmp),f);
	tmp[strlen(tmp)-1]=0;
	WysokoscWarstwy = atoi(tmp);

	fgets(tmp,sizeof(tmp),f);
	tmp[strlen(tmp)-1]=0;
	int tmpIloscWarstw = atoi(tmp);

	fgets(tmp,sizeof(tmp),f);
	tmp[strlen(tmp)-1]=0;
	WielkoscCeli = atof(tmp);

	fgets(tmp,sizeof(tmp),f);
	tmp[strlen(tmp)-1]=0;
	WartoscProgowa = atof(tmp);

	fgets(tmp,sizeof(tmp),f);
	tmp[strlen(tmp)-1]=0;
	Powiekszenie = atof(tmp);

	fgets(tmp,sizeof(tmp),f);
	tmp[strlen(tmp)-1]=0;
	Obserwator = atof(tmp);

	fgets(tmp,sizeof(tmp),f);
	tmp[strlen(tmp)-1]=0;
	if( !strcmp(strupr(tmp),"BMP") ) {
		ImageType = ImageTypeBmp;
	}
	else
	if( !strcmp(strupr(tmp),"RAW") ) {
		ImageType = ImageTypeRaw;
	}
	else {
		fclose(f);
		return false;
	}

	for( int i=0; i<tmpIloscWarstw; i++ ) {
		fgets(tmp,sizeof(tmp),f);
		tmp[strlen(tmp)-1]=0;
		DodajWarstwe(tmp);
	}

	fclose(f);

	return true;
}

bool I3D::SaveI3D(char *plik) {
	FILE *f;

	f = fopen(plik,"wt");
	if( f==NULL )
		return false;

	fprintf(f,"%d\n",SzerokoscWarstwy);
	fprintf(f,"%d\n",WysokoscWarstwy);
	fprintf(f,"%d\n",IloscWarstw);
	fprintf(f,"%.2f\n",WielkoscCeli);
	fprintf(f,"%.2f\n",WartoscProgowa);
	fprintf(f,"%.2f\n",Powiekszenie);
	fprintf(f,"%.2f\n",Obserwator);
	fprintf(f,"bmp\n");
	
	for( int i=0; i<IloscWarstw; i++ )
		fprintf(f,"%s\n",Warstwy[i]);

	fclose(f);

	return true;
}

bool I3D::SetParams(	int SzerokoscWarstwy,
						int WysokoscWarstwy,
						int IloscWarstw,
						float WielkoscCeli,
						float WartoscProgowa,
						float Powiekszenie,
						float Obserwator) {
	
	this->SzerokoscWarstwy=SzerokoscWarstwy;
	this->WysokoscWarstwy=WysokoscWarstwy;
	this->IloscWarstw=IloscWarstw;
	this->WielkoscCeli=WielkoscCeli;
	this->WartoscProgowa=WartoscProgowa;
	this->Powiekszenie=Powiekszenie;
	this->Obserwator=Obserwator;

	return true;
}

bool I3D::GetParams(	int &SzerokoscWarstwy,
						int &WysokoscWarstwy,
						int &IloscWarstw,
						float &WielkoscCeli,
						float &WartoscProgowa,
						float &Powiekszenie,
						float &Obserwator) {
	
	SzerokoscWarstwy=this->SzerokoscWarstwy;
	WysokoscWarstwy=this->WysokoscWarstwy;
	IloscWarstw=this->IloscWarstw;
	WielkoscCeli=this->WielkoscCeli;
	WartoscProgowa=this->WartoscProgowa;
	Powiekszenie=this->Powiekszenie;
	Obserwator=this->Obserwator;

	return true;
}

class bmpLoader {
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	RGBQUAD *colours;
	BYTE *PixelData;
public:
	bmpLoader();
	~bmpLoader();
	bool load(char *file);
	bool loadRaw(char *file,int Width,int Height);

	LONG getWidth() { return bmih.biWidth; }
	LONG getHeight() { return bmih.biHeight; }
	WORD getBitCount() { return bmih.biBitCount; }
	BYTE *getPixelData() { return PixelData; }
	RGBQUAD *getPalette() { return colours; };
};

bmpLoader::bmpLoader() {
	colours=NULL;
	PixelData=NULL;
};

bmpLoader::~bmpLoader() {
	if( colours != NULL )
		delete[] colours;
	if( PixelData != NULL )
		delete[] PixelData;
}

bool bmpLoader::load(char *file) {
	FILE *f;

	int numColours;
	DWORD size;

	f = fopen(file,"rb");
	if( f==NULL ) {
		return false;
	}
	else {
		//load BITMAPFILEHEADER
		fread(&bmfh,sizeof(BITMAPFILEHEADER),1,f);
	
		//load BITMAPINFOHEADER
		fread(&bmih,sizeof(BITMAPINFOHEADER),1,f);
			
		numColours=1 << bmih.biBitCount;
		//load the palette for 8 bits per pixel
		if(bmih.biBitCount == 8) {
			colours=new RGBQUAD[numColours];
			fread(colours,sizeof(RGBQUAD),numColours,f);
		}
		//load image
		size=bmfh.bfSize-bmfh.bfOffBits;
		PixelData=new BYTE[size];
		fread(PixelData,sizeof(BYTE),size,f);

		fclose(f);

		//padding
		if( getWidth()%4 != 0 ) {
			for( int x=0,xx=0,y=0; y<getHeight(); y++ ) {
				memcpy(PixelData+xx,PixelData+x,getWidth());
				xx+=getWidth();
				x+=getWidth()+(getWidth()%4);
			}
		}

		//order
		if( bmih.biHeight ) {
			BYTE *tmpPixelData = new BYTE[size];
			for( int y=0; y<getHeight(); y++ )
				for( int x=0; x<getWidth(); x++ )
					tmpPixelData[x+(getWidth()*y)] = PixelData[x+(getWidth()*(getHeight()-y))];
			delete[] PixelData;
			PixelData = tmpPixelData;
		}
	}

	return true;
}

bool bmpLoader::loadRaw(char *file,int Width,int Height) {
	FILE *f;

	int numColours;
	DWORD size;

	f = fopen(file,"rb");
	if( f==NULL ) {
		return false;
	}
	else {
		bmih.biWidth = Width;
		bmih.biHeight = Height;
		bmih.biBitCount = 8;
		
		//load image
		size=Width*Height;
		PixelData=new BYTE[size];
		fread(PixelData,sizeof(BYTE),size,f);

		fclose(f);
	}

	return true;
}

I3D *DaneI3D=NULL;
I3D *tmpDaneI3D=NULL;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MCB, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MCB);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_MCB);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= (LPCSTR)IDC_MCB;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   /*hWnd = CreateWindow(
							szWindowClass, 
							szTitle, 
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT, 
							0, 
							CW_USEDEFAULT, 
							0, 
							NULL, 
							NULL, 
							hInstance, 
							NULL);

				  );*/
	InitCommonControls();

	hWnd = CreateWindowEx(0,
						  szWindowClass,
						  szTitle,			
						  WS_OVERLAPPEDWINDOW,	 
						  Window_X_Start,						
						  Window_Y_Start,						
					 	  Window_X_Size+Window_Menu_Size,
						  Window_Y_Size,					    
						  NULL,                     
						  NULL,                     
						  hInstance,			
						  NULL); 	

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   hArrow = LoadCursor(NULL,IDC_ARROW);
   hHand1 = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_CURSOR1));
   hHand2 = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_CURSOR2));
   hZoom = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_CURSOR3));
   hAxis = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_CURSOR4));

   return TRUE;
}

bmpLoader bmpFile;

int DrawBitmap(HDC ekran) {
	BYTE *PixelData = bmpFile.getPixelData();
	RGBQUAD *Palette = bmpFile.getPalette();

	LONG Height = bmpFile.getHeight();
	LONG Width = bmpFile.getWidth();

	for(int y=0; y<Height; y++) {
		for(int x=0; x<Width; x++) {
			BYTE i=PixelData[x+(y*Width)];
			SetPixel(ekran, x,y,RGB( Palette[i].rgbRed,Palette[i].rgbGreen, Palette[i].rgbBlue) );
		}
	}

	return 0;
}

HWND hZoomEdit;

HFONT hFont = (HFONT)GetStockObject (ANSI_VAR_FONT);

HBITMAP mapa;
HDC sbufor;

POINT old,rmbold;
int kx = 0,ky = 0,rmbkx = 0,rmbky = 0;
int LMB=0,RMB=0;

char plik[255];
char plikdir[255];

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	PAINTSTRUCT ps;
	HDC ekran;
	RECT winRect;

	int	winSX,winSY;
	int mx,my;
	
	//HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	char tmp[255];

	RECT RectTmp;

	int MsgBoxStat;

	switch (message) 
	{
		case WM_CREATE:
			KolorTla = RGB(100,100,100);
			Drut_KolorLinii = RGB(10,10,10);
			Flat_KolorLinii = RGB(10,10,10);
			Flat_KolorWypelnienia = RGB(190,190,190);
			Phong_KolorPoczatkowy = RGB(10,10,0);
			Phong_KolorKoncowy = RGB(200,200,0);

			hBrush_KolorTla = CreateSolidBrush( KolorTla );

			hBrush_Drut_KolorLinii = CreateSolidBrush( Drut_KolorLinii );

			hBrush_Flat_KolorLinii = CreateSolidBrush( Flat_KolorLinii );
			hBrush_Flat_KolorWypelnienia = CreateSolidBrush( Flat_KolorWypelnienia );

			hBrush_Phong_KolorPoczatkowy = CreateSolidBrush( Phong_KolorPoczatkowy );
			hBrush_Phong_KolorKoncowy = CreateSolidBrush( Phong_KolorKoncowy );

			CalculatePhongPal();

			CreateWindow("BUTTON",
						 "Tryb renderowania [Drut]",
						 WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						 Window_X_Size+10,
						 0,
						 Window_Menu_Size-30,
						 140,
						 hWnd,
						 (HMENU)ID_RIGHTMENUBORDER,
						 hInst,
						 NULL);

			CreateWindow("BUTTON",
						 "Drut",
						 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						 Window_X_Size+40,
						 30,
						 140,
						 22,
						 hWnd,
						 (HMENU)ID_DRUT,
						 hInst,
						 NULL);

			CreateWindow("BUTTON",
						 "P³askie",
						 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						 Window_X_Size+40,
						 58,
						 140,
						 22,
						 hWnd,
						 (HMENU)ID_FLAT,
						 hInst,
						 NULL);

			CreateWindow("BUTTON",
						 "Oœwietlenie",
						 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						 Window_X_Size+40,
						 86,
						 140,
						 22,
						 hWnd,
						 (HMENU)ID_PHONG,
						 hInst,
						 NULL);


			CreateWindow("BUTTON",
						 "Parametry renderowania",
						 WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						 Window_X_Size+10,
						 150,
						 Window_Menu_Size-30,
						 140,
						 hWnd,
						 (HMENU)ID_RIGHTMENUBORDER2,
						 hInst,
						 NULL);

			CreateWindow("STATIC",
						 "Powiêkszenie:",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+30,
						 180,
						 80,
						 22,
						 hWnd,
						 (HMENU)ID_ZOOMTEXT,
						 hInst,
						 NULL);

			hZoomEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
						"EDIT",
						 "",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+120,
						 180-4,
						 60,
						 22,
						 hWnd,
						 (HMENU)ID_ZOOM,
						 hInst,
						 NULL);

			CreateWindow("STATIC",
						 "Obserwator:",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+30,
						 180+30,
						 80,
						 22,
						 hWnd,
						 (HMENU)ID_ZDEPTHTEXT,
						 hInst,
						 NULL);

			CreateWindowEx(WS_EX_CLIENTEDGE,
						"EDIT",
						 "",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+120,
						 180-4+30,
						 60,
						 22,
						 hWnd,
						 (HMENU)ID_ZDEPTH,
						 hInst,
						 NULL);

			CreateWindow("BUTTON",
						 "Zastosuj",
						 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						 Window_X_Size+40,
						 180-4+30+40,
						 140,
						 22,
						 hWnd,
						 (HMENU)ID_ZASTOSUJ,
						 hInst,
						 NULL);

			CreateWindow("BUTTON",
						 "Parametry voxeli",
						 WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						 Window_X_Size+10,
						 150+150,
						 Window_Menu_Size-30,
						 140,
						 hWnd,
						 (HMENU)ID_RIGHTMENUBORDER3,
						 hInst,
						 NULL);

			CreateWindow("STATIC",
						 "Wielkoœæ voxela:",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+30,
						 180+150,
						 100,
						 22,
						 hWnd,
						 (HMENU)ID_CELLSIZE,
						 hInst,
						 NULL);

			CreateWindowEx(WS_EX_CLIENTEDGE,
						"EDIT",
						 "",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+130,
						 180-4+150,
						 60,
						 22,
						 hWnd,
						 (HMENU)ID_CELLSIZEEDIT,
						 hInst,
						 NULL);


			CreateWindow("STATIC",
						 "Wartoœæ progowa:",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+30,
						 180+30+150,
						 100,
						 22,
						 hWnd,
						 (HMENU)ID_THRESHOLDVALUE,
						 hInst,
						 NULL);

			CreateWindowEx(WS_EX_CLIENTEDGE,
						"EDIT",
						 "",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+130,
						 180-4+30+150,
						 60,
						 22,
						 hWnd,
						 (HMENU)ID_THRESHOLDVALUEEDIT,
						 hInst,
						 NULL);

			CreateWindow("BUTTON",
						 "Przelicz",
						 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						 Window_X_Size+40,
						 180-4+30+40+150,
						 140,
						 22,
						 hWnd,
						 (HMENU)ID_PRZELICZ,
						 hInst,
						 NULL);

			CreateWindow("BUTTON",
						 "Informacje",
						 WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
						 Window_X_Size+10,
						 150+150+150,
						 Window_Menu_Size-30,
						 110,
						 hWnd,
						 (HMENU)ID_RIGHTMENUBORDER4,
						 hInst,
						 NULL);

			CreateWindow("STATIC",
						 "Iloœæ wierzcho³ków:",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+30,
						 180+150+150,
						 140,
						 22,
						 hWnd,
						 (HMENU)ID_VERT,
						 hInst,
						 NULL);

			CreateWindow("STATIC",
						 "Iloœæ trójk¹tów:",
						 WS_CHILD | WS_VISIBLE,
						 Window_X_Size+30,
						 180+150+150+30,
						 140,
						 22,
						 hWnd,
						 (HMENU)ID_FACES,
						 hInst,
						 NULL);

			SendDlgItemMessage(hWnd, ID_RIGHTMENUBORDER, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_DRUT, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_FLAT, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_PHONG, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));

			SendDlgItemMessage(hWnd, ID_RIGHTMENUBORDER2, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_ZOOM, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_ZOOMTEXT, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_ZDEPTH, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_ZDEPTHTEXT, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_ZASTOSUJ, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));

			SendDlgItemMessage(hWnd, ID_RIGHTMENUBORDER3, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_CELLSIZE, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_CELLSIZEEDIT, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_THRESHOLDVALUE, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_THRESHOLDVALUEEDIT, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_PRZELICZ, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));

			SendDlgItemMessage(hWnd, ID_RIGHTMENUBORDER4, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_VERT, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));
			SendDlgItemMessage(hWnd, ID_FACES, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));

			SendDlgItemMessage(hWnd, ID_RIGHTMENUBORDER5, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG (TRUE, 0));

			GetWindowRect(hWnd,&winRect);
			winSX = winRect.right - winRect.left-Window_Menu_Size;
			winSY = winRect.bottom - winRect.top;

			ekran = GetDC(hWnd);
			mapa = CreateCompatibleBitmap(ekran,winSX,winSY);
			sbufor = CreateCompatibleDC(ekran);
			SelectObject(sbufor,mapa);
			RectTmp.top=0;
			RectTmp.left=0;
			RectTmp.right=winSX;
			RectTmp.bottom=winSY;
			//PatBlt(sbufor,0,0,winSX,winSY,WHITENESS);			
			FillRect(sbufor, &RectTmp, hBrush_KolorTla);
			ReleaseDC(hWnd,ekran);

			EnableMenuItem( GetMenu(hWnd), ID_PLIK_EDYTUJ, MF_GRAYED );
			EnableMenuItem( GetMenu(hWnd), ID_PLIK_ZAPISZJAKO, MF_GRAYED );

			//DaneI3D = new I3D();
			//DaneI3D->ImageType = ImageTypeBmp;

			sprintf(tmp,"%.1f",zoom);
			SendDlgItemMessage(hWnd,ID_ZOOM,WM_SETTEXT,0,(LPARAM)tmp);
			sprintf(tmp,"%.1f",z_depth);
			SendDlgItemMessage(hWnd,ID_ZDEPTH,WM_SETTEXT,0,(LPARAM)tmp);

			sprintf(tmp,"%.1f",Cell_Size);
			SendDlgItemMessage(hWnd,ID_CELLSIZEEDIT,WM_SETTEXT,0,(LPARAM)tmp);
			sprintf(tmp,"%.1f",ThresholdValue);
			SendDlgItemMessage(hWnd,ID_THRESHOLDVALUEEDIT,WM_SETTEXT,0,(LPARAM)tmp);

			sprintf(tmp,"Iloœæ wierzcho³ków: %d",vert);
			SendDlgItemMessage(hWnd,ID_VERT,WM_SETTEXT,0,(LPARAM)tmp);
			sprintf(tmp,"Iloœæ trójk¹tów: %d",Triangles_len);
			SendDlgItemMessage(hWnd,ID_FACES,WM_SETTEXT,0,(LPARAM)tmp);

			sprintf(tmp,"Marching Cubes [Bez nazwy]");
			SetWindowText(hWnd,tmp);
		break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId) 
			{
				case ID_PLIK_OTWRZ:
					if (Open(hWnd,plik,sizeof(plik),"Pliki i3d\0*.i3d\0Wszystkie pliki\0*.*\0"))
					{
						strcpy(plikdir,plik);						
						if( strchr(plikdir,'\\') ) {
							for(int i=strlen(plikdir)-1; i--; i>=0)
								if( plikdir[i]=='\\' )
									break;
							plikdir[i]=0;
							if( i )
								strcat(plikdir,"\\");
						}

						if( DaneI3D != NULL ) {
							delete DaneI3D;
							DaneI3D = new I3D();
						}
						if( DaneI3D == NULL ) {
							DaneI3D = new I3D();
							DaneI3D->ImageType = ImageTypeBmp;							
						}
						if( !DaneI3D->LoadI3D(plik) ) {
							sprintf(tmp,"Nie mo¿na za³adowaæ pliku: %s",plik);
							MessageBox(hWnd,tmp,"B³¹d",MB_ICONEXCLAMATION);
							delete[] DaneI3D;
							return -1;
						}
						sprintf(tmp,"Marching Cubes [%s]",plik);
						SetWindowText(hWnd,tmp);

						DialogBox(hInst,MAKEINTRESOURCE(IDD_WCZYTYWANIE),hWnd,(DLGPROC)Dlg1Prg);

						EnableMenuItem( GetMenu(hWnd), ID_PLIK_EDYTUJ, MF_ENABLED );
						EnableMenuItem( GetMenu(hWnd), ID_PLIK_ZAPISZJAKO, MF_ENABLED );

						if( PrzeliczFlag==true ) {
							sprintf(tmp,"%.1f",zoom);
							SendDlgItemMessage(hWnd,ID_ZOOM,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"%.1f",z_depth);
							SendDlgItemMessage(hWnd,ID_ZDEPTH,WM_SETTEXT,0,(LPARAM)tmp);

							sprintf(tmp,"%.1f",Cell_Size);
							SendDlgItemMessage(hWnd,ID_CELLSIZEEDIT,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"%.1f",ThresholdValue);
							SendDlgItemMessage(hWnd,ID_THRESHOLDVALUEEDIT,WM_SETTEXT,0,(LPARAM)tmp);

							sprintf(tmp,"Iloœæ wierzcho³ków: %d",vert);
							SendDlgItemMessage(hWnd,ID_VERT,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"Iloœæ trójk¹tów: %d",Triangles_len);
							SendDlgItemMessage(hWnd,ID_FACES,WM_SETTEXT,0,(LPARAM)tmp);
						}
						
						InvalidateRect(hWnd,NULL,FALSE);
					}
				break;

				case ID_PLIK_ZAPISZJAKO:
					if (Save(hWnd,plik,sizeof(plik),"Pliki i3d\0*.i3d\0Wszystkie pliki\0*.*\0")) {
						DaneI3D->SetParams( Grid_X_Size,
											Grid_Y_Size,
											Grid_Z_Size,
											Cell_Size,
											ThresholdValue,
											zoom,
											z_depth);
						DaneI3D->SaveI3D(plik);
					}
					break;

				case ID_PLIK_NOWY:
					MsgBoxStat = IDYES;

					if( (DaneI3D != NULL) && (DaneI3D->GetIloscWarstw()>0) ) {
						MsgBoxStat = MessageBox( hWnd, 
												 "Czy jesteœ pewien ¿e chcesz utworzyæ nowy plik?",
												 "Nowy plik", 
												 MB_ICONWARNING | MB_YESNO );

						if( MsgBoxStat == IDYES ) {
							delete DaneI3D;

							DaneI3D = new I3D();
							DaneI3D->ImageType = ImageTypeBmp;
							
							Grid_X_Size=0;
							Grid_Y_Size=0;
							Grid_Z_Size=0;

							Cell_Size = 1.0;

							ThresholdValue = 5.0;
							zoom = 400.0;
							z_depth = 200.0;

							drawFlag=0;

							sprintf(tmp,"%.1f",zoom);
							SendDlgItemMessage(hWnd,ID_ZOOM,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"%.1f",z_depth);
							SendDlgItemMessage(hWnd,ID_ZDEPTH,WM_SETTEXT,0,(LPARAM)tmp);

							sprintf(tmp,"%.1f",Cell_Size);
							SendDlgItemMessage(hWnd,ID_CELLSIZEEDIT,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"%.1f",ThresholdValue);
							SendDlgItemMessage(hWnd,ID_THRESHOLDVALUEEDIT,WM_SETTEXT,0,(LPARAM)tmp);

							sprintf(tmp,"Iloœæ wierzcho³ków: %d",vert);
							SendDlgItemMessage(hWnd,ID_VERT,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"Iloœæ trójk¹tów: %d",Triangles_len);
							SendDlgItemMessage(hWnd,ID_FACES,WM_SETTEXT,0,(LPARAM)tmp);
							
							if( Cells!=NULL ) {
								delete[] Cells;
								Cells = NULL;
								delete[] object;
								delete[] normal;
								delete[] rcalc;
								delete[] rnormal;
								delete[] n_len;
								delete[] plane;
								delete[] light;
								delete[] ftab;
								for( int f=0; f<256; f++ )
									delete[] sfaces[f].datas;
								delete[] sfaces;
								delete[] Triangles;
								delete[] TrianglesIndex;
							}

							sprintf(tmp,"Marching Cubes [Bez nazwy]");
							SetWindowText(hWnd,tmp);

							plik[0]=0;
							InvalidateRect(hWnd,NULL,NULL);
						}
					}

					if( MsgBoxStat == IDYES ) {
						EnableMenuItem( GetMenu(hWnd), ID_PLIK_EDYTUJ, MF_ENABLED );
						EnableMenuItem( GetMenu(hWnd), ID_PLIK_ZAPISZJAKO, MF_ENABLED );

						I3DStat=false;
						sprintf(DlgI3DTitle,"Nowy plik");
						DialogBox(hInst,MAKEINTRESOURCE(IDD_I3D),hWnd,(DLGPROC)DlgI3DPrg);

						if( I3DStat==true ) {
							DaneI3D->SetParams( Grid_X_Size,
												Grid_Y_Size,
												Grid_Z_Size,
												Cell_Size,
												ThresholdValue,
												zoom,
												z_depth);

							DialogBox(hInst,MAKEINTRESOURCE(IDD_WCZYTYWANIE),hWnd,(DLGPROC)Dlg1Prg);
						}
					}
					
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_PLIK_EDYTUJ:
					I3DStat=false;
					sprintf(DlgI3DTitle,"Edytuj plik");
					DialogBox(hInst,MAKEINTRESOURCE(IDD_I3D),hWnd,(DLGPROC)DlgI3DPrg);

					if( I3DStat==true ) {
						DaneI3D->SetParams( Grid_X_Size,
											Grid_Y_Size,
											Grid_Z_Size,
											Cell_Size,
											ThresholdValue,
											zoom,
											z_depth);

						DialogBox(hInst,MAKEINTRESOURCE(IDD_WCZYTYWANIE),hWnd,(DLGPROC)Dlg1Prg);

						if( PrzeliczFlag==true ) {
							sprintf(tmp,"%.1f",zoom);
							SendDlgItemMessage(hWnd,ID_ZOOM,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"%.1f",z_depth);
							SendDlgItemMessage(hWnd,ID_ZDEPTH,WM_SETTEXT,0,(LPARAM)tmp);

							sprintf(tmp,"%.1f",Cell_Size);
							SendDlgItemMessage(hWnd,ID_CELLSIZEEDIT,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"%.1f",ThresholdValue);
							SendDlgItemMessage(hWnd,ID_THRESHOLDVALUEEDIT,WM_SETTEXT,0,(LPARAM)tmp);

							sprintf(tmp,"Iloœæ wierzcho³ków: %d",vert);
							SendDlgItemMessage(hWnd,ID_VERT,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"Iloœæ trójk¹tów: %d",Triangles_len);
							SendDlgItemMessage(hWnd,ID_FACES,WM_SETTEXT,0,(LPARAM)tmp);
						}
					}

					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_ZAPISZOBRAZ:
					if( DaneI3D->GetIloscWarstw() ) {
						if (Save(hWnd,plik,sizeof(plik),"Pliki bmp\0*.bmp\0Wszystkie pliki\0*.*\0")) {
							WriteBMPFile(mapa,plik,sbufor);
							sprintf(tmp,"Marching Cubes [%s]",plik);
							SetWindowText(hWnd,tmp);
							InvalidateRect(hWnd,NULL,NULL);
						}
					}
					else
						MessageBox(hWnd,"Brak definicji warstw!","B³¹d",MB_ICONEXCLAMATION);
					break;

				case ID_ZAPISZ3DSTUDIO:
					if (Save(hWnd,plik,sizeof(plik),"Pliki asc\0*.asc\0Wszystkie pliki\0*.*\0")) {
						Write3DFile(plik);					
					}
					break;

				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;

				case ID_USTAWIENIAKOLOROW:
					DialogBox(hInst, (LPCTSTR)IDD_USTAWIENIAKOLOROW, hWnd, (DLGPROC)UstawieniaKolorow);
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;

				case ID_DRUT:
					draw_mode = 1;
					sprintf(tmp,"Tryb renderowania: [Drut]");
					SendDlgItemMessage(hWnd,ID_RIGHTMENUBORDER,WM_SETTEXT,0,(LPARAM)tmp);
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_FLAT:
					draw_mode = 2;
					sprintf(tmp,"Tryb renderowania: [P³askie]");
					SendDlgItemMessage(hWnd,ID_RIGHTMENUBORDER,WM_SETTEXT,0,(LPARAM)tmp);
					InvalidateRect(hWnd,NULL,FALSE);
					break;

				case ID_PHONG:
					draw_mode = 3;
					sprintf(tmp,"Tryb renderowania: [Oœwietlenie]");
					SendDlgItemMessage(hWnd,ID_RIGHTMENUBORDER,WM_SETTEXT,0,(LPARAM)tmp);
					InvalidateRect(hWnd,NULL,FALSE);
					break;
	
				case ID_ZASTOSUJ:
					if( Cells != NULL ) {
						SendDlgItemMessage(hWnd,ID_ZOOM,WM_GETTEXT,sizeof(tmp),(LPARAM)tmp);
						zoom = atof(tmp);
						SendDlgItemMessage(hWnd,ID_ZDEPTH,WM_GETTEXT,sizeof(tmp),(LPARAM)tmp);
						z_depth = atof(tmp);

						InvalidateRect(hWnd,NULL,FALSE);
					}
					else
						MessageBox(hWnd,"Brak definicji obiektu!","B³¹d",MB_ICONEXCLAMATION);
					break;
				
				case ID_PRZELICZ:
					if( DaneI3D != NULL ) {
						if( DaneI3D->GetIloscWarstw() )
							DialogBox(hInst,MAKEINTRESOURCE(IDD_WCZYTYWANIE),hWnd,(DLGPROC)Dlg1Prg);
						else
							MessageBox(hWnd,"Brak definicji warstw!","B³¹d",MB_ICONEXCLAMATION);
					}
					else
						MessageBox(hWnd,"Brak definicji pliku!","B³¹d",MB_ICONEXCLAMATION);

					InvalidateRect(hWnd,NULL,NULL);
				break;

				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_LBUTTONDOWN:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);

			old.x = mx;
			old.y = my;

			LMB = 1;
			SetCursor(hAxis);
			break;

		case WM_RBUTTONDOWN:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);

			rmbold.x = mx;
			rmbold.y = my;

			RMB = 1;
			SetCursor(hZoom);
			break;

		case WM_MOUSEMOVE:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
	
			if (LMB)
			{
				kx = old.x - mx;
				ky = old.y - my;

				old.x = mx;
				old.y = my;

				if( Cells != NULL ) {
					sprintf(tmp,"Obrót: oœ X=%d, oœ Y=%d",-kx,-ky);
					SetWindowText(hWnd,tmp);
				}

				SetCursor(hAxis);
				InvalidateRect(hWnd,NULL,FALSE);
			}

			if (RMB)
			{
				rmbkx = rmbold.x - mx;
				rmbky = rmbold.y - my;

				rmbold.x = mx;
				rmbold.y = my;

				if( Cells != NULL ) {
					sprintf(tmp,"Powiêkszenie: %.1f",zoom);
					SetWindowText(hWnd,tmp);
				}

				SetCursor(hZoom);
				InvalidateRect(hWnd,NULL,FALSE);
			}
			break;
			
		case WM_LBUTTONUP:
			if( strcmp(plik,"") )
				sprintf(tmp,"Marching Cubes [%s]",plik);
			else
				sprintf(tmp,"Marching Cubes [Bez nazwy]");
			SetWindowText(hWnd,tmp);
			LMB = 0;
			return 0;

		case WM_RBUTTONUP:
			if( strcmp(plik,"") )
				sprintf(tmp,"Marching Cubes [%s]",plik);
			else
				sprintf(tmp,"Marching Cubes [Bez nazwy]");
			SetWindowText(hWnd,tmp);
			RMB = 0;
			break;

		case WM_PAINT:
			//hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			//RECT rt;
			//GetClientRect(hWnd, &rt);
			//DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			//EndPaint(hWnd, &ps);
			ekran = BeginPaint(hWnd,&ps);

				GetWindowRect(hWnd,&winRect);

				winSX = winRect.right - winRect.left - Window_Menu_Size;
				winSY = winRect.bottom - winRect.top;

				//PatBlt(sbufor,0,0,winSX,winSY,PATCOPY);
				RectTmp.top=0;
				RectTmp.left=0;
				RectTmp.right=winSX;
				RectTmp.bottom=winSY;
				FillRect(sbufor, &RectTmp, hBrush_KolorTla);

			if( drawFlag ) {
				zoom += rmbky*10;

				draw(sbufor,winSX,winSY,-ky,-kx);
			}

				BitBlt(ekran,0,0,winSX,winSY,sbufor,0,0,SRCCOPY);

				sprintf(tmp,"%.1f",zoom);
				SendDlgItemMessage(hWnd,ID_ZOOM,WM_SETTEXT,0,(LPARAM)tmp);

			kx = ky = 0;
			rmbkx = rmbky = 0;

			EndPaint(hWnd,&ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;
		case WM_COMMAND:	
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL )
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

bool CalculatePhongPal(void) {
	float start_r = (float)GetRValue(Phong_KolorPoczatkowy);
	float start_g = (float)GetGValue(Phong_KolorPoczatkowy);
	float start_b = (float)GetBValue(Phong_KolorPoczatkowy);
	float stop_r = (float)GetRValue(Phong_KolorKoncowy);
	float stop_g = (float)GetGValue(Phong_KolorKoncowy);
	float stop_b = (float)GetBValue(Phong_KolorKoncowy);

	float delta_r = (stop_r-start_r)/256.0;
	float delta_g = (stop_g-start_g)/256.0;
	float delta_b = (stop_b-start_b)/256.0;
	
	for(int i=0; i<256; i++) {
		PhongPal[i].r = (int)start_r;
		PhongPal[i].g = (int)start_g;
		PhongPal[i].b = (int)start_b;

		start_r += delta_r;
		start_g += delta_g;
		start_b += delta_b;
	}
	return true;
}

// Mesage handler for about box.
LRESULT CALLBACK UstawieniaKolorow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
			if( (HWND)lParam == (HWND)GetDlgItem(hDlg,IDC_KOLORTLA) ) {
				return (LPARAM) hBrush_KolorTla;
			}
			
			if( (HWND)lParam == (HWND)GetDlgItem(hDlg,IDC_KOLORLIN_DRUT) ) {				
				return (LPARAM) hBrush_Drut_KolorLinii;
			}			

			if( (HWND)lParam == (HWND)GetDlgItem(hDlg,IDC_KOLORLIN_FLAT) ) {
				return (LPARAM) hBrush_Flat_KolorLinii;
			}			
			if( (HWND)lParam == (HWND)GetDlgItem(hDlg,IDC_KOLORWYPEL) ) {
				return (LPARAM) hBrush_Flat_KolorWypelnienia;
			}			

			if( (HWND)lParam == (HWND)GetDlgItem(hDlg,IDC_KOLOR_POCZ) ) {
				return (LPARAM) hBrush_Phong_KolorPoczatkowy;
			}			
			if( (HWND)lParam == (HWND)GetDlgItem(hDlg,IDC_KOLOR_KON) ) {
				return (LPARAM) hBrush_Phong_KolorKoncowy;
			}			
			break;
		case WM_COMMAND:
			switch( LOWORD(wParam) ) {
				case IDC_ZMIEN_KOLORTLA:
					ChooseColorDialog( KolorTla );
					DeleteObject( hBrush_KolorTla );
					hBrush_KolorTla = CreateSolidBrush( KolorTla );
					InvalidateRect(hDlg,NULL,NULL);
				break;

				case IDC_ZMIEN_KOLORTLIN_DRUT:
					ChooseColorDialog( Drut_KolorLinii );
					DeleteObject( hBrush_Drut_KolorLinii );
					hBrush_Drut_KolorLinii = CreateSolidBrush( Drut_KolorLinii );
					InvalidateRect(hDlg,NULL,NULL);
				break;

				case IDC_ZMIEN_KOLORTLIN_FLAT:
					ChooseColorDialog( Flat_KolorLinii );
					DeleteObject( hBrush_Flat_KolorLinii );
					hBrush_Flat_KolorLinii = CreateSolidBrush( Flat_KolorLinii );
					InvalidateRect(hDlg,NULL,NULL);
				break;
				case IDC_ZMIEN_KOLORWYPEL:
					ChooseColorDialog( Flat_KolorWypelnienia );
					DeleteObject( hBrush_Flat_KolorWypelnienia );
					hBrush_Flat_KolorWypelnienia = CreateSolidBrush( Flat_KolorWypelnienia );
					InvalidateRect(hDlg,NULL,NULL);
				break;

				case IDC_ZMIEN_KOLOR_POCZ:
					ChooseColorDialog( Phong_KolorPoczatkowy );
					DeleteObject( hBrush_Phong_KolorPoczatkowy );
					hBrush_Phong_KolorPoczatkowy = CreateSolidBrush( Phong_KolorPoczatkowy );
					InvalidateRect(hDlg,NULL,NULL);
				break;
				case IDC_ZMIEN_KOLOR_KON:
					ChooseColorDialog( Phong_KolorKoncowy );
					DeleteObject( hBrush_Phong_KolorKoncowy );
					hBrush_Phong_KolorKoncowy = CreateSolidBrush( Phong_KolorKoncowy );
					InvalidateRect(hDlg,NULL,NULL);
				break;

				case IDOK:
				case IDCANCEL:
					CalculatePhongPal();
					EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

// Mesage handler for about box.
LRESULT CALLBACK DlgI3DPrg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HANDLE bmpHandle;

	char tmp[200];
	int i,j;

	char tmpplik[200];
	char tmpplikdir[200];

	bmpLoader *bmp;

	switch (message)
	{
		case WM_INITDIALOG:
			SetWindowText(hDlg,DlgI3DTitle);

			if( DaneI3D==NULL ) {
				DaneI3D = new I3D();
				DaneI3D->ImageType = ImageTypeBmp;
			}

			DaneI3D->SetParams( Grid_X_Size,
								Grid_Y_Size,
								Grid_Z_Size,
								Cell_Size,
								ThresholdValue,
								zoom,
								z_depth);

			sprintf(tmp,"%d",DaneI3D->IloscWarstw);
			SendDlgItemMessage(hDlg,IDC_STATIC1,WM_SETTEXT,0,(LPARAM)tmp);
			sprintf(tmp,"%d",DaneI3D->SzerokoscWarstwy);
			SendDlgItemMessage(hDlg,IDC_STATIC2,WM_SETTEXT,0,(LPARAM)tmp);
			sprintf(tmp,"%d",DaneI3D->WysokoscWarstwy);
			SendDlgItemMessage(hDlg,IDC_STATIC3,WM_SETTEXT,0,(LPARAM)tmp);

			sprintf(tmp,"%.2f",DaneI3D->WielkoscCeli);
			SendDlgItemMessage(hDlg,IDC_STATIC4,WM_SETTEXT,0,(LPARAM)tmp);
			sprintf(tmp,"%.2f",DaneI3D->WartoscProgowa);
			SendDlgItemMessage(hDlg,IDC_STATIC5,WM_SETTEXT,0,(LPARAM)tmp);

			sprintf(tmp,"%.2f",DaneI3D->Powiekszenie);
			SendDlgItemMessage(hDlg,IDC_STATIC6,WM_SETTEXT,0,(LPARAM)tmp);
			sprintf(tmp,"%.2f",DaneI3D->Obserwator);
			SendDlgItemMessage(hDlg,IDC_STATIC7,WM_SETTEXT,0,(LPARAM)tmp);

			if( strcmp(plik,"") )
				SendDlgItemMessage(hDlg,IDC_STATIC8,WM_SETTEXT,0,(LPARAM)plik);
			else
				SendDlgItemMessage(hDlg,IDC_STATIC8,WM_SETTEXT,0,(LPARAM)"Bez nazwy");
			
			for( i=0; i<DaneI3D->GetIloscWarstw(); i++ ) {
				sprintf(tmp,"[%.4d]: %s",i,DaneI3D->GetWarstwa(i));
				SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_ADDSTRING,0,(LPARAM)tmp);
			}

			tmpDaneI3D = new I3D();
			tmpDaneI3D->CopyI3D(DaneI3D);
			return TRUE;
		case WM_COMMAND:
			switch( LOWORD(wParam) ) {
				case IDC_WARSTWY_PODGLAD:
					i = SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_GETCURSEL,0,0);
					if( i!=LB_ERR ) {
						sprintf(tmp,"%s%s",plikdir,DaneI3D->GetWarstwa(i));
						bmpHandle = LoadImage(hInst, tmp, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
						SendDlgItemMessage(hDlg, IDC_PODGLAD_PICT, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpHandle);

						bmp=new bmpLoader();
						if( bmp->load(tmp) ) {
							sprintf(tmp,"%d",bmp->getWidth());
							SendDlgItemMessage(hDlg,IDC_PODGLAD_SZEROKOSC,WM_SETTEXT,0,(LPARAM)tmp);
							sprintf(tmp,"%d",bmp->getHeight());
							SendDlgItemMessage(hDlg,IDC_PODGLAD_WYSOKOSC,WM_SETTEXT,0,(LPARAM)tmp);
							delete bmp;
						}
					}
				break;

				case IDC_WARSTWY_USUN:
					i = SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_GETCURSEL,0,0);
					if( i!=LB_ERR ) {
						DaneI3D->UsunWarstwe(i);
						SendDlgItemMessage(hDlg,IDC_LIST1,LB_RESETCONTENT,0,0);
						for( i=0; i<DaneI3D->GetIloscWarstw(); i++ ) {
							sprintf(tmp,"[%.4d]: %s",i,DaneI3D->GetWarstwa(i));
							SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_ADDSTRING,0,(LPARAM)tmp);
						}
						sprintf(tmp,"%d",DaneI3D->IloscWarstw);
						SendDlgItemMessage(hDlg,IDC_STATIC1,WM_SETTEXT,0,(LPARAM)tmp);
					}
					break;

				case IDC_WARSTWY_KOPIA:
					i = SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_GETCURSEL,0,0);
					if( i!=LB_ERR ) {
						DaneI3D->DodajWarstwe(DaneI3D->GetWarstwa(i));
						SendDlgItemMessage(hDlg,IDC_LIST1,LB_RESETCONTENT,0,0);
						for( i=0; i<DaneI3D->GetIloscWarstw(); i++ ) {
							sprintf(tmp,"[%.4d]: %s",i,DaneI3D->GetWarstwa(i));
							SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_ADDSTRING,0,(LPARAM)tmp);
						}
						sprintf(tmp,"%d",DaneI3D->IloscWarstw);
						SendDlgItemMessage(hDlg,IDC_STATIC1,WM_SETTEXT,0,(LPARAM)tmp);
					}
					break;

				case IDC_WARSTWY_DODAJZPLIKU:
					if (Open(hDlg,tmpplik,sizeof(tmpplik),"Pliki bmp\0*.bmp\0Wszystkie pliki\0*.*\0"))
					{
						strcpy(tmp,tmpplik);
						strcpy(tmpplikdir,tmpplik);						
						if( strchr(tmpplikdir,'\\') ) {
							for(i=strlen(tmpplikdir)-1; i--; i>=0)
								if( tmpplikdir[i]=='\\' )
									break;
							tmpplikdir[i]=0;
							if( i )
								strcat(tmpplikdir,"\\");
						}

						bmp = new bmpLoader();
						if( bmp->load(tmp) ) {
								DaneI3D->DodajWarstwe(tmpplik+i+1);

								sprintf(tmp,"%d",bmp->getHeight());
								SendDlgItemMessage(hDlg,IDC_STATIC2,WM_SETTEXT,0,(LPARAM)tmp);
								sprintf(tmp,"%d",bmp->getWidth());
								SendDlgItemMessage(hDlg,IDC_STATIC3,WM_SETTEXT,0,(LPARAM)tmp);
						}
						delete bmp;

						SendDlgItemMessage(hDlg,IDC_LIST1,LB_RESETCONTENT,0,0);
						for( i=0; i<DaneI3D->GetIloscWarstw(); i++ ) {
							sprintf(tmp,"[%.4d]: %s",i,DaneI3D->GetWarstwa(i));
							SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_ADDSTRING,0,(LPARAM)tmp);
						}
						sprintf(tmp,"%d",DaneI3D->IloscWarstw);
						SendDlgItemMessage(hDlg,IDC_STATIC1,WM_SETTEXT,0,(LPARAM)tmp);
					}
					break;

				case IDC_WARSTWY_ZMIENWARSTWE:
					j = SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_GETCURSEL,0,0);
					if( j!=LB_ERR ) {
						if (Open(hDlg,tmpplik,sizeof(tmpplik),"Pliki bmp\0*.bmp\0Wszystkie pliki\0*.*\0"))
						{
							strcpy(tmpplikdir,tmpplik);						
							if( strchr(tmpplikdir,'\\') ) {
								for(i=strlen(tmpplikdir)-1; i--; i>=0)
									if( tmpplikdir[i]=='\\' )
										break;
								tmpplikdir[i]=0;
								if( i )
									strcat(tmpplikdir,"\\");
							}

							DaneI3D->ZmienWarstwe(tmpplik+i+1,j);

							SendDlgItemMessage(hDlg,IDC_LIST1,LB_RESETCONTENT,0,0);
							for( i=0; i<DaneI3D->GetIloscWarstw(); i++ ) {
								sprintf(tmp,"[%.4d]: %s",i,DaneI3D->GetWarstwa(i));
								SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_ADDSTRING,0,(LPARAM)tmp);
							}
							sprintf(tmp,"%d",DaneI3D->IloscWarstw);
							SendDlgItemMessage(hDlg,IDC_STATIC1,WM_SETTEXT,0,(LPARAM)tmp);
						}
					}
					break;

				case IDC_WARSTWY_USTAWSZERWYSOKOSC:
					i = SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_GETCURSEL,0,0);
					if( i!=LB_ERR ) {						
						bmp = new bmpLoader();
						sprintf(tmpplik,"%s%s",plikdir,DaneI3D->GetWarstwa(i));
						if( bmp->load(tmpplik) ) {
								sprintf(tmp,"%d",bmp->getHeight());
								SendDlgItemMessage(hDlg,IDC_STATIC2,WM_SETTEXT,0,(LPARAM)tmp);
								sprintf(tmp,"%d",bmp->getWidth());
								SendDlgItemMessage(hDlg,IDC_STATIC3,WM_SETTEXT,0,(LPARAM)tmp);
						}
						delete bmp;
					}
					break;

				case IDOK:
					SendDlgItemMessage(hDlg,IDC_STATIC1,WM_GETTEXT,sizeof(tmp),(LPARAM)tmp);
					i=atoi(tmp);

					if( i<2 ) {
						MessageBox(hDlg,"Trzeba wprowadziæ minimum 2 warstwy!","B³¹d",MB_ICONEXCLAMATION);
						break;
					}

					DaneI3D->IloscWarstw=i;

					SendDlgItemMessage(hDlg,IDC_STATIC2,WM_GETTEXT,sizeof(tmp),(LPARAM)tmp);
					DaneI3D->SzerokoscWarstwy=atoi(tmp);
					SendDlgItemMessage(hDlg,IDC_STATIC3,WM_GETTEXT,sizeof(tmp),(LPARAM)tmp);
					DaneI3D->WysokoscWarstwy=atoi(tmp);

					SendDlgItemMessage(hDlg,IDC_STATIC4,WM_GETTEXT,sizeof(tmp),(LPARAM)tmp);
					DaneI3D->WielkoscCeli=atof(tmp);
					SendDlgItemMessage(hDlg,IDC_STATIC5,WM_GETTEXT,sizeof(tmp),(LPARAM)tmp);
					DaneI3D->WartoscProgowa=atof(tmp);

					SendDlgItemMessage(hDlg,IDC_STATIC6,WM_GETTEXT,sizeof(tmp),(LPARAM)tmp);
					DaneI3D->Powiekszenie=atof(tmp);
					SendDlgItemMessage(hDlg,IDC_STATIC7,WM_GETTEXT,sizeof(tmp),(LPARAM)tmp);
					DaneI3D->Obserwator=atof(tmp);

					DaneI3D->GetParams(	Grid_X_Size,
										Grid_Y_Size,
										Grid_Z_Size,
										Cell_Size,
										ThresholdValue,
										zoom,
										z_depth);

					I3DStat=true;

					delete tmpDaneI3D;
					EndDialog(hDlg, LOWORD(wParam));
				return TRUE;

				case IDCANCEL:
					delete DaneI3D;
					DaneI3D = tmpDaneI3D;

					I3DStat=false;
					EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

BOOL Open(HWND okno,char* plik,int rozm,char *filtry)
{
	OPENFILENAME ofn;
	char szDirName[256];

	plik[0] = 0;

	GetCurrentDirectory(sizeof(szDirName),szDirName);

	memset(&ofn,0,sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = okno;
	ofn.lpstrFilter = filtry;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = plik;
	ofn.nMaxFile = rozm;
	ofn.lpstrInitialDir = szDirName;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	return GetOpenFileName(&ofn);
}

BOOL Save(HWND okno,char* plik,int rozm,char *filtry)
{
	OPENFILENAME ofn;
	char szDirName[256];

	plik[0] = 0;

	GetCurrentDirectory(sizeof(szDirName),szDirName);

	memset(&ofn,0,sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = okno;
	ofn.lpstrFilter = filtry;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = plik;
	ofn.nMaxFile = rozm;
	ofn.lpstrInitialDir = szDirName;
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;

	return GetSaveFileName(&ofn);
}

int CheckObjectXYZ(float x,float y,float z) {
	for( int i=0; i<vert; i++ )
		if( object[i].x==x && object[i].y==y && object[i].z==z )
			return i;
	return -1;
}

void calc_normals(void)
{
  float x1,y1,z1;
  float x2,y2,z2;
  float x3,y3,z3;

  float nx,ny,nz;
  float nl;

  int i;

  for (i=0;i<vert;i++) {
    normal[i].x=0.0;
    normal[i].y=0.0;
    normal[i].z=0.0;
    n_len[i]=0;
  }

  for (i=0;i<Triangles_len;i++) {
    x1=object[TrianglesIndex[i].p[0]].x;
    y1=object[TrianglesIndex[i].p[0]].y;
    z1=object[TrianglesIndex[i].p[0]].z;

    x2=object[TrianglesIndex[i].p[1]].x;
    y2=object[TrianglesIndex[i].p[1]].y;
    z2=object[TrianglesIndex[i].p[1]].z;

    x3=object[TrianglesIndex[i].p[2]].x;
    y3=object[TrianglesIndex[i].p[2]].y;
    z3=object[TrianglesIndex[i].p[2]].z;

    nx=(y2-y1)*(z3-z1)-(z2-z1)*(y3-y1);
    ny=(z2-z1)*(x3-x1)-(x2-x1)*(z3-z1);
    nz=(x2-x1)*(y3-y1)-(y2-y1)*(x3-x1);

    nl=sqrt(nx*nx+ny*ny+nz*nz);

    nx=(nx*ns)/nl;
    ny=(ny*ns)/nl;
    nz=(nz*ns)/nl;

    normal[TrianglesIndex[i].p[0]].x+=nx;
    normal[TrianglesIndex[i].p[0]].y+=ny;
    normal[TrianglesIndex[i].p[0]].z+=nz;

    n_len[TrianglesIndex[i].p[0]]++;

    normal[TrianglesIndex[i].p[1]].x+=nx;
    normal[TrianglesIndex[i].p[1]].y+=ny;
    normal[TrianglesIndex[i].p[1]].z+=nz;

    n_len[TrianglesIndex[i].p[1]]++;

    normal[TrianglesIndex[i].p[2]].x+=nx;
    normal[TrianglesIndex[i].p[2]].y+=ny;
    normal[TrianglesIndex[i].p[2]].z+=nz;

    n_len[TrianglesIndex[i].p[2]]++;
  }

  for (i=0;i<vert;i++) {
    normal[i].x/=n_len[i];
    normal[i].y/=n_len[i];
    normal[i].z/=n_len[i];
  }
}

HWND hDialog;
float Progress=0.0;
float ProgressStep=0.0;

int init_vect(void) {
	object = new VERTEX[Triangles_len*3];
	normal = new VERTEX[Triangles_len*3];
	rcalc = new VERTEX[Triangles_len*3];
	rnormal = new VERTEX[Triangles_len*3];
	n_len = new int[Triangles_len*3];
	plane = new POINT[Triangles_len*3];
	light = new int[Triangles_len];

	sfaces = new SFACE[256];
	for( int f=0; f<256; f++ )
		sfaces[f].datas = new SDATA[Triangles_len];

	ftab = new SDATA[Triangles_len];

	TrianglesIndex = new TRIANGLEINDEX[Triangles_len*3];

	Progress=0.0;
	ProgressStep=100.0/(float)Triangles_len;

	vert=0;
	for(int i=0; i<Triangles_len; i++) {
		for(int j=0; j<3; j++) {
			int index=CheckObjectXYZ(Triangles[i].p[j].x,Triangles[i].p[j].y,Triangles[i].p[j].z);
			if( index == -1 ) {
				object[vert].x = Triangles[i].p[j].x;
				object[vert].y = Triangles[i].p[j].y;
				object[vert].z = Triangles[i].p[j].z;
				
				TrianglesIndex[i].p[j] = vert;
				
				vert++;
			}
			else {
				TrianglesIndex[i].p[j] = index;
			}
		}

		SendMessage(GetDlgItem(hDialog,IDC_PROGRESS3),PBM_SETPOS,(WPARAM)(int)Progress,0);
		Progress += ProgressStep;
	}

	SendMessage(GetDlgItem(hDialog,IDC_PROGRESS3),PBM_SETPOS,(WPARAM)100,0);
	InvalidateRect(hDialog,NULL,FALSE);

	calc_normals();

	return 0;
}

void rotate_points(int angx,int angy)
{
	MATRIX rot;
	int i;

	float SinX,SinY;
	float CosX,CosY;

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

	for (i=0; i<vert; i++) {
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

	for (i=0; i<vert; i++) {
		rnormal[i].x = normal[i].x*rot[0][0] + normal[i].y*rot[0][1] + normal[i].z*rot[0][2];
		rnormal[i].y = normal[i].x*rot[1][0] + normal[i].y*rot[1][1] + normal[i].z*rot[1][2];
		rnormal[i].z = normal[i].x*rot[2][0] + normal[i].y*rot[2][1] + normal[i].z*rot[2][2];

		normal[i].x = rnormal[i].x;	
		normal[i].y = rnormal[i].y;	
		normal[i].z = rnormal[i].z;	
	}
}

void persp_calc(long sx,long sy)
{
	int i;

	long cx = sx/2;
	long cy = (sy/2)-10;

	for (i=0; i<vert; i++) {
		plane[i].x = cx + ((rcalc[i].x*zoom) / (rcalc[i].z+z_depth));
		plane[i].y = cy - ((rcalc[i].y*zoom) / (rcalc[i].z+z_depth));
	}
}

void light_calc(void) {
	int i;

	float x1,y1;
	float x2,y2;
	float x3,y3;
	
	//int l1,l2,l3;
	int l;

	float l1,l2,l3;

	for( i=0; i<ftablen; i++ ) {
/*		x1 = rnormal[TrianglesIndex[ftab[i].index].p[0]].x;
		y1 = rnormal[TrianglesIndex[ftab[i].index].p[0]].y;

		l1 = (int)sqrt(x1*x1+y1*y1);

		x2 = rnormal[TrianglesIndex[ftab[i].index].p[1]].x;
		y2 = rnormal[TrianglesIndex[ftab[i].index].p[1]].y;

		l2 = (int)sqrt(x2*x2+y2*y2);

		x3 = rnormal[TrianglesIndex[ftab[i].index].p[2]].x;
		y3 = rnormal[TrianglesIndex[ftab[i].index].p[2]].y;

		l3 = (int)sqrt(x3*x3+y3*y3);

		l = (l1+l2+l3)/3;

		light[ftab[i].index] = l;*/

		x1 = rnormal[TrianglesIndex[ftab[i].index].p[0]].x;
		y1 = rnormal[TrianglesIndex[ftab[i].index].p[0]].y;

		x2 = rnormal[TrianglesIndex[ftab[i].index].p[1]].x;
		y2 = rnormal[TrianglesIndex[ftab[i].index].p[1]].y;

		x3 = rnormal[TrianglesIndex[ftab[i].index].p[2]].x;
		y3 = rnormal[TrianglesIndex[ftab[i].index].p[2]].y;

		l = (x1*x1+y1*y1+x2*x2+y2*y2+x3*x3+y3*y3)/1000.0;

		light[ftab[i].index] = l;
	}
}

float Sqr(float x) {
	return x*x;
}

int Sqr(int x) {
	return x*x;
}

float ABS(float x) {
	if( x<0 )
		return -x;
	return x;
}

int ABS(int x) {
	if( x<0 )
		return -x;
	return x;
}

/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
XYZ VertexInterp(float isolevel,XYZ p1,XYZ p2,float valp1,float valp2)
//float isolevel;
//XYZ p1,p2;
//float valp1,valp2;
{
   float mu;
   XYZ p;

   if (ABS(isolevel-valp1) < 0.00001)
      return(p1);
   if (ABS(isolevel-valp2) < 0.00001)
      return(p2);
   if (ABS(valp1-valp2) < 0.00001)
      return(p1);
   mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return(p);
}

/*
   Given a grid cell and an isolevel, calculate the triangular
   facets required to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
	0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
*/
int Polygonise(GRIDCELL grid,float isolevel,TRIANGLE *triangles)
{
   int i,ntriang;
   int cubeindex;

   XYZ vertlist[12];

   /*
      Determine the index into the edge table which
      tells us which vertices are inside of the surface
   */
   cubeindex = 0;
   if (grid.val[0] < isolevel) cubeindex |= 1;
   if (grid.val[1] < isolevel) cubeindex |= 2;
   if (grid.val[2] < isolevel) cubeindex |= 4;
   if (grid.val[3] < isolevel) cubeindex |= 8;
   if (grid.val[4] < isolevel) cubeindex |= 16;
   if (grid.val[5] < isolevel) cubeindex |= 32;
   if (grid.val[6] < isolevel) cubeindex |= 64;
   if (grid.val[7] < isolevel) cubeindex |= 128;

   /* Cube is entirely in/out of the surface */
   if (edgeTable[cubeindex] == 0)
      return(0);

   /* Find the vertices where the surface intersects the cube */
   if (edgeTable[cubeindex] & 1)
      vertlist[0] =
         VertexInterp(isolevel,grid.p[0],grid.p[1],grid.val[0],grid.val[1]);
   if (edgeTable[cubeindex] & 2)
      vertlist[1] =
         VertexInterp(isolevel,grid.p[1],grid.p[2],grid.val[1],grid.val[2]);
   if (edgeTable[cubeindex] & 4)
      vertlist[2] =
         VertexInterp(isolevel,grid.p[2],grid.p[3],grid.val[2],grid.val[3]);
   if (edgeTable[cubeindex] & 8)
      vertlist[3] =
         VertexInterp(isolevel,grid.p[3],grid.p[0],grid.val[3],grid.val[0]);
   if (edgeTable[cubeindex] & 16)
      vertlist[4] =
         VertexInterp(isolevel,grid.p[4],grid.p[5],grid.val[4],grid.val[5]);
   if (edgeTable[cubeindex] & 32)
      vertlist[5] =
         VertexInterp(isolevel,grid.p[5],grid.p[6],grid.val[5],grid.val[6]);
   if (edgeTable[cubeindex] & 64)
      vertlist[6] =
         VertexInterp(isolevel,grid.p[6],grid.p[7],grid.val[6],grid.val[7]);
   if (edgeTable[cubeindex] & 128)
      vertlist[7] =
         VertexInterp(isolevel,grid.p[7],grid.p[4],grid.val[7],grid.val[4]);
   if (edgeTable[cubeindex] & 256)
      vertlist[8] =
         VertexInterp(isolevel,grid.p[0],grid.p[4],grid.val[0],grid.val[4]);
   if (edgeTable[cubeindex] & 512)
      vertlist[9] =
         VertexInterp(isolevel,grid.p[1],grid.p[5],grid.val[1],grid.val[5]);
   if (edgeTable[cubeindex] & 1024)
      vertlist[10] =
         VertexInterp(isolevel,grid.p[2],grid.p[6],grid.val[2],grid.val[6]);
   if (edgeTable[cubeindex] & 2048)
      vertlist[11] =
         VertexInterp(isolevel,grid.p[3],grid.p[7],grid.val[3],grid.val[7]);

   /* Create the triangle */
   ntriang = 0;
   for (i=0;triTable[cubeindex][i]!=-1;i+=3) {
      triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i  ]];
      triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i+1]];
      triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i+2]];
      ntriang++;
   }

   return(ntriang);
}

int PolygoniseAll(void) {
	Triangles = new TRIANGLE[1024];
	TRIANGLE *tmpTriangles;

	Progress=0.0;
	ProgressStep=100.0/(float)Cells_len;

	int maxtriang=1024;

	Triangles_len=0;
	for( int i=0; i<Cells_len; i++ ) {
		int n = Polygonise(Cells[i],ThresholdValue,&Triangles[Triangles_len]);
		Triangles_len += n;

		if( Triangles_len > maxtriang-100 ) {
			tmpTriangles = new TRIANGLE[Triangles_len+1024];
			memcpy(tmpTriangles,Triangles,sizeof(TRIANGLE)*Triangles_len);
			delete[] Triangles;
			Triangles=tmpTriangles;
			maxtriang = Triangles_len+1024;
		}

		SendMessage(GetDlgItem(hDialog,IDC_PROGRESS2),PBM_SETPOS,(WPARAM)(int)Progress,0);
		Progress += ProgressStep;

		InvalidateRect(hDialog,NULL,FALSE);
	}

	SendMessage(GetDlgItem(hDialog,IDC_PROGRESS2),PBM_SETPOS,(WPARAM)100,0);
	InvalidateRect(hDialog,NULL,FALSE);

	return 0;
}

float CheckCellValue(int px,int py,int pz,float x,float y,float z) {
	float Sphere_x0=0.0;
	float Sphere_y0=0.0;
	float Sphere_z0=0.0;
	float Sphere_r=25.0;

	float xx = Sqr(x-Sphere_x0);
	float yy = Sqr(y-Sphere_y0);
	float zz = Sqr(z-Sphere_z0);

	float rr = Sqr(Sphere_r);

	if( (xx+yy+zz) <= rr ) {
		return 10.0;
	}

	return 0.0;
}

int FillCellsBMP(void) {
	FILE *f;
	
	char tmp[255];
	char msg[255];

	int ImageType; /* 1: raw 2: bmp */
	bool r;

	bmpLoader *bmpFile1;
	bmpLoader *bmpFile2;

	BYTE *PixelData1;
	BYTE *PixelData2;

	Cells_len = Grid_X_Size*Grid_Y_Size*Grid_Z_Size;

	ImageType=DaneI3D->ImageType;
//	else {
//		sprintf(msg,"Niew³aœciwy format warstw! [%s]",tmp);
//		MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
//		return -1;
//	}

	bmpFile1 = new bmpLoader();
	bmpFile2 = new bmpLoader();

	if( ImageType==ImageTypeRaw )
		r = bmpFile1->loadRaw(DaneI3D->GetWarstwa(0),Grid_X_Size,Grid_Y_Size);
	else
	if( ImageType==ImageTypeBmp )
		r = bmpFile1->load(DaneI3D->GetWarstwa(0));

	if( !r ) {
		sprintf(msg,"Nie mo¿na otworzyæ pliku: %s",DaneI3D->GetWarstwa(0));
		MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
		delete bmpFile1;
		return -1;
	}
	if( bmpFile1->getWidth() != Grid_X_Size ) {
		sprintf(msg,"Niew³aœciwa szerokoœæ pliku: %s",DaneI3D->GetWarstwa(0));
		MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
		delete bmpFile1;
		return -1;
	}
	if( bmpFile1->getHeight() != Grid_Y_Size ) {
		sprintf(msg,"Niew³aœciwa wysokoœæ pliku: %s",DaneI3D->GetWarstwa(0));
		MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
		delete bmpFile1;
		return -1;
	}
	PixelData1 = bmpFile1->getPixelData();

	if( ImageType==ImageTypeRaw )
		r = bmpFile2->loadRaw(DaneI3D->GetWarstwa(1),Grid_X_Size,Grid_Y_Size);
	else
	if( ImageType==ImageTypeBmp )
		r = bmpFile2->load(DaneI3D->GetWarstwa(1));
	
	if( !r ) {
		sprintf(msg,"Nie mo¿na otworzyæ pliku: %s",DaneI3D->GetWarstwa(1));
		MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
		delete bmpFile1;
		delete bmpFile2;
		return -1;
	}
	if( bmpFile2->getWidth() != Grid_X_Size ) {
		sprintf(msg,"Niew³aœciwa szerokoœæ pliku: %s",DaneI3D->GetWarstwa(1));
		MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
		delete bmpFile1;
		delete bmpFile2;
		return -1;
	}
	if( bmpFile2->getHeight() != Grid_Y_Size ) {
		sprintf(msg,"Niew³aœciwa wysokoœæ pliku: %s",DaneI3D->GetWarstwa(1));
		MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
		delete bmpFile1;
		delete bmpFile2;
		return -1;
	}
	PixelData2 = bmpFile2->getPixelData();
	
	Cells = new GRIDCELL[Cells_len];
	if( Cells==NULL ) {
		sprintf(msg,"Nie uda³o siê zaalokowaæ %d bajtów pamiêci!",Cells_len*100);
		MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
		delete bmpFile1;
		delete bmpFile2;
		return -1;
	}

	int i=0;

	VERTEX v;

	Progress=0.0;
	ProgressStep=100.0/(float)Cells_len;

	int warstwa_index=1;	

	v.z = -((float)Grid_Z_Size*(float)Cell_Size/2.0);
	for( int z=0; z<Grid_Z_Size-1; z++ ) {
		v.y = -((float)Grid_Y_Size*(float)Cell_Size/2.0);
		for( int y=0; y<Grid_Y_Size-1; y++ ) {
			v.x = -((float)Grid_X_Size*(float)Cell_Size/2.0);
			for( int x=0; x<Grid_X_Size-1; x++ ) {
				//vertex
				Cells[i].p[0].x = v.x+(float)(0.0*Cell_Size);
				Cells[i].p[0].y = v.y+(float)(0.0*Cell_Size);
				Cells[i].p[0].z = v.z+(float)(0.0*Cell_Size);

				Cells[i].p[1].x = v.x+(float)(1.0*Cell_Size);
				Cells[i].p[1].y = v.y+(float)(0.0*Cell_Size);
				Cells[i].p[1].z = v.z+(float)(0.0*Cell_Size);

				Cells[i].p[2].x = v.x+(float)(1.0*Cell_Size);
				Cells[i].p[2].y = v.y+(float)(1.0*Cell_Size);
				Cells[i].p[2].z = v.z+(float)(0.0*Cell_Size);

				Cells[i].p[3].x = v.x+(float)(0.0*Cell_Size);
				Cells[i].p[3].y = v.y+(float)(1.0*Cell_Size);
				Cells[i].p[3].z = v.z+(float)(0.0*Cell_Size);

				Cells[i].p[4].x = v.x+(float)(0.0*Cell_Size);
				Cells[i].p[4].y = v.y+(float)(0.0*Cell_Size);
				Cells[i].p[4].z = v.z+(float)(1.0*Cell_Size);

				Cells[i].p[5].x = v.x+(float)(1.0*Cell_Size);
				Cells[i].p[5].y = v.y+(float)(0.0*Cell_Size);
				Cells[i].p[5].z = v.z+(float)(1.0*Cell_Size);

				Cells[i].p[6].x = v.x+(float)(1.0*Cell_Size);
				Cells[i].p[6].y = v.y+(float)(1.0*Cell_Size);
				Cells[i].p[6].z = v.z+(float)(1.0*Cell_Size);

				Cells[i].p[7].x = v.x+(float)(0.0*Cell_Size);
				Cells[i].p[7].y = v.y+(float)(1.0*Cell_Size);
				Cells[i].p[7].z = v.z+(float)(1.0*Cell_Size);
				
				//value
				Cells[i].val[0] = PixelData1[(x+0)+((y+0)*Grid_X_Size)];
				Cells[i].val[1] = PixelData1[(x+1)+((y+0)*Grid_X_Size)];
				Cells[i].val[2] = PixelData1[(x+1)+((y+1)*Grid_X_Size)];
				Cells[i].val[3] = PixelData1[(x+0)+((y+1)*Grid_X_Size)];
				Cells[i].val[4] = PixelData2[(x+0)+((y+0)*Grid_X_Size)];
				Cells[i].val[5] = PixelData2[(x+1)+((y+0)*Grid_X_Size)];
				Cells[i].val[6] = PixelData2[(x+1)+((y+1)*Grid_X_Size)];
				Cells[i].val[7] = PixelData2[(x+0)+((y+1)*Grid_X_Size)];

				i++;
				v.x += (float)Cell_Size;

				SendMessage(GetDlgItem(hDialog,IDC_PROGRESS1),PBM_SETPOS,(WPARAM)(int)Progress,0);
				Progress += ProgressStep;

				InvalidateRect(hDialog,NULL,FALSE);
			}
			v.y += (float)Cell_Size;
		}
	
		delete bmpFile1;

		bmpFile1=bmpFile2;
		PixelData1=PixelData2;

		if( z<Grid_Z_Size-2 ) {
			bmpFile2 = new bmpLoader();

			warstwa_index++;

			if( ImageType==ImageTypeRaw )
				r = bmpFile2->loadRaw(DaneI3D->GetWarstwa(warstwa_index),Grid_X_Size,Grid_Y_Size);
			else
			if( ImageType==ImageTypeBmp )
				r = bmpFile2->load(DaneI3D->GetWarstwa(warstwa_index));			

			if( !r ) {
				sprintf(msg,"Nie mo¿na otworzyæ pliku: %s",DaneI3D->GetWarstwa(warstwa_index));
				MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
				delete bmpFile1;
				delete bmpFile2;
				return -1;
			}
			if( bmpFile2->getWidth() != Grid_X_Size ) {
				sprintf(msg,"Niew³aœciwa szerokoœæ pliku: %s",DaneI3D->GetWarstwa(warstwa_index));
				MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
				delete bmpFile1;
				delete bmpFile2;
				return -1;
			}
			if( bmpFile2->getHeight() != Grid_Y_Size ) {
				sprintf(msg,"Niew³aœciwa wysokoœæ pliku: %s",DaneI3D->GetWarstwa(warstwa_index));
				MessageBox(hDialog,msg,"B³¹d",MB_ICONEXCLAMATION);
				delete bmpFile1;
				delete bmpFile2;
				return -1;
			}
			PixelData2 = bmpFile2->getPixelData();
		}

		v.z += (float)Cell_Size;
	}

	SendMessage(GetDlgItem(hDialog,IDC_PROGRESS1),PBM_SETPOS,(WPARAM)100,0);
	InvalidateRect(hDialog,NULL,FALSE);

	delete bmpFile1;
	return 1;
}

int FillCellsSphere(void) {
	Cells = new GRIDCELL[Cells_len];

	int i=0;

	VERTEX v;

	Progress=0.0;
	ProgressStep=100.0/(float)Cells_len;

	v.z = -((float)Grid_Z_Size*(float)Cell_Size/2.0);
	for( int z=0; z<Grid_X_Size-1; z++ ) {
		v.y = -((float)Grid_Y_Size*(float)Cell_Size/2.0);
		for( int y=0; y<Grid_Y_Size-1; y++ ) {
			v.x = -((float)Grid_X_Size*(float)Cell_Size/2.0);
			for( int x=0; x<Grid_Z_Size-1; x++ ) {
				//vertex
				Cells[i].p[0].x = v.x+(float)(0.0*Cell_Size);
				Cells[i].p[0].y = v.y+(float)(0.0*Cell_Size);
				Cells[i].p[0].z = v.z+(float)(0.0*Cell_Size);

				Cells[i].p[1].x = v.x+(float)(1.0*Cell_Size);
				Cells[i].p[1].y = v.y+(float)(0.0*Cell_Size);
				Cells[i].p[1].z = v.z+(float)(0.0*Cell_Size);

				Cells[i].p[2].x = v.x+(float)(1.0*Cell_Size);
				Cells[i].p[2].y = v.y+(float)(1.0*Cell_Size);
				Cells[i].p[2].z = v.z+(float)(0.0*Cell_Size);

				Cells[i].p[3].x = v.x+(float)(0.0*Cell_Size);
				Cells[i].p[3].y = v.y+(float)(1.0*Cell_Size);
				Cells[i].p[3].z = v.z+(float)(0.0*Cell_Size);

				Cells[i].p[4].x = v.x+(float)(0.0*Cell_Size);
				Cells[i].p[4].y = v.y+(float)(0.0*Cell_Size);
				Cells[i].p[4].z = v.z+(float)(1.0*Cell_Size);

				Cells[i].p[5].x = v.x+(float)(1.0*Cell_Size);
				Cells[i].p[5].y = v.y+(float)(0.0*Cell_Size);
				Cells[i].p[5].z = v.z+(float)(1.0*Cell_Size);

				Cells[i].p[6].x = v.x+(float)(1.0*Cell_Size);
				Cells[i].p[6].y = v.y+(float)(1.0*Cell_Size);
				Cells[i].p[6].z = v.z+(float)(1.0*Cell_Size);

				Cells[i].p[7].x = v.x+(float)(0.0*Cell_Size);
				Cells[i].p[7].y = v.y+(float)(1.0*Cell_Size);
				Cells[i].p[7].z = v.z+(float)(1.0*Cell_Size);
				
				//value
				Cells[i].val[0] = CheckCellValue(x,y,z,Cells[i].p[0].x,Cells[i].p[0].y,Cells[i].p[0].z);
				Cells[i].val[1] = CheckCellValue(x,y,z,Cells[i].p[1].x,Cells[i].p[1].y,Cells[i].p[1].z);
				Cells[i].val[2] = CheckCellValue(x,y,z,Cells[i].p[2].x,Cells[i].p[2].y,Cells[i].p[2].z);
				Cells[i].val[3] = CheckCellValue(x,y,z,Cells[i].p[3].x,Cells[i].p[3].y,Cells[i].p[3].z);
				Cells[i].val[4] = CheckCellValue(x,y,z,Cells[i].p[4].x,Cells[i].p[4].y,Cells[i].p[4].z);
				Cells[i].val[5] = CheckCellValue(x,y,z,Cells[i].p[5].x,Cells[i].p[5].y,Cells[i].p[5].z);
				Cells[i].val[6] = CheckCellValue(x,y,z,Cells[i].p[6].x,Cells[i].p[6].y,Cells[i].p[6].z);
				Cells[i].val[7] = CheckCellValue(x,y,z,Cells[i].p[7].x,Cells[i].p[7].y,Cells[i].p[7].z);

				i++;
				v.x += (float)Cell_Size;

				SendMessage(GetDlgItem(hDialog,IDC_PROGRESS1),PBM_SETPOS,(WPARAM)(int)Progress,0);
				Progress += ProgressStep;

				InvalidateRect(hDialog,NULL,FALSE);
			}
			v.y += (float)Cell_Size;
		}
		v.z += (float)Cell_Size;
	}

	SendMessage(GetDlgItem(hDialog,IDC_PROGRESS1),PBM_SETPOS,(WPARAM)100,0);
	InvalidateRect(hDialog,NULL,FALSE);

	return 1;
}

int Step=0;

BOOL CALLBACK Dlg1Prg(HWND hDlg,UINT komunikat,WPARAM wParam,LPARAM lParam)
{
	char tmp[255];
	
	switch(komunikat)
	{
		case WM_INITDIALOG:
			SendDlgItemMessage(hDlg,IDC_PLIK,WM_SETTEXT,0,(LPARAM)plik);
			SendMessage(GetDlgItem(hDlg,IDC_PROGRESS1),PBM_SETPOS,(WPARAM)0,0);
			SendMessage(GetDlgItem(hDlg,IDC_PROGRESS2),PBM_SETPOS,(WPARAM)0,0);
			SendMessage(GetDlgItem(hDlg,IDC_PROGRESS3),PBM_SETPOS,(WPARAM)0,0);
			hDialog=hDlg;
		return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
					if( Cells!=NULL ) {
						drawFlag=0;
						
						delete[] Cells;
						delete[] object;
						delete[] normal;
						delete[] rcalc;
						delete[] rnormal;
						delete[] n_len;
						delete[] plane;
						delete[] light;
						delete[] ftab;
						for( int f=0; f<256; f++ )
							delete[] sfaces[f].datas;
						delete[] sfaces;
						delete[] Triangles;
						delete[] TrianglesIndex;
					}

					DaneI3D->GetParams(	Grid_X_Size,
										Grid_Y_Size,
										Grid_Z_Size,
										Cell_Size,
										ThresholdValue,
										zoom,
										z_depth);

					if( FillCellsBMP() ) {
						PolygoniseAll();
						init_vect();
						drawFlag=1;

						sprintf(tmp,"Operacja wczytywania zakoñczona.\n\nIloœæ wierzcho³ków: %d\nIloœæ trójk¹tów: %d",vert,Triangles_len);
						MessageBox(hDlg,tmp,"Informacje",MB_ICONINFORMATION);
					}
					PrzeliczFlag=true;
					EndDialog(hDlg,FALSE);
				break;

				case IDCANCEL:
					PrzeliczFlag=false;
					EndDialog(hDlg,FALSE);
				break;
			}
			return TRUE;

		case WM_CLOSE:
			PrzeliczFlag=false;
			EndDialog(hDlg,FALSE);
			return TRUE;
	}

	return FALSE;
}

void prepare_faces(void) {
	int i;

	long x1,y1;
	float z1;
	
	long x2,y2;
	float z2;
	
	long x3,y3;
	float z3;

	int w;

	for (i=0,ftablen=0; i<Triangles_len; i++) {
		x1 = plane[TrianglesIndex[i].p[0]].x;
		y1 = plane[TrianglesIndex[i].p[0]].y;
		z1 = rcalc[TrianglesIndex[i].p[0]].z + z_depth;

		x2 = plane[TrianglesIndex[i].p[1]].x;
		y2 = plane[TrianglesIndex[i].p[1]].y;
		z2 = rcalc[TrianglesIndex[i].p[1]].z + z_depth;

		x3 = plane[TrianglesIndex[i].p[2]].x;
		y3 = plane[TrianglesIndex[i].p[2]].y;
		z3 = rcalc[TrianglesIndex[i].p[2]].z + z_depth;

		w = ((x1-x2)*(y3-y2)) - ((x2-x3)*(y2-y1));
		w = 1;

		if (w > 0) {
			ftab[ftablen].zet = (int)((z1+z2+z3)*1.0);
			ftab[ftablen].index = i;
			ftablen++;
		}
	}
}

void sort_faces(void) {
	int i,j,k,r;
	int p;

	for (j=0; j < 8*4; j+=8) {
		for (i=0; i<256; i++) 
			sfaces[i].fcount = 0;

		for (i=0; i<ftablen; i++) {
			p = (ftab[i].zet >> j) & 0xff;
			sfaces[p].datas[sfaces[p].fcount] = ftab[i];
			sfaces[p].fcount++;
		}

		for (i=0,k=0; i<256; i++) {
			if (sfaces[i].fcount) {
				for (r=0; r<sfaces[i].fcount; r++)  
					ftab[r+k] = sfaces[i].datas[r];
				k+=sfaces[i].fcount;
			}
		}
	}
}

int draw_vect_drut(HDC ekran) {
	HPEN nPen,oPen;

	nPen = CreatePen(PS_SOLID,1,Drut_KolorLinii);
	oPen = (HPEN)SelectObject(ekran,nPen);
	
	for( int i=0; i<Triangles_len; i++ ) {
		MoveToEx(ekran,plane[TrianglesIndex[i].p[0]].x,plane[TrianglesIndex[i].p[0]].y,NULL);
		LineTo(ekran,plane[TrianglesIndex[i].p[1]].x,plane[TrianglesIndex[i].p[1]].y);

		MoveToEx(ekran,plane[TrianglesIndex[i].p[1]].x,plane[TrianglesIndex[i].p[1]].y,NULL);
		LineTo(ekran,plane[TrianglesIndex[i].p[2]].x,plane[TrianglesIndex[i].p[2]].y);

		MoveToEx(ekran,plane[TrianglesIndex[i].p[2]].x,plane[TrianglesIndex[i].p[2]].y,NULL);
		LineTo(ekran,plane[TrianglesIndex[i].p[0]].x,plane[TrianglesIndex[i].p[0]].y);
	}

	SelectObject(ekran,oPen);
	DeleteObject(nPen);

	return 0;
}

int draw_vect_flat(HDC ekran) {
	HPEN nPen,oPen;
	HBRUSH nBrush,oBrush;

	POINT pt[3];

	nPen = CreatePen(PS_SOLID,1,Flat_KolorLinii);
	nBrush = CreateSolidBrush(Flat_KolorWypelnienia);

	oPen=(HPEN)SelectObject(ekran,nPen);
	oBrush=(HBRUSH)SelectObject(ekran,nBrush);

	for( int i=ftablen; i>=0; i-- ) {
		pt[0].x = plane[TrianglesIndex[ftab[i].index].p[0]].x;
		pt[0].y = plane[TrianglesIndex[ftab[i].index].p[0]].y;

		pt[1].x = plane[TrianglesIndex[ftab[i].index].p[1]].x;
		pt[1].y = plane[TrianglesIndex[ftab[i].index].p[1]].y;

		pt[2].x = plane[TrianglesIndex[ftab[i].index].p[2]].x;
		pt[2].y = plane[TrianglesIndex[ftab[i].index].p[2]].y;

		Polygon(ekran,pt,3);
	}

	SelectObject(ekran,oPen);
	SelectObject(ekran,oBrush);

	return 0;
}

void draw_phong(HDC ekran) {
	int i;

	POINT pt[3];

	HPEN oPen,nPen;
	HBRUSH oBrush,nBrush;

	int l;

	for (i=ftablen-1; i>=0; i--)
	{
		pt[0].x = plane[TrianglesIndex[ftab[i].index].p[0]].x;
		pt[0].y = plane[TrianglesIndex[ftab[i].index].p[0]].y;

		pt[1].x = plane[TrianglesIndex[ftab[i].index].p[1]].x;
		pt[1].y = plane[TrianglesIndex[ftab[i].index].p[1]].y;

		pt[2].x = plane[TrianglesIndex[ftab[i].index].p[2]].x;
		pt[2].y = plane[TrianglesIndex[ftab[i].index].p[2]].y;

		l = light[ftab[i].index];

		if (l < 0) l = -l;
		if (l > 255) l = 255;

		l = 255 - l;

		nPen = CreatePen(PS_SOLID,1,RGB(PhongPal[l].r,PhongPal[l].g,PhongPal[l].b));
		oPen = (HPEN)SelectObject(ekran,nPen);

		nBrush = CreateSolidBrush(RGB(PhongPal[l].r,PhongPal[l].g,PhongPal[l].b));
		oBrush = (HBRUSH)SelectObject(ekran,nBrush);

		Polygon(ekran,pt,3);

		SelectObject(ekran,oPen);
		SelectObject(ekran,oBrush);

		DeleteObject(nPen);
		DeleteObject(nBrush);
	}
}

int draw_vect(HDC ekran,int tryb) {
	switch( tryb ) {
		case 1:
			draw_vect_drut(ekran);
			break;

		case 2:
			draw_vect_flat(ekran);
			break;

		case 3:
			draw_phong(ekran);
			break;
	}
	return 0;
}

int draw(HDC ekran,int wsx,int wsy,int kx,int ky) {
	rotate_points(kx,ky);
	rotate_normals(kx,ky);
	persp_calc(wsx,wsy);
	prepare_faces();
	sort_faces();
	light_calc();
	draw_vect(ekran,draw_mode);

	return 0;
}

void Msg(char *m) {
	MessageBox(hWnd,m,"Ostrze¿enie",MB_ICONEXCLAMATION);
}

void WriteBMPFile(HBITMAP bitmap, LPTSTR filename, HDC hDC) {
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD cClrBits;
	HANDLE hf; // file handle
	BITMAPFILEHEADER hdr; // bitmap file-header
	PBITMAPINFOHEADER pbih; // bitmap info-header
	LPBYTE lpBits; // memory pointer
	DWORD dwTotal; // total count of bytes
	DWORD cb; // incremental count of bytes
	BYTE *hp; // byte pointer
	DWORD dwTmp;

	// create the bitmapinfo header information
	if( !GetObject( bitmap, sizeof(BITMAP), (LPSTR)&bmp) ) {
		Msg("Could not retrieve bitmap info");
		return;
	}
	
	// Convert the color format to a count of bits.
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;
	// Allocate memory for the BITMAPINFO structure.
	if (cClrBits != 24)
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR,sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits));
	else
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits);
	// If the bitmap is not compressed, set the BI_RGB flag.
	pbmi->bmiHeader.biCompression = BI_RGB;
	// Compute the number of bytes in the array of color
	// indices and store the result in biSizeImage.
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 * pbmi->bmiHeader.biHeight * cClrBits;
	// Set biClrImportant to 0, indicating that all of the
	// device colors are important.
	pbmi->bmiHeader.biClrImportant = 0;
	// now open file and save the data
	pbih = (PBITMAPINFOHEADER) pbmi;
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) {
		Msg("writeBMP::Could not allocate memory");
		return;
	}
	
	// Retrieve the color table (RGBQUAD array) and the bits
	if (!GetDIBits(hDC, HBITMAP(bitmap), 0, (WORD) pbih->biHeight, lpBits, pbmi,DIB_RGB_COLORS)) {
		Msg("writeBMP::GetDIB error");
		return;
	}

	// Create the .BMP file.
	hf = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, (DWORD) 0,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,(HANDLE) NULL);
	if (hf == INVALID_HANDLE_VALUE){
		Msg("Could not create file for writing");
		return;
	}

	hdr.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M"
	// Compute the size of the entire file.
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	// Compute the offset to the array of color indices.
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed* sizeof (RGBQUAD);
	// Copy the BITMAPFILEHEADER into the .BMP file.
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER),(LPDWORD) &dwTmp, NULL)) {
		Msg("Could not write in to file");
		return;
	}
	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp, ( NULL))){
		Msg("Could not write in to file");
		return;
	}
	// Copy the array of color indices into the .BMP file.
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)){
		Msg("Could not write in to file");
		return;
	}
	// Close the .BMP file.
	if (!CloseHandle(hf)){
		Msg("Could not close file");
		return;
	}

	// Free memory.
	GlobalFree((HGLOBAL)lpBits);
}

void Write3DFile(LPTSTR filename) {
	FILE *f;
	
	int i;

	f = fopen(filename,"wt");
	if( f==NULL ) {
		return;
	}

	fprintf(f,"Named Object: \"%s\"\n",filename);
	fprintf(f,"Tri-mesh, Vertices: %d     Faces: %d\n",vert,Triangles_len);
	fprintf(f,"Vertex list:\n");
	
	for(i=0; i<vert; i++)
		fprintf(f,"Vertex %d: X: %.6f     Y: %.6f     Z: %.6f\n",i,object[i].x,object[i].z,-object[i].y);

	
	fprintf(f,"Face list:\n");

	for(i=0; i<Triangles_len; i++) {
		fprintf(f,"Face %d:    A:%d B:%d C:%d AB:1 BC:1 CA:1\n",i,TrianglesIndex[i].p[0],TrianglesIndex[i].p[1],TrianglesIndex[i].p[2]);
		fprintf(f,"Smoothing: 1\n");
	}

	fclose(f);
}

void ChooseColorDialog(COLORREF &pcrColor)
{
   CHOOSECOLOR cc;
   static COLORREF crCustomColors[16];
 
   // Uzupe³niamy pola struktury
   cc.lStructSize = sizeof(cc);
   cc.hwndOwner = NULL;
   cc.hInstance = NULL;
   cc.rgbResult = pcrColor;
   cc.lpCustColors = crCustomColors;
   cc.Flags = CC_RGBINIT|CC_FULLOPEN;
   cc.lCustData = 0;
   cc.lpfnHook = NULL;
   cc.lpTemplateName = NULL;
 
   // Wywo³ujemy okno dialogowe do wyboru koloru
   ChooseColor(&cc);
 
   // Zwracamy wybrany kolor
   pcrColor = cc.rgbResult;
}
