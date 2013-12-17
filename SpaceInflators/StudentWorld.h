#ifndef _STUDENTWORLD_H_
#define _STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"

#include "actor.h"
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, actor.h, and actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld();
	~StudentWorld();
	void addActor(Actor* actor);   // Adds an actor to the vector
	void addAliensOrStars();      // Adds an alien or a star
	void getPlayerLocation(int& x, int& y);  // Gets the Player's current location
	int getRound() const;         // Gets the round number
	int getNumAlienFiredProjectiles() const;    // Get the number of projectiles fired by aliens
	std::vector<Alien*> getCollidingAliens(Actor* a);   // A list of aliens on the same coordinate the player
	void increaseDead();          // Increases the number of dead aliens
	Player* getCollidingPlayer(Actor* a);       // Get a pointer to Player if actor is at same location
	void removeDeadActors();      // Removes dead actors
	void setDisplayText();        // Sets the display at ttop of screen
	// Initializes a StudentWorld
	virtual void init()
    {
		m_player = new Player(this);
    }
	// Action each tick
	virtual int move()
    {
		addAliensOrStars();    // Attempt to add an alien or a star
		setDisplayText();      // Set the display text
		m_player->doSomething();   // Make the player do something

		for (int k = 0; k < m_actors.size(); k++)
		{
			// If actor is still alive, make each actor in the vector do something
			if (!(m_actors[k]->isDead()))
				m_actors[k]->doSomething();
		}
		removeDeadActors();    // Remove dead actors
		// If the number of dead aliens equals the goal, increase the round and reset dead
		if (m_numDead == 4*getRound())
		{
			m_round++;
			m_numDead = 0;
		}
		// If player is dead, decrease lives, reset round, and return that the player has died
		if (m_player->isDead())
		{
			decLives();
			m_numDead = 0;
			return GWSTATUS_PLAYER_DIED;
		}
		return GWSTATUS_CONTINUE_GAME;    // Continue game as long as player is still alive
    }
	// Cleanup the StudentWorld
	virtual void cleanUp()
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

private:
	std::vector<Actor*> m_actors;   // Vector of pointers to actors
	Player* m_player;          // Pointer to the player
	int m_round;               // The current round number
	int m_numDead;             // Current total of dead aliens
};

#endif // _GAMEWORLD_H_
