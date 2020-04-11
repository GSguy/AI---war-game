#include "Node.h"
#include <math.h>

Node::Node()
{
	value = SPACE;
	parent = nullptr;
	g = 0;
}

Node::~Node()
{
}

Node::Node(Point2D &pt, Point2D* t, int v, double g, Node* pr) {
	point = pt;
	target = t;
	value = v;
	parent = pr;
	this->g = g;
	h = ComputeH();
}

void Node::SetValue( int value)
{
	this->value = value;
}

int Node::GetValue()
{
	return value;
}

double Node::getG()
{
	return g;
}

double Node::ComputeH()
{
	return sqrt(pow(point.getRow()-target->getRow(),2)+
		pow(point.getCol() - target->getCol(), 2));
}

double Node::getF()
{
	return g+h;
}

Point2D Node::getPoint()
{
	return point;
}
void Node::SetParent(Node* p)
{
	parent = p;
}

Node * Node::getParent()
{
	return parent;
}

Point2D * Node::getTarget()
{
	return target;
}

void Node::SetLine(int line)
{
	this->line = line;
}


void Node::SetColumn(int col)
{
	this->column = col;
}

int Node::GetLine()
{
	return line;
}


int Node::GetColumn()
{
	return column;
}