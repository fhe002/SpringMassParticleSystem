#include "particlesystem.h"

#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <stdio.h>
#include "const.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;


/////////////////////////////////////
/// Particle Class Implementation ///
/////////////////////////////////////

Particle::Particle(const Vector3 & p, 
						const Vector3 & v, 
						const Vector3 & a, double m, 
						double t, double sz, const Color4 & c)
	: pos(p), vel(v), acc(a), mass(m), timer(t), size(sz), col(c), isLocked(false)
{
}

void Particle::applyForce(const Vector3 & force)
{
	acc += (force / mass);
}

void Particle::applyForces(const std::vector<Vector3> & forces)
{
	Vector3 accumulator = Vector3();
	for (int i = 0; i < forces.size(); ++i)
		accumulator += forces[i];
	acc += (accumulator / mass);
}

void Particle::update(double dt)
{

    if(pos.x >= WINDOW_WIDTH)
    {   
        pos.x = WINDOW_WIDTH;
        vel.x *= -1.0;
    }
    if(pos.x <= 0.0) 
    {
        pos.x = 0.0;
        vel.x *= -1.0;
    }
    if(pos.y >= WINDOW_HEIGHT)
    {
        pos.y = WINDOW_HEIGHT;
        vel.y *= -1.0;
    }
    if(pos.y <= 0.0)
    {
        pos.y = 0.0;    
        vel.y *= -1.0;
        this->isLocked = true;
    }
    
	if (timer > 0.0) timer -= dt;
	if(!isLocked)
	{
		vel += acc * dt;
		pos += vel * dt;
	}
}

void Particle::render() const
{
	glColor4d(col.r, col.g, col.b, col.a);
	glPointSize(size);
	glBegin(GL_POINTS);
	glVertex3d(pos.x, pos.y, pos.z);
	glEnd();
}

/////////////////////////////////
/// Base Class Implementation ///
/////////////////////////////////

ParticleSystem::ParticleSystem(const Vector3 & startingLocation)
	: location(startingLocation), particles()
{
}

ParticleSystem::~ParticleSystem()
{
	for (int i = 0; i < particles.size(); ++i)
		delete particles[i];
}

void ParticleSystem::update(double dt)
{
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->update(dt);
}

void ParticleSystem::render() const
{
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->render();
}

void ParticleSystem::cleanup()
{
	std::vector<Particle*> remaining(particles.size());
	int nsize = 0;
	for (int i = 0; i < particles.size(); ++i)
	{
		if (particles[i]->timer > 0.0)
		{
			remaining[nsize] = particles[i];
			++nsize;
		}
		else
			delete particles[i];
	}
	remaining.resize(nsize);
	particles = remaining;
}

bool ParticleSystem::isDone() const
{
	return particles.size() <= 0;
}

void updateParticleSystems(std::vector<ParticleSystem*> & psystems, double dt)
{
	for (int i = 0; i < psystems.size(); ++i)
		psystems[i]->update(dt);
}

void cleanupParticleSystems(std::vector<ParticleSystem*> & psystems)
{
	for (int i = 0; i < psystems.size(); ++i)
		psystems[i]->cleanup();

	std::vector<ParticleSystem*> remaining(psystems.size());
	int nsize = 0;
	for (int i = 0; i < psystems.size(); ++i)
	{
		if (!psystems[i]->isDone())
		{
			remaining[nsize] = psystems[i];
			++nsize;
		}
		else
			delete psystems[i];
	}
	remaining.resize(nsize);
	psystems = remaining;
}

////////////////////////////////////////
/// Spring-Mass Class Implementation ///
////////////////////////////////////////

// Spring Joint Constructor
ParticleSystemSpringMass::SpringJoint::SpringJoint(Particle* p1, Particle* p2, double k, double d, double l)
	: particle1(p1), particle2(p2), stiffness(k), length(l), damp(d)
{
}

// Spring Joint function to calculate force
Vector3 ParticleSystemSpringMass::SpringJoint::calculateSpringForce() const
{
	double kd = this->damp;
	double ks = this->stiffness;
	Vector3 p1 = particle1->pos;
	Vector3 p2 = particle2->pos;
	Vector3 v1 = particle1->vel;
	Vector3 v2 = particle2->vel;

	Vector3 x = p2-p1;
	Vector3 b = v2-v1;
	
	Vector3 fspring = x * ks * -1.0;
	Vector3 fdamp = b * kd * -1.0;
	Vector3 f = fspring + fdamp;
	return f;
}

// Spring Joint render function
void ParticleSystemSpringMass::SpringJoint::render() const
{
    glBegin(GL_LINES);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if(particle1 != NULL && particle2 != NULL)
    {
        glVertex3f(particle1->pos.x, particle1->pos.y, particle1->pos.z);
        glVertex3f(particle2->pos.x, particle2->pos.y, particle2->pos.z);
    }
    glEnd();
}

// ParticleSystemSpringMass Constructor
ParticleSystemSpringMass::ParticleSystemSpringMass(const Vector3 & startingLocation)
	: ParticleSystem(startingLocation), springConnections()
{
	init();
} 

// ParticleSystemSpringMass Destructor
ParticleSystemSpringMass::~ParticleSystemSpringMass()
{
}
 
// ParticleSystemSpringMass initialization function
void ParticleSystemSpringMass::init()
{
	const int NUM_PARTICLES = 10;
	particles = std::vector<Particle*>();
	springConnections = std::vector<SpringJoint>();
	for(int i = 0; i < NUM_PARTICLES; ++i)
	{  
	    for(int j = 0; j < NUM_PARTICLES; ++j)
	    {
	        Vector3 pos = location + Vector3(i * 10, j , 0);
	        
	        
		    Vector3 vel = Vector3(0.0, 0.0, 0.0);
		    double magnitude = 10.0;
		    vel *= magnitude;
		    
		   /*
		   double randomAngle;
		Vector3 vel = Vector3(0, 1, 0);
		randomAngle = randDouble(-PI / 4, PI / 4);
		vel = vel.rotate(Vector3(1, 0, 0), randomAngle);
		randomAngle = randDouble(-PI / 4, PI / 4);
		vel = vel.rotate(Vector3(0, 0, 1), randomAngle);
		double magnitude = randDouble(50, 150);
		vel *= magnitude;
		*/
		
		    // Initialize other attributes
		    Vector3 acc = Vector3();
		    double mass = 2.0;
		    double time = 90.0;
		    double size = 1.0;
		    double stiffness = 1.8;     //3.8
		    double damp = 5.0;          //5.0
		    Color4 col = Color4(0.0, 1.0, 0.0, 1.0);

		    Particle* p = new Particle(pos, vel, acc, mass, time, size, col);
		    particles.push_back(p);
		    
            if(i > 0)
		        springConnections.push_back(SpringJoint(particles[(i-1)*NUM_PARTICLES+j], particles[i*NUM_PARTICLES+j], stiffness, damp, magnitude));
		    
		    if(j > 0)
		        springConnections.push_back(SpringJoint(particles[i*NUM_PARTICLES+(j-1)], particles[i*NUM_PARTICLES+j], stiffness, damp, magnitude));

		   
		    if(i > 0 && j > 0)
		    {
		       springConnections.push_back(SpringJoint(particles[(i-1)*NUM_PARTICLES+(j-1)], particles[i*NUM_PARTICLES+j], stiffness, damp, magnitude));
		       springConnections.push_back(SpringJoint(particles[(i)*NUM_PARTICLES+(j-1)], particles[(i-1)*NUM_PARTICLES+(j)], stiffness, damp, magnitude));
		    }
		    
		}
	}
}

// ParticleSystemSpringMass update function
void ParticleSystemSpringMass::update(double dt)
{
	// *** Complete this function
	// Reset by zeroing out the acceleration vector
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i]->acc = Vector3();
	}
	
	// Apply force of gravity to all particles
	for (int i = 0; i < particles.size(); ++i)
	{
	    //buoyant force
	    particles[i]->applyForce(Vector3(0.0, 28.0, 0.0));
	    
	    //enviroment force
	    particles[i]->applyForce(particles[i]->enviromentForce);
	    
	}
    for(int i = 0; i < particles.size(); ++i)
    {
	    //force applied by balls
	    particles[i]->applyForces(particles[i]->ballForce);
	    particles[i]->ballForce.clear();
	}
	
	for(int i = 0; i < springConnections.size(); ++i)
	{
	    Vector3 fa = springConnections[i].calculateSpringForce();
	    Vector3 fb = fa * -1.0;

	    springConnections[i].particle1->applyForce(fb);
	    springConnections[i].particle2->applyForce(fa);
	}
		
	ParticleSystem::update(dt);
}

// ParticleSystemSpringMass render function
void ParticleSystemSpringMass::render() const
{
	// *** Complete this function
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i]->render();
    }
    for(int i = 0; i < springConnections.size(); ++i)
    {
        springConnections[i].render();
    }
}

// ParticleSystemSpringMass cleanup function
void ParticleSystemSpringMass::cleanup()
{
	return;
}

// ParticleSystemSpringMass isDone function
bool ParticleSystemSpringMass::isDone() const
{
	return false;
}
