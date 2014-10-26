#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

smooth in vec4 vEyeSpacePos;
smooth in vec3 vNormal;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D DiffuseTextureSampler;

void main()
{
	vec4 lightColor = vec4( 1.0, 1.0, 1.0, 1.0 );

	vec4  fogColor = vec4( 0.7, 0.7, 0.7, 1.0 );
	float fFogCoord = abs( vEyeSpacePos.z / vEyeSpacePos.w );
	float fEnd = 5.0;
	float fStart = 1.0;

	float fResult = ( fEnd-fFogCoord )/( fEnd-fStart );
	fResult = 1.0 - clamp(fResult, 0.0, 1.0);

	// specular
	//vec3 halfAngle = normalize(cameraDir + lightDir);
	//vec3 specularColor = min( lightColor + 0.5, 1.0);
	//float specularDot = dot( normal, halfAngle);
	//specular += specularColor * pow(clamp(specularDot, 0.0, 1.0), 16.0) * distFactor;

	// Material properties
	vec4 MaterialDiffuseColor = texture( DiffuseTextureSampler, UV );
	
	color = mix( MaterialDiffuseColor, fogColor, fResult );
}