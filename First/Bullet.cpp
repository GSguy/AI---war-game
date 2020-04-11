#include "Bullet.h"
#include "GLUT.H"
#include <math.h>



Bullet::Bullet()
{
}

Bullet::Bullet(double x, double y)
{
	double len;
	this->x = x;
	this->y = y;
	dirx = (rand() % 101)-50;
	diry = (rand() % 101)-50;
	len = sqrt(dirx*dirx + diry * diry);
	dirx /= len;
	diry /= len;
	isMoving = false;
}


Bullet::~Bullet()
{
}

void Bullet::showMe()
{
	glColor3d(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2d(x - 0.01, y);
	glVertex2d(x , y+ 0.01);
	glVertex2d(x + 0.01, y);
	glVertex2d(x, y - 0.01);
	glEnd();
}

void Bullet::SetIsMoving(bool move)
{
	isMoving = move;
}

bool Bullet::GetIsMoving()
{
	return isMoving;
}

void Bullet::move(Node maze[MSZ][MSZ])
{
	int i, j;
	i = MSZ * (y + 1) / 2;
	j = MSZ * (x + 1) / 2;

	if (isMoving && maze[i][j].GetValue()==SPACE)
	{
		x += 0.001*dirx;
		y += 0.001*diry;
	}
}

double Bullet::getX()
{
	return x;
}

double Bullet::getY()
{
	return y;
}

void Bullet::SetDir(double angle)
{
	dirx = cos(angle);
	diry = sin(angle);
}

void Bullet::SimulateMotion(double map[MSZ][MSZ], Node maze[MSZ][MSZ])
{
	int i, j;
	i = MSZ * (y + 1) / 2;
	j = MSZ * (x + 1) / 2;

	while(maze[i][j].GetValue() == SPACE)
	{
		map[i][j] += delta;
		x += 0.001*dirx;
		y += 0.001*diry;
		i = MSZ * (y + 1) / 2;
		j = MSZ * (x + 1) / 2;
	}

}
