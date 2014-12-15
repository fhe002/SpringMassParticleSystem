#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

#include "vector3.h"
#include "color.h"

#include <vector>
#include <map>
 
// Particle information and its functions
struct Particle
{
	Vector3 pos;
	Vector3 vel;
	Vector3 acc;
	double mass;
	bool isLocked;
	std::vector<Vector3> ballForce;
	Vector3 enviromentForce;
	// For particles which may expire can use this value to countdown
	double timer;

	// Size of the particle to render on the screen
	double size;
	
	// Color of the particle
	Color4 col;
	
	// Functions which add to the particle's acceleration
	void applyForce(const Vector3 & force);
	void applyForces(const std::vector<Vector3> & forces);

	Particle(const Vector3 & p = Vector3(), 
				const Vector3 & v = Vector3(), 
				const Vector3 & a = Vector3(), double m = 1.0, 
				double t = 1.0, double sz = 1.0, const Color4 & c = Color4());
	void update(double dt);
	void render() const;
};

// Base class for a Particle System
class ParticleSystem
{
/*
protected:
	Vector3 location;
	std::vector<Particle*> particles;
*/
public:
	Vector3 location;
	std::vector<Particle*> particles;
	ParticleSystem(const Vector3 & startingLocation = Vector3());
	virtual ~ParticleSystem();

	// A particle system should implement the following functions

	// Initializes the particle system generating particles and any other information
	virtual void init() = 0;

	// Calculates any forces and updates all particles
	virtual void update(double dt);

	// Renders all particles and anything else particular to that particle system
	virtual void render() const;

	// Scans the list of particles and removes expired particles (unless overriden)
	virtual void cleanup();

	// If there are no more particles in the list, the particle system is done
	virtual bool isDone() const;
};

// Main functions to update and clean all particle systems
void updateParticleSystems(std::vector<ParticleSystem*> & psystems, double dt);
void cleanupParticleSystems(std::vector<ParticleSystem*> & psystems);


// Interface for the Spring-Mass based Particle System
//
// You are to complete this class and free to add
// any additional variables/functions necessary
class ParticleSystemSpringMass : public ParticleSystem
{
protected:

	// Helper struct that identifies a connection between two objects
	// using Springs
	struct SpringJoint
	{
		// Reference to the two linked particles
		Particle* particle1;
		Particle* particle2;

		// Strength of the connection
		double stiffness;	
		
		// The length which the spring is at equilibrium
		double length;
        
        double damp;
        
		// Constructor
		SpringJoint(Particle* p1, Particle* p2, double k, double d, double l);

		// Returns the force of the spring applied to particle
		// Note: particle2 is simply the negative return value
		Vector3 calculateSpringForce() const;

		// Renders the spring joint
		void render() const;
	};

public:
    // Tracks all spring connections in the particle system
	std::vector<SpringJoint> springConnections;	
	
	ParticleSystemSpringMass(const Vector3 & startingLocation = Vector3());
	virtual ~ParticleSystemSpringMass();
	
	// Extended functions from the base class Particle System
	virtual void init();
	virtual void update(double dt);
	virtual void render() const;
	virtual void cleanup();
	virtual bool isDone() const;
};

#endif
