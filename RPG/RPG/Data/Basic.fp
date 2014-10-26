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
uniform bool u_UseParallax;

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

/*######################################################*/
// OUTPUTS
/*######################################################*/
layout(location = 0) out vec4 o_FragColor;

/*######################################################*/
// FUNCTIONS
/*######################################################*/
float stepmix(float edge0, float edge1, float E, float x)
{
	float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
	return mix(edge0, edge1, T);
}

float GetFogFactor( FogParameters params, float fFogCoord )
{
	float fogIntensity = 0.0;
	if( params.Mode == 0 ) // linear
		fogIntensity = ( params.End - fFogCoord ) / ( params.End - params.Start );
	else if( params.Mode == 1 ) // exp
		fogIntensity = exp( -params.Density * fFogCoord );
	else if( params.Mode == 2 ) // exp modified
		fogIntensity = exp( -pow( params.Density * fFogCoord, 2.0 ));
		
	fogIntensity = 1.0 - clamp( fogIntensity, 0.0, 1.0 );
	
	return fogIntensity;
}

vec3 computeFireEmissive() 
{
	//Rangemap Position on size of model
	float relativeHeight = 0.5 + 0.5 * VertexPosition_worldspace.z;
	
	float relativeTime1 = ( 0.21 * u_time ) - ( 0.11 * VertexPosition_worldspace.z ) - ( 0.01 * sin( -1.0 * u_time + 13.0 * VertexPosition_worldspace.x )) + ( 0.02 * sin( 1.3 + 7.0 * VertexPosition_worldspace.y ));
	float relativeTime2 = ( 0.37 * u_time ) - ( 0.23 * VertexPosition_worldspace.z ) - ( 0.02 * sin( 3.2 * u_time + 6.0 * VertexPosition_worldspace.x )) + ( 0.01 * sin( 1.3 + 11.0 * VertexPosition_worldspace.y ) );
	float relativeTime3 = ( 0.19 * u_time ) - ( 0.17 * VertexPosition_worldspace.z ) - ( 0.01 * sin( -2.7 * u_time + 4.0 * VertexPosition_worldspace.x )) + ( 0.01 * sin( 1.3 + 7.0 * VertexPosition_worldspace.y ));
	float relativeTime4 = ( 0.43 * u_time ) - ( 0.37 * VertexPosition_worldspace.z ) - ( 0.01 * sin( 4.7 * u_time + 8.0 * VertexPosition_worldspace.x )) + ( 0.01 * sin( 1.3 + 5.0 * VertexPosition_worldspace.y ));
	
	vec2 noiseTexCoords1 = vec2( relativeTime1, 0.1 );
	vec2 noiseTexCoords2 = vec2( relativeTime2, 0.2 );
	vec2 noiseTexCoords3 = vec2( relativeTime3, 0.3 );
	vec2 noiseTexCoords4 = vec2( relativeTime4, 0.4 );
	
	vec4 noiseTexel1 = texture2D( DiffuseTextureSampler, noiseTexCoords1 );
	vec4 noiseTexel2 = texture2D( DiffuseTextureSampler, noiseTexCoords2 );
	vec4 noiseTexel3 = texture2D( DiffuseTextureSampler, noiseTexCoords3 );
	vec4 noiseTexel4 = texture2D( DiffuseTextureSampler, noiseTexCoords4 );
	
	float noiseIntensity1 = noiseTexel1.r;
	float noiseIntensity2 = noiseTexel2.r;
	float noiseIntensity3 = noiseTexel3.r;
	float noiseIntensity4 = noiseTexel4.r;
	
	//float totalNoise = 0.25 * ( noiseIntensity1 + noiseIntensity2 + noiseIntensity3 + noiseIntensity4 );
	float totalNoise = ( noiseIntensity1 * noiseIntensity1 + noiseIntensity2 * noiseIntensity2 + noiseIntensity3 * noiseIntensity3 + noiseIntensity4 * noiseIntensity4 );
	
	float fireIntensity = max( 0.0, ( 0.3 + totalNoise ) * ( 1.0 - relativeHeight ));
	float red = fireIntensity;
	float green = red * red * red;
	float blue = green * green * green;
	return vec3( red, green, blue );
}

/*######################################################*/
// PROGRAM
/*######################################################*/
void main()
{
	// Material Default properties
	vec2 bumpTexCoord;
	vec4 MaterialDiffuseColor;
	vec4 MaterialAmbientColor;
	vec4 MaterialEmissiveColor;
	vec4 MaterialSpecularColor;
	
	if( UseDiffuseTextureSampler )
	{
		MaterialDiffuseColor = texture( DiffuseTextureSampler, texCoord );
		MaterialAmbientColor = vec4( 0.1, 0.1, 0.1, 1.0 ) * MaterialDiffuseColor;
	}

	if( UseEmissiveTextureSampler )
	{
		MaterialEmissiveColor = texture( EmissiveTextureSampler, texCoord );		
	}

	if( u_UseParallax )
	{
		
		vec3 cameraDirection = normalize( EyeDirection_tangentspace );
		float height = texture( HeightTextureSampler, texCoord ).r;		
		height = height * u_pParallaxParams.Scale + u_pParallaxParams.Bias;
		cameraDirection.y *= -1.0; // Flipping the axis
		bumpTexCoord = texCoord + ( height * cameraDirection.xy );		
	}
	else
	{
		bumpTexCoord = texCoord;
	}

	// Bumps!
	vec4 TextureNormal_tangentspace = normalize( texture( NormalTextureSampler, vec2( bumpTexCoord.x, bumpTexCoord.y ) ) * 2.0 - 1.0 );

	// Distance to the light
	float lightDistance = length( LightPosition_worldspace - VertexPosition_worldspace );	

	// Normal of the computed fragment, in camera space
	vec4 normal = TextureNormal_tangentspace;
	
	// Direction of the light (from the fragment to the light)
	vec4 dirToLight = normalize( vec4( LightDirection_tangentspace, 0.0 ));

	float cosTheta = clamp( dot( normal, dirToLight ), 0, 1 );

	// Eye vector (towards the camera)
	vec4 eye = normalize( vec4( EyeDirection_tangentspace, 0.0 ));

	// Direction in which the triangle reflects the light
	vec4 reflection = reflect( -dirToLight, normal );	

	// Setting the lights
	lights[0] = light0;
	lights[1] = light1;
	lights[2] = light2;
	lights[3] = light3;
	lights[4] = light4;
	lights[5] = light5;
	lights[6] = light6;
	lights[7] = light7;	
	
	vec4 LightColor = vec4(0);
	for (int index = 0; index < u_NumberOfLights; index++) // for all light sources
	{
		if( lights[ index ].Type == 0 )
		{
			LightColor += GetPointLightColor( lights[ index ], VertexPosition_worldspace, normal.xyz );
		}

		if( lights[ index ].Type ==  1 )
		{
			LightColor += GetDirectionalLightColor( lights[ index ], normal.xyz );
		}

		if( lights[ index ].Type ==  2 )
		{
			LightColor += GetSpotLightColor( lights[ index ], VertexPosition_worldspace, normal.xyz );
		}
	}


	if( UseNormalTextureSampler )
	{
		MaterialDiffuseColor = ( MaterialDiffuseColor * LightColor/*u_LightColor * u_LightPower * cosTheta / (lightDistance * lightDistance )*/);
	}	

	vec4 specColor;
	if( UseSpecularTextureSampler )
	{
		// Specular
		float specularIntensity = 10.0;//dot( reflection,  eye);
		specularIntensity		= clamp( specularIntensity, 0.0, 1.0 );	

		specularIntensity 		= pow( specularIntensity, 32 * 1.0 );
		specularIntensity 	    *= 1.0;
	
		specColor = vec4( specularIntensity * /*MaterialSpecularColor.rgb*/vec3(1.0), 1.0 ) * LightColor;			
	}	

	o_FragColor = MaterialAmbientColor +
				  MaterialEmissiveColor +	
				  MaterialDiffuseColor +
				  specColor;				 
				 	
	if( u_UseSpecialEffect )
	{
		vec4 specialEffectColor = vec4( computeFireEmissive(), 1.0 ) 
								* LightColor /** u_LightPower * cosTheta / (lightDistance * lightDistance )*/;
		vec4 whiteColor = vec4( 1.0, 1.0, 1.0, 1.0 );
		if( specialEffectColor.r < 0.5 && specialEffectColor.g < 0.5 )
			discard;
					
		o_FragColor = MaterialAmbientColor + 
					  MaterialEmissiveColor;	 
		o_FragColor += specialEffectColor;
	}

	// Fog blend always at the end
	if( u_UseFog )
	{	
		// Add fog	
		float fFogCoord = abs( VertexPosition_cameraspace.z / VertexPosition_cameraspace.w );

		// Lerp it!
		o_FragColor = mix( o_FragColor, u_fogParams.Color, GetFogFactor( u_fogParams, fFogCoord ));
	}
}