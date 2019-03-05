#include <stdlib.h>
#include <glm\gtc\matrix_transform.hpp>
#include <time.h>
#include <vector>
#include <math.h>

double getModule(glm::vec3 vec) {
	return sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
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

struct Collider {
	virtual bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) = 0;
	virtual void getPlane(glm::vec3& normal, float& d) = 0;
	void computeCollision(const glm::vec3& old_pos, const glm::vec3& old_vel, glm::vec3& new_pos, glm::vec3& new_vel) {
		glm::vec3 normal;
		float d;

		getPlane(normal, d);
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

	}

	void getPlane(glm::vec3& normal, float& d) {
		getNormal(vector1, vector2);
		getD(normal, point);
	}
};

struct SphereCol : Collider {
	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {

	}
	void getPlane(glm::vec3& normal, float& d) {

	}
};

struct CapsuleCol : Collider {
	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {

	}

	void getPlane(glm::vec3& normal, float& d) {

	}
};

struct ParticleSystem {
	glm::vec3 positions[5000];
	glm::vec3 velocity[5000];
	glm::vec3 acceleration;

	ParticleSystem() {
		for (int i = 0; i < 5000; i += 3) {
			positions[i] = glm::vec3((float)((rand() % 101) - 50) / 10, (float)((rand() % 50) + 50) / 10, (float)((rand() % 101) - 50) / 10);
		}
		for (int i = 0; i < 5000; i += 3) {
			velocity[i] = glm::vec3((float)(rand() % 10)-5, (float)(rand() % 10)-5 , (float)(rand() % 10)-5);
		}
		acceleration = glm::vec3(0, -9.81, 0);
	}
};

struct ForceActuator {
	virtual glm::vec3 computeForce(float mass, const glm::vec3& position) = 0;
};

//Gravetat general
struct GravityForce : ForceActuator {
	glm::vec3 computeForces(float mass, const glm::vec3& position, const std::vector<ForceActuator*>& force_acts) {

	}
}; 

//Centre de gravetat a un objecte 
struct PositionalGravityForce : ForceActuator {
	glm::vec3 computeForces(float mass, const glm::vec3& position, const std::vector<ForceActuator*>& force_acts) {

	}
};

void euler(float dt, ParticleSystem& particles, const std::vector<Collider*>& colliders, const std::vector<ForceActuator*>& force_acts) {

}