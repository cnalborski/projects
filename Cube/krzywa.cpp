#include <windows.h>

/*
//---------

#define dx 3
#define dy 3

void MarkPoints(int pCount)
{
	for (int i = 0; i < pCount; i++)
		Ellipse(ekran,(int)tab[i].x-dx,(int)tab[i].y-dy,(int)tab[i].x+dx,(int)tab[i].y+dy);
}

//---------

int CheckPoint(int x,int y,int mx,int my)
{
	if ((mx > (x-dx)) && (mx < (x+dx)) && (my > (y-dy)) && (my < (y+dy))) return 1;
	return 0;
}

//---------

void ShowCurve(void)
{
	int i;

	curve(tab[0].x,tab[0].y, tab[0].x,tab[0].y, tab[1].x,tab[1].y, tab[2].x,tab[2].y);
    
	for (i = 0; i < nPoints-3; i++)
		curve(tab[i].x,tab[i].y, tab[i+1].x,tab[i+1].y, tab[i+2].x,tab[i+2].y, tab[i+3].x,tab[i+3].y);

	curve(tab[i].x,tab[i].y, tab[i+1].x,tab[i+1].y, tab[i+2].x,tab[i+2].y, tab[i+2].x,tab[i+2].y);
}

//---------

void curve(double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4)
{
	double t = 0.0;
	POINT p,o;

	o.x = calc(x1,x2,x3,x4,t);
	o.y = calc(y1,y2,y3,y4,t);

	while (1)
	{
		MoveToEx(ekran,o.x,o.y,NULL);

		t += krok;

		if (t >= 1.0)
		{
			p.x = calc(x1,x2,x3,x4,1.0);
			p.y = calc(y1,y2,y3,y4,1.0);
			LineTo(ekran,p.x,p.y);
			break;
		}

		p.x = calc(x1,x2,x3,x4,t);
		p.y = calc(y1,y2,y3,y4,t);
 
		LineTo(ekran,p.x,p.y);

		o = p;
	}
}

//---------

LRESULT calc(double p1,double p2,double p3,double p4,double t)
{
  double t2 = t*t;
  double t3 = t2*t;

  return (LRESULT)(t3*(-0.5*p1))+(t3*1.5*p2)+(t3*(-1.5*p3))+(t3*0.5*p4)+(t2*p1)+(t2*(-2.5*p2))+(t2*2*p3)+(t2*(-0.5*p4))+(t*(-0.5*p1))+(t*0.5*p3)+p2;
}

typedef struct 
{
	double x,y;
} CPOINT;

CPOINT init_tab[4]=
			{
				{40,132},
				{120,154},
				{200,68},
				{234,140}
			};

CPOINT* tab;
CPOINT* tmp;

int nPoints = 4;

double krok = 0.02;

		case WM_LBUTTONDOWN:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);

			if (wParam & MK_SHIFT)
			{
				for (i = 0; i < nPoints; i++)
				{
					if (CheckPoint((int)tab[i].x,(int)tab[i].y,mx,my))
					{
						if (nPoints == 3)
						{
							MessageBox(okno,"Krzywa musi posiadaæ min. 3 punkty kontrolne !!!","Konunikat",MB_ICONEXCLAMATION | MB_OK);
							return 0;
						}

						nPoints--;
						tmp = new CPOINT[nPoints];

						for (p = 0,k = 0; p < nPoints+1; p++)
						{
							if (p != i) 
							{
								tmp[k] = tab[p];
								k++;
							}
						}
						
						delete[] tab;
						tab = tmp;

						InvalidateRect(okno,NULL,FALSE);

						break;
					}
				}
			}
			else {
					for (i = 0; i < nPoints; i++)
					{
						if (CheckPoint((int)tab[i].x,(int)tab[i].y,mx,my))
						{
							ktory = i;
							break;
						}
					}
				}

			return 0;

		case WM_MOUSEMOVE:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);

			if (ktory >= 0)
			{		
				tab[ktory].x = mx;
				tab[ktory].y = my;

				InvalidateRect(okno,NULL,FALSE);
			}

			bufor[0] = 0;
			itoa(mx,bufor,10);
			strcat(bufor," , ");
			itoa(my,bufor+strlen(bufor),10);
			SendMessage(statOkno,SB_SETTEXT,(WPARAM)0,(LPARAM)bufor);

			return 0;

		case WM_LBUTTONUP:
			ktory = -1;
			return 0;

		case WM_RBUTTONDOWN:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
	
			tmp = new CPOINT[nPoints+1];
			memcpy(tmp,tab,nPoints*sizeof(CPOINT));
			delete[] tab;
			tab = tmp;

			tab[nPoints].x = mx;
			tab[nPoints].y = my;

			nPoints++;
	
			InvalidateRect(okno,NULL,FALSE);
			return 0;
*/