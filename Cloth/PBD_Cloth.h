#ifndef __PBD_CLOTH_H__
#define __PBD_CLOTH_H__

#pragma once
#include "Vec3.h"
#include <vector>
#include "GL/glut.h"

using namespace std;

typedef Vec3<double> vec3;

class PBD_Spring
{
public:
	int		_particle[2];
	double	_length0;
	double	_ks;
	double	_kd;
};

class PBD_Cloth
{
public:
	int					_res[2]; // width and height
	vector<double>		_invMass;
	vector<vec3>		_pos;
	vector<vec3>		_pos1; // new pos.
	vector<vec3>		_vel;
	vector<PBD_Spring>	_springs;
public:
	PBD_Cloth();
	PBD_Cloth(int w, int h);
	~PBD_Cloth();
public:
	inline vec3	pos(int i, int j) { return _pos[j * _res[0] + i]; }
public:
	void	init(void);
	void	simulation(double dt);
	void	wind(double dt);
public:
	void	draw(void);
	void	drawOutline(void);
};

#endif

