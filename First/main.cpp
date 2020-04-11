
#include "GLUT.h"
#include <math.h>
#include <time.h>
#include "Node.h"
#include "Room.h"
#include <vector>
#include <iostream>
#include <queue>
#include "CompareNodes.h"
#include "Bullet.h"
#include "Granade.h"
#include "Player.h"

using namespace std;

const int W = 800; // window width
const int H = 800; // window height
const int numOfEquipment = 5;

const int NUM_ROOMS = 20;
const int NUM_PLAYRES_IN_GROUP = 2;

bool run_bfs = false;
bool run_Astar = false;

Node maze[MSZ][MSZ];
double map[MSZ][MSZ] = { 0 };
Room rooms[NUM_ROOMS];
int numExistingRooms = 0;

Player* group1[NUM_PLAYRES_IN_GROUP];
Player* group2[NUM_PLAYRES_IN_GROUP];

Node medicines[numOfEquipment];
Node bullets[numOfEquipment];
Node granades[numOfEquipment];


Bullet* pb = NULL;
Granade* pg;
bool move_on = false;

vector <Node> gray,grayBfs; // gray nodes

// functions decleration:
void SetupMaze();
void createEquipment();
void create_group(Player* group[NUM_PLAYRES_IN_GROUP], const int color);
void chooseTarget();
void BfsIteration(int start, int target);
void resetValues(vector<Node> dirty, int value_start);
int counter = 0;

void init()
{
	srand(time(0)); // pseudo randomizer

	glClearColor(0.7, 0.7, 0.7, 0);

	SetupMaze();

	glOrtho(-1, 1, -1, 1, -1, 1);

}

Room GenerateRoom()
{
	int w, h, ci, cj;
	Room* pr = nullptr;
	bool isOveralaping;

	do
	{
		delete pr;
		isOveralaping = false;
		w = 6 + rand() % 10;
		h = 6 + rand() % 10;

		ci = h / 2 + rand() % (MSZ - h);
		cj = w / 2 + rand() % (MSZ - w);

		pr = new Room(ci, cj, w, h);
		//		cout << "check new Room " << "center: (" << ci << "," << cj << "), width: " << w << ", height" << h << endl;
		for (int i = 0; i < numExistingRooms && !isOveralaping; i++)
		{
			//			cout << "room # " << i << " ";
			//			rooms[i].toString();
			if (rooms[i].CheckOverlapping(pr))
				isOveralaping = true;

		}
	} while (isOveralaping);

	// pr is not overlapping with other rooms
	for (int i = pr->getLeftTop().getRow(); i <= pr->getRightBottom().getRow(); i++)
		for (int j = pr->getLeftTop().getCol(); j <= pr->getRightBottom().getCol(); j++)
			maze[i][j].SetValue(SPACE);
	return *pr;
}

// check if the node at row,col is white or gray that is better then the previous one
// and if so add it to pq
void AddNode(int row, int col, Node* pn, vector<Node> &gray, vector<Node> &black,
	priority_queue <Node*, vector<Node*>, CompareNodes> &pq)
{
	Point2D pt;
	Node* pn1;
	vector<Node>::iterator gray_it;
	vector<Node>::iterator black_it;
	double cost;

	pt.setRow(row);
	pt.setCol(col);
	if (maze[row][col].GetValue() == SPACE)
		cost = 0.1; // space cost
	else if (maze[row][col].GetValue() == WALL)
		cost = 3;
	// cost depends on is it a wall or a space
	pn1 = new Node(pt, pn->getTarget(), maze[pt.getRow()][pt.getCol()].GetValue(), pn->getG() + cost, pn);

	black_it = find(black.begin(), black.end(), *pn1);
	gray_it = find(gray.begin(), gray.end(), *pn1);
	if (black_it == black.end() && gray_it == gray.end()) // it is not black and not gray!
	{// i.e. it is white
		pq.push(pn1);
		gray.push_back(*pn1);
	}
}


void AddNeighbours(Node* pn, vector<Node> &gray, vector<Node> &black,
	priority_queue <Node*, vector<Node*>, CompareNodes> &pq)
{
	// try down
	if (pn->getPoint().getRow() < MSZ - 1)
		AddNode(pn->getPoint().getRow() + 1, pn->getPoint().getCol(), pn, gray, black, pq);
	// try up
	if (pn->getPoint().getRow() > 0)
		AddNode(pn->getPoint().getRow() - 1, pn->getPoint().getCol(), pn, gray, black, pq);
	// try left
	if (pn->getPoint().getCol() > 0)
		AddNode(pn->getPoint().getRow(), pn->getPoint().getCol() - 1, pn, gray, black, pq);
	// try right
	if (pn->getPoint().getCol() < MSZ - 1)
		AddNode(pn->getPoint().getRow(), pn->getPoint().getCol() + 1, pn, gray, black, pq);
}

void Astar(Node* start, Node* Target) 
{

}

void resetValues(vector<Node> dirty,int value_start) {
	Node dirty_node;
	while (!dirty.empty()) {
		dirty_node = dirty[0];
		dirty.erase(dirty.begin());
		if (dirty_node.GetValue() != value_start)
			maze[dirty_node.GetLine()][dirty_node.GetColumn()].SetValue(SPACE);
	}
}

void RestorePath(Node* first, int startValue)
{
	Node* current = first;
	Node* next= first;

	while (current->GetValue() != startValue)
	{
		maze[current->GetLine()][current->GetColumn()].SetValue(SPACE);
		next = current;
		current = current->getParent();
	}
	maze[next->GetLine()][current->GetColumn()].SetValue(startValue);
	maze[current->GetLine()][current->GetColumn()].SetValue(SPACE);
}

// Bfs for attack mode
void BfsIteration(int start,int target) // start = start  value, target = target  value
{
	printf("\n BFS!!!");

	Node* current;
	vector <Node> dirty;
	while (run_bfs == true) 
	{
		if (grayBfs.empty()) // there is no way to target
		{
			run_bfs = false;
			resetValues(dirty, start);
		}
		else
		{
			// kind of pop
			current = new Node(grayBfs[0]);
			grayBfs.erase(grayBfs.begin());
			dirty.push_back(*current);
			// check down 
			if (maze[current->GetLine()][current->GetColumn()].GetValue() != start)
				maze[current->GetLine()][current->GetColumn()].SetValue(BLACK); // visited
			if (maze[current->GetLine() - 1][current->GetColumn()].GetValue() == target) // found
			{
				run_bfs = false;
				// restore path
				resetValues(grayBfs, start);
				resetValues(dirty, start);
				RestorePath(current, start);
			}
			else if (maze[current->GetLine() - 1][current->GetColumn()].GetValue() == SPACE)
			{
				maze[current->GetLine() - 1][current->GetColumn()].SetValue(GRAY); // paint it gray
				maze[current->GetLine() - 1][current->GetColumn()].SetParent(current);
				grayBfs.push_back(maze[current->GetLine() - 1][current->GetColumn()]);
			}
			if (run_bfs) // the target wasn't found yet
			{
				// check UP
				if (maze[current->GetLine() + 1][current->GetColumn()].GetValue() == target) // found
				{
					run_bfs = false;
					// restore path
					resetValues(grayBfs, start);
					resetValues(dirty, start);
					RestorePath(current, start);
				}
				else if (maze[current->GetLine() + 1][current->GetColumn()].GetValue() == SPACE)
				{
					maze[current->GetLine() + 1][current->GetColumn()].SetValue(GRAY); // paint it gray
					maze[current->GetLine() + 1][current->GetColumn()].SetParent(current);
					grayBfs.push_back(maze[current->GetLine() + 1][current->GetColumn()]);
				}
			}
			if (run_bfs) // the target wasn't found yet
			{
				// check LEFT
				if (maze[current->GetLine()][current->GetColumn() - 1].GetValue() == target) // found
				{
					run_bfs = false;
					// restore path
					resetValues(grayBfs, start);
					resetValues(dirty, start);
					RestorePath(current, start);
				}
				else if (maze[current->GetLine()][current->GetColumn() - 1].GetValue() == SPACE)
				{
					maze[current->GetLine()][current->GetColumn() - 1].SetValue(GRAY); // paint it gray
					maze[current->GetLine()][current->GetColumn() - 1].SetParent(current);
					grayBfs.push_back(maze[current->GetLine()][current->GetColumn() - 1]);
				}
			}
			if (run_bfs) // the target wasn't found yet
			{
				// check RIGHT
				if (maze[current->GetLine()][current->GetColumn() + 1].GetValue() == target) // found
				{
					run_bfs = false;
					// restore path
					resetValues(grayBfs, start);
					resetValues(dirty, start);
					RestorePath(current, start);
				}
				else if (maze[current->GetLine()][current->GetColumn() + 1].GetValue() == SPACE)
				{
					maze[current->GetLine()][current->GetColumn() + 1].SetValue(GRAY); // paint it gray
					maze[current->GetLine()][current->GetColumn() + 1].SetParent(current);
					grayBfs.push_back(maze[current->GetLine()][current->GetColumn() + 1]);
				}
			}
		}
	}
}
/*
void A_Star_Algorithm()
{
	Node* current, * next;

	if (pq.empty())
	{
		cout << "There is no path" << endl;
		AStar = false;
	}
	else
	{
		current = pq.top();
		pq.pop();

		if (current->GetValue() == TARGET)
		{
			cout << "Target was found" << endl;
			AStar = false;
			RestorePathInGraph(current->GetParent());
			return;
		}

		if (current->GetValue() != START)
			current->SetValue(BLACK);

		// check all non-black neighbors of current
		for (auto it : current->GetOutgoing())
		{
			// next is the Node that is the secondend of edge it
			next = graph.GetNode(it->GetSecond());
			if (next->GetValue() != BLACK)
			{
				double tmp = current->GetG() + it->GetCost();
				// update G of next
				if (next->GetValue() == SPACE || // white target
					(next->GetValue() == TARGET && next->GetG() <= 0.01))
				{
					// Add next  to PQ only if next was WHITE 
					next->SetG(tmp);
					if (next->GetValue() == SPACE)
						next->SetValue(GRAY);
					next->SetParent(current);
					pq.push(next);
				}
				else // compare it to its actual G
				{
					if (tmp < next->GetG())
					{
						next->SetG(tmp);
						next->SetParent(current);
					}
				}
			}

		}
	}
}
*/
void chooseTarget()
{	
	int i,state;
	Node target;

	//for loop - group1
	for (i = 0;i < NUM_PLAYRES_IN_GROUP;i++) {
		group1[i]->decisionTree();
		state = group1[i]->getState();
		switch(state) {
			case FIGHT:
				grayBfs.push_back(*(group1[i]->getNode()));
				run_bfs = true;

				printf("\n before BFS %d", group1[i]->getNode()->GetColumn());

				BfsIteration(group1[i]->getNode()->GetValue(), group2[0]->getNode()->GetValue());

				printf("\n after BFS");

				//run_Astar = false;
				break;
			case ESCAPE:
				//BfsIteration(group1[i]->getNode()->GetValue(), group2[i]->getNode()->GetValue(), ESCAPE);
				break;
			case FILLING_HEALTH:
				//A_Star_Algorithm();
				break;
			case FILLING_WEAPON:
				//A_Star_Algorithm();
				break;
		}
	}
	//for loop - group2
	for (i = 0;i < NUM_PLAYRES_IN_GROUP;i++) {
		group2[i]->decisionTree();
		state = group2[i]->getState();
	}
	
	run_Astar = false;

}

// implement A* from start to target
void GeneratePath(Point2D start, Point2D target)
{
	priority_queue <Node*, vector<Node*>, CompareNodes> pq;
	vector<Node> gray;
	vector<Node> black;
	Node *pn;
	bool stop = false;
	vector<Node>::iterator gray_it;
	vector<Node>::iterator black_it;
	double wall_cost = 10;
	double space_cost = 0.2;
	pn = new Node(start, &target, maze[start.getRow()][start.getCol()].GetValue(), 0, nullptr);
	pq.push(pn);
	gray.push_back(*pn);
	while (!pq.empty() && !stop)
	{
		// take the best node from pq
		pn = pq.top();
		// remove top Node from pq
		pq.pop();
		if (pn->getPoint() == target) // the path has been found
		{
			stop = true;
			// restore path to dig tunnels
			// set SPACE instead of WALL on the path
			while (!(pn->getPoint() == start))
			{
				maze[pn->getPoint().getRow()][pn->getPoint().getCol()].SetValue(SPACE);
				pn = pn->getParent();
			}
			return;
		}
		else // pn is not target
		{
			// remove Node from gray and add it to black
			gray_it = find(gray.begin(), gray.end(), *pn); // operator == must be implemented in Node
			if (gray_it != gray.end())
				gray.erase(gray_it);
			black.push_back(*pn);
			// check the neighbours
			AddNeighbours(pn, gray, black, pq);
			// try up
/*			if (pn->getPoint().getRow() < MSZ - 1)
			{
				Point2D pt;
				pt.setCol(pn->getPoint().getCol());
				pt.setRow(pn->getPoint().getRow() + 1); // going up
				int value = maze[pt.getRow()][pt.getCol()].GetValue();
				double cost;
				if (value == SPACE) cost = space_cost;
				else cost = wall_cost;
				pn1 = new Node(pt, &target, value, pn->getG() + cost, pn);
				// check if this is not black neighbour
				black_it = find(black.begin(), black.end(), pn1); // operator == must be implemented in Node
				if (black_it != black.end())
				{
					// check if pn1 is gray
					gray_it = find(gray.begin(), gray.end(), pn1); // operator == must be implemented in Node
					if (gray_it != gray.end()) // it is already gray
					{
						// check if pn1 has lower f then what was foud before
						if (pn1->getF() < (*gray_it)->getF())
						{
							(*gray_it) = pn1;
							// and update it in PQ!!!!!
						}
					}
						// add pn1 to pq and to gray
				}
			}*/

		}
	}
}

void DigTunnels()
{
	int i, j;

	for (i = 0; i < NUM_ROOMS; i++)
	{
		cout << "Path from " << i << endl;
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			cout << " to " << j << endl;
			GeneratePath(rooms[i].getCenter(), rooms[j].getCenter());
		}
	}
}

void SetupMaze()
{
	int i, j, k;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			maze[i][j].SetValue(WALL);
			maze[i][j].SetColumn(j);
			maze[i][j].SetLine(i);
		}

	for (numExistingRooms = 0; numExistingRooms < NUM_ROOMS; numExistingRooms++)
		rooms[numExistingRooms] = GenerateRoom();

	for (k = 0; k < 30; k++)
	{
		i = rand() % MSZ;
		j = rand() % MSZ;
		maze[i][j].SetValue(WALL);
	}

	DigTunnels();

	//create two groups and add to maze:
	create_group(group1, GROUP1);
	create_group(group2, GROUP2);
	createEquipment();
}

void createEquipment() {
	int x, y, i;
	for (int i = 0; i < numOfEquipment * 3; i++) // numOfEquipment*3 = 15
	{
		do {
			x = rand() % MSZ;
			y = rand() % MSZ;
		} while (maze[x][y].GetValue() != SPACE);
		if (i < numOfEquipment) // i < 5
		{
			maze[x][y].SetValue(MEDICINE);
			medicines[i % 5] = maze[x][y];
		}
		else if (i < numOfEquipment * 2) // i < 10
		{
			maze[x][y].SetValue(BULLETS);
			bullets[i % 5] = maze[x][y];
		}
		else // 10 < i < 15
		{
			maze[x][y].SetValue(GRANADES);
			granades[i % 5] = maze[x][y];
		}
	}
}

void create_group(Player* group[NUM_PLAYRES_IN_GROUP], const int color)
{
	int i, x, y;
	for (i = 0; i < NUM_PLAYRES_IN_GROUP; i++)
	{
		do {
			x = rand() % MSZ;
			y = rand() % MSZ;
		} while (maze[x][y].GetValue() != SPACE);

		Player* p = new Player();
		p->setNode(&maze[x][y]);
		p->getNode()->SetValue(color);
		group[i] = p;
	}
}

void DrawMaze()
{
	int i, j;
	double sz, x, y;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			// set color
			switch (maze[i][j].GetValue())
			{
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case WALL:
				glColor3d(0.4, 0, 0); // dark red
				break;
			case GRAY:
				glColor3d(0, 0.5, 0.5); // light green
				break;
			case BLACK:
				glColor3d(0, 0.7, 0.3); // dark green
				break;
			case PATH:
				glColor3d(1, 0, 1); // magenta
				break;
			case GROUP1:
				glColor3d(0, 0, 1); // light blue
				break;
			case GROUP2:
				glColor3d(1, 0, 0); // red
				break;
			case MEDICINE:
				glColor3d(0, 1, 0); // turkies
				break;
			case BULLETS:
				glColor3d(1, 0.5, 0); // red special
				break;
			case GRANADES:
				glColor3d(1, 0.5, 0.7); //  
				break;
			}
			// draw rectangle
			sz = 2.0 / MSZ;
			x = j * sz - 1;
			y = i * sz - 1;
			if (maze[i][j].GetValue() == MEDICINE ||
				maze[i][j].GetValue() == GRANADES ||
				maze[i][j].GetValue() == BULLETS)
			{
				glBegin(GL_POLYGON);
				glVertex2d(x + sz / 4, y + sz / 4);
				glVertex2d(x + sz * 3 / 4, y + sz / 4);
				glVertex2d(x + sz / 4, y + sz * 3 / 4);
				glVertex2d(x + sz * 3 / 4, y + sz * 3 / 4);
				glEnd();
			}
			else
			{
				glBegin(GL_POLYGON);
				glVertex2d(x, y);
				glVertex2d(x + sz, y);
				glVertex2d(x + sz, y + sz);
				glVertex2d(x, y + sz);
				glEnd();
			}
		}
}

void DrawMap()
{
	int i, j;
	double sz, xx, yy;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			if (maze[i][j].GetValue() == SPACE)
			{
				double c;
				c = 1 - map[i][j];// 1(white) is very safe, 0(black) is very dangerous
				glColor3d(c, c, c);
				// draw rectangle
				sz = 2.0 / MSZ;
				xx = (j * sz - 1);
				yy = i * sz - 1;

				glBegin(GL_POLYGON);
				glVertex2d(xx, yy);
				glVertex2d(xx + sz, yy);
				glVertex2d(xx + sz, yy + sz);
				glVertex2d(xx, yy + sz);

				glEnd();
			}
		}
}

void GenerateMap()
{
	int num_tries = 1000;
	int i;
	int col, row;
	double x, y, sz;
	Granade* pg = nullptr;

	for (i = 0; i < num_tries; i++)
	{
		do
		{
			col = rand() % MSZ;
			row = rand() % MSZ;
		} while (maze[row][col].GetValue() != SPACE);
		sz = 2.0 / MSZ;
		x = col * sz - 1;
		y = row * sz - 1;
		pg = new Granade(x, y);
		pg->SimulateExplosion(map, maze);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawMaze();

	if (pg != NULL)
	{
		//	pb->showMe();
		pg->showMe();
	}

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

void displayMap()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawMaze();
	DrawMap();

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

// checks if dx,dy is on SPACE in maze
bool CheckIsSpace(double dx, double dy)
{
	int i, j;

	i = MSZ * (dy + 1) / 2;
	j = MSZ * (dx + 1) / 2;
	return  maze[i][j].GetValue() == SPACE;
}

void idle()
{
	if (move_on && pg != NULL)
	{
		//		pb->SetIsMoving(CheckIsSpace(pb->getX(),pb->getY()));
		//		pb->move();
		pg->moveBullets(maze);

		//		move_on = pg->GetIsMoving();
	}
	if (run_Astar)
	{
		chooseTarget();
		printf("\n after one choose target");
	}
	glutPostRedisplay();// calls indirectly to display
}

void Menu(int choice)
{
	if (choice == 1) // generate security map
	{
		move_on = false;
		GenerateMap();
		glutDisplayFunc(displayMap);
	}
	else if (choice == 2) // generate security map
	{
		glutDisplayFunc(display);
		//		pb->SetIsMoving(true);
		pg->explode();
		move_on = true;
	}
	else if (choice == 3) {
		run_Astar = true;
	}

}

void mouse(int button, int state, int x, int y)
{
	double xx, yy;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		xx = 2 * (double)x / W - 1;
		yy = 2 * ((double)H - y) / H - 1;

		//		pb = new Bullet(xx,yy);
		pg = new Granade(xx, yy);
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//		pb->SetIsMoving(true);
		//		pg->explode();

		//		move_on = true;
	}

}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Dungeon ");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // idle: when nothing happens
	glutMouseFunc(mouse);
	// menu
	glutCreateMenu(Menu);
	glutAddMenuEntry("Generate map", 1);
	glutAddMenuEntry("Explode", 2);
	glutAddMenuEntry("Astar", 3);
	//glutAddMenuEntry("Bfs", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}