#include "Player.h"
#include <stdlib.h>
#include <time.h>


Player::Player()
{
	life = FULL_LIFE;
	num_of_bullets = FULL_BULLETS;
	num_of_geanade = FULL_GRANADES;
	srand(time(NULL));
	behavior = rand()%2; // 0 - defence , 1- attack
	state = (behavior == 0) ? ESCAPE : FIGHT;
}

Player::~Player()
{
}

void Player::setState(int state)
{
	this->state = state;
}

int Player::getState()
{
	return this->state;
}

void Player::add_life()
{
	life += LIFE_SCORES;
	if (life > 100)
		life = 100;
}

bool Player::hit_life()
{
	life -= LIFE_HIT;
	if (life <= 0)
		return false;
	return true;
}

int Player::get_num_of_bullets()
{
	return this->num_of_bullets;
}

void Player::set_num_of_bullets(int num_of_bullets)
{
	this->num_of_bullets = num_of_bullets;
}

int Player::get_num_of_geanade()
{
	return this->num_of_geanade;
}

void Player::set_num_of_geanade(int num_of_geanade)
{
	this->num_of_geanade = num_of_geanade;
}

void Player::setNode(Node * node)
{
	this->node = node;
}

Node * Player::getNode()
{
	return node;
}

int Player::getBehavior()
{
	return behavior;
}

void Player::decisionTree()
{
	int numOfWeapon = get_num_of_bullets() + get_num_of_geanade();

	if (getBehavior() == 0) // defance behave
	{
		if (life < 30)
			setState(FILLING_HEALTH);
		else if (life < 60)
			setState(ESCAPE);
		else if (numOfWeapon < 5)
			setState(FILLING_WEAPON);
		else 
			setState(FIGHT);
	}

	else // attack behave
	{
		if (numOfWeapon > (FULL_BULLETS + FULL_GRANADES) / 2 )
			setState(FIGHT);
		else if (life > 40) // numOfWeapon < 12 && life > 40
			setState(FILLING_WEAPON);
		else if (life > 20) // life between 20 - 40
			setState(ESCAPE);
		else
			setState(FILLING_HEALTH);
	}
}
