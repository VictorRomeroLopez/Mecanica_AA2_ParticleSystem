#include "../ParticleSystem.h"

namespace Box {
	void drawCube();
}
namespace Axis {
	void drawAxis();
}

namespace Sphere {
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
}

namespace Capsule {
	extern void updateCapsule(glm::vec3 posA, glm::vec3 posB, float radius = 1.f);
	extern void drawCapsule();
}
namespace Particles {
	extern const int maxParticles;
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}
namespace Mesh {
	extern const int numCols;
	extern const int numRows;
	extern void updateMesh(float* array_data);
	extern void drawMesh();
}
namespace Fiber {
extern const int numVerts;
	extern void updateFiber(float* array_data);
	extern void drawFiber();
}
namespace Cube {
	extern void updateCube(const glm::mat4& transform);
	extern void drawCube();
}

// Boolean variables allow to show/hide the primitives
bool renderSphere = true;
bool renderCapsule = true;
bool renderParticles = true;
bool renderMesh = false;
bool renderFiber = false;
bool renderCube = false;

//You may have to change this code
void renderPrims() {
	Box::drawCube();
	Axis::drawAxis();

	if (renderSphere)
		Sphere::drawSphere();
	if (renderCapsule)
		Capsule::drawCapsule();

	if (renderParticles) {
		int startDrawingFromParticle = 0;
		int numParticlesToDraw = 5000;
		Particles::drawParticles(startDrawingFromParticle, numParticlesToDraw);
	}

	if (renderMesh)
		Mesh::drawMesh();

	if (renderFiber)
		Fiber::drawFiber();

	if (renderCube)
		Cube::drawCube();
}

//Variables del GUI
bool play_simulation = false;
bool reset_simulation = false;
float bounceCoefficient = 0.0f;
float nu = 0.0f;
bool use_Gravity = false;
glm::vec3 gravity;


glm::vec3 SphereCenter(0,3,0);
float CurrentSphereRadius(1.0f);
SphereCol sphere(SphereCenter, CurrentSphereRadius);

extern bool use_Gravity;
extern glm::vec3 gravity;

float sphere_Mass = 1.0f;

glm::vec3 capsule_position_a(-3,2,0);
glm::vec3 capsule_position_b(0,2,-3);
float capsule_Radius = 1.0f;
glm::vec3 gravity_Accel;

ParticleSystem particleSystem = ParticleSystem();

std::vector<Collider*> colliders;
std::vector<ForceActuator*> forceActuators;

void GUI() {
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	// Do your GUI code here....
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
		ImGui::Checkbox("Play Simulation", &play_simulation);
		if (ImGui::Button("Reset Simulation"))
		{
			reset_simulation = true;
		}
		ImGui::DragFloat("Particle Mass", &particleSystem.particleMass, 0.1f, 0, 10.0f);
		
		if (ImGui::TreeNode("Elasticity and Friction"))
		{
			ImGui::DragFloat("Elastic Coeficient", &bounceCoefficient, 0.1f, 0, 1.0f);
			ImGui::DragFloat("Friction Coeficient", &nu, 0.1f, 0, 1.0f);
			ImGui::TreePop();
		}
	
		if (ImGui::TreeNode("Sphere"))
		{
			ImGui::Checkbox("Use Sphere Collider", &renderSphere);
			ImGui::DragFloat("Sphere Mass", &sphere_Mass);
			ImGui::DragFloat3("Sphere Position", &SphereCenter.x);
			ImGui::DragFloat("Sphere Radius", &CurrentSphereRadius);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Capsule"))
		{
			ImGui::Checkbox("Use Capsule Collider", &renderCapsule);
			ImGui::DragFloat3("Capsule Pos A", &capsule_position_a.x, 0.1f);
			ImGui::DragFloat3("Capsule Pos B", &capsule_position_b.x, 0.1f);
			ImGui::DragFloat("Capsule Radius", &capsule_Radius, 0.1f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Force"))
		{
			ImGui::Checkbox("Use Gravity", &use_Gravity);
			ImGui::DragFloat3("Gravity Accel", &gravity.x, 0.1f);
			ImGui::TreePop();
		}
		
	}
	// .........................

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	bool show_test_window = false;
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}


void PhysicsInit() {
	// Do your initialization code here...
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(5, 0, -5), glm::vec3(5, 10, -5), glm::vec3(-5, 10, -5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(-5, 0, 5), glm::vec3(-5, 0, -5), glm::vec3(5, 0, -5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(5, 0, -5), glm::vec3(5, 0, 5), glm::vec3(5, 10, 5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(-5, 0, 5), glm::vec3(5, 0, 5), glm::vec3(5, 10, 5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(-5, 0, -5), glm::vec3(-5, 10, -5), glm::vec3(-5, 0, 5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(-5, 10, 5), glm::vec3(-5, 10, -5), glm::vec3(5, 10, -5)}));
	if(renderSphere)
		colliders.push_back(new SphereCol(SphereCenter, CurrentSphereRadius));
	if(renderCapsule)
		colliders.push_back(new CapsuleCol(capsule_position_a, capsule_position_b, capsule_Radius));

	forceActuators.push_back(new GravityForce());
	forceActuators.push_back(new PositionalGravityForce());
	
	// ...................................
}

void PhysicsUpdate(float dt) {
	// Do your update code here...
	if(renderSphere)
		colliders.pop_back();
	if(renderCapsule)
		colliders.pop_back();
	if(renderSphere)
		colliders.push_back(new SphereCol(SphereCenter, CurrentSphereRadius));
	if(renderCapsule)
		colliders.push_back(new CapsuleCol(capsule_position_a, capsule_position_b, capsule_Radius));

	Sphere::updateSphere(SphereCenter, CurrentSphereRadius);
	Capsule::updateCapsule(capsule_position_a, capsule_position_b, capsule_Radius);

	Particles::updateParticles(0, NUM_PARTICLES, &particleSystem.positions[0].x);
	euler(dt, particleSystem, colliders, forceActuators);

	if (reset_simulation)
		particleSystem.ResetParticlesPosition();
	// ...........................
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
	
}