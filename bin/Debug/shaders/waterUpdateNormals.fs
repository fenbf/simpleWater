// waterUpdateNormals.fs
// fragment shader that is used to calculate normal map of the water

#version 330

// uniform: water height map values
uniform sampler2D texture0;

// uniform: size of a one texel
uniform vec2 texelSize;

// strength of the norma map in Z direction
uniform float normalScale;

// input: standard texture coord
in vec2 vVaryingTexCoord0;

// input: pressure that will be aplied to the water surface
//        it is a new water height at given position - not used in this shader
in float vVaryingPressure;

//
// output: new normal in format XYZ, Y is the top...
//
out vec4 vFragColor;

void main()
{
	//  
	// gather all four neighbours:
	//
    float yn = texture(texture0, vVaryingTexCoord0.xy + vec2(0.0, texelSize.y)).r;
    float yw = texture(texture0, vVaryingTexCoord0.xy + vec2(texelSize.x, 0.0)).r;
    float ys = texture(texture0, vVaryingTexCoord0.xy + vec2(0.0, -texelSize.y)).r;
    float ye = texture(texture0, vVaryingTexCoord0.xy + vec2(-texelSize.x, 0.0)).r;

	// normalize:
    vec3 normal = normalize(vec3(yw-ye, ys-yn, normalScale));
   
    // code in the form of color (values from 0 to 1):
    vFragColor.rgb = normal*0.5+vec3(0.5);
    vFragColor.a = 0.0;
}