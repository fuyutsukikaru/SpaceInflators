#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Player;

// Students:  Add code to this file, actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* world, int imageID, int startX, int startY);   // Constructor
	virtual void doSomething() = 0;   // Pure virutal doSomething function
	StudentWorld* getWorld();         // Return a pointer to StudentWorld
	void setDead();                   // Sets actor as dead
	bool isDead() const;              // Returns an actor as dead or not
	int everyOtherTick(int n);        // Used to perform an action within an interval
private:
	StudentWorld* m_world;        // A pointer to StudentWorld
	bool m_dead;                  // Returns true if dead
	int m_ticks;                  // Counts the number of ticks in an interval
};

class Star : public Actor
{
public:
	Star(StudentWorld* world);
	virtual void doSomething();
};

class Projectile : public Actor
{
public:
	Projectile(StudentWorld* world, int imageID, int startX, int startY, bool playerFired, int damagePoints);
	virtual void doSomething();
	bool playerFired();          // Was it fired by the Player?
private:
	bool m_playerFired;          // Returns true if player fired
	int m_damage;                // Damage of the projectile
};

class Bullet : public Projectile
{
public:
	Bullet(StudentWorld* world, int startX, int startY, bool playerFired);
};

class Torpedo : public Projectile
{
public:
	Torpedo(StudentWorld* world, int startX, int startY, bool playerFired);
};

class Goodie : public Actor
{
public:
	Goodie(StudentWorld* world, int imageID, int startX, int startY);
	virtual void doSomething();
	virtual void doSpecialAction(Player* p) = 0;    // Do something to the player
private:
	int m_ticksLeftToLive;         // Ticks left until dead
	int m_goodieTickLifetime;      // Total tick lifetime
};

class FreeShipGoodie : public Goodie
{
public:
	FreeShipGoodie(StudentWorld* world, int startX, int startY);
	virtual void doSpecialAction(Player* p);
};

class EnergyGoodie : public Goodie
{
public:
	EnergyGoodie(StudentWorld* world, int startX, int startY);
	virtual void doSpecialAction(Player* p);
};

class TorpedoGoodie : public Goodie
{
public:
	TorpedoGoodie(StudentWorld* world, int startX, int startY);
	virtual void doSpecialAction(Player* p);
};

class Ship : public Actor
{
public:
	enum ProjectileType { BULLET, TORPEDO };
	Ship(StudentWorld* world, int imageID, int startX, int startY, int startEnergy);
	int getEnergy() const;            // Return the energy of the actor
	double getEnergyPct() const;      // Return the percentage of the energy
	void decreaseEnergy(int points);  // Decrease energy by the points
	void restoreFullEnergy();         // Restore energy to starting level
	void launchProjectile(ProjectileType pt, bool playerFired);   // Launch a specified projectile either from player or alien
private:
	int m_fullEnergy;       // Full energy capacity
	int m_energy;           // Current energy
};

class Player : public Ship
{
public:
	Player(StudentWorld* world);
	virtual void doSomething();
	void damage(int points, bool hitByProjectile);   // Inflict damaged based on hit by projectile or alien
	int getNumTorpedoes() const;    // Number of torpedos Player has
	void addTorpedoes(int n);       // Add torpedos to Player
private:
	int m_torpedoes;          // Current number of torpedoes
	bool m_fired;     // True if Player fired this turn
};

class Alien : public Ship
{
public:
	Alien(StudentWorld* world, int imageID, int startEnergy, int worth);
	virtual void damage(int points, bool hitByProjectile);   // Inflict damaged based on hit by projectile or Player
	virtual void maybeDropGoodie();            // Chance of dropping a goodie
	void fireProjectile(ProjectileType pt);    // Fire a projectile of indicated type
private:
	int m_worth;      // The point value of the alien
};

class NachlingBase : public Alien
{
public:
	NachlingBase(StudentWorld* world, int imageID, int round, int worth);
	virtual void doSomething();
private:
	int m_state, MDB, HMD, HMR;    // m_state is the state of the Nachling. Rest used to calculate movement
	char m_dir;                    // Direction Nachling is facing
};

class Nachling : public NachlingBase
{
public:
	Nachling(StudentWorld* world, int round);
};

class WealthyNachling : public NachlingBase
{
public:
	WealthyNachling(StudentWorld* world, int round);
	virtual void doSomething();
	virtual void maybeDropGoodie();      // Virtual function to calculate probablity of dropping a goodie
private:
	bool m_malfunction;           // Returns true if malfunctioning
};

class Smallbot : public Alien
{
public:
	Smallbot(StudentWorld* world, int round);
	virtual void doSomething();
	virtual void damage(int points, bool hitByProjectile);
	virtual void maybeDropGoodie();
private:
	bool m_hit;           // Returns true if hit this tick
};

#endif // _ACTOR_H_