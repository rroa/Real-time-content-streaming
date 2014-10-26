#version 330 core

/*######################################################*/
// INPUTS
/*######################################################*/
// Vertex
in vec3 VertexPosition_worldspace;
in vec4 VertexPosition_cameraspace;

// Textures
in vec2 texCoord;

// Light
in vec3 LightDirection_cameraspace;
in vec3 LightDirection_tangentspace;
in vec3 LightPosition_cameraspace;
in vec3 LightPosition_worldspace;

// Camera
in vec3 EyeDirection_cameraspace;
in vec3 EyeDirection_tangentspace;
in vec3 EyePosition_worldspace;
smooth in vec4 vEyeSpacePos;

// Light & Camera
in vec3 HalfwayVector;

/*######################################################*/
// UNIFORMS
/*######################################################*/

// Fog
uniform struct FogParameters
{
	vec4  Color;	// Fog color
	float Start;	// This is only for linear fog
	float End;		// This is only for linear fog
	float Density;  // For exp and exp2 equation	
	int   Mode;		// 0 = linear, 1 = exp, 2 = exp2
} u_fogParams;
uniform bool u_UseFog;

// Lights
uniform vec4  u_LightColor;
uniform float u_LightPower;

// Textures
uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D SpecularTextureSampler;
uniform sampler2D EmissiveTextureSampler;
uniform sampler2D HeightTextureSampler;

uniform bool UseDiffuseTextureSampler;
uniform bool UseSpecularTextureSampler;
uniform bool UseNormalTextureSampler;
uniform bool UseEmissiveTextureSampler;
uniform bool UseHeightTextureSampler;

// Misc
uniform vec4  u_Ambient;
uniform float u_Alpha;
uniform float u_Shininess;
uniform float u_time;

/*######################################################*/
// OUTPUTS
/*######################################################*/
out vec4 o_FragColor;

void main()
{
	// Bumps!
	vec4 TextureNormal_tangentspace = vec4( normalize( LightDirection_tangentspace ), 1.0 );

	// Normal of the computed fragment, in camera space
	vec4 normal = TextureNormal_tangentspace;
	
	// Direction of the light (from the fragment to the light)
	vec4 dirToLight = normalize( vec4( LightDirection_tangentspace, 0.0 ));

	float intensity;
	vec4 toonColor;
	intensity = dot( dirToLight, normal );

	if ( intensity > 0.95 )
		toonColor = vec4( 0.8, 0.5, 0.5, 1.0 );
	else if ( intensity > 0.5 )
		toonColor = vec4( 0.6, 0.3, 0.3, 1.0 );
	else if ( intensity > 0.25 )
		toonColor = vec4( 0.4, 0.2, 0.2, 1.0 );
	else
		toonColor = vec4( 0.2, 0.1, 0.1, 1.0 );

	o_FragColor = toonColor;
}