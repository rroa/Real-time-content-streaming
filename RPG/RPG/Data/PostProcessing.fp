#version 330


/*######################################################*/
// INPUTS
/*######################################################*/

// Textures
in vec2 texCoord;
in vec2 offsetLeftTexCoord;
in vec2 offsetRightTexCoord;
in vec2 offsetTopTexCoord;
in vec2 offsetBottomTexCoord;

/*######################################################*/
// UNIFORMS
/*######################################################*/

// Textures
uniform sampler2D ColorTextureSampler;
uniform sampler2D DepthTextureSampler;

uniform int       u_effectType;
uniform float	  u_time;
uniform float	  u_width;
uniform float	  u_height;
uniform vec2      u_tcOffset[ 25 ]; // Texture coordinate offsets

// Uniform Constants
uniform float vx_offset             = 0.5;
uniform float divisionThickness     = 0.001;
uniform float stitching_size        = 6.0;
uniform int	  invert			    = 0;
uniform float rnd_factor			= 0.05;
uniform float rnd_scale				= 5.1;
uniform vec2 v1						= vec2(92.,80.);
uniform vec2 v2						= vec2(41.,62.);
// Swirl effect parameters
uniform float radius				= 200.0;
uniform float angle					= 0.8;

// Depth Test
const float blurclamp = 3.0;
const float bias      = 0.6;
uniform float focus   = 0.5;

/*######################################################*/
// OUTPUTS
/*######################################################*/
out vec4 o_FragColor;

/*######################################################*/
// FUNCTIONS
/*######################################################*/
vec4 Swirl( sampler2D textureSampler, vec2 uv )
{
	vec2 center	  = vec2( u_width * 0.5, u_height * 0.5 );
	vec2 texSize	  = vec2( u_width, u_height );
	vec2 coords	  = uv * texSize;
	
	coords         -= center;
	float dist      = length( coords );
	
	if (dist < radius) 
	{
		float percent = ( radius - dist ) / radius;
		float theta   = percent * percent * angle * 8.0;
		float s		  = sin( theta );
		float c		  = cos( theta );
		coords		  = vec2( dot( coords, vec2( c, -s )), dot( coords, vec2( s, c )));
	}
	
	coords	     += center;
	vec3 color	  = texture( textureSampler, coords / texSize ).rgb;

	return vec4( color, 1.0 );
}

float RandomCoords( vec2 coords )
{
	return fract( sin( dot( coords.xy, v1 )) + cos( dot( coords.xy, v2 )) * rnd_scale );
}

vec4 CrossStitching( sampler2D textureSampler, vec2 uv )
{
	vec4 color = vec4( 0.0 ); // Black
	float size = stitching_size; // Size of the stitch
	
	vec2 centeredPos = uv * vec2( u_width, u_height );
	vec2 tlPos = floor( centeredPos / vec2( size, size ));
	tlPos *= size;

	int remX = int( mod( centeredPos.x, size ));
	int remY = int( mod( centeredPos.y, size ));
	if( remX == 0 && remY == 0 )
	{	
		tlPos = centeredPos;
	}
	
	vec2 blPos = tlPos;
	blPos.y += (size - 1.0);
	if (( remX == remY ) || 
	   ((( int( centeredPos.x ) - int( blPos.x )) == ( int( blPos.y ) - int( centeredPos.y )))))
	{
		if (invert == 1)
		{
			color = vec4(0.2, 0.15, 0.05, 1.0);
		}
		else
		{
		  color = texture( textureSampler, tlPos * vec2( 1.0 /u_width, 1.0 / u_height )) * 1.4;
		}
	}
	else
	{
		if (invert == 1)
		{	
			color = texture( textureSampler, tlPos * vec2( 1.0/u_width, 1.0/u_height )) * 1.4;
		}
		else
		{
			color = vec4( 0.0, 0.0, 0.0, 1.0 );
		}
	}
	return color;
}

float CalcLuminanceForColor( vec3 color )
{	
	// Convert to grayscale using NTSC weightings
	// Weighting source: http://www.varioustopics.com/image-processing/1006655-rgb-conversion-to-8-bit-gray-scale.html
	const vec3 luminanceWeightPerRGB = vec3( 0.299, 0.587, 0.114 );
	//const vec4 luminanceWeightPerRGB = vec3( 0.2125, 0.7154, 0.0721 );
	return dot( color, luminanceWeightPerRGB );
}


void main(void)
{
	// Standard
	if( u_effectType == 0 )
	{
		o_FragColor = texture( ColorTextureSampler, texCoord );		
	}

	if( u_effectType == 1 )
	{
		o_FragColor = texture( DepthTextureSampler, texCoord );		
	}

	if( u_effectType == 2 )
	{
		if ( texCoord.x < 0.125 )
		{
			// 240:252:248
			o_FragColor = vec4( 0.9418, 0.9882, 0.9725, 1.0 );
		}
		else if( texCoord.x >= 0.125 && texCoord.x < 0.25 )
		{	
			// 240:252:32
			o_FragColor = vec4( 0.9418, 0.9882, 0.1255, 1.0 );
		}
		else if( texCoord.x >= 0.25 && texCoord.x < 0.375 )
		{
			// 48:252:248
			o_FragColor = vec4( 0.1882, 0.9882, 0.9725, 1.0 );
		}
		else if( texCoord.x >= 0.375 && texCoord.x < 0.5 )
		{
			// 40:252:0
			o_FragColor = vec4( 0.1569, 0.9882, 0.0, 1.0 );
		}
		else if( texCoord.x >= 0.5 && texCoord.x < 0.625 )
		{
			// 248:0:248
			o_FragColor = vec4( 0.9725, 0.0, 0.9725, 1.0 );
		}
		else if( texCoord.x >= 0.625 && texCoord.x < 0.75 )
		{
			// 248:32:112
			o_FragColor = vec4( 0.9725, 0.1255, 0.4392, 1.0 );
		}
		else if( texCoord.x >= 0.75 && texCoord.x < 0.875 )
		{
			// 72:44:248
			o_FragColor = vec4( 0.2824, 0.1725, 0.9725, 1.0 );
		}
		else
		{
			// 64:64:136
			o_FragColor = vec4( 0.2509, 0.2509, 0.5333, 1.0 );
		}
	}

	// Grayscale
	if( u_effectType == 3 )
	{
		float luminance = CalcLuminanceForColor( texture( ColorTextureSampler, texCoord).rgb );//dot( texture( ColorTextureSampler, texCoord).rgb, vec3(0.299, 0.587, 0.114));
		o_FragColor = vec4( luminance, luminance, luminance, 1.0 );
	}
 
	// Sepia tone
	if( u_effectType == 4 )
	{
		float luminance = CalcLuminanceForColor( texture( ColorTextureSampler, texCoord).rgb );//dot(texture( ColorTextureSampler, texCoord ).rgb, vec3( 0.299, 0.587, 0.114 ));
 
		// Played around with the rgb colors until I got a brownish color, prolly a little more red could
		// make it more sepia tonish
		o_FragColor = vec4( luminance * vec3(1.5, 1.0, 0.8), 1.0);
	}
 
	// Negative
	if( u_effectType == 5 )
	{
		vec4 color = texture( ColorTextureSampler, texCoord );
		o_FragColor = vec4( 1.0 - color.rgb, 1.0);
	}
 
	// Blur (gaussian)
	if( u_effectType == 6 )
	{
		vec4 sample[ 25 ];
		for ( int i = 0; i < 25; ++i )
		{
			// Sample a grid around and including our texel
			sample[ i ] = texture( ColorTextureSampler, texCoord + u_tcOffset[i] );
		}
		
		// Convolution Filter (convolution kernel): Concept grabbed from OpenGL 4.0 Shading Language Cookbook
		// Gaussian weighting kernel:
		// 1  4  7  4 1
		// 4 16 26 16 4
		// 7 26 41 26 7 / 273 (i.e. divide by total of weightings)
		// 4 16 26 16 4
		// 1  4  7  4 1
		o_FragColor = (
					   (1.0  * (sample[0] + sample[4]  + sample[20] + sample[24])) +
					   (4.0  * (sample[1] + sample[3]  + sample[5]  + sample[9] + sample[15] + sample[19] + sample[21] + sample[23])) +
					   (7.0  * (sample[2] + sample[10] + sample[14] + sample[22])) +
					   (16.0 * (sample[6] + sample[8]  + sample[16] + sample[18])) +
					   (26.0 * (sample[7] + sample[11] + sample[13] + sample[17])) +
					   (41.0 * sample[12])
					   ) / 273.0;
 
	}
 
	// Blur (median filter)
	if ( u_effectType == 7 )
	{
		o_FragColor = vec4( 0.0 ); // Black
		for (int i = 0; i < 25; i++)
		{
			// Sample the offset grid around and including our texel
			o_FragColor += texture( ColorTextureSampler, texCoord + u_tcOffset[ i ] );
		}
		o_FragColor /= 25;
	}
 
	// Sharpen
	if ( u_effectType == 8 )
	{
		vec4 sample[ 25 ];
		for( int i = 0; i < 25; ++i )
		{
			// Sample a grid around and including our texel
			sample[ i ] = texture( ColorTextureSampler, texCoord + u_tcOffset[ i ] );
		}

		// Convolution Filter (convolution kernel): Concept grabbed from OpenGL 4.0 Shading Language Cookbook
		// Sharpen weighting kernel:
		// -1 -1 -1 -1 -1
		// -1 -1 -1 -1 -1
		// -1 -1 25 -1 -1
		// -1 -1 -1 -1 -1
		// -1 -1 -1 -1 -1
 
		o_FragColor = 25.0 * sample[ 12 ];
		for( int i = 0; i < 25; ++i )
		{
			if (i != 12)
			{
				o_FragColor -= sample[ i ];
			}
		}
	}
 
	// Dilate
	if ( u_effectType == 9 )
	{
		vec4 sample[ 25 ];
		vec4 maxValue = vec4( 0.0 ); // Black
 
		for ( int i = 0; i < 25; ++i )
		{
			// Sample the offset grid around and including our texel
			sample[ i ] = texture( ColorTextureSampler, texCoord + u_tcOffset[ i ] );
 
			// Keep the maximum value		
			maxValue = max( sample[ i ], maxValue );
		}
		o_FragColor = maxValue;
	}
 
	// Erode
	if( u_effectType == 10 )
	{
		vec4 sample[ 25 ];
		vec4 minValue = vec4( 1.0 );
		for( int i = 0; i < 25; ++i )
		{
			// Sample the offset grid around and including our texel
			sample[ i ] = texture( ColorTextureSampler, texCoord + u_tcOffset[ i ] );

			// Keep the minimum value		
			minValue = min( sample[ i ], minValue );
		}
		o_FragColor = minValue;
	}
 
	// Toon
	if ( u_effectType == 11 )
	{
		vec4 colorTexel = texture( ColorTextureSampler, texCoord );
		if( colorTexel.r < 0.7 )
		{
			colorTexel.r = 0.0;
		}
		else
		{
			colorTexel.r = 1.0;
		}
		
		if( colorTexel.g < 0.7 )
		{
			colorTexel.g = 0.0;
		}
		else
		{
			colorTexel.g = 1.0;
		}

		if( colorTexel.b < 0.7 )
		{
			colorTexel.b = 0.0;
		}
		else
		{
			colorTexel.b = 1.0;
		}

		o_FragColor = colorTexel;
		o_FragColor += colorTexel; // Color saturation attempt
	}

	// Wobbly
	if( u_effectType == 12 )
	{		
		o_FragColor = texture( ColorTextureSampler, 
						 texCoord + 0.005 * vec2( sin( u_time + u_width * texCoord.x),
											cos( u_time + u_height * texCoord.y )));
	}

	// Edge detection
	if( u_effectType == 13 )
	{		
		o_FragColor = texture( ColorTextureSampler, texCoord );		
		vec4 diffuse_color  = texture( ColorTextureSampler, texCoord );
		vec4 diffuse_colorL = texture( ColorTextureSampler, offsetLeftTexCoord );
		vec4 diffuse_colorR = texture( ColorTextureSampler, offsetRightTexCoord );
		vec4 diffuse_colorA = texture( ColorTextureSampler, offsetTopTexCoord );
		vec4 diffuse_colorB = texture( ColorTextureSampler, offsetBottomTexCoord );
		
		o_FragColor = ( diffuse_colorL + diffuse_colorR + diffuse_colorA +
				diffuse_colorB ) - ( 4.0 * diffuse_color );
						
		o_FragColor = vec4( o_FragColor.r, o_FragColor.r, o_FragColor.r, 1.0 );
		o_FragColor.a = 1.0;
	}

	// Watery
	if( u_effectType == 14 )
	{		
		vec2 distortedTexCoords = texCoord;
		distortedTexCoords.x   += 0.02 * sin( 2 * u_time + 15 * texCoord.y );
		o_FragColor             = texture( ColorTextureSampler, distortedTexCoords );
	}

	// CrossStitching
	if( u_effectType == 15 )
	{		
		if (texCoord.y > 0.5)
		{
			o_FragColor = CrossStitching( ColorTextureSampler, texCoord );
		}
		else
		{
			vec2 coords = vec2( texCoord.x, texCoord.y + 0.5 );
			vec4 color  = texture(ColorTextureSampler, coords );
			o_FragColor = color;			
		}
	}

	// Custom Crazy Effect
	if( u_effectType == 16 )
	{		
		vec2 coords = texCoord.xy;
		vec2 p      = -1.0 + 2.0 * coords;
		float len   = length( p );
		vec2 uv     = coords + ( p/len ) * cos( len * 12.0 - u_time * 4.0 ) * 0.03;
		vec3 col    = texture(ColorTextureSampler,uv).xyz;
		o_FragColor = vec4(col,1.0);		
	}

	// Custom Crazy Effect
	if( u_effectType == 17 )
	{	
		vec2 uv = texCoord.xy;  
		vec3 division = vec3(0.0, 0.0, 0.0);
		if ( uv.x < ( vx_offset - divisionThickness ))
		{
			vec2 rnd = vec2( RandomCoords(uv.xy), RandomCoords( uv.yx ));  
			division = texture( ColorTextureSampler, uv + rnd * rnd_factor ).rgb;  
		}
		else if (uv.x>=( vx_offset + divisionThickness ))
		{
			division = texture(ColorTextureSampler, uv).rgb;
		}
  
		o_FragColor = vec4( division, 1.0 );			
	}

	// Swirl
	if( u_effectType == 18 )
	{		
		vec2 coords = texCoord.xy;
		o_FragColor = Swirl( ColorTextureSampler, coords );
	}

	// Thermal
	if( u_effectType == 19 )
	{				
		vec2 coords   = texCoord.xy;
		vec3 division = vec3( 0.0, 0.0, 0.0 ); // Black
		if( coords.x < ( vx_offset - divisionThickness ))
		{
			vec3 colors[ 3 ];
			vec3 pixcol = texture( ColorTextureSampler, coords ).rgb;			
			colors[0]   = vec3( 0.0, 0.0, 1.0 );
			colors[1]   = vec3( 1.0, 1.0, 0.0 );
			colors[2]   = vec3( 1.0, 0.0, 0.0);
			float lum   = ( pixcol.r + pixcol.g + pixcol.b ) / 3.0; // Color blend
			int ix      = ( lum < 0.5 ) ? 0 : 1; // if the blend is intense then reflect it.
			division    = mix( colors[ ix ], colors[ ix + 1 ], (lum - float( ix ) * 0.5) / 0.5 );
		}
		else if ( coords.x >= ( vx_offset + divisionThickness ))
		{
			division = texture( ColorTextureSampler, coords ).rgb;
		}
		o_FragColor = vec4( division, 1.0 );
	}

	// Scope
	if( u_effectType == 20 )
	{
		vec2 midpoint = vec2( u_width * 0.5, u_height * 0.5 );
		float radius  = min( u_width, u_height ) * 0.5;
		
		float dist    = distance( texCoord.xy, midpoint.xy );
		float circle  = smoothstep(radius - 1.0, radius + 1.0, dist);
		
		if(( texCoord.x * texCoord.x ) + ( texCoord.y * texCoord.y ) < ( vx_offset + 0.005 ))
			o_FragColor = texture(ColorTextureSampler, /*vec2( texCoord.x - 0.5, texCoord - 0.5 )*/texCoord);
		else
			o_FragColor = vec4(0.0);
				
		//vec2 lensRadius = vec2( 0.5, 0.28 );
		//vec4 Color      = texture( ColorTextureSampler, texCoord );
		//float dist      = distance( texCoord.xy, vec2( 0.5, 0.5 ));
		//Color.rgb	   *= smoothstep( lensRadius.x, lensRadius.y, dist );
		//o_FragColor		= Color;
	}

	if( u_effectType == 21 )
	{	
		float aspectratio = u_width / u_height;
		vec2 aspectcorrect = vec2( 1.0, aspectratio );
	   
		vec4 depth   = texture( DepthTextureSampler, texCoord );
 
		float factor = ( depth.x - focus );
		 
		vec2 dofblur = vec2 ( clamp( factor * bias, -blurclamp, blurclamp ));
 
		vec4 color = vec4( 0.0 ); // Black
	   
		color += texture(ColorTextureSampler, texCoord);
		color += texture(ColorTextureSampler, texCoord + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur);
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur);
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur);
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur);       
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur);   
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur);       
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur);       
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur);
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur); 
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur);
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur);
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur); 
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur); 
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur);       
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur);       
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur);
	   
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur*0.9);
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur*0.9);           
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur*0.9);           
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur*0.9);
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur*0.9);
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur*0.9);            
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur*0.9);   
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur*0.9);   
		//
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.7);
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.7);       
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.7);   
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.7);     
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.7);
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.7);     
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.7);   
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.7);
						 
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.4);
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.4);       
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.4);   
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.4);     
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.4);
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.4);     
		//col += texture(ColorTextureSampler, texCoord + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.4);   
		//col += texture(ColorTextureSampler, texCoord + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.4);       
					   
		o_FragColor   = color / 2.0;
		o_FragColor.a = 1.0;
	}

	if( u_effectType == 22 )
	{
		float sampleTexCoordOffsetX = 1 / u_width;
		float sampleTexCoordOffsetY = 1 / u_height;
		int checkRadius = 2;		
		
		float minDepth = 1.0;
		float maxDepth = 0.0;
		
		for( int yOffset = -checkRadius; yOffset <= checkRadius; ++yOffset )
		{
			for( int xOffset = -checkRadius; xOffset <= checkRadius; ++xOffset )
			{
				vec2 offsetTexCoords = texCoord + vec2( sampleTexCoordOffsetX * float( xOffset ), sampleTexCoordOffsetY * float( yOffset ) );
				vec4 depthBufferTexel = texture( DepthTextureSampler, offsetTexCoords );
				float nonLinearDepth = depthBufferTexel.r;
				float linearDepth = 1.0 - sqrt(1.0 - nonLinearDepth);
				maxDepth = max(linearDepth, maxDepth);
				minDepth = min(linearDepth, minDepth);
			}
		}

		float treshold = 0.01;
		if( ( maxDepth - minDepth ) > treshold )
		{
			//pixel black
			//o_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
			o_FragColor = vec4( 1.0 );
		}
		else
		{
			//white
			//o_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );
			o_FragColor = vec4( 0.0 );
		}
	}
}