#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec4 EyeDirection_cameraspace;
in vec4 LightDirection_cameraspace;

in vec4 LightDirection_tangentspace;
in vec4 EyeDirection_tangentspace;

uniform vec3 LightPosition_worldspace;

// Ouput data
out vec4 color;

uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;

void main()
{	
	vec4 LightColor  = vec4( 1.0, 1.0, 1.0, 1.0 );
	float LightPower = 40.0;

	// Material properties
	vec4 MaterialDiffuseColor  = texture( DiffuseTextureSampler, UV );
	vec4 MaterialAmbientColor  = vec4( 0.1, 0.1, 0.1, 1.0) * MaterialDiffuseColor;
	
	vec4 TextureNormal_tangentspace = normalize( texture( NormalTextureSampler, vec2( UV.x,-UV.y ) ) * 2.0 - 1.0 );	

	// Distance to the light
	float lightDistance = length( LightPosition_worldspace - Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec4 normal = TextureNormal_tangentspace;
	
	// Direction of the light (from the fragment to the light)
	vec4 light = normalize( LightDirection_tangentspace );

	float cosTheta = clamp( dot( normal, light ), 0, 1 );

	// Eye vector (towards the camera)
	vec4 eye = normalize( EyeDirection_tangentspace );

	// Direction in which the triangle reflects the light
	vec4 reflection = reflect( -light, normal );

	// Output color = color of the texture at the specified UV
	color = MaterialAmbientColor +
			( MaterialDiffuseColor * LightColor * LightPower * cosTheta / (lightDistance * lightDistance) );
}