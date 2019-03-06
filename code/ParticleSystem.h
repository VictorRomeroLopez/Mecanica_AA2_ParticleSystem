#include <stdlib.h>
#include <glm\gtc\matrix_transform.hpp>
#include <time.h>
#include <vector>
#include <math.h>

float getModule(glm::vec3& vec) {
	return sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
}

void normalize(glm::vec3& vec) {
	vec /= getModule(vec);
}

glm::vec3 getNormal(const glm::vec3& vector1, const glm::vec3& vector2) {

	glm::vec3 normal;

	normal.x = vector1.y * vector2.z + vector2.y * vector1.z;
	normal.y = -(vector1.x * vector2.z + vector2.x * vector1.z);
	normal.z = vector1.x * vector2.y + vector2.x * vector1.y;

	return normal;
}

float getD(glm::vec3 normal, glm::vec3 point) {
	return -(normal.x * point.x + normal.y * point.y + normal.z * point.z);
}

float dotProduct(glm::vec3 vec1, glm::vec3 vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

struct Collider {

	float bounceCoefficient;
	virtual bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) = 0;
	virtual void getPlane(glm::vec3& normal, float& d) = 0;
	void computeCollision(const glm::vec3& old_pos, const glm::vec3& old_vel, glm::vec3& new_pos, glm::vec3& new_vel) {
		glm::vec3 normal;
		float d;

		getPlane(normal, d);

		new_pos = new_pos - (1 + bounceCoefficient)*(dotProduct(normal, new_pos) + d) * normal;
		new_vel = new_vel - (2)*(dotProduct(normal, new_vel)) * normal;
	}
};

struct PlaneCol : Collider {

	glm::vec3 point;
	glm::vec3 vector1;
	glm::vec3 vector2;

	PlaneCol(std::vector<glm::vec3> points) {
		point = points[0];

		vector1 = points[1] - points[0];
		vector2 = points[2] - points[0];
	}

	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {
		glm::vec3 normal;
		float d;
		getPlane(normal, d);

		float value_prev_pos;
		float value_next_pos;

		value_prev_pos = normal.x * prev_pos.x + normal.y * prev_pos.y + normal.z * prev_pos.z + d;
		value_next_pos = normal.x * next_pos.x + normal.y * next_pos.y + normal.z * next_pos.z + d;

		return (value_prev_pos <= 0 && value_next_pos > 0) || (value_prev_pos >= 0 && value_next_pos < 0);
	}

	void getPlane(glm::vec3& normal, float& d) {
		normal = getNormal(vector1, vector2);
		normalize(normal);
		d = getD(normal, point);
	}
};

struct SphereCol : Collider {
	
	glm::vec3 old_pos;
	glm::vec3 new_pos;
	glm::vec3 center;
	float radius;

	SphereCol(glm::vec3 _center, float _radius) : center{ _center }, radius{ _radius }, old_pos{ glm::vec3{0,0,0} }, new_pos{ glm::vec3{0,0,0} }{ };

	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {
		old_pos = prev_pos;
		new_pos = next_pos;
		return getModule(next_pos - center) < radius && getModule(prev_pos - center) > radius;
	}

	void getPlane(glm::vec3& normal, float& d) {
		glm::vec3 v = new_pos - old_pos;
		float a = dotProduct(v, v);
		float b = 2 * dotProduct(old_pos, v) + 2 * dotProduct(v, center);
		float c = dotProduct(old_pos, old_pos) - 2 * dotProduct(old_pos, center) + dotProduct(center, center);
		float landa = (-b - sqrt(pow(b, 2) - 4 * a * c)) / 2 * a;

		glm::vec3 punt_tall = old_pos * landa;
		normal = punt_tall - center;
		normalize(normal);
		d = getD(normal, punt_tall);
	}
};

/*
struct CapsuleCol : Collider {
	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {

	}

	void getPlane(glm::vec3& normal, float& d) {

	}
};*/

struct ParticleSystem {
	glm::vec3 positions[5000];
	glm::vec3 velocity[5000];

	ParticleSystem() {
		for (int i = 0; i < 5000; i += 3) {
			positions[i] = glm::vec3((float)((rand() % 101) - 50) / 10, (float)((rand() % 50) + 50) / 10, (float)((rand() % 101) - 50) / 10);
		}
		for (int i = 0; i < 5000; i += 3) {
			velocity[i] = glm::vec3((float)(rand() % 10)-5, (float)(rand() % 10)-5 , (float)(rand() % 10)-5);
		}
	}
};



struct ForceActuator {
	virtual glm::vec3 computeForce(float mass, const glm::vec3& position) = 0;
};

glm::vec3 computeForces(float mass, const glm::vec3& position, const std::vector<ForceActuator*>& force_acts) {
	glm::vec3 aux = glm::vec3(0,0,0);
	for (int i = 0; i < force_acts.size(); i++) {
		aux += force_acts[i]->computeForce(mass, position);
	}
	return aux;
}

//Gravetat general
struct GravityForce : ForceActuator {
	glm::vec3 gravity = glm::vec3(0, -9.81, 0);

	glm::vec3 computeForce(float mass, const glm::vec3& position) {
		return gravity;
	}
}; 

//Centre de gravetat a un objecte 
struct PositionalGravityForce : ForceActuator {
	float massSphere = 3;
	float G = 0.6674f;
	glm::vec3 sphere_position = glm::vec3(0,0,0);

	glm::vec3 computeForce(float mass, const glm::vec3& position) {
		return ((G * mass * massSphere) / pow(getModule(position - sphere_position), 2)) * ((position - sphere_position) / getModule(position - sphere_position));
	}
};

void euler(float dt, ParticleSystem& particles, const std::vector<Collider*>& colliders, const std::vector<ForceActuator*>& force_acts) {

	for (int i = 0; i < colliders.size(); i++) {
		for (int j = 0; j < particles.positions->length(); j++) {
			if (colliders[i]->checkCollision(particles.positions[j], particles.positions[j] + dt * particles.velocity[j])){
				colliders[i]->computeCollision(particles.positions[j], particles.velocity[j], particles.positions[j], particles.velocity[j]);
			}
			else {
				computeForces(1, particles.positions[j], force_acts);
			}

		}
	}

}