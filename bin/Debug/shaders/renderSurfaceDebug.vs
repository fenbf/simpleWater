#version 330

uniform mat4 modelviewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;

// light pos in view space
uniform vec3 lightPos;

layout(location = 0) in vec3 vVertex; 
layout(location = 1) in vec3 vNormal; 
layout(location = 2) in vec2 vTexCoord0;

out Vertex
{
	vec2 vTexCoord0;
} vertexOut;
     

void main() 
{
    vertexOut.vTexCoord0 = vTexCoord0;      	
    
	vec4 v = vec4(vVertex, 1.0);       
	
	gl_Position = projectionMatrix * modelviewMatrix * v;         
}