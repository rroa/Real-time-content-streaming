#pragma once

#include <string>
#include <sstream>
#include <utility>

#include "Shader.h"
#include "Settings.h"
#include "TextureManager.h"
#include "ReportErrors.h"
#include "Camera.h"
#include "Light.h"

namespace Solidus
{
	struct Material
	{
		// CTOR
		//
		Material( const char * szMaterialName )
			: Name( szMaterialName )			
			, Shininess( 0.0f )
			, Alpha( 1.0f )
			, LightsOff( false )
			, Ambient( Vector4( 1.0f, 0.1f, 0.1f, 1.0f ))
		{}

		// FUNCTIONS
		//
		void SetupMatrices( const Camera& _camera )
		{
			int amountOfMatrices = 5;
			std::string names[] = { "u_matrices.MVP", 
									"u_matrices.Model", 
									"u_matrices.View", 
									"u_matrices.Projection", 
									"u_matrices.ModelView" };

			Matrix44 ProjectionMatrix = _camera.ProjectionMatrix();
			Matrix44 ViewMatrix       = _camera.ViewMatrix();
			Matrix44 ModelMatrix      = Matrix44::MakeIdentity();
			Matrix44 ModelViewMatrix  = ViewMatrix * ModelMatrix;
			Matrix44 MVP			  = ProjectionMatrix * ViewMatrix * ModelMatrix;
			
			std::vector< const float * > matrixValues;
			matrixValues.push_back( MVP.GetMatrix() );
			matrixValues.push_back( ModelMatrix.GetMatrix() );
			matrixValues.push_back( ViewMatrix.GetMatrix() );
			matrixValues.push_back( ProjectionMatrix.GetMatrix() );
			matrixValues.push_back( ModelViewMatrix.GetMatrix() );

			for( int i = 0; i < amountOfMatrices; ++i )
			{
				GLUniform matrix;				
				if( CurrentShader->GetUniform( names[ i ], matrix ) )
				{
					glUniformMatrix4fv( matrix.Location, 1, GL_FALSE, matrixValues[ i ] );
				}
			}

			GLUniform cameraPos;						
			if( CurrentShader->GetUniform( "u_CameraPosition_worldspace", cameraPos ))
			{
				Vector3 position( _camera.Position() );
				glUniform3f( cameraPos.Location, position.x, position.y, position.z );
			}

			GLUniform cameraDir;						
			if( CurrentShader->GetUniform( "u_CameraDirection_worldspace", cameraDir ))
			{				
				Vector3 direction( _camera.ViewVector() );
				glUniform3f( cameraDir.Location, direction.x, direction.y, direction.z );
			}
		}

		void SetupLights( const std::vector< Light >& _lights  )
		{
			// Amount of lights
			const int amountOfLights = ( _lights.size() > 8 ) ? 8 : _lights.size();
			GLUniform u_NumberOfLights;
			if( CurrentShader->GetUniform( "u_NumberOfLights", u_NumberOfLights ))
			{				
				glUniform1i( u_NumberOfLights.Location, amountOfLights );
			}

			for( int i = 0; i < amountOfLights; ++i )
			{
				std::stringstream buffer;
				std::string lightName = "light";
				buffer << lightName << i;				

				GLUniform u_LightDirection;
				if( CurrentShader->GetUniform( buffer.str() + ".Direction", u_LightDirection ))
				{
					Direction lightDir = _lights[ i ].GetDirection();
					glUniform3f( u_LightDirection.Location, lightDir.x, lightDir.y, lightDir.z );
				}

				GLUniform u_LightPosition;
				if( CurrentShader->GetUniform( buffer.str() + ".Position", u_LightPosition ))
				{
					Position lightPos = _lights[ i ].GetPosition();
					glUniform3f( u_LightPosition.Location, lightPos.x, lightPos.y, lightPos.z );
				}

				GLUniform u_LightOnOff;
				if( CurrentShader->GetUniform( buffer.str() + ".OnOff", u_LightOnOff ))
				{
					glUniform1i( u_LightDirection.Location, _lights[ i ].GetOnOffState() );
				}

				GLUniform u_LightType;

				if( CurrentShader->GetUniform( buffer.str() + ".Type", u_LightType ))
				{
					glUniform1i( u_LightType.Location, static_cast< int >( _lights[ i ].GetLightType()));
				}

				GLUniform u_LightDiffuseColor;
				if( CurrentShader->GetUniform( buffer.str() + ".DiffuseColor", u_LightDiffuseColor ))
				{
					Color diffuse = _lights[ i ].GetDiffuseColor();
					glUniform3f( u_LightDiffuseColor.Location, diffuse.x, diffuse.y, diffuse.z );
				}

				GLUniform u_LightSpecularColor;
				if( CurrentShader->GetUniform( buffer.str() + ".SpecularColor", u_LightSpecularColor ))
				{
					Color specular = _lights[ i ].GetSpecularColor();
					glUniform3f( u_LightSpecularColor.Location, specular.x, specular.y, specular.z );
				}

				GLUniform u_LightAmbientColor;
				if( CurrentShader->GetUniform( buffer.str() + ".AmbientColor", u_LightAmbientColor ))
				{
					Color ambient = _lights[ i ].GetAmbientColor();
					glUniform3f( u_LightAmbientColor.Location, ambient.x, ambient.y, ambient.z );
				}

				GLUniform  u_LightSpotCutOff;
				if( CurrentShader->GetUniform( buffer.str() + ".SpotCutOff", u_LightSpotCutOff ))
				{
					glUniform1f( u_LightSpotCutOff.Location, _lights[ i ].GetSpotCutOff() );
				}

				GLUniform  u_LightSpotExponent;
				if( CurrentShader->GetUniform( buffer.str() + ".SpotExponent", u_LightSpotExponent ))
				{
					glUniform1f( u_LightSpotExponent.Location, _lights[ i ].GetSpotExponent() );
				}

				GLUniform  u_LightConstantAttenuation;
				if( CurrentShader->GetUniform( buffer.str() + ".ConstantAttenuation", u_LightConstantAttenuation ))
				{
					glUniform1f( u_LightConstantAttenuation.Location, _lights[ i ].GetConstantAttenuation() );
				}

				GLUniform  u_LightLinearAttenuation;
				if( CurrentShader->GetUniform( buffer.str() + ".LinearAttenuation", u_LightLinearAttenuation ))
				{
					glUniform1f( u_LightLinearAttenuation.Location, _lights[ i ].GetLinearAttenuation() );
				}

				GLUniform  u_LightExponentialAttenuation;
				if( CurrentShader->GetUniform( buffer.str() + ".ExponentialAttenuation", u_LightExponentialAttenuation ))
				{
					glUniform1f( u_LightExponentialAttenuation.Location, _lights[ i ].GetExponentialAttenuation() );
				}

				GLUniform  u_LightAmbientColorIntensity;
				if( CurrentShader->GetUniform( buffer.str() + ".AmbientColorIntensity", u_LightAmbientColorIntensity ))
				{
					glUniform1f( u_LightAmbientColorIntensity.Location, _lights[ i ].GetAmbientColorIntensity() );
				}

				GLUniform  u_LightPower;
				if( CurrentShader->GetUniform( buffer.str() + ".Power", u_LightPower ))
				{
					glUniform1f( u_LightPower.Location, _lights[ i ].GetPower() );
				}
			}
			//// Position
			////
			//GLUniform lightPos;
			//if( CurrentShader->GetUniform( "u_LightPosition_worldspace", lightPos ))
			//{
			//	Vector3 position( _light.GetPosition() );
			//	glUniform3f( lightPos.Location, position.x, position.y, position.z );
			//}

			//// Power
			////
			//GLUniform lightPow;
			//if( CurrentShader->GetUniform( "u_LightPower", lightPow ))
			//{				
			//	glUniform1f( lightPow.Location, _light.GetPower() );
			//}

			//// Color
			////
			//GLUniform lightColor;
			//if( CurrentShader->GetUniform("u_LightColor", lightColor ))
			//{
			//	Color color( _light.GetColor() );
			//	color = color * ( _light.GetOnOffState() ? 1.0f : 0.0f );
			//	glUniform4f( lightColor.Location, color.x, color.y, color.z, color.w );
			//}

			//// #####################################################################
			//// POINT
			//// #####################################################################
			//GLUniform vColorPt;
			//if( CurrentShader->GetUniform( "ptLight.vColor", vColorPt ))
			//{				
			//	glUniform3f( vColorPt.Location, 1.0f, 0.0f, 1.0f );
			//}

			//GLUniform vPositionPt;
			//if( CurrentShader->GetUniform( "ptLight.vPosition", vPositionPt ))
			//{		
			//	Vector3 position( _light.GetPosition() );
			//	glUniform3f( vPositionPt.Location, position.x, position.y, position.z );
			//}

			//GLUniform fAmbientPt;
			//if( CurrentShader->GetUniform( "ptLight.fAmbient", fAmbientPt ))
			//{				
			//	glUniform1f( fAmbientPt.Location, 0.15f );
			//}
			//
			//GLUniform fConstantAttPt;
			//if( CurrentShader->GetUniform( "ptLight.fConstantAtt", fConstantAttPt ))
			//{				
			//	glUniform1f( fConstantAttPt.Location, 0.3f );
			//}

			//GLUniform fLinearAttPt;
			//if( CurrentShader->GetUniform( "ptLight.fLinearAtt", fLinearAttPt ))
			//{				
			//	glUniform1f( fLinearAttPt.Location, 0.007f );
			//}

			//GLUniform fExpAttPt;
			//if( CurrentShader->GetUniform( "ptLight.fExpAtt", fExpAttPt ))
			//{				
			//	glUniform1f( fExpAttPt.Location, 0.00008f );
			//}

			//// #####################################################################
			//// SPOT
			//// #####################################################################
			//GLUniform vColorSp;
			//if( CurrentShader->GetUniform( "spotLight.vColor", vColorSp ))
			//{				
			//	glUniform3f( vColorSp.Location, 1.0f, 1.0f, 1.0f );
			//}
			//
			//Vector3 vSpotLightPos = Vector3( 0.0f, 5.0f, 0.0f );
			//Vector3 vCameraDir = Vector3(0.0f,0.0f, -1.0f);
			//vCameraDir.Normalize();			
			//vSpotLightPos.y -= 3.2f;
			//Vector3 vSpotLightDir = (vSpotLightPos+vCameraDir*75.0f)-vSpotLightPos;
			//vSpotLightDir.Normalize();
			//Vector3 vHorVector = Vector3::CrossProduct(_camera.ViewVector() - _camera.Position(), Vector3( 0.0f, 1.0f, 0.0f ));
			//vSpotLightPos += vHorVector*3.3f;

			//GLUniform vPositionSp;
			//if( CurrentShader->GetUniform( "spotLight.vPosition", vPositionSp ))
			//{				
			//	glUniform3f( vPositionSp.Location, vSpotLightPos.x, vSpotLightPos.y, vSpotLightPos.z );
			//}

			//GLUniform vDirectionSp;
			//if( CurrentShader->GetUniform( "spotLight.vDirection", vDirectionSp ))
			//{				
			//	glUniform3f( vDirectionSp.Location, vSpotLightDir.x, vSpotLightDir.y, vSpotLightDir.z );
			//}
			//
			//GLUniform bOnSp;
			//if( CurrentShader->GetUniform( "spotLight.bOn", bOnSp ))
			//{				
			//	glUniform1i( bOnSp.Location, 1 );
			//}

			//GLUniform fConeAngleSp;
			//if( CurrentShader->GetUniform( "spotLight.fConeAngle", fConeAngleSp ))
			//{				
			//	glUniform1f( fConeAngleSp.Location, 15.0f );
			//}

			//GLUniform fConeCosineSp;
			//if( CurrentShader->GetUniform( "spotLight.fConeCosine", fConeCosineSp ))
			//{				
			//	glUniform1f( fConeCosineSp.Location, float(cos(15.0f*3.1415f/180.0f)) );
			//}
			//
			//GLUniform fLinearAttSp;
			//if( CurrentShader->GetUniform( "spotLight.fLinearAtt", fLinearAttSp ))
			//{				
			//	glUniform1f( fLinearAttSp.Location, 0.0170f );
			//}			
		}

		void SetupFog( GLfloat fDensity_, GLfloat fStart_, GLfloat fEnd_, Vector4 fColor_, GLint fMode_ )
		{
			// Density
			//
			GLUniform fogDensity;
			if( CurrentShader->GetUniform("u_fogParams.Density", fogDensity ))
			{				
				glUniform1f( fogDensity.Location, fDensity_ );
			}

			// Start
			//
			GLUniform fogStart;
			if( CurrentShader->GetUniform("u_fogParams.Start", fogStart ))
			{				
				glUniform1f( fogStart.Location, fStart_ );
			}

			// End
			//
			GLUniform fogEnd;
			if( CurrentShader->GetUniform("u_fogParams.End", fogEnd ))
			{				
				glUniform1f( fogEnd.Location, fEnd_ );
			}

			// Color
			//
			GLUniform fogColor;
			if( CurrentShader->GetUniform("u_fogParams.Color", fogColor ))
			{				
				glUniform4f( fogColor.Location, fColor_.x, fColor_.y, fColor_.z, fColor_.w );
			}

			// Mode
			//
			GLUniform fogCalculationMode;
			if( CurrentShader->GetUniform("u_fogParams.Mode", fogCalculationMode ))
			{				
				glUniform1i( fogCalculationMode.Location, fMode_ );
			}
		}

		void SetupParallax( float bias_, float scale_ )
		{
			// Bias
			//
			GLUniform parallaxBias;
			if( CurrentShader->GetUniform("u_pParallaxParams.Bias", parallaxBias ))
			{				
				glUniform1f( parallaxBias.Location, bias_ );
			}

			// Scale
			//
			GLUniform parallaxScale;
			if( CurrentShader->GetUniform("u_pParallaxParams.Scale", parallaxScale ))
			{				
				glUniform1f( parallaxScale.Location, scale_ );
			}
		}

		void SetupTextures()
		{
			for( auto tex = Textures.begin(); tex != Textures.end(); ++tex ) 
			{				
				TextureType type_ = (*tex).first;
				int textuerId     = (*tex).second;

				std::string uniformName = Settings::Instance().GetTextureUniformName( type_ );

				GLUniform useTexture;
				CurrentShader->GetUniform( "Use" + uniformName, useTexture );			

				GLUniform texture;
				bool bindTexture = CurrentShader->GetUniform( uniformName, texture );
				if( !bindTexture ) 
				{
					glUniform1i( useTexture.Location, 0 );
					continue;			
				}

				glUniform1i( useTexture.Location, 1 );
				glUniform1i( texture.Location, type_  );

				glActiveTexture( GL_TEXTURE8 );
				glDisable( GL_TEXTURE_2D );

				glActiveTexture( GL_TEXTURE4 );
				glDisable( GL_TEXTURE_2D );

				glActiveTexture( GL_TEXTURE3 );
				glDisable( GL_TEXTURE_2D );

				glActiveTexture( GL_TEXTURE2 );
				glDisable( GL_TEXTURE_2D );

				glActiveTexture( GL_TEXTURE1 );
				glDisable( GL_TEXTURE_2D );

				glActiveTexture( GL_TEXTURE0 );
				glDisable( GL_TEXTURE_2D );		

				if( (*tex).first == HEIGHTMAP_TEXTURE )
					glActiveTexture( GL_TEXTURE4 );
				if( (*tex).first == EMISSIVE_TEXTURE )
					glActiveTexture( GL_TEXTURE3 );
				if( (*tex).first == SPECULAR_TEXTURE )
					glActiveTexture( GL_TEXTURE2 );
				if( (*tex).first == NORMALMAP_TEXTURE )
					glActiveTexture( GL_TEXTURE1 );
				if( (*tex).first == DIFFUSE_TEXTURE )
					glActiveTexture( GL_TEXTURE0 );

				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, textuerId );
			}
		}

		void SetTexture( const TextureType& type_, const char * szTexturePath )
		{
			int textureId = Solidus::TextureManager::Instance().GetTexture( szTexturePath )->Id();
			Textures.insert( std::pair< TextureType, int >( type_, textureId ));
		}

		void UseShader( const std::shared_ptr< Shader >& shader )
		{			
			CurrentShader = shader;

			// Default settings per material
			//
			GLUniform u_alpha;
			if( CurrentShader->GetUniform( "u_Alpha", u_alpha ))
			{
				glUniform1f( u_alpha.Location, Alpha );
			}	

			GLUniform u_shininess;
			if( CurrentShader->GetUniform( "u_Shininess", u_shininess ))
			{
				glUniform1f( u_shininess.Location, Shininess );
			}

			GLUniform u_ambient;
			if( CurrentShader->GetUniform( "u_Ambient", u_ambient ))
			{
				glUniform4f( u_ambient.Location, Ambient.x, Ambient.y, 
					Ambient.z, Ambient.w ); // TODO: This is not being sent properly. Check out why
			}
		}

		// MEMBERS
		//		
		std::string Name;
		std::shared_ptr< Shader > CurrentShader;	
		std::map< TextureType, int > Textures;
		
		float Shininess;
		float Alpha;
		bool  LightsOff;

		// Default Colors
		Vector4 Ambient;
	};
}