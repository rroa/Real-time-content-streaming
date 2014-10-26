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
	vec4  LightColor  = vec4( 1.0, 1.0, 1.0, 1.0 );
	float LightPower = 20.0;
	//vec3 lightPos = vec3( 2.0, 2.0, 2.0 );

	// Material properties
	vec4 MaterialDiffuseColor       = texture( DiffuseTextureSampler, texCoord );
	vec4 MaterialAmbientColor       = vec4( 0.1, 0.1, 0.1, 1.0) * MaterialDiffuseColor;
	vec4 MaterialSpecularColor      = texture(SpecularTextureSampler, texCoord ) * 0.3;
	vec4 MaterialEmissiveColor      = texture(EmissiveTextureSampler, texCoord ) * 0.3;
	vec4 TextureNormal_tangentspace = normalize( texture( NormalTextureSampler, vec2( texCoord.x, texCoord.y ) ) * 2.0 - 1.0 );

	// Distance to the light
	float lightDistance = length( LightPosition_worldspace - VertexPosition_worldspace );

	// Normal of the computed fragment, in camera space
	vec4 normal = TextureNormal_tangentspace;
	
	// Direction of the light (from the fragment to the light)
	vec4 light = normalize( vec4( LightDirection_tangentspace, 1.0 ));

	float cosTheta = clamp( dot( normal, light ), 0, 1 );

	// Eye vector (towards the camera)
	vec4 eye = normalize( vec4( EyeDirection_tangentspace, 1.0 ));

	// Direction in which the triangle reflects the light
	vec4 reflection = reflect( -light, normal );


	float shininess = 1.0;
	/*//shininess = 0.2126 * SpecularColor.r + 0.7152 * SpecularColor.g + 0.0722 * SpecularColor.b;
	vec3  worldUnitToNorlmal = normalize( vec3( eyePosition_worldspace * normal.xyz )); 
	vec3  dirToCamera = vec3( eye.xyz ); 
	vec3  reflectedCameraPos = -1.0 * reflect( dirToCamera, worldUnitToNorlmal );
	vec3  dirToLight = normalize( LightPosition_worldspace - Position_worldspace.xyz );

	float specIntensity = dot( reflectedCameraPos, dirToLight );
	specIntensity = clamp( specIntensity, 0.0, 1.0 );
	specIntensity = pow( specIntensity, 32.0 * shininess );
	vec4 test = vec4( specIntensity * vec3(1.0), 1.0);
	*/
	// Specular
	float specularIntensity = dot( reflection,  eye);
	specularIntensity		= clamp( specularIntensity, 0.0, 1.0 );

	//shininess = max( MaterialSpecularColor.r, MaterialSpecularColor.g );
	//shininess = max( shininess, MaterialSpecularColor.b );

	specularIntensity 		= pow( specularIntensity, 32 * shininess );
	//specularIntensity 	    *= shininess;

	//spectTexel.rgb * lightColor.rgb;
	vec4 specColor = vec4( specularIntensity * MaterialSpecularColor.rgb, 1.0 ) * LightColor;
	// Output color = color of the texture at the specified UV

	// Eye vector (towards the camera)
	vec4 E = normalize( vec4(EyeDirection_cameraspace, 1.0 ));
	// Direction in which the triangle reflects the light
	vec4 R = reflect(-light,normal);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );

	o_FragColor = 
			MaterialAmbientColor +	
			//MaterialEmissiveColor +
			( MaterialDiffuseColor * LightColor * LightPower * cosTheta / (lightDistance * lightDistance) );			
	o_FragColor = mix( specColor, o_FragColor, specularIntensity );			
}