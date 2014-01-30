#version 330

in Vertex
{
	vec2 vTexCoord0;
} vertexIn;

uniform sampler2D texture0;

out vec4 vFragColor;

void main()
{
    vFragColor = texture(texture0, vertexIn.vTexCoord0);
}