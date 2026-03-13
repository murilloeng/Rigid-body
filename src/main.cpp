//std
#include <cmath>
#include <cstdio>
#include <stdexcept>

//Rigid-body
#include "Rigid-body/inc/Top.hpp"
#include "Rigid-body/inc/scenes.hpp"
#include "Rigid-body/inc/StabilityMap.hpp"

static void test_stability_map(void)
{
	StabilityMap map;
	map.start();
}
static void test_vertical_symmetric(double gs, bool stability)
{
	//data
	Top top;
	const unsigned ns = 10000;
	const double l = 1.00e-01;
	const double m = 1.00e+00;
	const double J3 = 1.00e+00;
	const double tr = 1.00e-05;
	//setup
	top.m_M = m;
	top.m_l = l;
	top.m_J[8] = J3;
	top.m_steps = ns;
	top.m_dt = M_PI / 100 / sqrt(gs);
	top.m_J[0] = top.m_J[4] = gs * J3;
	const double wr = top.reference_velocity();
	top.m_velocity_old[0] = 2 * sqrt(gs) * wr * tr;
	top.m_velocity_old[2] = 2 * sqrt(gs) * wr * (stability ? 1.01 : 0.99);
	sprintf(top.m_label, "tests/vertical-symmetric-%.2lf-%s", gs, stability ? "stable" : "unstable");
	//solve
	top.setup();
	top.solve();
	top.finish();
}
static void test_vertical(double g1, double g2, double wp, bool stability)
{
	//data
	Top top;
	const unsigned ns = 10000;
	const double l = 1.00e-01;
	const double m = 1.00e+00;
	const double J3 = 1.00e+00;
	const double tr = 1.00e-05;
	//setup
	top.m_M = m;
	top.m_l = l;
	top.m_J[8] = J3;
	top.m_steps = ns;
	top.m_J[0] = g1 * J3;
	top.m_J[4] = g2 * J3;
	top.m_dt = 2 * M_PI / 100 / wp;
	const double wr = top.reference_velocity();
	const char* str = stability ? "stable" : "unstable";
	top.m_velocity_old[0] = wp * wr * tr;
	top.m_velocity_old[2] = wp * wr * (stability ? 1.01 : 0.99);
	sprintf(top.m_label, "tests/vertical-%.2lf-%.2lf-%.2lf-%s", g1, g2, wp, str);
	//solve
	top.setup();
	top.solve();
	top.finish();
}
static void test_tilted(double g1, double g2, double wp, bool axis, bool stability)
{
	//data
	Top top;
	const unsigned ns = 10000;
	const double l = 1.00e-01;
	const double m = 1.00e+00;
	const double J3 = 1.00e+00;
	const double tr = 1.00e-05;
	const math::vec3 e1 = {1, 0, 0};
	const math::vec3 e2 = {0, 1, 0};
	const double gp = axis ? g1 : g2;
	const double ws = (stability ? 1.01 : 0.99) * wp;
	const double bp = acos(1 / (ws * ws * (1 - gp)));
	printf("bp: %.2e\n", bp * 180 / M_PI);
	return;
	//setup
	top.m_M = m;
	top.m_l = l;
	top.m_J[8] = J3;
	top.m_steps = ns;
	top.m_J[0] = g1 * J3;
	top.m_J[4] = g2 * J3;
	top.m_dt = 2 * M_PI / 100 / wp;
	const double wr = top.reference_velocity();
	const char* str = stability ? "stable" : "unstable";
	top.m_velocity_old[2] = ws * wr * cos(bp);
	top.m_velocity_old[0] = axis ? -ws * wr * (sin(bp) + tr) : 0;
	top.m_velocity_old[1] = axis ? 0 : +ws * wr * (sin(bp) + tr);
	math::quat(top.m_state_old) = math::quat(bp, axis ? e2 : e1);
	sprintf(top.m_label, "tests/tilted-%d-%.2lf-%.2lf-%.2lf-%s", axis, g1, g2, wp, str);
	//solve
	top.setup();
	top.solve();
	top.finish();
}

int main(int argc, char** argv)
{
	//test
	test_stability_map();
	//return
	return 0;
}