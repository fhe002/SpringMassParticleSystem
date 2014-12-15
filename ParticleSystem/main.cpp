#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>
#include <GL/glut.h>

#ifdef _WIN32
#include <windows.h>
#define FREEGLUT_STATIC
void usleep(__int64 usec) 
{ 
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#else
#include <unistd.h>
#endif

#include "const.h"
#include "color.h"
#include "vector3.h"
#include "particlesystem.h"

//800x800 window
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float VIEW_LEFT = 0.0;
const float VIEW_RIGHT = WINDOW_WIDTH;
const float VIEW_BOTTOM = 0.0;
const float VIEW_TOP = WINDOW_HEIGHT;
const float VIEW_FRONT = -800;
const float VIEW_BACK = 800;

int currentTime = 0;
int previousTime = 0;
const int FRAME_RATE = 25;
const float pi = 3.14159265359;
const float globalDrag = 0.999;
std::vector<ParticleSystem*> psystems;

void GLrender();
void GLupdate();
void setupScene();
void GLprocessMouse(int button, int state, int x, int y);
void DrawCircle(float cx, float cy, float r, int num_segments) ;

double randDouble(double min, double max)
{
	return rand() / static_cast<double>(RAND_MAX) * (max - min) + min;
}

//controls ball
class Player
{
public:
    float r, m;
    bool rotate, isPlayer;
    Vector3 pos, vel, acc;
    Color4 col;
    
    void update()
    {
    	if(pos.x + r >= WINDOW_WIDTH || pos.x - r <= 0.0)
     	   vel.x *= -1;

    	if(pos.y + r >= WINDOW_HEIGHT || pos.y - r <= 0.0)
     	   vel.y *= -1;
     	
     	//velocity of ball is affected by the global drag force
    	vel *= globalDrag;
    	pos += vel * (FRAME_RATE / 1000.0);
	}
	Player()
	{
	    pos = Vector3(WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0, 0.0);
        vel = Vector3(0.0, 0.0, 0.0);
        acc = Vector3(0.0, 0.0, 0.0);
	    r = 40.0;
	    m = 40.0;
	    rotate = false;
	    isPlayer = false;
	    col = Color4(1.0, 0.0, 0.0, 1.0);
	}
	Player(Vector3 center, float size, float mass)
	{
	    pos = center;
        vel = Vector3(0.0, 0.0, 0.0);
        acc = Vector3(0.0, 0.0, 0.0);
	    r = size;
	    m = mass;
	    rotate = false;
	    isPlayer = false;
	    col = Color4(1.0, 0.0, 0.0, 1.0);
	}
	void render()
	{
	    glPushMatrix();
        glColor3f(col.r, col.g, col.b);
        
        //controls rotation of ball
        if(rotate)
        {
            float rot_x = sin(currentTime/4.0 * (FRAME_RATE / 1000.0)) * 300.0;
            float rot_y = cos(currentTime/4.0 * (FRAME_RATE / 1000.0)) * 300.0;  
          
            //sets velocity of ball as rotation 
            vel = Vector3(rot_x, rot_y, 0.0);
        }
        glTranslatef(pos.x, pos.y, pos.z);
        glutSolidSphere(this->r, 80.0, 80.0);
        glPopMatrix();
    }
    float volume()
    {
        return (4.0/3.0) * pi * r*r*r;
    }
    float perimeter()
    {
        return 2.0 * pi * r;
    }
    float area()
    {
        return 4.0 * pi * r;
    }
    bool isInside(const Vector3& p) const
    {
        Vector3 temp = this->pos - p;
        double distance = temp.x*temp.x + temp.y*temp.y + temp.z*temp.z;
        if(distance <= r*r)
            return true;
        return false;
    }
    bool lineCollision(const Vector3& p1, const Vector3& p2) const
    {
        //distance direction vector
        Vector3 d = p2 - p1;
        
        //vector from first point of line to center of sphere
        Vector3 f = p1 - pos;
        
        //calculate using pythagorean formula
        float a = d.dot(d);
        float b = 2.0 * f.dot(d);
        float c = f.dot(f) - r*r;
        
        float check = (b * b) - (4.0 * a * c);
        
        //no collision
        if(check < 0.0)
            return false;
        
        //there may be a collision
        else
        {
            //apply pythagorean formula
            check = sqrt(check);
            float t1 = (-1.0 * b - check) / (2.0 * a);
            float t2 = (-1.0 * b + check) / (2.0 * a);
            
            //check if point is within sphere
            if(t1 >= 0.0 && t1 <= 1.0)
                return true;
            if(t2 >= 0.0 && t2 <= 1.0)
                return true;
            return false;
        }
        
        return false;
    }
    void rotation(bool trigger)
    {
        rotate = trigger;
    }
};
std::vector<Player*> fish;
Player p1;
void Keyboard(unsigned char key, int x, int y);
void GLCollisions(Player& ball);
void GLrunItAll();

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{ 
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Assn2 - Foley He");
	glutDisplayFunc(GLrender);
	glutIdleFunc(GLupdate);
	glClearColor(0.0f, 0.3f, 1.0f, 0.0f);
	glOrtho(VIEW_LEFT, VIEW_RIGHT, VIEW_BOTTOM, VIEW_TOP, VIEW_FRONT, VIEW_BACK);
	glutIdleFunc(GLrunItAll);
	 
	glMatrixMode(GL_MODELVIEW);
    glEnable(GL_COLOR_MATERIAL);    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);    
 
    setupScene();
}

int main(int argc, char** argv)
{
    p1 = Player(Vector3(WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0, 0.0), 40.0, 20.0);
    p1.isPlayer = true;
    for(int i = 0; i < 6; ++i)
    {
        Vector3 weed_origin(((i+1)*100.0), 0.0, 0.0);
        psystems.push_back(new ParticleSystemSpringMass(weed_origin));
    }

	srand(time(NULL));
	
	//creates random fish
    for(int i = 0; i < 7; ++i)
    {
        Player* newFish = new Player(Vector3(randDouble(50, 750), randDouble(50, 550), 0.0), randDouble(10, 30), randDouble(10, 30));
        Color4 color(randDouble(0, 1), randDouble(0, 1), randDouble(0, 1), 0);
        newFish->col = color;
        if(randDouble(0, 1) > 0.5)
            newFish->rotation(1);
        else
            newFish->vel = Vector3(randDouble(150, 250), randDouble(150, 250), 0.0);
        fish.push_back(newFish);
    }
    
	GLInit(&argc, argv);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}

void GLrunItAll()
{
    GLCollisions(p1);
    for(int i = 0; i < fish.size(); ++i)
        GLCollisions(*fish[i]);
    GLupdate();
    glutPostRedisplay();
}

void GLupdate()
{
	double dt = FRAME_RATE / 1000.0;
	updateParticleSystems(psystems, dt);
	cleanupParticleSystems(psystems);

	glutPostRedisplay();
    p1.update();
    for(int i = 0; i < fish.size(); ++i)
        fish[i]->update();
        
	//sleep is not effective in capturing constant time between frames because sleep
	//doesn't consider the time it takes for context-switching. However, this reduces
	//the cpu-usage. If accurate time frames are desire, use a time accumulator
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int diffTime = currentTime - previousTime;
	previousTime = currentTime;
	usleep(1000 * std::max(FRAME_RATE - diffTime, 0));
}

void GLrender()
{
	glClear(GL_COLOR_BUFFER_BIT); 
	for (int i = 0; i < psystems.size(); ++i)
	{
		psystems[i]->render();
    }
    for(int i = 0; i < fish.size(); ++i)
        fish[i]->render();
        
    p1.render();
        
	glFlush();	
	glutSwapBuffers();
}

void GLprocessMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		psystems.push_back(new ParticleSystemSpringMass(Vector3(x, WINDOW_HEIGHT - y, 0)));
	}
} 

//controls ball by incrementing velocity
void Keyboard(unsigned char key, int x, int y)
{
    if(key == 'w')
    { 
        p1.rotation(0);
        p1.vel.y += 10.0;
    }
    if(key == 's')
    {
        p1.rotation(0);
        p1.vel.y -= 10.0;
    }
    if(key == 'a')
    {
        p1.rotation(0);
        p1.vel.x -= 10.0;
    }
    if(key == 'd')
    {
        p1.rotation(0);
        p1.vel.x += 10.0;
    }
    if(key == 'f')
    {
        p1.rotation(0);
        p1.acc = p1.vel = Vector3();
    }
    if(key == 'o')
    {
        p1.rotation(1);
    }
}

void GLCollisions(Player& ball)
{
    double dt = FRAME_RATE / 1000.0;

	for(int i = 0; i < psystems.size(); ++i)
	{
	    ParticleSystemSpringMass* a = dynamic_cast<ParticleSystemSpringMass*>(psystems[i]);
	    
	    //applys force to each spring
		for(int j = 0; j < a->springConnections.size(); ++j)
		{
		    Particle* p1 = a->springConnections[j].particle1;
		    Particle* p2 = a->springConnections[j].particle2;
		    
		    //oscillates enviroment force applied to seaweed
		    if(ball.isPlayer)
		    {
		        int dir = 1;
		        if((int)currentTime % 2 == 0)
		            dir = -1;
		        p1->enviromentForce = Vector3(((int)currentTime % 4000) * dir * dt, 0.0, 0.0);
		        p2->enviromentForce = Vector3(((int)currentTime % 4000) * dir * dt, 0.0, 0.0);
		    }
		    //applys ball force to weeds if ball collides
		    if(ball.lineCollision(p1->pos, p2->pos))
		    {
    			p1->ballForce.push_back(ball.vel * ball.m * dt);
    			p2->ballForce.push_back(ball.vel * ball.m * dt);
    			ball.vel *= 0.9999;
    	    }
    	   //else
    	    //{
    			//p1->ballForce.push_back(Vector3());
    			//p2->ballForce.push_back(Vector3());
    	    //}
		}
	}
}

void setupScene()
{
    float ambient[] = { 0.5, 0.6, 0.8, 1.0 };
        // Fourth parameter if 0.0: <x,y,z> stand for directional components
        // otherwise, it is <x,y,z> is positional
    float position[] = { 0.0, WINDOW_HEIGHT, 0.0, 1.0 };                  

    // setup the light in the scene
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);      // Light decay rate
} 

