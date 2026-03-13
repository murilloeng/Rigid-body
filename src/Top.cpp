//std
#include <cmath>
#include <cstdio>

//Rigid
#include "Rigid-body/inc/Top.hpp"

//constructors
Top::Top(void) : m_position_data(nullptr)
{
	return;
}

//destructor
Top::~Top(void)
{
	delete[] m_position_data;
}

//solver
void Top::setup(void)
{
	RigidBody::setup();
	delete[] m_position_data;
	m_position_data = new double[3 * (m_steps + 1)];
	m_me = [this](double t, math::quat q){
		const double m = m_M;
		const double l = m_l;
		const double g = 9.81;
		const math::vec3 e3(0, 0, 1);
		return -m * g * l * e3.cross(q.conjugate(e3));
	};
	m_Ke = [this](double t, math::quat q){
		const double m = m_M;
		const double l = m_l;
		const double g = 9.81;
		const math::vec3 e3(0, 0, 1);
		return -m * g * l * e3.spin() * q.conjugate(e3).spin();
	};
	math::vec3(m_position_data + 0) = math::quat(m_state_old).rotate({0, 0, -m_l});
}
void Top::record(void)
{
	RigidBody::record();
	const double g = 9.81;
	math::vec3(m_position_data + 3 * (m_step + 1)) = math::quat(m_state_new).rotate({0, 0, -m_l});
	m_energy_data[2 * (m_step + 1) + 1] = -m_M * g * (m_position_data[3 * (m_step + 1) + 2] + m_l) + m_energy_data[0];
}
void Top::finish(void)
{
	//data
	char path[200];
	RigidBody::finish();
	sprintf(path, "data/%s-position.dat", m_label);
	//write
	FILE* file = fopen(path, "w");
	for(unsigned j = 0; j < m_steps; j++)
	{
		fprintf(file, "%+.6e ", m_dt * j);
		for(unsigned k = 0; k < 3; k++)
		{
			fprintf(file, "%+.6e ", m_position_data[3 * j + k]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

//stability
double Top::reference_velocity(void) const
{
	//data
	const double m = m_M;
	const double l = m_l;
	const double J3 = m_J[8];
	const double g = 9.81e+00;
	//return
	return sqrt(m * g * l / J3);
}

bool Top::stability_vertical(double w0) const
{
	const double J1 = m_J[0];
	const double J2 = m_J[4];
	const double J3 = m_J[8];
	const double g1 = J1 / J3;
	const double g2 = J2 / J3;
	const double wr = reference_velocity();
	return stability_vertical(g1, g2, w0 / wr);
}
bool Top::stability_vertical(double g1, double g2, double wp)
{
	//data
	const double e = 1 / g1 / g2;
	const double b = (g1 + g2) / g1 / g2;
	const double d = (g1 + g2 - 2) / g1 / g2;
	const double c = (g1 * g2 - g1 - g2 + 1) / g1 / g2;
	const double a = (2 * g1 * g2 - g1 - g2 + 1) / g1 / g2;
	//stability
	const double B = a * wp * wp - b;
	const double C = c * wp * wp * wp * wp + d * wp * wp + e;
	return abs(g1 - g2) < 1 && g1 + g2 > 1 && B > 0 && C > 0 && B * B - 4 * C > 0;
}

bool Top::stability_tilted(uint32_t index, double tilt_angle) const
{
	//data
	double w0;
	const double J1 = m_J[0];
	const double J2 = m_J[4];
	const double J3 = m_J[8];
	const double g1 = J1 / J3;
	const double g2 = J2 / J3;
	const double wr = reference_velocity();
	if(index == 0) w0 = wr / sqrt(cos(tilt_angle) * (1 - g2));
	if(index == 1) w0 = wr / sqrt(cos(tilt_angle) * (1 - g1));
	//return
	return stability_tilted(index, g1, g2, w0 / wr);
}
bool Top::stability_tilted(uint32_t index, double g1, double g2, double wp)
{
	//data
	const double c2 = (g1 - g2) * (1 - g2) / g1;
	const double c3 = (g2 - g1) * (1 - g1) / g2;
	const double a2 = 2 - g2 * (1 + g1 - g2) / g1;
	const double a3 = 2 - g1 * (1 + g2 - g1) / g2;
	const double d2 = (g1 - g2) * (2 * g2 - 3) / g1 / g2 / (1 - g2);
	const double d3 = (g2 - g1) * (2 * g1 - 3) / g2 / g1 / (1 - g1);
	const double e2 = 3 * (g1 - g2) / g1 / g2 / (1 - g2) / (1 - g2);
	const double e3 = 3 * (g2 - g1) / g2 / g1 / (1 - g1) / (1 - g1);
	const double b2 = (1 - 2 * g1 - g2 * (1 + g1 - g2)) / g1 / g2 / (1 - g2);
	const double b3 = (1 - 2 * g2 - g1 * (1 + g2 - g1)) / g2 / g1 / (1 - g1);
	//return
	const double B2 = a2 * wp * wp + b2 / wp / wp;
	const double B3 = a3 * wp * wp + b3 / wp / wp;
	const double C2 = c2 * wp * wp * wp * wp + d2 + e2 / wp / wp / wp / wp;
	const double C3 = c3 * wp * wp * wp * wp + d3 + e3 / wp / wp / wp / wp;
	return index == 0 ? 
		abs(g1 - g2) < 1 && g1 + g2 > 1 && B2 > 0 && C2 > 0 && B2 * B2 - 4 * C2 > 0 : 
		abs(g1 - g2) < 1 && g1 + g2 > 1 && B3 > 0 && C3 > 0 && B3 * B3 - 4 * C3 > 0;
}