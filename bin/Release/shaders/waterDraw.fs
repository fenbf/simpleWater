// waterDraw.fs
// fragment shader that draws on the water height map surface, it
// changes height of the water at given position

#version 330

// input: standard texture coord
in vec2 vVaryingTexCoord0;

// input: pressure that will be aplied to the water surface
//        it is a new water height at given position
in float vVaryingPressure;

//
// output: RGBA color (but only R is set - new height)
//
out vec4 vFragColor;

void main()
{
    vFragColor = vec4(vVaryingPressure, 0.0, 0.0, 0.0);
}