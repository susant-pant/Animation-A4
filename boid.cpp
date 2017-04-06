#include "boid.h"
#include <iostream>
#include <cstdlib>

float Boid::angleTo(Boid otherBoid){
	vec3 vecA = velocity;
	vec3 vecB = vec3(pos - otherBoid.pos);
	float numer = dot(vecA, vecB);
	float denom = length(vecA) * length(vecB);
	return (180.f * acos(numer/denom) / M_PI);
}

bool Boid::inLineOfSight(Boid otherBoid){
	return (angleTo(otherBoid) < FOV);
}

float Boid::distanceTo(Boid otherBoid){
	return length(vec3(otherBoid.pos - pos));
}

bool Boid::inVisibleRange(Boid otherBoid){
	return (distanceTo(otherBoid) < visibleRange);
}

bool Boid::inFlockRange(Boid otherBoid){
	return (distanceTo(otherBoid) < flockRange);
}

void Boid::setVelocity(vec3 newVel){
	velocity = newVel;
}

void Boid::updatePos(){
	vec3 newPos = pos + velocity;
	if (!(length(newPos) <= 50.f)) {
		velocity = -velocity;
	}
	pos += velocity/6.f;
}