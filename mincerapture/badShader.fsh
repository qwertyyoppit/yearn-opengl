#version 330 core
#define PI 3.1415926538

in vec4 vertexColor;

out vec4 FragColor;

uniform float time;

void main()
{
	FragColor = vec4(vertexColor.xyz * mod(vec3((sin(time) / 2.0f) + 0.5f, 
									(sin(PI/3 + time) / 2.0f) + 0.5f, 
									(sin(PI/3*2 + time) / 2.0f) + 0.5f), vec3(1,1,1)), vertexColor.w);
}