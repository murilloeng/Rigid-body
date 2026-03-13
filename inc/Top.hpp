#pragma once

//Rigid-body
#include "Rigid-body/inc/RigidBody.hpp"

class Top : public RigidBody
{
public:
	//constructors
	Top(void);

	//destructor
	~Top(void);

	//solver
	void setup(void) override;
	void record(void) override;
	void finish(void) override;

	//stability
	double reference_velocity(void) const;

	bool stability_vertical(double) const;
	static bool stability_vertical(double, double, double);

	bool stability_tilted(uint32_t, double) const;
	static bool stability_tilted(uint32_t, double, double, double);

	//data
	double m_l;
	double* m_position_data;
};