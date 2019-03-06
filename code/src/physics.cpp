#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\matrix_transform.hpp>
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
bool renderCapsule = false;
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
bool use_Sphere_Collider = true;
bool use_Capsule_Collider = true;
bool use_Gravity = true;
float particle_Mass = 1.0f;
float elastic_coeficient = 1.0f;
float friction_coeficient = 1.0f;
float sphere_Mass = 1.0f;
float sphere_Radius = 1.0f;
glm::vec3 sphere_position;
glm::vec3 capsule_position_a;
glm::vec3 capsule_position_b;
float capsule_Radius = 1.0f;
glm::vec3 gravity_Accel;


void GUI() {
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	// Do your GUI code here....
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
		ImGui::Checkbox("Play Simulation", &play_simulation);
		ImGui::Button("Reset Simulation");
		ImGui::DragFloat("Particle Mass", &particle_Mass);
		ImGui::BulletText("E L A S T I C I T Y  A N D  F R I C T I O N");
		ImGui::DragFloat("Elastic Coeficient", &elastic_coeficient);
		ImGui::DragFloat("Friction Coeficient", &friction_coeficient);
		ImGui::BulletText("C O L L I D E R S");
		ImGui::Checkbox("Use Sphere Collider", &use_Sphere_Collider);
		ImGui::DragFloat("Sphere Mass", &sphere_Mass);
		ImGui::DragFloat3("Sphere Position", &sphere_position.x);
		ImGui::DragFloat("Sphere Radius", &sphere_Radius);
		ImGui::Checkbox("Use Capsule Collider", &use_Capsule_Collider);
		ImGui::DragFloat3("Capsule Pos A", &capsule_position_a.x);
		ImGui::DragFloat3("Capsule Pos B", &capsule_position_b.x);
		ImGui::DragFloat("Capsule Radius", &capsule_Radius);
		ImGui::BulletText("F O R C E S");
		ImGui::Checkbox("Use Gravity", &use_Gravity);
		ImGui::DragFloat3("Gravity Accel", &gravity_Accel.x);
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

ParticleSystem particleSystem = ParticleSystem();
std::vector<Collider*> colliders;
std::vector<ForceActuator*> forceActuators;

void PhysicsInit() {
	// Do your initialization code here...
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(5, 0, -5), glm::vec3(5, 10, -5), glm::vec3(-5, 10, -5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(-5, 0, 5), glm::vec3(-5, 0, -5), glm::vec3(5, 0, -5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(5, 0, -5), glm::vec3(5, 0, 5), glm::vec3(5, 10, 5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(-5, 0, 5), glm::vec3(5, 0, 5), glm::vec3(5, 10, 5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(-5, 0, -5), glm::vec3(-5, 10, -5), glm::vec3(-5, 0, 5)}));
	colliders.push_back(new PlaneCol(std::vector<glm::vec3>{glm::vec3(-5, 10, 5), glm::vec3(-5, 10, -5), glm::vec3(5, 10, -5)}));
	colliders.push_back(new SphereCol(glm::vec3(0, 5, 0), 1));
	forceActuators.push_back(new GravityForce());
	Sphere::updateSphere(glm::vec3(0, 5, 0), 1);
	// ...................................
}

void PhysicsUpdate(float dt) {
	// Do your update code here...
	Particles::updateParticles(0, 5000, &particleSystem.positions[0].x);
	euler(dt, particleSystem, colliders , forceActuators);
	// ...........................
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}