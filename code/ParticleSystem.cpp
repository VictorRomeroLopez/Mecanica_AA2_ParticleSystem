#include "ParticleSystem.h"

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
	return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

glm::vec3 computeForces(float mass, const glm::vec3& position, const std::vector<ForceActuator*>& force_acts) {
	glm::vec3 aux = glm::vec3(0, 0, 0);
	for (int i = 0; i < force_acts.size(); i++) {
		aux += force_acts[i]->computeForce(mass, position);
	}
	return aux;
}

void euler(float dt, ParticleSystem& particles, const std::vector<Collider*>& colliders, const std::vector<ForceActuator*>& force_acts) {
	for (int j = 0; j < 5000; j++) {
		for (int i = 0; i < colliders.size(); i++) {
			if (colliders[i]->checkCollision(particles.positions[j], particles.positions[j] + dt * particles.velocity[j])) {
				colliders[i]->computeCollision(particles.positions[j], particles.velocity[j], particles.positions[j] + dt * particles.velocity[j], particles.velocity[j]);
			}
		}
		particles.positions[j] += dt * particles.velocity[j];
		particles.velocity[j] += dt * computeForces(1, particles.positions[j], force_acts);
	}
}