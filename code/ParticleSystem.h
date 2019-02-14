#include <stdlib.h>
#include <time.h>

struct Vec3 {
	float coordenates[3];

	Vec3() {
		coordenates[0] = 0;
	}

	Vec3(float x, float y, float z) {
		coordenates[0] = x;
		coordenates[1] = y;
		coordenates[2] = z;
	}

	float getX() { return coordenates[0]; }
	//void setX(float value) { coordenates[0] = value; }
	float getY() { return coordenates[1]; }
	//void setX(float value) { coordenates[1] = value; }
	float getZ() { return coordenates[2]; }
	//void setX(float value) { coordenates[2] = value; }

};

struct Particle {
	Vec3 position;
	
	Particle() {
		position = Vec3(0, 0, 0);
	}

	Particle(float x, float y, float z) {
		position = Vec3(x, y, z);
	};
};

struct ParticleSystem {
	int size;
	Particle particles[5000];

	ParticleSystem() {
		size = 0;
		particles[0] = Particle();
	}

	ParticleSystem(int _size, int height) {
		size = _size;
		for (int i = 0; i < _size; i++) {
			particles[i] = Particle(((float)((rand()%101)-50)/10), (float)((rand()%50)+50)/10,((float)((rand()%101)-50)/10));
		}
	}

	void updateParticles(float dt) {
		for (int i = 0; i < 5000; i++) {
			if(particles[i].position.getY() - (float)(9.81*dt) > 0)
				particles[i] = Particle(particles[i].position.getX(), particles[i].position.getY() - (9.81*dt), particles[i].position.getZ());
			else if(particles[i].position.getY() - (float)(9.81*dt) < 0)
				particles[i] = Particle(particles[i].position.getX(), 0, particles[i].position.getZ());
		}
	}
};