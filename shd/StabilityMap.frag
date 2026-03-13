#version 460 core

out vec4 fragment_color;

layout(std140, binding = 0) uniform screen
{
	uint width;
	uint height;
};

bool zone_test(uint zone, float gt, float gn)
{
	return 
		zone == 0 ? gt > 1.0 && gn > 1.0 : 
		zone == 1 ? (gt < 0.5 && gn > 1.0) || (gt > 1.0 && gn < 0.5) :
		zone == 2 ? (gt < 0.5 && gn < 1.0) || (gn < 0.5 && gt < 1.0) : 
		zone == 3 ? (gt > 0.5 && gt < 1.0) && (gn > 0.5 && gn < 1.0) : 
		zone == 4 ? (gt > 0.5 && gt < 1.0 && gn > 1.0) || (gt > 1.0 && gn > 0.5 && gn < 1.0) : true;
}
vec4 zone_color(float gt, float gn)
{
	return
		zone_test(0, gt, gn) ? vec4(1, 0, 0, 1) : 
		zone_test(1, gt, gn) ? vec4(0, 1, 0, 1) : 
		zone_test(2, gt, gn) ? vec4(0, 0, 1, 1) : 
		zone_test(3, gt, gn) ? vec4(1, 1, 0, 1) : 
		zone_test(4, gt, gn) ? vec4(1, 0, 1, 1) : vec4(1, 1, 1, 1);
}

void main(void)
{
	//data
	const float w = width;
	const float h = height;
	const float gt_min = 0;
	const float gt_max = 2;
	const float gn_min = 0;
	const float gn_max = 2;
	const float m = min(w, h);
	//inertia
	const float gt = w / m * ((gt_max - gt_min) * gl_FragCoord.x / w + gt_min);
	const float gn = h / m * ((gn_max - gn_min) * gl_FragCoord.y / h + gn_min);
	//parameters
	const float e1 = 1 / gt / gn;
	const float b1 = (gt + gn) / gt / gn;
	const float d1 = (gt + gn - 2) / gt / gn;
	const float c1 = (1 - gt - gn + gt * gn) / gt / gn;
	const float a1 = (1 - gt - gn + 2 * gt * gn) / gt / gn;
	//derived
	const float f1 = a1 * a1 - 4 * c1;
	const float h1 = b1 * b1 - 4 * e1;
	const float g1 = -2 * b1 * a1 - 4 * d1;
	//deltas
	const float q1 = d1 * d1 - 4 * c1 * e1;
	const float q2 = g1 * g1 - 4 * f1 * h1;
	//roots
	const float w12 = b1 / a1;
	const float w22 = (-d1 - sqrt(q1)) / (2 * c1);
	const float w32 = (-d1 + sqrt(q1)) / (2 * c1);
	const float w42 = (-g1 - sqrt(q2)) / (2 * f1);
	const float w52 = (-g1 + sqrt(q2)) / (2 * f1);
	//roots
	const float w1 = sqrt(b1 / a1);
	const float w2 = sqrt((-d1 - sqrt(q1)) / (2 * c1));
	const float w3 = sqrt((-d1 + sqrt(q1)) / (2 * c1));
	const float w4 = sqrt((-g1 - sqrt(q2)) / (2 * f1));
	const float w5 = sqrt((-g1 + sqrt(q2)) / (2 * f1));
	//boundaries
	if(!zone_test(3, gt, gn)) discard;
	if(gt < gt_min || gt > gt_max) discard;
	if(gn < gn_min || gn > gn_max) discard;
	if(abs(gt - gn) > 1 || gt + gn < 1) discard;
	//fragment
	fragment_color = w1 < w2 ? vec4(0, 0, 1, 1) : vec4(1, 0, 0, 1);
}