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
	vec3 vPosTS;		// vertex position in Tangent Space
	vec3 vEyePosTS;		// eye pos in Tangent Space
	vec3 vLightPosTS;   // light pos in Tangent Space
	vec2 vTexCoord0;
} vertexOut;
     

void main() 
{
    vertexOut.vTexCoord0 = vTexCoord0;      	
    
	vec4 v = vec4(vVertex, 1.0);       
    
	vec4 pos = modelviewMatrix * v;

	//
	// compute tangent frame
	//
	vec3 T = normalize(normalMatrix * vec3(1.0, 0.0, 0.0));
	vec3 N = normalize(normalMatrix * vNormal);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	vertexOut.vPosTS      = pos.xyz    * TBN;		// note the order... not TBN * pos
	vertexOut.vEyePosTS   = vec3(0.0) * TBN;
	vertexOut.vLightPosTS = lightPos  * TBN;
	
	gl_Position = projectionMatrix * modelviewMatrix * v;         
}