#include"windows.h"
#include"stdio.h"
#include"stdlib.h"
#define WIDTH 800
#define HEIGHT 600

HDC bufdc;

void DrawBullet(int x, int y)
{
	Ellipse(bufdc, x-10,y-10,x+10,y+10);
}

void Render(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	BitBlt(hdc, 0,0,WIDTH,HEIGHT,bufdc,0,0,SRCCOPY);
	ReleaseDC(hwnd, hdc);
}


// Define game states
enum S_Game{
	SG_splash, SG_mainmenu, SG_ingame, SG_pause
};

// Define player states
enum S_Player{
	SP_stand, SP_Lm1, SP_Lm2, SP_Rm1, SP_Rm2
};



// Define bullets
class Bullet{
	private:
		int x;
		int y;
		int dx;
		int dy;
	public:
		Bullet(){
			
		}
		Bullet(int xi, int yi, int xu, int yu)
		{
			x=xi;y=yi;
			dx=xu;
			dy=yu;
		}
		void Set(int xi, int yi, int xu, int yu)
		{
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
		void Draw()
		{
			DrawBullet(x,y);
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
		void addBullet(int xi, int yi, int xu, int yu)
		{
			BulletList* nn = new BulletList;
			nn->B.Set(xi, yi, xu, yu);
			nn->next = sira;
			sira = nn;
		}
		void UpdateAllBullets()
		{
			// Update position
			BulletList* iter = sira; BulletList* prev=sira;
			while(iter->next!=0)
			{
				if(!iter->B.Update())
				{
					// Delete the bullet
					prev->next=iter->next;
					delete iter;
					iter=prev->next;
					continue;
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

// Define bullet generator
class Spawner{
	private:
		int x;
		int y;
		int (*dx)(int);
		int (*dy)(int);
		int freq;
		int ind;
		BulletHandler beta;
	public:
		Spawner(){
			
		}
		Spawner(int xx, int yy, int f, int (*dxx)(int), int (*dyy)(int))
		{
			ind=0;
			x=xx;
			y=yy;
			dx=dxx;
			dy=dyy;
			freq=f;
			printf("Spawner generated\n");
		}
		void move()
		{
			x=dx(x);
			y=dy(y);
		}
		void Shoot()
		{
			Rectangle(bufdc, x-10, y-10, x+10, y+10);
			beta.UpdateAllBullets();
			beta.DrawAllBullets();
			static int res=0;
			res++;
			if(res<freq)return;
			res=0;
			beta.addBullet(x,y,rand()%10-5,5);
		}
};

#define FPS 30
Spawner Sss(WIDTH/2,HEIGHT/2,5, linear, constant);
void King(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	SetTimer(hwnd, 1, 1000/FPS, 0);
	HDC screendc=GetDC(hwnd);
	bufdc = CreateCompatibleDC(screendc);
	HBITMAP bufbit = CreateCompatibleBitmap(screendc, WIDTH, HEIGHT);
	SelectObject(bufdc, bufbit);
	ReleaseDC(hwnd, screendc);
	Rectangle(bufdc, 0,0,WIDTH, HEIGHT);
}

void Godfather(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	Rectangle(bufdc, 0,0,WIDTH, HEIGHT);
	Sss.move();
	Sss.Shoot();
	Render(hwnd);
}