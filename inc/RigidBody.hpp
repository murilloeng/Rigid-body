#pragma once

//std
#include <functional>

//Math
#include "Math/Math/inc/linear/vec3.hpp"
#include "Math/Math/inc/linear/quat.hpp"
#include "Math/Math/inc/linear/mat3.hpp"

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
	math::mat3 inertia(void) const;
	math::mat3 damping(void) const;
	math::mat3 stiffness(void) const;

	//data
	double m_M;
	double m_dt;
	math::mat3 m_J;
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

	std::function<math::vec3(double, math::quat)> m_me;
	std::function<math::mat3(double, math::quat)> m_Ke;
};