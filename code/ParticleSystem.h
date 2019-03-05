#include <stdlib.h>
#include <glm\gtc\matrix_transform.hpp>
#include <time.h>
#include <vector>
/*
struct Collider {
	virtual bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& newxt_pos) = 0;
	virtual void getPlane(glm::vec3& normal, float& d) = 0;
	void computeCollision(const glm::vec3& old_pos, const glm::vec3& old_vel, glm::vec3& new_pos, glm::vec3& new_vel) {
		//...
	}
};
*/

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

	void update(float dt) {
		for (int i = 0; i < 5000; i += 3) {
			positions[i] += velocity[i] * dt;
			velocity[i] += acceleration * dt;
		}
	}
};
/*
void euler(float dt, ParticleSystem& particles, const std::vector<Collider*>& colliders,	 )*/