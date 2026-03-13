#version 460 core

out vec4 fragment_color;

layout(std140, binding = 0) uniform screen
{
	uint width;
	uint height;
};

float w = width;
float h = height;
float gt_min = 0;
float gt_max = 2;
float gn_min = 0;
float gn_max = 2;
float m = min(w, h);

float segmentDistance(vec2 xp, vec2 x1, vec2 x2)
{
	float t = dot(xp - x1, x2 - x1) / dot(x2 - x1, x2 - x1);
	return distance(xp, x1 + clamp(t, 0.0, 1.0) * (x2 - x1));
}

vec3 palette(float v, float v_min, float v_max)
{
	//data
	const vec3 colors[] = {
		vec3(0.0, 0.0, 0.5),
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.5, 1.0),
		vec3(0.0, 1.0, 1.0),
		vec3(0.5, 1.0, 0.5),
		vec3(1.0, 1.0, 0.0),
		vec3(1.0, 0.5, 0.0),
		vec3(1.0, 0.0, 0.0),
		vec3(0.5, 0.0, 0.0)
	};
	const uint colors_count = 9;
	//scaling
	v = max(min(v, v_max), v_min);
	v = (colors_count - 1) * (v - v_min) / (v_max - v_min);
	//indexes
	const float x = fract(v);
	const uint b = uint(ceil(v));
	const uint a = uint(floor(v));
	//return
	return (1 - x) * colors[a] + x * colors[b];
}

bool boundaries(float gt, float gn, bool inner)
{
	return 
		segmentDistance(vec2(gt, gn), vec2(1.0, 0.0), vec2(2.0, 1.0)) < 4e-3 ||
		segmentDistance(vec2(gt, gn), vec2(0.0, 1.0), vec2(1.0, 2.0)) < 4e-3 ||
		segmentDistance(vec2(gt, gn), vec2(1.0, 0.0), vec2(0.0, 1.0)) < 4e-3 ||
		(inner && segmentDistance(vec2(gt, gn), vec2(0.0, 1.0), vec2(2.0, 1.0)) < 4e-3) || 
		(inner && segmentDistance(vec2(gt, gn), vec2(1.0, 0.0), vec2(1.0, 2.0)) < 4e-3) || 
		(inner && segmentDistance(vec2(gt, gn), vec2(0.5, 0.5), vec2(0.5, 1.5)) < 4e-3) ||
		(inner && segmentDistance(vec2(gt, gn), vec2(0.5, 0.5), vec2(1.5, 0.5)) < 4e-3);
}

bool zone_test(uint zone, float gt, float gn)
{
	return 
		zone == 0 ? gt > 1.0 && gn > 1.0 : 
		zone == 1 ? (gt < 0.5 && gn > 1.0) || (gt > 1.0 && gn < 0.5) :
		zone == 2 ? (gt < 0.5 && gn < 1.0) || (gn < 0.5 && gt < 1.0) : 
		zone == 3 ? (gt > 0.5 && gt < 1.0) && (gn > 0.5 && gn < 1.0) : 
		zone == 4 ? (gt > 0.5 && gt < 1.0 && gn > 1.0) || (gt > 1.0 && gn > 0.5 && gn < 1.0) : true;
}
uint zone_search(float gt, float gn)
{
	for(uint i = 0; i < 5; i++)
	{
		if(zone_test(i, gt, gn)) return i;
	}
	return 5;
}
vec3 zone_color(float gt, float gn)
{
	return palette(zone_search(gt, gn), 0, 4);
}

void main(void)
{
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
	if(boundaries(gt, gn, true))
	{
		fragment_color = vec4(0, 0, 0, 1);
		return;
	}
	//discard
	// if(!zone_test(3, gt, gn) && !zone_test(4, gt, gn)) discard;
	if(gt < gt_min || gt > gt_max) discard;
	if(gn < gn_min || gn > gn_max) discard;
	if(abs(gt - gn) > 1 || gt + gn < 1) discard;
	//fragment
	fragment_color = vec4(zone_color(gt, gn), 1);
	// fragment_color = w1 < w5 ? vec4(0, 0, 1, 1) : vec4(1, 0, 0, 1);
}