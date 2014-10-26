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
in vec3 EyeDirection_worldspace;

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

// Parallax
uniform struct ParallaxParameters
{
	float Scale;
	float Bias;
} u_pParallaxParams;

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
uniform bool  u_UseSpecialEffect;

/*####################################################################*/
/* ########################### NEW LIGHTS ########################### */
const int numberOfLights = 8;
struct Light
{
	vec3 Direction;
	vec3 Position;
	bool OnOff;
	int  Type;
		
	// Colors
	vec3 DiffuseColor;
	vec3 SpecularColor;
	vec3 AmbientColor;
		
	// CutOff
	float SpotCutOff;
	float SpotExponent;
	vec3  SpotDirection;

	// Attenuation
	float ConstantAttenuation;
	float LinearAttenuation;
	float ExponentialAttenuation;

	//
	float AmbientColorIntensity;
	float Power;
};

uniform Light light0;
uniform Light light1;
uniform Light light2;
uniform Light light3;
uniform Light light4;
uniform Light light5;
uniform Light light6;
uniform Light light7;

uniform int u_NumberOfLights;
Light  lights[numberOfLights];
smooth in vec3 vNormal;

vec4 GetDirectionalLightColor( const Light dirLight, vec3 vNormal )
{
	float fDiffuseIntensity = max( 0.0, dot( vNormal, -dirLight.Direction ));
	return vec4( dirLight.DiffuseColor * ( dirLight.AmbientColorIntensity + fDiffuseIntensity ) * dirLight.Power, 1.0 );	
}

vec4 GetPointLightColor( const Light ptLight, vec3 vWorldPos, vec3 vNormal)
{
	vec3 vPosToLight = ptLight.Position - vWorldPos;
	float fDist = length( vPosToLight );
	vPosToLight = normalize( vPosToLight);
	
	float fDiffuse = max( 0.0, dot( vNormal, vPosToLight ));

	float fAttTotal = ptLight.ConstantAttenuation 
					  + ( ptLight.LinearAttenuation * fDist ) 
					  + ( ptLight.ExponentialAttenuation * fDist * fDist );

	return vec4( ptLight.DiffuseColor * ( ptLight.AmbientColorIntensity + fDiffuse ) / fAttTotal, 1.0 );
}

vec4 GetSpotLightColor( const Light spLight, vec3 vWorldPos, vec3 vNormal )
{
	vec4 final_color;
	mat4 v_inv;
	vec3 normalDirection = normalize( vNormal );
	vec4 inv = v_inv * vec4(0.0, 0.0, 0.0, 1.0);
	vec3 viewDirection = normalize(vec3(inv.xyz - vWorldPos));
	vec3 lightDirection;
	float attenuation;
 
	vec3 positionToLightSource = vec3(spLight.Position - vWorldPos);
	float distance = length(positionToLightSource);
	lightDirection = normalize(positionToLightSource);
	attenuation = 1.0 / (  spLight.ConstantAttenuation
				   + spLight.LinearAttenuation * distance
				   + spLight.ExponentialAttenuation * distance * distance);
 
	if ( spLight.SpotCutOff <= 90.0 ) // spotlight?
	{
		float clampedCosine = max(0.0, dot(-lightDirection, spLight.Direction));
		if (clampedCosine < cos(radians(spLight.SpotCutOff))) // outside of spotlight cone?
		{
			attenuation = 0.0;
		}
		else
		{
			attenuation = attenuation * pow(clampedCosine, spLight.SpotExponent);
		}
	}	
	
	vec3 diffuseReflection = attenuation 
						   * vec3(spLight.DiffuseColor)
						   * max(0.0, dot(normalDirection, lightDirection));
 
	vec3 specularReflection;
	if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
	{
		specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
	}
	else // light source on the right side
	{
		specularReflection = attenuation * vec3(spLight.SpecularColor) /** vec3(frontMaterial.specular) */
								* pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), 1.0f);
	}

	final_color = vec4( diffuseReflection + specularReflection, 1.0 );

	return final_color;
}

/*####################################################################*/

/*######################################################*/
// OUTPUTS
/*######################################################*/
out vec4 o_FragColor;

/*######################################################*/
// PROGRAM
/*######################################################*/
void main()
{	
	// Setting the lights
	lights[0] = light0;
	lights[1] = light1;
	lights[2] = light2;
	lights[3] = light3;
	lights[4] = light4;
	lights[5] = light5;
	lights[6] = light6;
	lights[7] = light7;

	vec3 vNormalized = normalize(vNormal);
	
	vec4 LightColor = vec4(0);
	for (int index = 0; index < u_NumberOfLights; index++) // for all light sources
	{
		if( lights[ index ].Type == 0 )
		{
			LightColor += GetPointLightColor( lights[ index ], VertexPosition_worldspace, vNormalized );
		}

		if( lights[ index ].Type ==  1 )
		{
			LightColor += GetDirectionalLightColor( lights[ index ], vNormal );
		}

		if( lights[ index ].Type ==  2 )
		{
			LightColor += GetSpotLightColor( lights[ index ], VertexPosition_worldspace, vNormalized );
		}
	}

	vec4 MaterialDiffuseColor = texture( DiffuseTextureSampler, texCoord );
	vec4 MaterialEmissiveColor = texture( EmissiveTextureSampler, texCoord );

	o_FragColor = (MaterialDiffuseColor + MaterialEmissiveColor) * LightColor;
}