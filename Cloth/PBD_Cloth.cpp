#include "PBD_Cloth.h"
#include <time.h>

PBD_Cloth::PBD_Cloth()
{
	srand(time(NULL));
}

PBD_Cloth::PBD_Cloth(int w, int h)
{
	_res[0] = w;
	_res[1] = h;
	int size = _res[0] * _res[1];

	_pos.resize(size);
	_pos1.resize(size);
	_vel.resize(size);
	_invMass.resize(size);

	init();
}

PBD_Cloth::~PBD_Cloth()
{
}

void PBD_Cloth::init(void)
{
	for (int i = 0; i < _res[0]; i++) {
		for (int j = 0; j < _res[1]; j++) {
			int index = j * _res[0] + i;
			_invMass[index] = 1.0;
			_pos[index].Set(2.0 * i / (double)_res[0], 0.0, 2.0 * j / (double)_res[1]);
		}
	}
}

void PBD_Cloth::simulation(double dt)
{
	Vec3<double> gravity (0.0, -9.8, 0.0);
	double damping = 0.99;
	double horiStrutlength0 = 2.0 / (double)_res[0];
	double vertStrutlength0 = 2.0 / (double)_res[1];

	// Apply external force
	for (int i = 0; i < _res[0]; i++) {
		for (int j = 0; j < _res[1]; j++) {
			int index = j * _res[0] + i;
			_vel[index] += gravity * dt * _invMass[index];
			_vel[index] *= damping;
			_pos1[index] = _pos[index] + (_vel[index] * dt);
		}
	}

	// Constraint projection
	int iter = 5;
	for (int k = 0; k < iter; k++) {
		// Vertical and horizontal structural springs
		for (int i = 0; i < _res[1]; i++) {
			for (int j = 0; j < _res[0] - 1; j++) {
				int index0 = j * _res[0] + i;
				int index1 = (j + 1) * _res[0] + i;
				double c_p1p2 = (_pos1[index0] - _pos1[index1]).Length() - horiStrutlength0;
				Vec3<double> dp1 = (_pos1[index0] - _pos1[index1]);
				Vec3<double> dp2 = (_pos1[index0] - _pos1[index1]);
				dp1.Normalize();
				dp2.Normalize();
				dp1 *= -_invMass[index0] / (_invMass[index0] + _invMass[index1]) * c_p1p2;
				dp2 *= _invMass[index1] / (_invMass[index0] + _invMass[index1]) * c_p1p2;
				_pos1[index0] += dp1;
				_pos1[index1] += dp2;
			}
		}
		for (int i = 0; i < _res[1] - 1; i++) {
			for (int j = 0; j < _res[0]; j++) {
				int index0 = j * _res[0] + i;
				int index1 = j * _res[0] + (i + 1);
				float c_p1p2 = (_pos1[index0] - _pos1[index1]).Length() - vertStrutlength0;
				Vec3<double> dp1 = _pos1[index0] - _pos1[index1];
				Vec3<double> dp2 = _pos1[index0] - _pos1[index1];
				dp1.Normalize();
				dp2.Normalize();
				dp1 *= -_invMass[index0] / (_invMass[index0] + _invMass[index1]) * c_p1p2;
				dp2 *= _invMass[index1] / (_invMass[index0] + _invMass[index1]) * c_p1p2;
				_pos1[index0] += dp1;
				_pos1[index1] += dp2;
			}
		}
	}

	// Integrate
	for (int i = 1; i < _res[1]; i++) {
		for (int j = 0; j < _res[0]; j++) {
			int index = j * _res[0] + i;
			_vel[index] = (_pos1[index] - _pos[index]) / dt;
			_pos[index] = _pos1[index];
		}
	}
}
 
void PBD_Cloth::wind(double dt)
{
	Vec3<double> windVector(-50, 0.0, 0.0);
	double damping = 0.99;
	
	for (int i = 0; i < 1000; i++)
	{
		int index = rand() % (_res[0] * _res[1]);
		_vel[index] += windVector * dt * _invMass[index];
		_vel[index] *= damping;
		_pos1[index] = _pos[index] + _vel[index] * dt;
	}
}

void PBD_Cloth::draw(void)
{
	glDisable(GL_LIGHTING);
	for (int i = 0; i < _res[0] - 1; i++) {
		for (int j = 0; j < _res[1] - 1; j++) {
			auto p00 = pos(i, j);
			auto p10 = pos(i + 1, j);
			auto p11 = pos(i + 1, j + 1);
			auto p01 = pos(i, j + 1);
			int c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			glColor3f((float)c, (float)c, (float)c);
			glBegin(GL_QUADS);
			glVertex3f(p00.x(), p00.y(), p00.z());
			glVertex3f(p10.x(), p10.y(), p10.z());
			glVertex3f(p11.x(), p11.y(), p11.z());
			glVertex3f(p01.x(), p01.y(), p01.z());
			glEnd();
		}
	}
	drawOutline();
}

void PBD_Cloth::drawOutline(void)
{
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < _res[0] - 1; i++) {
		auto p0 = pos(i, 0);
		auto p1 = pos(i + 1, 0);
		auto p2 = pos(i, _res[1] - 1);
		auto p3 = pos(i + 1, _res[1] - 1);
		auto p4 = pos(0, i);
		auto p5 = pos(0, i + 1);
		auto p6 = pos(_res[1] - 1, i);
		auto p7 = pos(_res[1] - 1, i + 1);
		glVertex3f(p0.x(), p0.y(), p0.z());
		glVertex3f(p1.x(), p1.y(), p1.z());
		glVertex3f(p2.x(), p2.y(), p2.z());
		glVertex3f(p3.x(), p3.y(), p3.z());
		glVertex3f(p4.x(), p4.y(), p4.z());
		glVertex3f(p5.x(), p5.y(), p5.z());
		glVertex3f(p6.x(), p6.y(), p6.z());
		glVertex3f(p7.x(), p7.y(), p7.z());
	}
	glEnd();
	glLineWidth(1.0f);
}