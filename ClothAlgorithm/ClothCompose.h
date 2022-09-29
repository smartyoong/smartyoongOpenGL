#pragma once

#include<vector>

#define DAMPING 0.01 
#define TIME_STEPSIZE2 0.5*0.5 
#define LIENS_ITERATIONS 15


class Particle {
private:
	bool movable;
	float mass;
	Vec3 pos;
	Vec3 old_pos;
	Vec3 acceleration;
	Vec3 accumulated_normal;

public:
	Particle(Vec3 pos) : pos(pos), old_pos(pos), acceleration(Vec3(0, 0, 0)), mass(1), movable(true), accumulated_normal(Vec3(0, 0, 0)) {}
	Particle() {}

	void addForce(Vec3 f) {
		acceleration += f / mass;
	}

	/*verlet 계산*/
	void timeStep() {
		if (movable)
		{
			Vec3 temp = pos;
			pos = pos + (pos - old_pos) * (1.0 - DAMPING) + acceleration * TIME_STEPSIZE2;
			old_pos = temp;
			acceleration = Vec3(0, 0, 0);
		}
	}

	Vec3& getPos() {
		return pos;
	}

	void resetAcceleration() {
		acceleration = Vec3(0, 0, 0);
	}

	void offsetPos(const Vec3 v) {
		if (movable) pos += v;
	}

	void makeUnmovable() {
		movable = false;
	}

	void makeMovable() {
		movable = true;
	}

	void addToNormal(Vec3 normal) {
		accumulated_normal += normal.normalized();
	}

	Vec3& getNormal() {
		return accumulated_normal;
	}

	void resetNormal() {
		accumulated_normal = Vec3(0, 0, 0);
	}

};

class Line {
private:
	float rest_distance;

public:
	Particle* p1, * p2;

	Line(Particle* p1, Particle* p2) : p1(p1), p2(p2) { // 두 개의 결합
		Vec3 vec = p1->getPos() - p2->getPos();
		rest_distance = vec.length();
	}
	void lienPossible() {
		Vec3 p1_to_p2 = p2->getPos() - p1->getPos();
		float current_distance = p1_to_p2.length();
		Vec3 correctionVector = p1_to_p2 * (1 - rest_distance / current_distance);
		Vec3 correctionVectorHalf = correctionVector * 0.5;
		p1->offsetPos(correctionVectorHalf);
		p2->offsetPos(-correctionVectorHalf);
	}

};

class Cloth {
private:
	std::vector<Particle> particles;
	std::vector<Line> liens;
	int nb_particles_large; // 너비
	int nb_particles_hauteur; //높이
	Particle* getParticle(int x, int y) {
		return &particles[y * nb_particles_large + x];
	}
	void BombLine(Particle* p1, Particle* p2) {
		liens.push_back(Line(p1, p2));
	}
	Vec3 calcTriangleNormal(Particle* p1, Particle* p2, Particle* p3) {
		Vec3 pos1 = p1->getPos();
		Vec3 pos2 = p2->getPos();
		Vec3 pos3 = p3->getPos();

		Vec3 v1 = pos2 - pos1;
		Vec3 v2 = pos3 - pos1;

		return v1.cross(v2);
	}
	void addWindForcesForTriangle(Particle* p1, Particle* p2, Particle* p3, const Vec3 direction) {
		Vec3 normal = calcTriangleNormal(p1, p2, p3);
		Vec3 d = normal.normalized();
		Vec3 force = normal * (d.dot(direction));
		p1->addForce(force);
		p2->addForce(force);
		p3->addForce(force);
	}
	void drawTriangle(Particle* p1, Particle* p2, Particle* p3, const Vec3 color) {
		glColor3fv((float*)&color);

		Vec3 v1 = p1->getNormal().normalized();
		glNormal3fv((float*)&v1);
		glVertex3fv((float*)&(p1->getPos()));

		Vec3 v2 = p2->getNormal().normalized();
		glNormal3fv((float*)&v2);
		glVertex3fv((float*)&(p2->getPos()));

		Vec3 v3 = p3->getNormal().normalized();
		glNormal3fv((float*)&v3);
		glVertex3fv((float*)&(p3->getPos()));
	}

public:

	/*천을 생성하고 일일히 연결하는 용*/
	Cloth(float large, float hauteur, int nb_particles_large, int nb_particles_hauteur) : nb_particles_large(nb_particles_large), nb_particles_hauteur(nb_particles_hauteur) {
		particles.resize(nb_particles_large * nb_particles_hauteur);
		for (int x = 0; x < nb_particles_large; x++) {
			for (int y = 0; y < nb_particles_hauteur; y++) {
				Vec3 pos = Vec3(large * (x / (float)nb_particles_large), hauteur * (y / (float)nb_particles_hauteur), 0);
			particles[y * nb_particles_large + x] = Particle(pos);
			}
		}
		for (int x = 0; x < nb_particles_large; x++) {
			for (int y = 0; y < nb_particles_hauteur; y++) {
				if (x < nb_particles_large - 1) BombLine(getParticle(x, y), getParticle(x + 1, y));
				if (y < nb_particles_hauteur - 1) BombLine(getParticle(x, y), getParticle(x, y + 1));
				if (x < nb_particles_large - 1 && y < nb_particles_hauteur - 1) BombLine(getParticle(x, y), getParticle(x + 1, y + 1));
				if (x < nb_particles_large - 1 && y < nb_particles_hauteur - 1) BombLine(getParticle(x + 1, y), getParticle(x, y + 1));
			}
		}

		for (int x = 0; x < nb_particles_large; x++) {
			for (int y = 0; y < nb_particles_hauteur; y++) {
				if (x < nb_particles_large - 2) BombLine(getParticle(x, y), getParticle(x + 2, y));
				if (y < nb_particles_hauteur - 2) BombLine(getParticle(x, y), getParticle(x, y + 2));
				if (x < nb_particles_large - 2 && y < nb_particles_hauteur - 2) BombLine(getParticle(x, y), getParticle(x + 2, y + 2));
				if (x < nb_particles_large - 2 && y < nb_particles_hauteur - 2) BombLine(getParticle(x + 2, y), getParticle(x, y + 2));
			}
		}
		for (int i = nb_particles_large / 2.5; i < nb_particles_large; i++) {
			getParticle(0 + i, 0)->offsetPos(Vec3(0.5, 0.0, 0.0));
			getParticle(0 + i, 0)->makeUnmovable();
		}




	}
	void drawShaded() {
		/*계속 변하는 값들을 다시 초기화 해주는용*/
		std::vector<Particle>::iterator particle;
		for (particle = particles.begin(); particle != particles.end(); particle++) {
			(*particle).resetNormal();
		}
		for (int x = 0; x < nb_particles_large - 1; x++) {
			for (int y = 0; y < nb_particles_hauteur - 1; y++) {
				Vec3 normal = calcTriangleNormal(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1));
				getParticle(x + 1, y)->addToNormal(normal);
				getParticle(x, y)->addToNormal(normal);
				getParticle(x, y + 1)->addToNormal(normal);

				normal = calcTriangleNormal(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1));
				getParticle(x + 1, y + 1)->addToNormal(normal);
				getParticle(x + 1, y)->addToNormal(normal);
				getParticle(x, y + 1)->addToNormal(normal);
			}
		}

		glBegin(GL_TRIANGLES);
		for (int x = 0; x < nb_particles_large - 1; x++) {
			for (int y = 0; y < nb_particles_hauteur - 1; y++) {
				Vec3 color(0, 0, 0);

				color = Vec3(0.69f, 0.13f, 0.13f);

				drawTriangle(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1), color);
				drawTriangle(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1), color);
			}
		}
		glEnd();
	}
	void timeStep() {
		std::vector<Line>::iterator line;
		for (int i = 0; i < LIENS_ITERATIONS; i++) {
			for (line = liens.begin(); line != liens.end(); line++) {
				(*line).lienPossible();
			}
		}

		std::vector<Particle>::iterator particule;
		for (particule = particles.begin(); particule != particles.end(); particule++) {
			(*particule).timeStep();
		}
	}

	void addForce(const Vec3 direction) {
		std::vector<Particle>::iterator particle;
		for (particle = particles.begin(); particle != particles.end(); particle++) {
			(*particle).addForce(direction);
		}

	}
	void windForce(const Vec3 direction) {
		for (int x = 0; x < nb_particles_large - 1; x++) {
			for (int y = 0; y < nb_particles_hauteur - 1; y++) {
				addWindForcesForTriangle(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1), direction);
				addWindForcesForTriangle(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1), direction);
			}
		}
	}
	/*아래는 충돌관련 함수, 천과 물체의 위치를 비교합니다*/
	void ballCollision(const Vec3 center, const float radius) {
		std::vector<Particle>::iterator particle;
		for (particle = particles.begin(); particle != particles.end(); particle++) {
			Vec3 v = (*particle).getPos() - center;
			float l = v.length();
			if (l < radius) {
				(*particle).offsetPos(v.normalized() * (radius - l));
			}
		}
	}

	void cubeCollision(const Vec3 center, const float cube_size, const Vec3 cube_pos) {
		std::vector<Particle>::iterator particle;
		for (particle = particles.begin(); particle != particles.end(); particle++) {
			Vec3 v = (*particle).getPos() - cube_pos;
			float l = v.distanceCube();
			if (l < (cube_size)) {
				(*particle).offsetPos(v.normalized() * (cube_size - l));
			}
		}
	}

	void doFrame() {

	}
};
