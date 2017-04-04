#include "glm/glm.hpp"

using namespace glm;

class Boid{
public:
	vec3 pos;
	vec3 velocity;
	float FOV = 160.f;
	float visibleRange = 9.f; 	//distance at which other Boids are visible
	float flockRange = 3.f;	//distance at which a Boid considers its neighbours to be part of the same Flock

	Boid(){
		pos = vec3(float((rand() % 20) - 10), float((rand() % 20) - 10), float((rand() % 20) - 10));
		velocity = normalize(vec3(float((rand() % 20) - 10), float((rand() % 20) - 10), float((rand() % 20) - 10)));
	};

	float angleTo(Boid otherBoid);
	bool inLineOfSight(Boid otherBoid);
	float distanceTo(Boid otherBoid);
	bool inVisibleRange(Boid otherBoid);
	bool inFlockRange(Boid otherBoid);
	void setVelocity(vec3 newVel);
	void updatePos();
};