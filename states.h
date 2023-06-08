#include"windows.h"
#include"stdio.h"
#include"stdlib.h"
#define WIDTH 800
#define HEIGHT 600

// Define game states
enum S_Game {SG_splash, SG_mainmenu, SG_loading, SG_ingame, SG_pause};
S_Game ss;

HDC bufdc;
HPEN enc, plc, def,brdrls;

HDC PauseScreen;
void DrawPauseScreen()
{
	BitBlt(bufdc, WIDTH/2-125, HEIGHT/2-50, 250, 100, PauseScreen,0,0,SRCCOPY);
}

HDC mainmenu;
void DrawMainMenu(int value)
{
	BitBlt(bufdc, 0, 0, WIDTH, HEIGHT, mainmenu, 0,0, SRCCOPY);
	//HBRUSH hbr = CreateSolidBrush(RGB(value,value,value));
	// 418,84 thru 730,361
	//RECT r={418,84,730,361};
}

void DrawBullet(bool ID, int x, int y)
{
	if(ID)SelectObject(bufdc, plc);
	else SelectObject(bufdc, enc);
	
	Ellipse(bufdc, x-10,y-10,x+10,y+10);
}

void ClearScreen()
{
	SelectObject(bufdc, def);
	Rectangle(bufdc, 0,0,WIDTH, HEIGHT);
}

void Render(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	BitBlt(hdc, 0,0,WIDTH,HEIGHT,bufdc,0,0,SRCCOPY);
	ReleaseDC(hwnd, hdc);
}

HDC herosprites[2],hedzth;
int spritelatch=3;
bool spritechange=false;

void RenderSpawner(bool ID, int x, int y)
{
	if(ID)
	{
		if(spritechange&&(spritelatch>0))
		{
			TransparentBlt(bufdc, x-32, y-32, 64, 64, herosprites[1],0,0,64,64,RGB(255,255,255));
			spritelatch--;
		}
		else
		{
			TransparentBlt(bufdc, x-32, y-32, 64, 64, herosprites[0],0,0,64,64,RGB(255,255,255));
		}
	}
	else
	{
		TransparentBlt(bufdc, x-32, y-32, 64, 64, hedzth,0,0,64,64,RGB(255,255,255));
	}
}

// Define bullets
class Bullet{
	private:
		bool ID;
		int x;
		int y;
		int dx;
		int dy;
	public:
		Bullet(){
			
		}
		Bullet(bool I,int xi, int yi, int xu, int yu)
		{
			ID=I;
			x=xi;y=yi;
			dx=xu;
			dy=yu;
		}
		void Set(bool I,int xi, int yi, int xu, int yu)
		{
			ID=I;
			x=xi;y=yi;
			dx=xu;
			dy=yu;
		}
		bool Update()
		{
			x=x+dx;
			if(x<10)
			{
				x=10;
				return false;
			}
			if(x>WIDTH-10)
			{
				x=WIDTH-10;
				return false;
			}
			y=y+dy;
			if(y<10)
			{
				y=10;
				return false;
			}
			if(y>HEIGHT-10)
			{
				y=HEIGHT-10;
				return false;
			}
			return true;
		}
		bool checkCollision()// For spawner
		{
			if(ID)// If player 
			{
				// Check if 
			}
			else// If opponent
			{
				
			}
			return false;
		}
		void Draw()
		{
			DrawBullet(ID,x,y);
		}
};

class BulletList{
	public:
	Bullet B;
	BulletList* next;
};


class BulletHandler{
	private:
		BulletList* sira;
	public:
		BulletHandler()
		{
			sira = new BulletList;
			sira->next=0;
		}
		~BulletHandler()
		{
			printf("Destruct?\n");
			for(BulletList* iter=sira;iter->next!=0;)
			{
				BulletList* temp;
				temp = iter->next;
				delete iter;
				iter = temp;
			}
		}
		void addBullet(bool ID,int xi, int yi, int xu, int yu)
		{
			BulletList* nn = new BulletList;
			nn->B.Set(ID,xi, yi, xu, yu);
			nn->next = sira;
			sira = nn;
		}
		void UpdateAllBullets()
		{
			// Update position
			BulletList* iter = sira; BulletList* prev=sira;
			while(iter->next!=0)
			{
				if( (!iter->B.Update())
						/*||
				(!iter->B.checkCollision())*/ )
				{
					// Delete the bullet
					if(prev!=iter)
					{
						prev->next=iter->next;
						delete iter;
						iter=prev->next;
						continue;
					}
				}
				prev=iter;
				iter=iter->next;
			}
		}
		void DrawAllBullets()
		{
			for(BulletList* iter = sira;iter->next!=0;iter=iter->next)
			{
				iter->B.Draw();
			}
		}
};
BulletHandler beta;

int constant(int x)
{
	return x;
}

int linear(int x)
{
	static int mult=3;
	if(x>WIDTH-50)mult=-3;
	if(x<50)mult=3;
	return x+mult;
}

bool moveleft=false, moveright=false;
int USR(int x)
{
	if(moveleft==true)x=x-5;
	if(moveright==true)x=x+5;
	if(x>WIDTH-32)x=WIDTH-32;
	if(x<32)x=32;
	return x;
}
bool shot=false,toggle=false;
// Define bullet generator
class Spawner{
	private:
		int x;
		int y;
		int (*dx)(int);
		int (*dy)(int);
		int freq;
		signed int res;
		bool ind;
	public:
		Spawner(){
			
		}
		Spawner(bool in,int xx, int yy, int f, int (*dxx)(int), int (*dyy)(int))
		{
			ind=in;
			x=xx;
			y=yy;
			res=0;
			dx=dxx;
			dy=dyy;
			freq=f;
			printf("Spawner generated\n");
		}
		void move()
		{
			x=dx(x);
			y=dy(y);
			RenderSpawner(ind, x, y);
		}
		int retx(){return x;}
		int rety(){return y;}
		void Shoot()
		{
			if(ind)
			{
				if(shot&(!toggle))
				{
					beta.addBullet(true,x,y-40,0,-10);
					toggle=true;
					return;
				}
			}
			else
			{
				res++;
				if(res<freq)return;
				res=0;
				beta.addBullet(false,x,y+40,rand()%10-5,5);
			}
		}
};

class Tiles{
	private:
		int x;
		int y;
		
};

#define FPS 60
Spawner Sss(false, WIDTH/2,HEIGHT/2-100, 4, linear, constant);
Spawner Player(true, WIDTH/2, HEIGHT-10, 60, USR, constant);
void King(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	SetTimer(hwnd, 1, 1000/FPS, 0);
	HDC screendc=GetDC(hwnd);
	// Generate Screen buffer
	bufdc = CreateCompatibleDC(screendc);
	HBITMAP bufbit = CreateCompatibleBitmap(screendc, WIDTH, HEIGHT);
	SelectObject(bufdc, bufbit);
	// Load hero bitmap 1
	HBITMAP kek = (HBITMAP)LoadImage(0, "Hero.bmp",0, 0, 0, LR_LOADFROMFILE);
	herosprites[0]=CreateCompatibleDC(screendc);
	SelectObject(herosprites[0],kek);
	// Load hero bitmap 2
	kek = (HBITMAP)LoadImage(0, "Hero-fire.bmp",0, 0, 0, LR_LOADFROMFILE);
	herosprites[1]=CreateCompatibleDC(screendc);
	SelectObject(herosprites[1],kek);
	// Load antagonist
	kek = (HBITMAP)LoadImage(0, "Antagonist.bmp",0, 0, 0, LR_LOADFROMFILE);
	hedzth=CreateCompatibleDC(screendc);
	SelectObject(hedzth,kek);
	// Load pause screen
	kek = (HBITMAP)LoadImage(0, "Pause.bmp",0, 0, 0, LR_LOADFROMFILE);
	PauseScreen=CreateCompatibleDC(screendc);
	SelectObject(PauseScreen,kek);
	// Load main menu screen
	kek = (HBITMAP)LoadImage(0, "Main Menu.bmp",0, 0, 0, LR_LOADFROMFILE);
	mainmenu=CreateCompatibleDC(screendc);
	SelectObject(mainmenu,kek);
	// Create a red pen
	enc = CreatePen(PS_SOLID, 1, RGB(255,0,0));
	// Create a green pen
	plc = CreatePen(PS_SOLID, 1, RGB(0,255,0));
	// Create default pen
	def = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	// Create a white pen
	brdrls = CreatePen(PS_SOLID, 1, RGB(255,255,255));
	// Release screen
	ss=SG_splash;
	ReleaseDC(hwnd, screendc);
}

void Godfather(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(ss)
	{
		case SG_ingame:
			ClearScreen();
			Player.move();
			Player.Shoot();
			Sss.move();
			Sss.Shoot();
			beta.UpdateAllBullets();
			beta.DrawAllBullets();
			Render(hwnd);
			break;
		case SG_pause:
			DrawPauseScreen();
			Render(hwnd);
			break;
		case SG_splash:
			{
				ClearScreen();
				static int counter=0;
				DrawMainMenu(counter);
				counter=counter+4;
				if(counter>255)ss=SG_mainmenu;
				Render(hwnd);
				break;
			}
		case SG_mainmenu:
			DrawMainMenu(255);
			Render(hwnd);
			break;
	}
}


void KeyDown(WPARAM w)
{
	switch(w)
	{
		case VK_LEFT:
			moveleft=true;
			break;
		case VK_RIGHT:
			moveright=true;
			break;
		case 0x5a: //Z
			if(ss==SG_ingame)
			{
				shot=true;
				spritechange=true;
				break;
			}
	}
}

void KeyUp(WPARAM w)
{
	switch(w)
	{
		case VK_LEFT:
			moveleft=false;
			break;
		case VK_RIGHT:
			moveright=false;
			break;
		case 0x5a: //Z
			if(ss==SG_ingame)
			{
				shot=false;
				toggle=false;
				spritechange=false;
				spritelatch=3;
				break;
			}
			else if (ss==SG_mainmenu)
			{
				ss=SG_ingame;
				break;
			}
		case 0x50: //P
			switch(ss)
			{
				case SG_ingame:
					ss=SG_pause;
					break;
				case SG_pause:
					ss=SG_ingame;
					break;
			}
			break;
	}
}