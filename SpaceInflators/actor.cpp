#include "actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), actor.h, StudentWorld.h, and StudentWorld.cpp

// Construct an Actor with parameters of GraphObject and a pointer to StudentWorld
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY) : GraphObject(imageID, startX, startY)
{
	m_world = world;  // Set the StudentWorld
	m_dead = false;   // Set death state to false
	m_ticks = 0;      // Initialize ticks to 0
	setVisible(true);  // Make the object visible
}

// Get a pointer to the StudentWorld
StudentWorld* Actor::getWorld()
{
	return m_world;
}

// Set actor to dead
void Actor::setDead()
{
	m_dead = true;
}

// Check the death state
bool Actor::isDead() const
{
	return m_dead;
}

// Perform an action in given interval n
int Actor::everyOtherTick(int n)
{
	m_ticks++;      // Increase the number of ticks
	if (m_ticks >= n)  // If ticks equals the given interval, reset to 0
		m_ticks = 0;
	return (m_ticks%n);   // Return the remainder of ticks divided by the interval
}

// Star's constructor
Star::Star(StudentWorld* world) : Actor(world, IID_STAR, rand() % VIEW_WIDTH, VIEW_HEIGHT-1)
{
	getWorld()->addActor(this);   // Add to vector in StudentWorld
}

// Star's version of doSomething
void Star::doSomething()
{
	// Move one unit down per tick
	moveTo(getX(), getY()-1);
	// If y is less than 0, set to dead
	if (getY() < 0)
		setDead();
}

// Projectile's constructor
Projectile::Projectile(StudentWorld* world, int imageID, int startX, int startY, bool playerFired, int damagePoints)
	: Actor (world, imageID, startX, startY)
{
	m_playerFired = playerFired;   // Set if player fired
	m_damage = damagePoints;       // Set damage
	getWorld()->addActor(this);    // Add to vector
}

// Projectile's doSomething
void Projectile::doSomething()
{
	// If player fired the projectile
	if (playerFired())
	{
		moveTo(getX(),getY()+1);    // Move up every tick
		std::vector<Alien*> aliens = getWorld()->getCollidingAliens(this);   // Get a vector of pointers to collided aliens
		for (int k = 0; k < aliens.size(); k++)
			aliens[k]->damage(m_damage, true);      // For each alien in vector, inflict damage
		if (!aliens.empty())
			setDead();              // Since the vector was not empty and aliens collided, set projectile as dead
	}
	// If alien fired projectile
	else if (!playerFired())
	{
		Player* hit = getWorld()->getCollidingPlayer(this);   // Get a pointer to the player if collided
		moveTo(getX(),getY()-1);    // Move down every tick
		if (hit != NULL)
		{
			hit->damage(m_damage, true);   // Inflict damage on player, projectile set as true
			setDead();              // Set the projectile as dead
		}
	}
	if (getY() < 0 || getY() >= VIEW_HEIGHT)   // If the projectile goes out of bounds, set as dead
		setDead();
}

// Return whether or not player fired
bool Projectile::playerFired()
{
	return m_playerFired;
}

// Bullet's constructor
Bullet::Bullet(StudentWorld* world, int startX, int startY, bool playerFired)
	: Projectile(world, IID_BULLET, startX, startY, playerFired, 2)
{

}

// Torpedo's constructor
Torpedo::Torpedo(StudentWorld* world, int startX, int startY, bool playerFired)
	: Projectile(world, IID_TORPEDO, startX, startY, playerFired, 8)
{

}

// Goodie's constructor
Goodie::Goodie(StudentWorld* world, int imageID, int startX, int startY) : Actor(world, imageID, startX, startY)
{
	m_goodieTickLifetime = m_ticksLeftToLive = int(100 / getWorld()->getRound()) + 30;  // Set the total and current lifetime to initial
	getWorld()->addActor(this);   // Add to vector
}

void Goodie::doSomething()
{
	// If the goodie did not collide with the player
	if (getWorld()->getCollidingPlayer(this) == NULL)
	{
		setBrightness(((double)m_ticksLeftToLive/(double)m_goodieTickLifetime) + 0.2);
		m_ticksLeftToLive--;    // Decrease brightness per tick
		if (everyOtherTick(3) == 0)
		{
			moveTo(getX(),getY()-1);  // Move down only every 3 ticks
			if (getWorld()->getCollidingPlayer(this) != NULL)
				doSpecialAction(getWorld()->getCollidingPlayer(this));    // Check if collided, and if so, do effect
		}
	}
	else
		doSpecialAction(getWorld()->getCollidingPlayer(this));    // If collided this tick, do effect
	if (m_ticksLeftToLive == 0 || getY() < 0)         // Set dead if current life becomes 0 or goes out of bounds
		setDead();
}

// FreeShipGoodie's constructor
FreeShipGoodie::FreeShipGoodie(StudentWorld* world, int startX, int startY)
	: Goodie(world, IID_FREE_SHIP_GOODIE, startX, startY)
{

}

// If FreeShipGoodie collides with Player
void FreeShipGoodie::doSpecialAction(Player *p)
{
	p->getWorld()->increaseScore(5000);    // Increase score by 5000
	p->getWorld()->playSound(SOUND_GOT_GOODIE);   // Play the goodie sound
	setDead();                   // Set as dead
	getWorld()->incLives();      // Increase player's life

}

// EnergyGoodie's constructor
EnergyGoodie::EnergyGoodie(StudentWorld* world, int startX, int startY)
	: Goodie(world, IID_ENERGY_GOODIE, startX, startY)
{

}

// If EnergyGoodie collides with Player
void EnergyGoodie::doSpecialAction(Player *p)
{
	p->getWorld()->increaseScore(5000);   // Increase score
	p->getWorld()->playSound(SOUND_GOT_GOODIE);    // Play sound
	setDead();            // Set dead
	p->restoreFullEnergy();    // Restore energy
}

// TorpedoGoodie's constructor
TorpedoGoodie::TorpedoGoodie(StudentWorld* world, int startX, int startY)
	: Goodie(world, IID_TORPEDO_GOODIE, startX, startY)
{

}

// If TorpedoGoodie collided with Player
void TorpedoGoodie::doSpecialAction(Player *p)
{
	p->getWorld()->increaseScore(5000);   // Increase score
	p->getWorld()->playSound(SOUND_GOT_GOODIE);   // Play sound
	setDead();        // Set dead
	p->addTorpedoes(5);    // Increase torpedoes by 5
}

// Ship's constructor
Ship::Ship(StudentWorld* world, int imageID, int startX, int startY, int startEnergy)
	: Actor(world, imageID, startX, startY)
{
	m_fullEnergy = m_energy = startEnergy;    // Set fullEnergy and current energy to initial
}

// Get the actor's energy
int Ship::getEnergy() const
{
	return m_energy;
}

// Return current percentage of energy
double Ship::getEnergyPct() const
{
	return ((double)m_energy/(double)m_fullEnergy);
}

// Decrease energy by specified amount
void Ship::decreaseEnergy(int energy)
{
	m_energy -= energy;
}

// Restore energy to full
void Ship::restoreFullEnergy()
{
	m_energy = m_fullEnergy;
}

// Launch a projectile
void Ship::launchProjectile(ProjectileType pt, bool playerFired)
{
	// If the player fired
	if (playerFired)
	{
		// If it's a bullet
		if (pt == BULLET)
		{
			new Bullet(getWorld(), getX(), getY()+1, true);   // Construct new bullet above Player
			getWorld()->playSound(SOUND_PLAYER_FIRE);         // Play sound
		}
		// If it's a torpedo
		else if (pt == TORPEDO)
		{
			new Torpedo(getWorld(), getX(), getY()+1, true);  // Construct new torpedo above Player
			getWorld()->playSound(SOUND_PLAYER_TORPEDO);      // Play sound
		}
	}
	// If an alien fired
	else if (!playerFired)
	{
		if (pt == BULLET)
		{
			new Bullet(getWorld(), getX(), getY()-1, false);   // Construct new bullet below alien
			getWorld()->playSound(SOUND_ENEMY_FIRE);
		}
		else if (pt == TORPEDO)
		{
			new Torpedo(getWorld(), getX(), getY()-1, false);  // Construct new torpedo below alien
			getWorld()->playSound(SOUND_ENEMY_FIRE);
		}
	}
}

// Player's constructor. Takes in a pointer to StudentWorld
Player::Player(StudentWorld* world) : Ship(world, IID_PLAYER_SHIP, VIEW_WIDTH/2, 1, 50)
{
	m_torpedoes = 0;
	m_fired = false;
}

// Player's doSomething
void Player::doSomething()
{
	std::vector<Alien*> aliens = getWorld()->getCollidingAliens(this);
	for (int k = 0; k < aliens.size(); k++)
		// For each alien that collided with player, decrease by their remaining energy
		aliens[k]->damage(aliens[k]->getEnergy(), false);
	// Since there were aliens in the vector, decrease Player's energy by 15
	if (!aliens.empty())
		damage(15, false);
	// If the Player's energy hits 0, return dead
	if (getEnergy() <= 0)
	{
		setDead();
		return;
	}
	if (everyOtherTick(2) == 0 && m_fired)
		m_fired = false;
	int ch;
	// Get an input key
	if (getWorld()->getKey(ch))
	{
		// User hit a key this tick
		switch (ch)
		{
			// Move to correct location if able
			case KEY_PRESS_LEFT:
				if (getX()-1 >= 0)
					moveTo(getX()-1,getY());
				break;
			case KEY_PRESS_RIGHT:
				if (getX()+1 < VIEW_WIDTH)
					moveTo(getX()+1,getY());
				break;
			case KEY_PRESS_UP:
				if (getY()+1 < VIEW_HEIGHT)
					moveTo(getX(),getY()+1);
				break;
			case KEY_PRESS_DOWN:
				if (getY()-1 >= 0)
					moveTo(getX(),getY()-1);
				break;
			// If space is pushed, fire a bullet every 2 ticks
			case KEY_PRESS_SPACE:
				if (!m_fired)
				{
					launchProjectile(BULLET, true);
					m_fired = true;
				}
				break;
			// If tab is pushed, fire a torpedo every 2 ticks
			case KEY_PRESS_TAB:
				if (!m_fired && getNumTorpedoes() > 0)
				{
					launchProjectile(TORPEDO, true);
					m_fired = true;
					m_torpedoes--;
				}
				break;
		}
	}
	// Check if collided with alien again
	std::vector<Alien*> aliens2 = getWorld()->getCollidingAliens(this);
	for (int p = 0; p < aliens2.size(); p++)
		aliens2[p]->damage(aliens2[p]->getEnergy(), false);
	if (!aliens2.empty())
		damage(15, false);
}

// Player's damage function
void Player::damage(int points, bool hitByProjectile)
{
	// If damaged by projectile
	if (hitByProjectile)
	{
		decreaseEnergy(points);
		getWorld()->playSound(SOUND_PLAYER_HIT);
	}
	// If collided
	else if (!hitByProjectile)
	{
		decreaseEnergy(points);
		getWorld()->playSound(SOUND_ENEMY_PLAYER_COLLISION);
	}
	// If energy becomes 0, set dead and play death sound
	if (getEnergy() <= 0)
	{
		setDead();
		getWorld()->playSound(SOUND_PLAYER_DIE);
	}
}

// Return current number of torpedoes
int Player::getNumTorpedoes() const
{
	return m_torpedoes;
}

// Add specified number of torpedoes
void Player::addTorpedoes(int n)
{
	m_torpedoes += n;
}

// Alien's constructor
Alien::Alien(StudentWorld* world, int imageID, int startEnergy, int worth)
	: Ship(world, imageID, rand() % 30, 39, startEnergy)
{
	m_worth = worth;   // Alien's point value
	getWorld()->addActor(this);   // Add to vector
}

// Nachbergs' damage function
void Alien::damage(int points, bool hitByProjectile)
{
	Player* player = getWorld()->getCollidingPlayer(this);   // Get a pointer to collided player (if available)
	// If hit by a projectile
	if (hitByProjectile)
	{
		decreaseEnergy(points);   // Decrease energy
		// If energy becomes 0
		if (getEnergy() <= 0)
		{
			getWorld()->increaseDead();   // Increase the number of dead aliens
			getWorld()->playSound(SOUND_ENEMY_DIE);   // Play death sound
			getWorld()->increaseScore(m_worth);    // Increase score by worth
			setDead();                    // Set as dead
			// One in 3 chance of dropping a goodie
			if (rand() % 3 == 0)
				maybeDropGoodie();
		}
		else
			// Just play the hit sound if not dead
			getWorld()->playSound(SOUND_ENEMY_HIT);
	}
	// If collided with player, decrease remaining energy and set as dead
	else if (!hitByProjectile && player != NULL)
	{
		decreaseEnergy(getEnergy());
		setDead();
	}
}

// Empty for WealthyNachenberg and Smallbots
void Alien::maybeDropGoodie()
{
	return;
}

// Fire a projectile of specified type
void Alien::fireProjectile(ProjectileType pt)
{
	launchProjectile(pt, false);
}

// NachlingBase's constructor
NachlingBase::NachlingBase(StudentWorld* world, int imageID, int round, int worth)
	: Alien(world, imageID, round, worth)
{
	m_state = 0;      // Set state to 0
	HMR = HMD = MDB = 0;   // Set distance calculations to 0
}

// NachlingBase's doSomething
void NachlingBase::doSomething()
{
	// Perform an action every 2 ticks
	if (everyOtherTick(2) == 0)
		return;
	else
	{
		int x, y;
		// Get the player's location
		getWorld()->getPlayerLocation(x,y);
		// Calculate the distance to left and right border
		int leftBorder = getX(), rightBorder = 29 - getX();
		// Calculate the chance of firing
		int chancesOfFiring = int(10/(getWorld()->getRound())) + 1;
		// Based on its state
		switch(m_state)
		{
			case 0:
				// If Nachling is not in same x as player
				if (x != getX())
				{
					// One in 3 chance of moving down
					if (rand() % 3 != 0)
					{
						moveTo(getX(),getY()-1);
						// If move off screen or energy is 0, set dead
						if (getY() < 0 || getEnergy() <= 0)
							setDead();
						break;
					}
					else
					{
						// Move closer to the player's x coordinate in bounds
						if (x > getX() && getX()+1 < VIEW_WIDTH)
							moveTo(getX()+1,getY()-1);
						else if (x < getX() && getX()-1 >= 0)
							moveTo(getX()-1,getY()-1);
						break;
					}
				}
				else
				{
					m_state = 1;    // Set state to 1
					// Choose smaller border and set direction and MDB
					if (leftBorder > rightBorder)
					{
						MDB = rightBorder;
						m_dir = 'R';
					}
					else if (leftBorder < rightBorder)
					{
						MDB = leftBorder;
						m_dir = 'L';
					}
					// If MDB is greater than 3, set HMD to a number between 1 and 3
					if (MDB > 3)
						HMD = (rand() % 3) + 1;
					else
						// Else, set HMD to MDB
						HMD = MDB;
				}
				// Move straight down
				moveTo(getX(),getY()-1);
				// If move off screen, set as dead
				if (getY() < 0)
					setDead();
				break;
			case 1:
				// If player is above Nachling
				if (y > getY())
				{
					// Change to state 2
					m_state = 2;
					break;
				}
				// If HMR is 0, reverse direction and set HMR to 2 times HMD
				if (HMR == 0)
				{
					if (m_dir == 'R')
						m_dir = 'L';
					else if (m_dir == 'L')
						m_dir = 'R';
					HMR = 2*HMD;
				}
				else
					// Otherwise decrease HMR
					HMR--;
				// Move in the specified direction in bounds
				if (m_dir == 'L' && getX()-1 >= 0)
					moveTo(getX()-1,getY());
				else if (m_dir == 'R' && getX()+1 < VIEW_WIDTH)
					moveTo(getX()+1,getY());
				// Fire based on chance if the number of current bullets is smaller than the limit
				if (rand() % chancesOfFiring == 0)
				{
					if (getWorld()->getNumAlienFiredProjectiles() < 2*(getWorld()->getRound()))
						fireProjectile(BULLET);
				}
				// One out of 20 chance, change to state 2
				if (rand() % 20 == 0)
					m_state = 2;
				break;
			case 2:
				// If at top of screen, change to state 0
				if (getY() == 39)
				{
					m_state = 0;
					break;
				}
				// If at left of screen, change direction to R and move
				if (getX() == 0)
				{
					m_dir = 'R';
					moveTo(getX()+1,getY()+1);
					break;
				}
				// If at right of screen, change direction to L and move
				else if (getX() == 29)
				{
					m_dir = 'L';
					moveTo(getX()-1,getY()+1);
					break;
				}
				else
				{
					// 50-50 chance move left or right
					int k = rand() % 2;
					if (k == 0)
					{
						m_dir = 'L';
						moveTo(getX()-1,getY()+1);
					}
					else if (k == 1)
					{
						m_dir = 'R';
						moveTo(getX()+1,getY()+1);
					}
					break;
				}
				break;
		}
	}
	// If move off screen, set as dead
	if (getY() < 0)
		setDead();
}

// Nachling's constructor
Nachling::Nachling(StudentWorld* world, int round)
	: NachlingBase(world, IID_NACHLING, int(5*(0.9+(0.1*(double)round))), 1000)
{

}

// WealthNachling's constructor
WealthyNachling::WealthyNachling(StudentWorld* world, int round)
	: NachlingBase(world, IID_WEALTHY_NACHLING, int(8*(0.9+(0.1*(double)round))), 1200)
{
	m_malfunction = false;    // Initally not malfunctioning
}

// WealthyNachling's doSomething
void WealthyNachling::doSomething()
{
	// If malfunctioning, wait 30 ticks
	if (m_malfunction)
	{
		if (everyOtherTick(30) == 0)
			m_malfunction = false;
		return;
	}
	// One out of 200 chance of malfunctioning
	if (rand() % 200 == 0)
		m_malfunction = true;
	// Otherwise do same behavior as a Nachling
	NachlingBase::doSomething();
}

// Probablility of WealthyNachling dropping goodies
void WealthyNachling::maybeDropGoodie()
{
	// 50% chance that WealthyNachling will drop an EnergyGoodie or a TorpedoGoodie
	if (rand() % 2 == 0)
		new EnergyGoodie(getWorld(), getX(), getY());
	else
		new TorpedoGoodie(getWorld(), getX(), getY());
}

// Smallbot's constructor
Smallbot::Smallbot(StudentWorld* world, int round) : Alien(world, IID_SMALLBOT, int(12*(0.9+(0.1*(double)round))), 1500)
{
	m_hit = false;    // Set hit state to false
}

// Smallbot's doSomething
void Smallbot::doSomething()
{
	// Do action every other tick
	if (everyOtherTick(2) == 0)
		return;
	// If the Smallbot was hit
	if (m_hit)
	{
		m_hit = false;
		// One out of 3 chance of moving left or right
		if (rand() % 3 == 0)
		{
			if (getX() == 0)
				moveTo(getX()+1,getY()-1);
			else if (getX() == 29)
				moveTo(getX()-1,getY()-1);
			else
			{
				if (rand() % 2 == 0)
					moveTo(getX()+1,getY()-1);
				else
					moveTo(getX()-1,getY()-1);
			}
		}
		else
			// Otherwise just move down
			moveTo(getX(),getY()-1);
	}
	else
		// If not hit, move down
		moveTo(getX(),getY()-1);
	int x, y;
	// Get player's location
	getWorld()->getPlayerLocation(x,y);
	// If player at same x coordinate
	if (x == getX())
	{
		// Chance of firing a torpedo
		int q = int(100.0/getWorld()->getRound()) + 1;
		if (rand() % q == 0)
			fireProjectile(TORPEDO);
		// Otherwise fire bullets based on round limit
		else if (getWorld()->getNumAlienFiredProjectiles() < 2*getWorld()->getRound())
			fireProjectile(BULLET);
	}
	// If move off screen, set as dead
	if (getY() < 0)
		setDead();
}

// Smallbot's damage function
void Smallbot::damage(int points, bool hitByProjectile)
{
	// Get a pointer to player if collided
	Player* player = getWorld()->getCollidingPlayer(this);
	// If hit by a projectile
	if (hitByProjectile)
	{
		m_hit = true;   // Set hit state to true
		decreaseEnergy(points);   // Decrease energy
		// If energy hits 0
		if (getEnergy() <= 0)
		{
			getWorld()->increaseDead();  // Increase number of dead aliens
			getWorld()->playSound(SOUND_ENEMY_DIE);   // Play sound
			getWorld()->increaseScore(1500);   // Increase score
			setDead();             // Set as dead
			// One out of 3 chance of dropping a goodie
			if (rand() % 3 == 0)
				maybeDropGoodie();
		}
		else
			// If still alive, play hit sound
			getWorld()->playSound(SOUND_ENEMY_HIT);
	}
	// If collided with player, decrease by remaining energy and set dead
	else if (!hitByProjectile && player != NULL)
	{
		decreaseEnergy(getEnergy());
		setDead();
	}
}

// Smallbot's goodies
void Smallbot::maybeDropGoodie()
{
	new FreeShipGoodie(getWorld(), getX(), getY());
}