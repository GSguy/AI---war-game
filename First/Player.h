#include "Node.h"
#pragma once

const int FIGHT = 0;
const int ESCAPE = 1;
const int FILLING_HEALTH = 2;
const int FILLING_WEAPON = 3;

const int LIFE_SCORES = 10;
const int LIFE_HIT = 10;

const int FULL_LIFE = 100;
const int FULL_BULLETS = 20;
const int FULL_GRANADES = 5;

class Player
{
	
public:
	Player();
	~Player();

	void setState(int state);
	int getState();
	void add_life();
	bool hit_life(); // if life=0 => return true;
	int get_num_of_bullets();
	void set_num_of_bullets(int num_of_bullets);
	int get_num_of_geanade();
	void set_num_of_geanade(int num_of_geanade);
	void setNode(Node* node);
	Node* getNode();
	int getBehavior();
	void decisionTree();

private:
	int state;
	int life;
	int num_of_bullets;
	int num_of_geanade;
	Node* node;
	int behavior; 
};

