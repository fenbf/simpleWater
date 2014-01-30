#version 330

in Vertex
{
	vec3 vPosTS;		// vertex position in Tangent Space
	vec3 vEyePosTS;		// eye pos in Tangent Space
	vec3 vLightPosTS;   // light pos in Tangent Space
	vec2 vTexCoord0;
} vertexIn;

uniform float refractionFactor;

uniform vec4 waterColor;

uniform sampler2D texture0;
uniform sampler2D normalMap;

out vec4 vFragColor;

void main()
{
	//
	// read normal
	//
	vec3 texNorm = texture(normalMap, vertexIn.vTexCoord0).rgb;
	vec3 norm = normalize(texNorm*2.0-1.0);

	//
	// lighting
	//
	vec3 ambient  = vec3(0.1);
	vec3 diffuse  = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	vec3 eye = normalize(vertexIn.vEyePosTS - vertexIn.vPosTS);
	vec3 light = normalize(vertexIn.vLightPosTS - vertexIn.vPosTS);
	
	vec3 halfv = normalize(eye + light);

	// diffuse
	float diffElem = max(0.0, dot(norm, light));
	diffuse += vec3(diffElem);

	// specular
	float specularElem = pow(max(0.0, dot(norm, halfv)), 64.0);
	specular = vec3(specularElem);

	//
	// final
	//

	vec3 tex = texture(texture0, vertexIn.vTexCoord0+norm.xy*refractionFactor).rgb;

    vFragColor.rgb = waterColor.rgb * tex * (ambient + diffuse) + specular;
	vFragColor.a = waterColor.a;
}