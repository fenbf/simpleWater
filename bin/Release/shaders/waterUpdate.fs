// waterUpdate.fs
// fragment shader that updates the water surface, height map algorithm

#version 330

uniform sampler2D texture0;

uniform vec3 density;
uniform vec2 texelSize;

// input: standard texture coord
in vec2 vVaryingTexCoord0;

// input: pressure that will be aplied to the water surface
//        it is a new water height at given position - not used in this shader
in float vVaryingPressure;

//
// output: RGBA, R - new height, G - velocity
//
out vec4 vFragColor;

void main()
{
	vec2 data  = texture(texture0, vVaryingTexCoord0.xy).rg; 

	//
	// get the change of height from four neightbours
	//
	float y  = data.r;
	float yn = texture(texture0, vVaryingTexCoord0.xy + vec2(0.0, texelSize.y)).r  - y;
	float yw = texture(texture0, vVaryingTexCoord0.xy + vec2(texelSize.x, 0.0)).r  - y;
	float ys = texture(texture0, vVaryingTexCoord0.xy + vec2(0.0, -texelSize.y)).r - y;
	float ye = texture(texture0, vVaryingTexCoord0.xy + vec2(-texelSize.x, 0.0)).r - y;

	// add to the current 'velocity'
	data.g  += (yn + yw + ys + ye) * density.y;
   
	// reduce the speed a bit
    data.g *=  density.x;
    
	// move the 'height', but not with full speed
    data.r = (data.r + data.g) * density.z;
    
	vFragColor = vec4(data.r, data.g, 0.0, 0.0);
}