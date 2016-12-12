#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "types.h"
#include "proc3d.h"
#include "resource.h"
#include "proc3d.h"

//------------

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void InitApp(HINSTANCE hInstance,int iCmdShow);
BOOL Open(HWND okno,char* plik,int rozm,char *filtry);
void SetMenuOpt(UINT OptID);

BOOL CALLBACK DlgPrg(HWND dialog,UINT komunikat,WPARAM wParam,LPARAM lParam);

//------------

HINSTANCE zad;

//------------

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine,int iCmdShow)
{
    MSG msg;

	InitApp(hInstance,iCmdShow);

    while (GetMessage(&msg,NULL,0,0))
    {
		TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

//------------

void InitApp(HINSTANCE hInstance,int iCmdShow)
{
	static char szAppName[] = "Przegl¹darka obiektow 3D Studio.";
    HWND hwnd;
    WNDCLASS mywin;

	zad = hInstance;

	mywin.style = 0;
	mywin.hInstance = hInstance;
	mywin.lpszClassName = szAppName;
	mywin.lpfnWndProc = WndProc;
	mywin.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));
	mywin.hCursor = LoadCursor(NULL,IDC_ARROW);
	mywin.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	mywin.cbClsExtra = 0;
	mywin.cbWndExtra = 0;
	mywin.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    RegisterClass(&mywin);

	InitCommonControls();

    hwnd = CreateWindowEx(0,
						  szAppName,
						  szAppName,			
						  WS_OVERLAPPEDWINDOW,	 
						  160,						
						  80,						
					 	  620,					    
						  600,					    
						  NULL,                     
						  NULL,                     
						  hInstance,			
						  NULL); 				

    ShowWindow(hwnd,iCmdShow);
    UpdateWindow(hwnd);
}

//------------

HBITMAP mapa;
HDC sbufor;

HWND hOkno;
HWND statOkno;

HMENU hMenu;

UINT Grafika = ID_DRUT;

POINT old;
int kx = 0,ky = 0;

int file_loaded;
int LMB;

char plik3ds[256];

//------------

LRESULT CALLBACK WndProc(HWND okno,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC ekran;
	RECT winRect;

	int	winSX,winSY;
	int mx,my;

	int czesci[] = { 80,-1 };

	char bufor[256];

	hOkno = okno;

    MSG msg;

	FILE *f;
	int i;

	switch (iMsg)
	{
		case WM_CREATE:
			statOkno = CreateStatusWindow(WS_CHILD | WS_VISIBLE,"0,0",okno,100);
			SendMessage(statOkno,SB_SETPARTS,(WPARAM)2,(LPARAM)czesci);

			GetWindowRect(okno,&winRect);
			winSX = winRect.right - winRect.left;
			winSY = winRect.bottom - winRect.top;

			ekran = GetDC(okno);
			mapa = CreateCompatibleBitmap(ekran,winSX,winSY);
			sbufor = CreateCompatibleDC(ekran);
			SelectObject(sbufor,mapa);
			PatBlt(sbufor,0,0,winSX,winSY,WHITENESS);
			ReleaseDC(okno,ekran);

			hMenu = GetMenu(okno);

			SetTimer(okno,1,4,NULL);
			file_loaded = 0;
			LMB = 0;
			return 0;

		case WM_TIMER:			
			return 0;

		case WM_LBUTTONDOWN:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);

			old.x = mx;
			old.y = my;

			LMB = 1;
			return 0;

		case WM_MOUSEMOVE:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
	
			if (LMB)
			{
				kx = old.x - mx;
				ky = old.y - my;

				old.x = mx;
				old.y = my;

				InvalidateRect(okno,NULL,FALSE);
			}
			
			bufor[0] = 0;
			itoa(mx,bufor,10);
			strcat(bufor," , ");
			itoa(my,bufor+strlen(bufor),10);
			SendMessage(statOkno,SB_SETTEXT,(WPARAM)0,(LPARAM)bufor);

			return 0;

		case WM_LBUTTONUP:
			LMB = 0;
			return 0;

        case WM_PAINT:
	        ekran = BeginPaint(okno,&ps);

			GetWindowRect(okno,&winRect);

			winSX = winRect.right - winRect.left;
			winSY = winRect.bottom - winRect.top;

            if (file_loaded) display_vect(sbufor,winSX,winSY,-ky,-kx);

			BitBlt(ekran,0,0,winSX,winSY,sbufor,0,0,SRCCOPY);
			PatBlt(sbufor,0,0,winSX,winSY,PATCOPY);

			EndPaint(okno,&ps);

			kx = ky = 0;
			return 0;
	
		case WM_SIZE:
    	    SendMessage(statOkno,WM_SIZE,wParam,lParam);

			InvalidateRect(okno,NULL,FALSE);
			return 0;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_OTWORZ:
					if (Open(okno,plik3ds,sizeof(plik3ds),"Pliki 3DS\0*.3ds\0Wszystkie pliki\0*.*\0"))
					{
					  if (file_loaded) reset_vect();
					  if (init_vect(plik3ds)) SendMessage(okno,WM_DESTROY,(WPARAM)0,(LPARAM)0);
					  file_loaded = 1;
			  		  SendMessage(statOkno,SB_SETTEXT,(WPARAM)1,(LPARAM)plik3ds);
				  
					  InvalidateRect(okno,NULL,FALSE);
					}
				break;

				case ID_PARAMETRY:
					DialogBox(zad,MAKEINTRESOURCE(IDD_DIALOG1),okno,(DLGPROC)DlgPrg);
  				    InvalidateRect(okno,NULL,FALSE);

				break;

				case ID_DRUT:
					SetMenuOpt(ID_DRUT);
  				    InvalidateRect(okno,NULL,FALSE);
				break;

				case ID_WYPEL_FLAT:
					SetMenuOpt(ID_WYPEL_FLAT);
  				    InvalidateRect(okno,NULL,FALSE);
				break;

				case ID_WYPEL_PHONG:
					SetMenuOpt(ID_WYPEL_PHONG);
  				    InvalidateRect(okno,NULL,FALSE);
				break;

				case ID_USTAWIENIA:
					InvalidateRect(okno,NULL,FALSE);
				break;

				case ID_KONIEC:
					DestroyWindow(okno);
				break;
			}
			return 0;

        case WM_DESTROY:
			//DeleteDC(bufor);
			DeleteObject(mapa);
			reset_vect();
			KillTimer(okno,1);
			PostQuitMessage(0);
            return 0;
	}

    return DefWindowProc(okno, iMsg, wParam, lParam);
}

//------------

void SetMenuOpt(UINT OptID)
{
	CheckMenuItem(hMenu,ID_DRUT,MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_WYPEL_FLAT,MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_WYPEL_PHONG,MF_UNCHECKED);
	
	switch(OptID)
	{
		case ID_DRUT:
			CheckMenuItem(hMenu,ID_DRUT,MF_CHECKED);
		break;

		case ID_WYPEL_FLAT:
			CheckMenuItem(hMenu,ID_WYPEL_FLAT,MF_CHECKED);
		break;

		case ID_WYPEL_PHONG:
			CheckMenuItem(hMenu,ID_WYPEL_PHONG,MF_CHECKED);
		break;
	}

	Grafika = OptID;
}

//------------

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

//------------

extern float zoom;
extern float z_depth;

extern double tx;
extern double ty;
extern double tz;

BOOL CALLBACK DlgPrg(HWND dialog,UINT komunikat,WPARAM wParam,LPARAM lParam)
{
	char bufor[256];
	int i;

	switch(komunikat)
	{
		case WM_INITDIALOG:
				sprintf(bufor,"%.1lf",z_depth);
				SetDlgItemText(dialog,IDC_EDIT1,bufor);
				sprintf(bufor,"%.1lf",zoom);
				SetDlgItemText(dialog,IDC_EDIT2,bufor);

				sprintf(bufor,"%.1lf",tx);
				SetDlgItemText(dialog,IDC_EDIT3,bufor);
				sprintf(bufor,"%.1lf",ty);
				SetDlgItemText(dialog,IDC_EDIT4,bufor);
				sprintf(bufor,"%.1lf",tz);
				SetDlgItemText(dialog,IDC_EDIT5,bufor);	
		return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
					GetDlgItemText(dialog,IDC_EDIT1,bufor,sizeof(bufor));
					z_depth = atof(bufor);
					GetDlgItemText(dialog,IDC_EDIT2,bufor,sizeof(bufor));
					zoom = atof(bufor);

					//GetDlgItemText(dialog,IDC_EDIT3,bufor,sizeof(bufor));
					//tx = atof(bufor);
					//GetDlgItemText(dialog,IDC_EDIT4,bufor,sizeof(bufor));
					//ty = atof(bufor);
					//GetDlgItemText(dialog,IDC_EDIT5,bufor,sizeof(bufor));
					//tz = atof(bufor);

					EndDialog(dialog,FALSE);
				break;

				case IDCANCEL:
					EndDialog(dialog,FALSE);
				break;
			}

			return TRUE;

		case WM_CLOSE:
			EndDialog(dialog,FALSE);
			return TRUE;
	}

	return FALSE;
}