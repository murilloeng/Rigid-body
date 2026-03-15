#version 460 core

out vec4 fragment_color;

layout(std140, binding = 0) uniform screen
{
	uint width;
	uint height;
};

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

vec3 admissible_color(float gt, float gn)
{
	return boundaries(gt, gn, false) ? vec3(0) : abs(gt - gn) < 1 && gt + gn > 1 ? vec3(0.5) : vec3(1);
}

vec3 symmetric_color(void)
{
	//data
	const uint n = 100;
	const float w = width;
	const float h = height;
	const float gs_min = 0.5;
	const float gs_max = 4.0;
	const float wp_min = 1.4;
	const float wp_max = 4.0;
	const float m = min(w, h);
	//parameters
	const float gs = w / m * ((gs_max - gs_min) * gl_FragCoord.x / w + gs_min);
	const float wp = h / m * ((wp_max - wp_min) * gl_FragCoord.y / h + wp_min);
	//boundary
	for(uint i = 0; i < n; i++)
	{
		const float g1 = (gs_max - gs_min) * (i + 0) / n + gs_min;
		const float g2 = (gs_max - gs_min) * (i + 1) / n + gs_min;
		if(segmentDistance(vec2(gs, wp), vec2(g1, 2 * sqrt(g1)), vec2(g2, 2 * sqrt(g2))) < 1e-2) return vec3(0);
	}
	//return
	return wp < 2 * sqrt(gs) ? vec3(1, 0, 0) : vec3(0, 0, 1);
}

vec3 stability_color(float gt, float gn)
{
	//data
	const uint n = 100;
	const uint zone = zone_search(gt, gn);
	const vec3 color_type_0 = vec3(1.00, 0.00, 0.00);
	const vec3 color_type_1 = vec3(0.00, 0.00, 1.00);
	const vec3 color_type_2 = vec3(1.00, 0.65, 0.00);
	const vec3 color_type_3 = vec3(0.50, 1.00, 0.50);
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
	const float w1 = sqrt(b1 / a1);
	const float w2 = sqrt((-d1 - sqrt(q1)) / (2 * c1));
	const float w3 = sqrt((-d1 + sqrt(q1)) / (2 * c1));
	const float w4 = sqrt((-g1 - sqrt(q2)) / (2 * f1));
	const float w5 = sqrt((-g1 + sqrt(q2)) / (2 * f1));
	//curve
	for(uint i = 0; i < n; i++)
	{
		//curve 1
		const float gna1 = 1.14 * (i + 0) / n + 0.5;
		const float gna2 = 1.14 * (i + 1) / n + 0.5;
		const float gta1 = (2 - 3 * gna1 + sqrt(9 * gna1 * gna1 - 4 * gna1)) / 2;
		const float gta2 = (2 - 3 * gna2 + sqrt(9 * gna2 * gna2 - 4 * gna2)) / 2;
		//curve 2
		const float gtb1 = 1.14 * (i + 0) / n + 0.5;
		const float gtb2 = 1.14 * (i + 1) / n + 0.5;
		const float gnb1 = (2 - 3 * gtb1 + sqrt(9 * gtb1 * gtb1 - 4 * gtb1)) / 2;
		const float gnb2 = (2 - 3 * gtb2 + sqrt(9 * gtb2 * gtb2 - 4 * gtb2)) / 2;
		//check
		if(segmentDistance(vec2(gt, gn), vec2(gta1, gna1), vec2(gta2, gna2)) < 4e-3) return vec3(0);
		if(segmentDistance(vec2(gt, gn), vec2(gtb1, gnb1), vec2(gtb2, gnb2)) < 4e-3) return vec3(0);
	}
	//return
	return 
		boundaries(gt, gn, false) ? vec3(0) : 
		abs(gt - gn) > 1 || gt + gn < 1 ? vec3(1) : 
		segmentDistance(vec2(gt, gn), vec2(0, 1), vec2(2, 1)) < 4e-3  ? vec3(0) : 
		segmentDistance(vec2(gt, gn), vec2(1, 0), vec2(1, 2)) < 4e-3  ? vec3(0) : 
		segmentDistance(vec2(gt, gn), vec2(0.5, 0.5), vec2(1, 1)) < 4e-3  ? color_type_1 : 
		zone == 4 && max(w1, w5) < w2 ? color_type_2 :
		zone == 3 && max(w1, w5) < w2 ? color_type_3 :
		zone == 1 || (zone == 4 && max(w1, w5) > w2) ? color_type_0 : 
		zone == 0 || zone == 2 || (zone == 3 && max(w1, w5) > w2) ? color_type_1 : vec3(0.5);
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
	fragment_color = vec4(stability_color(gt, gn), 1);
}