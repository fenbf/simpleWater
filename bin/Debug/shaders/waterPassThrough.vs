// waterPassThrough.vs
// vertex shader that is used by all programs in height map water simulation

#version 330

// attrib: vertex pos + height
// x, y - position from -1 to 1
// z    - water pressure that is applied to water's height map
//        that way you can draw on the water
layout(location = 0) in vec3 vVertex; 

// attrib: standard texture coords
layout(location = 1) in vec2 vTexCoord0;

// output: tex coord
out vec2 vVaryingTexCoord0;    

// output: new height of the water a a given point
out float vVaryingPressure;   

void main() 
{
	vVaryingTexCoord0 = vTexCoord0;    
	vVaryingPressure  = vVertex.z;
	  
    gl_Position = vec4(vVertex.x, vVertex.y, 0.0, 1.0);         
}