#pragma once

//std
#include <functional>

//Math
#include "Math/inc/Linear/Vec3.hpp"
#include "Math/inc/Linear/Quat.hpp"
#include "Math/inc/Linear/Mat3.hpp"

class RigidBody
{
public:
	//constructor
	RigidBody(void);

	//destructor
	virtual ~RigidBody(void);

	//solver
	void solve(void);
	virtual void setup(void);
	virtual void record(void);
	virtual void finish(void);

	//analysis
	math::Mat3 inertia(void) const;
	math::Mat3 damping(void) const;
	math::Mat3 stiffness(void) const;

	//data
	double m_M;
	double m_dt;
	math::Mat3 m_J;
	unsigned m_step;
	unsigned m_steps;
	unsigned m_iteration;
	unsigned m_iteration_max;

	char m_label[200];
	double* m_state_data;
	double* m_energy_data;
	double* m_velocity_data;
	double* m_acceleration_data;

	double m_state_old[4];
	double m_state_new[4];
	double m_velocity_old[3];
	double m_velocity_new[3];
	double m_acceleration_old[3];
	double m_acceleration_new[3];

	std::function<math::Vec3(double, math::Quat)> m_me;
	std::function<math::Mat3(double, math::Quat)> m_Ke;
};