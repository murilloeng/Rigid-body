#version 460 core

//data
const vec2 positions[] = {vec2(-1, -1), vec2(+1, -1), vec2(+1, +1), vec2(-1, +1)};

void main(void)
{
	gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
}