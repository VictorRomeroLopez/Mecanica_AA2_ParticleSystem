#include <stdlib.h>
#include <glm\gtc\matrix_transform.hpp>
#include <time.h>
#include <vector>
#include <math.h>
#include <algorithm>

float getModule(glm::vec3& vec);

void normalize(glm::vec3& vec);

glm::vec3 getNormal(const glm::vec3& vector1, const glm::vec3& vector2);

float getD(glm::vec3 normal, glm::vec3 point);

float dotProduct(glm::vec3 vec1, glm::vec3 vec2);

struct Collider {

	float bounceCoefficient = .2f;
	float nu = 1.f;
	virtual bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) = 0;
	virtual void getPlane(glm::vec3& normal, float& d) = 0;
	void computeCollision(const glm::vec3& old_pos, const glm::vec3& old_vel, glm::vec3& new_pos, glm::vec3& new_vel) {
		glm::vec3 normal;
		float d;

		getPlane(normal, d);

		new_pos = new_pos - (1 + bounceCoefficient) * (dotProduct(normal, new_pos) + d) * normal;
		new_vel = new_vel - (1 + bounceCoefficient) * (dotProduct(normal, new_vel)) * normal;
		new_vel -= nu * (old_vel - ((dotProduct(normal, old_vel)) * normal));
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
		float landa1 = (-b + sqrt(pow(b, 2) - 4 * a * c)) / 2 * a;
		float landa2 = (-b - sqrt(pow(b, 2) - 4 * a * c)) / 2 * a;
		glm::vec3 punt_tall;

		if(landa1 < landa2)
			punt_tall = old_pos + landa1 * v;
		else
			punt_tall = old_pos + landa2 * v;

		normal = punt_tall - center;
		normalize(normal);
		d = getD(normal, punt_tall);
	}
};

struct CapsuleCol : Collider {
	glm::vec3 center1;
	glm::vec3 center2;
	glm::vec3 Q;
	glm::vec3 old_pos;
	glm::vec3 new_pos;
	float radius;

	CapsuleCol(glm::vec3 _center1, glm::vec3 _center2, float _radius) :center1{ _center1 }, center2{ _center2 }, radius{ _radius }{};

	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {
		old_pos = prev_pos;
		new_pos = next_pos;
		float alpha = glm::clamp(( dotProduct((next_pos-center1), (center2-center1)) / getModule(center2 - center1)), 0.f, getModule(center2-center1));
		Q = center1 + alpha * (center2 - center1);
		return getModule(next_pos-Q) < radius;
	}

	void getPlane(glm::vec3& normal, float& d) {
		glm::vec3 punt_tall;
		glm::vec3 v = (new_pos - old_pos);
		v /= 2;
		glm::vec3 aux = old_pos + v;
		for (int i = 0; i < 5; i++) {
			
			float alpha = glm::clamp(((dotProduct((aux - center1), (center2 - center1))) / getModule(center2 - center1)), 0.f, getModule(center2 - center1));
			Q = center1 + alpha * (center2 - center1);
			
			if (getModule(aux - Q) > radius) 
			{
				v = old_pos - aux;
				v /= 2;
				new_pos = aux + v;
				v = new_pos - aux;
				v /= 2;
				aux += v;
			}
			else if(getModule(aux - Q) < radius) 
			{
				v = new_pos - aux;
				v /= 2;
				old_pos = aux + v;
				v = old_pos - aux;
				v /= 2;
				aux += v;
			}
		}

		punt_tall = aux;
		normal = punt_tall - Q;
		normalize(normal);
		d = getD(normal, punt_tall);
	}
};

struct ParticleSystem {
	glm::vec3 positions[5000];
	glm::vec3 velocity[5000];

	ParticleSystem() {
		for (int i = 0; i < 5000; i ++) {
			positions[i] = glm::vec3((float)((rand() % 9000) - 4500) / 1000, (float)((rand() % 40) + 40) / 10, (float)((rand() % 9000) - 4500) / 1000);
		}
		for (int i = 0; i < 5000; i ++) {
			//velocity[i] = glm::vec3((float)((rand() % 10)- 5), (float)((rand() % 10)-5), (float)((rand() % 10) - 5));
			velocity[i] = glm::vec3(0, 0, 0);
		}
	}
};

struct ForceActuator {
	virtual glm::vec3 computeForce(float mass, const glm::vec3& position) = 0;
};

//Gravetat general
struct GravityForce : ForceActuator {
	glm::vec3 gravity;

	GravityForce() :gravity{ glm::vec3(0, -9.81, 0) } {};

	glm::vec3 computeForce(float mass, const glm::vec3& position) {
		return gravity;
	}
}; 

//Centre de gravetat a un objecte 
struct PositionalGravityForce : ForceActuator {
	float massSphere = 3;
	float G = -0.6674f;
	glm::vec3 sphere_position = glm::vec3(0,0,0);

	glm::vec3 computeForce(float mass, const glm::vec3& position) {
		return ((G * mass * massSphere) / pow(getModule(position - sphere_position), 2)) * ((position - sphere_position) / getModule(position - sphere_position));
	}
};

void euler(float dt, ParticleSystem& particles, const std::vector<Collider*>& colliders, const std::vector<ForceActuator*>& force_acts);

glm::vec3 computeForces(float mass, const glm::vec3& position, const std::vector<ForceActuator*>& force_acts);