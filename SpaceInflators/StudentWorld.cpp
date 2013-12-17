#include "StudentWorld.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

GameWorld* createStudentWorld()
{
    return new StudentWorld();
}

// Students:  Add code to this file (if you wish), StudentWorld.h, actor.h and actor.cpp

// Constructor
StudentWorld::StudentWorld()
{
	m_round = 1;     // Start at round 1
	m_numDead = 0;   // Start with 0 aliens killed
}

// Destructor
StudentWorld::~StudentWorld()
{
	delete m_player;   // Delete the player
	std::vector<Actor*>::iterator iter = m_actors.end();
	// Delete all the actors in the vector
	while (iter != m_actors.begin())
	{
		iter--;
		delete (*iter);
	}
	// Empty the vector
	while (!m_actors.empty())
		m_actors.pop_back();
}

// Add a pointer to an actor to the vector
void StudentWorld::addActor(Actor* actor)
{
	m_actors.push_back(actor);
}

// Add an alien or a star
void StudentWorld::addAliensOrStars()
{
	// If test parameters were provided
	if (testParamsProvided())
	{
		// Only add something if there are no actors on the screen
		if (m_actors.empty())
		{
			// Get the test parameter and create a new actor
			getTestParam(TEST_PARAM_ACTOR_INDEX);
			if (TEST_PARAM_ACTOR_INDEX == TEST_PARAM_NACHLING)
				new Nachling(this, getRound());
			else if (TEST_PARAM_ACTOR_INDEX == TEST_PARAM_WEALTHY_NACHLING)
				new WealthyNachling(this, getRound());
			else if (TEST_PARAM_ACTOR_INDEX == TEST_PARAM_SMALLBOT)
				new Smallbot(this, getRound());
			else if (TEST_PARAM_ACTOR_INDEX == TEST_PARAM_GOODIE_ENERGY)
				new EnergyGoodie(this, 15, 39);
			else if (TEST_PARAM_ACTOR_INDEX == TEST_PARAM_GOODIE_TORPEDO)
				new TorpedoGoodie(this, 15, 39);
			else if (TEST_PARAM_ACTOR_INDEX == TEST_PARAM_GOODIE_FREE_SHIP)
				new FreeShipGoodie(this, 15,29);
		}
		// Do nothing else this tick
		return;
	}

	int num = 0;
	// Count the number of aliens on the screen
	for (int k = 0; k < m_actors.size(); k++)
	{
		Alien *p = dynamic_cast<Alien*>(m_actors[k]);
		if (p != NULL)
			num++;
	}
	// If the number of aliens is smaller than the round limit and the number needed to move to next round
	if (num < int(2+(.5*m_round)) && num < (4*getRound() - m_numDead))
	{
		// 70% chance of adding a kind Nachling
		if (rand() % 100 < 70)
		{
			// 20% chance of adding a WealthyNachling
			if (rand() % 100 < 20)
				new WealthyNachling(this, getRound());
			// Otherwise add a regular Nachling
			else
				new Nachling(this, getRound());
		}
		// Otherwise add a Smallbot
		else
			new Smallbot(this, getRound());
	}
	// One in three chance of adding a new Star
	if (rand() % 100 < 33)
		new Star(this);
}

// Get the player's current location
void StudentWorld::getPlayerLocation(int& x, int& y)
{
	x = m_player->getX();
	y = m_player->getY();
}

// Get the world's current round
int StudentWorld::getRound() const
{
	return m_round;
}

// Get a vector of aliens that have collided with specified actor
std::vector<Alien*> StudentWorld::getCollidingAliens(Actor* a)
{
	std::vector<Alien*> m_aliens;
	for (int k = 0; k < m_actors.size(); k++)
	{
		// Find all aliens in m_actors that have the same coordinates as actor
		Alien *p = dynamic_cast<Alien*>(m_actors[k]);
		if (p != NULL && !(p->isDead()) && m_actors[k]->getX() == a->getX() && m_actors[k]->getY() == a->getY())
			// Push into the vector of collided aliens
			m_aliens.push_back(p);
	}
	// Return this new vector
	return m_aliens;
}

// Increase the number of dead aliens by one
void StudentWorld::increaseDead()
{
	m_numDead++;
}

// Count the number of projectiles fired by aliens
int StudentWorld::getNumAlienFiredProjectiles() const
{
	int num = 0;
	for (int k = 0; k < m_actors.size(); k++)
	{
		// Count the number of projectiles on the screen
		Projectile *p = dynamic_cast<Projectile*>(m_actors[k]);
		// If the projectile was not fired by a player, then increase the count
		if (p != NULL && !(p->playerFired()))
			num++;
	}
	return num;
}

// Get a pointer to the player if the actor collided with it
Player* StudentWorld::getCollidingPlayer(Actor* a)
{
	if (a->getX() == m_player->getX() && a->getY() == m_player->getY())
		return m_player;
	// Otherwise return null
	return NULL;
}

// Remove all dead actors
void StudentWorld::removeDeadActors()
{
	for (int k = 0; k < m_actors.size(); k++)
	{
		// For each actor in the vector, if they are dead, remove them and delete them
		if (m_actors[k]->isDead())
		{
			// Find the actor that has died and set an iterator to it
			std::vector<Actor*>::iterator iter = find(m_actors.begin(), m_actors.end(), m_actors[k]);
			// If the the find function did not yield end
			if (iter != m_actors.end())
			{
				// Delete the actor and erase the node
				Actor* temp = m_actors[k];
				m_actors.erase(iter);
				delete temp;
			}
		}
	}
}

void StudentWorld::setDisplayText()
{
	int score = getScore();        // The current score
	int round = getRound();        // The current round
	double energyPercent = m_player->getEnergyPct()*100;   // Get the percentage of energy and multiply by 100
	int torpedoes = m_player->getNumTorpedoes();      // Get current num of torpedoes
	int shipsLeft = getLives();     // Get current num of lives
	std::ostringstream oscore, oround, oenergy, otorpedoes, oships;   // Crease ostringstream for each variable
	oscore.fill('0');     // Fill remaining space with 0
	oscore << std::setw(7) << score;   // Create a 7 digit long ostreamstream for score
	oround.fill('0');
	oround << std::setw(2) << round;   // Create 2 digit long for round
	oenergy << energyPercent;          // Create for energy percentage
	otorpedoes.fill('0');
	otorpedoes << std::setw(3) << torpedoes;   // Create 3 digit long for torpedoes
	oships.fill('0');
	oships << std::setw(2) << shipsLeft;      // Create 2 digit long for ships
	// Create a string with all these variables
	std::string s = "Score: "+oscore.str() + "  Round: "+oround.str() + "  Energy: "+oenergy.str()+"% "
		+ " Torpedoes:"+otorpedoes.str() + "  Ships: "+oships.str();
	// Finally,update the display text at the top of the screen with your stats
	setGameStatText(s);  // calls GameWorld::setGameStatText
}