#include "stdafx.h"
#include "ModelLoaderOBJ.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <string>

#include "TextureManager.h"

namespace
{
	bool MeshCompFunc(const Solidus::TriangleSet &lhs, const Solidus::TriangleSet &rhs)
	{
		return lhs.pMaterial->Alpha > rhs.pMaterial->Alpha;
	}
}

namespace Solidus
{
	ModelLoaderOBJ::ModelLoaderOBJ()
		: m_hasPositions( false ),
		  m_hasNormals( false ),
		  m_hasTangents( false ),
		  m_hasTextureCoords( false ),
		  m_numberOfVertexCoords( 0 ),
		  m_numberOfTextureCoords( 0 ),
		  m_numberOfNormals( 0 ),
		  m_numberOfTriangles( 0 ),
		  m_numberOfMaterials( 0 ),
		  m_numberOfMeshes( 0 ),
		  m_width( 0.0f ),
		  m_height( 0.0f ),
		  m_length( 0.0f ),
		  m_radius( 0.0f )
	{}

	ModelLoaderOBJ::~ModelLoaderOBJ()
	{
		m_hasPositions			= false;
		m_hasTextureCoords		= false;
		m_hasNormals			= false;
		m_hasTangents			= false;

		m_numberOfVertexCoords  = 0;
		m_numberOfTextureCoords = 0;
		m_numberOfNormals		= 0;
		m_numberOfTriangles		= 0;
		m_numberOfMaterials		= 0;
		m_numberOfMeshes		= 0;

		m_center				= Vector3::ZeroVector;
		m_width					= 0.0f;
		m_height				= 0.0f;
		m_length				= 0.0f;
		m_radius				= 0.0f;

		// Clearing STL containers and complex structures		
		m_directoryPath.clear();
		m_triangles.clear();
		m_materials.clear();
		m_vbos.clear();
		m_ibos.clear();
		m_attributeBuffer.clear();
		m_vertexCoords.clear();
		m_textureCoords.clear();
		m_normals.clear();
		m_materialCache.clear();
		m_vertexCache.clear();
	}

	void ModelLoaderOBJ::Bounds( Vector3 center, float& width, float& height,
		float& length, float& radius) const
	{
		float xMax = std::numeric_limits<float>::min();
		float yMax = std::numeric_limits<float>::min();
		float zMax = std::numeric_limits<float>::min();

		float xMin = std::numeric_limits<float>::max();
		float yMin = std::numeric_limits<float>::max();
		float zMin = std::numeric_limits<float>::max();

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		int numVerts = static_cast<int>( m_vbos.size() );

		for (int i = 0; i < numVerts; ++i)
		{
			x = m_vbos[ i ].position[ 0 ];
			y = m_vbos[ i ].position[ 1 ];
			z = m_vbos[ i ].position[ 2 ];

			if (x < xMin) xMin = x;
			if (x > xMax) xMax = x;
			if (y < yMin) yMin = y;
			if (y > yMax) yMax = y;
			if (z < zMin) zMin = z;
			if (z > zMax) zMax = z;
		}

		center.x = ( xMin + xMax ) / 2.0f;
		center.y = ( yMin + yMax ) / 2.0f;
		center.z = ( zMin + zMax ) / 2.0f;

		width    = xMax - xMin;
		height   = yMax - yMin;
		length   = zMax - zMin;

		radius   = std::max( std::max( width, height ), length);
	}

	bool ModelLoaderOBJ::Load(const char *szFilePath, bool rebuildNormals)
	{
		// using stdio to handle files :-P
		FILE *pFile = fopen( szFilePath, "r" );

		if ( !pFile ) return false; // If file failed to open end here.

		
		m_directoryPath.clear();

		std::string filename = szFilePath;
		std::string::size_type offset = filename.find_last_of( '\\' );

		if ( offset != std::string::npos )
		{
			m_directoryPath = filename.substr( 0, ++offset );
		}
		else
		{
			offset = filename.find_last_of( '/' );
			if ( offset != std::string::npos ) m_directoryPath = filename.substr( 0, ++offset );
		}

		// Import the OBJ file.
		//
		
		// Counts how many geometry elements are in the obj
		CountGeometry( pFile );

		// Reset the cursor position in the file. Was previously using seek but this looks cleaner.
		rewind( pFile ); 
		
		// Loads the geometry elements into the structure
		LoadGeometry(pFile);

		// Geometry was calculated. We don't need the file anymore
		fclose( pFile );

		// Perform post import tasks.
		
		// Building the mesh
		BuildTriangleSet();

		// Build vertex normals if required.
		if ( rebuildNormals )
		{
			GenerateNormals();
		}
		else
		{
			if ( !HasNormals() ) GenerateNormals();
		}

		// Build tangents if the obj has bump map file
		for (int i = 0; i < m_numberOfMaterials; ++i)
		{
			if ( !m_materials[i].BumpMapFilename.empty() )
			{
				GenerateTangents();
				break;
			}
		}

		LoadMaterials();

		return true;
	}

	void ModelLoaderOBJ::LoadMaterials()
	{
		// Normalizing!
		//
		Normalize();

		// Loading Materials
		//
		const Solidus::MaterialData *pMaterial = 0;
		int texture = 0;

		// Resetting the variables for re-use
		//
		std::string::size_type offset = 0;
		std::string filename = "";

		for ( int i = 0; i < GetNumberOfMaterials(); ++i )
		{
			pMaterial = &GetMaterial( i );

			if ( pMaterial->DiffuseFilename.empty() ) continue;

			// Try load the texture using the path in the .MTL file.
			//
			texture = Solidus::TextureManager::Instance().GetTexture( pMaterial->DiffuseFilename.c_str() )->Id();

			if ( !texture )
			{
				offset = pMaterial->DiffuseFilename.find_last_of('\\');

				if ( offset != std::string::npos )
					filename = pMaterial->DiffuseFilename.substr( ++offset );
				else
					filename = pMaterial->DiffuseFilename;

				// Try loading the texture from the same directory as the OBJ file.
				texture = Solidus::TextureManager::Instance().GetTexture( ( GetPath() + filename).c_str() )->Id();
			}

			if (texture)
				m_modelTextures[pMaterial->DiffuseFilename] = texture;

			// Look for and load any normal map textures.

			if (pMaterial->BumpMapFilename.empty())
				continue;

			// Try load the texture using the path in the .MTL file.
			texture = Solidus::TextureManager::Instance().GetTexture( pMaterial->BumpMapFilename.c_str() )->Id();

			if (!texture)
			{
				offset = pMaterial->BumpMapFilename.find_last_of('\\');

				if (offset != std::string::npos)
					filename = pMaterial->BumpMapFilename.substr(++offset);
				else
					filename = pMaterial->BumpMapFilename;

				// Try loading the texture from the same directory as the OBJ file.
				texture = Solidus::TextureManager::Instance().GetTexture( ( GetPath() + filename).c_str())->Id();
			}

			if (texture)
				m_modelTextures[ pMaterial->BumpMapFilename ] = texture;

			// Try load the texture using the path in the .MTL file.
			texture = Solidus::TextureManager::Instance().GetTexture( pMaterial->EmissiveFilename.c_str() )->Id();

			if (!texture)
			{
				offset = pMaterial->EmissiveFilename.find_last_of('\\');

				if (offset != std::string::npos)
					filename = pMaterial->EmissiveFilename.substr(++offset);
				else
					filename = pMaterial->EmissiveFilename;

				// Try loading the texture from the same directory as the OBJ file.
				texture = Solidus::TextureManager::Instance().GetTexture( ( GetPath() + filename).c_str())->Id();
			}

			if (texture)
				m_modelTextures[ pMaterial->EmissiveFilename ] = texture;
		}
	}

	void ModelLoaderOBJ::Normalize(float scaleTo, bool center)
	{
		float width       = 0.0f;
		float height      = 0.0f;
		float length      = 0.0f;
		float radius      = 0.0f;
		Vector3 centerPos = Vector3::ZeroVector;

		Bounds( centerPos, width, height, length, radius );

		float scalingFactor = scaleTo / radius;
		float offset[ 3 ] = { 0.0f };

		if (center)
		{
			offset[ 0 ]   = -centerPos[ 0 ];
			offset[ 1 ]   = -centerPos[ 1 ];
			offset[ 2 ]   = -centerPos[ 2 ];
		}
		else
		{
			offset[ 0 ]   = 0.0f;
			offset[ 1 ]   = 0.0f;
			offset[ 2 ]   = 0.0f;
		}

		Scale( scalingFactor, offset );
		Bounds( m_center, m_width, m_height, m_length, m_radius );
	}

	void ModelLoaderOBJ::Scale( float scaleFactor, float offset[ 3 ] /*TODO: Change to Vector3*/ )
	{
		float *pPosition  = 0;

		for( int i = 0; i < static_cast<int>( m_vbos.size() ); ++i )
		{
			pPosition       = m_vbos[ i ].position;

			pPosition[ 0 ] += offset[ 0 ];
			pPosition[ 1 ] += offset[ 1 ];
			pPosition[ 2 ] += offset[ 2 ];
					   	 
			pPosition[ 0 ] *= scaleFactor;
			pPosition[ 1 ] *= scaleFactor;
			pPosition[ 2 ] *= scaleFactor;
		}
	}

	void ModelLoaderOBJ::AddTrianglePos( int index, int material, int x, int y, int z )
	{
		Vertex vertex =
		{
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f
		};

		m_attributeBuffer[ index ] = material;

		vertex.position[ 0 ]       = m_vertexCoords[ x * 3 ];
		vertex.position[ 1 ]       = m_vertexCoords[ x * 3 + 1 ];
		vertex.position[ 2 ]       = m_vertexCoords[ x * 3 + 2 ];
		m_ibos[ index * 3 ]        = AddVertex( x, &vertex );

		vertex.position[ 0 ]       = m_vertexCoords[ y * 3 ];
		vertex.position[ 1 ]       = m_vertexCoords[ y * 3 + 1 ];
		vertex.position[ 2 ]       = m_vertexCoords[ y * 3 + 2 ];
		m_ibos[ index * 3 + 1 ]    = AddVertex( y, &vertex );

		vertex.position[ 0 ]       = m_vertexCoords[ z * 3 ];
		vertex.position[ 1 ]       = m_vertexCoords[ z * 3 + 1 ];
		vertex.position[ 2 ]       = m_vertexCoords[ z * 3 + 2 ];
		m_ibos[ index * 3 + 2 ]    = AddVertex( z, &vertex );
	}

	void ModelLoaderOBJ::AddTrianglePosNormal( int index, int material, 
											   int x, int y, int z, 
											   int nx, int ny, int nz )
	{
		Vertex vertex =
		{
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f
		};

		// TODO: Change this settings to be made with a for loop maybe?
		m_attributeBuffer[ index ] = material;

		// Setting vertex position
		vertex.position[ 0 ]	   = m_vertexCoords[ x * 3 ];
		vertex.position[ 1 ]	   = m_vertexCoords[ x * 3 + 1 ];
		vertex.position[ 2 ]	   = m_vertexCoords[ x * 3 + 2 ];

		// Setting vertex normals
		vertex.normals[ 0 ]		   = m_normals[ nx * 3 ];
		vertex.normals[ 1 ]		   = m_normals[ nx * 3 + 1 ];
		vertex.normals[ 2 ]		   = m_normals[ nx * 3 + 2 ];

		// Adding vertex to ibo
		m_ibos[ index * 3 ]		   = AddVertex( x, &vertex );

		vertex.position[ 0 ]	   = m_vertexCoords[ y * 3 ];
		vertex.position[ 1 ]	   = m_vertexCoords[ y * 3 + 1 ];
		vertex.position[ 2 ]	   = m_vertexCoords[ y * 3 + 2 ];

		// Setting vertex normals
		vertex.normals[ 0 ]        = m_normals[ ny * 3 ];
		vertex.normals[ 1 ]        = m_normals[ ny * 3 + 1 ];
		vertex.normals[ 2 ]        = m_normals[ ny * 3 + 2 ];

		// Adding vertex to ibo
		m_ibos[ index * 3 + 1 ]	   = AddVertex( y, &vertex );

		// Setting vertex position
		vertex.position[ 0 ]	   = m_vertexCoords[ z * 3 ];
		vertex.position[ 1 ]	   = m_vertexCoords[ z * 3 + 1 ];
		vertex.position[ 2 ]	   = m_vertexCoords[ z * 3 + 2 ];

		// Setting vertex normals
		vertex.normals[ 0 ]		   = m_normals[ nz * 3 ];
		vertex.normals[ 1 ]		   = m_normals[ nz * 3 + 1 ];
		vertex.normals[ 2 ]		   = m_normals[ nz * 3 + 2 ];

		// Adding vertex to ibo
		m_ibos[ index * 3 + 2 ]	   = AddVertex( z, &vertex );
	}

	void ModelLoaderOBJ::AddTrianglePosTexCoord( int index, int material, 
												 int x, int y, int z, 
												 int vtx, int vty, int vtz)
	{
		Vertex vertex =
		{
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f
		};

		// TODO: Change this settings to be made with a for loop maybe?
		m_attributeBuffer[ index ] = material;

		// Setting vertex position
		vertex.position[ 0 ]	   = m_vertexCoords[ x * 3 ];
		vertex.position[ 1 ]	   = m_vertexCoords[ x * 3 + 1 ];
		vertex.position[ 2 ]	   = m_vertexCoords[ x * 3 + 2 ];

		// Setting vertex uv's
		vertex.texCoords[ 0 ]	   = m_textureCoords[ vtx * 2 ];
		vertex.texCoords[ 1 ]	   = 1 - m_textureCoords[ vtx * 2 + 1 ]; // Texture fix, coordinates are backwards

		// Adding vertex to ibo
		m_ibos[ index * 3 ]		   = AddVertex( x, &vertex );

		// Setting vertex position
		vertex.position[ 0 ]	   = m_vertexCoords[ y * 3 ];
		vertex.position[ 1 ]	   = m_vertexCoords[ y * 3 + 1 ];
		vertex.position[ 2 ]	   = m_vertexCoords[ y * 3 + 2 ];
		
		// Setting vertex uv's
		vertex.texCoords[ 0 ]      = m_textureCoords[ vty * 2 ];
		vertex.texCoords[ 1 ]      = 1 - m_textureCoords[ vty * 2 + 1 ]; // Texture fix, coordinates are backwards

		// Adding vertex to ibo
		m_ibos[ index * 3 + 1 ]    = AddVertex( y, &vertex );

		// Setting vertex position
		vertex.position[ 0 ]       = m_vertexCoords[ z * 3 ];
		vertex.position[ 1 ]       = m_vertexCoords[ z * 3 + 1 ];
		vertex.position[ 2 ]       = m_vertexCoords[ z * 3 + 2 ];
		
		// Setting vertex uv's
		vertex.texCoords[ 0 ]      = m_textureCoords[ vtz * 2 ];
		vertex.texCoords[ 1 ]      = 1 - m_textureCoords[ vtz * 2 + 1 ]; // Texture fix, coordinates are backwards

		// Adding vertex to ibo
		m_ibos[ index * 3 + 2 ]    = AddVertex( z, &vertex );
	}

	void ModelLoaderOBJ::AddTrianglePosTexCoordNormal( int index, int material, 
													   int x, int y, int z, 
													   int vtx, int vty, int vtz, 
													   int vnx, int vny, int vnz )
	{
		Vertex vertex =
		{
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
		};

		// TODO: Change this settings to be made with a for loop maybe?
		m_attributeBuffer[ index ] = material;

		// Setting vertex position
		vertex.position[ 0 ]       = m_vertexCoords[ x * 3 ];
		vertex.position[ 1 ]       = m_vertexCoords[ x * 3 + 1 ];
		vertex.position[ 2 ]       = m_vertexCoords[ x * 3 + 2 ];

		// Setting vertex uv's
		vertex.texCoords[ 0 ]      = m_textureCoords[ vtx * 2 ];
		vertex.texCoords[ 1 ]      = 1 - m_textureCoords[ vtx * 2 + 1 ]; // Texture fix, coordinates are backwards
		
		// Setting vertex normals
		vertex.normals[ 0 ]        = m_normals[ vnx * 3 ];
		vertex.normals[ 1 ]        = m_normals[ vnx * 3 + 1 ];
		vertex.normals[ 2 ]        = m_normals[ vnx * 3 + 2 ];
		
		// Adding vertex to ibo
		m_ibos[ index * 3 ]        = AddVertex( x, &vertex );

		// Setting vertex position
		vertex.position[ 0 ]       = m_vertexCoords[ y * 3 ];
		vertex.position[ 1 ]       = m_vertexCoords[ y * 3 + 1 ];
		vertex.position[ 2 ]       = m_vertexCoords[ y * 3 + 2 ];

		// Setting vertex uv's
		vertex.texCoords[ 0 ]      = m_textureCoords[ vty * 2 ];
		vertex.texCoords[ 1 ]      = 1 - m_textureCoords[ vty * 2 + 1 ]; // Texture fix, coordinates are backwards
		
		// Setting vertex normals
		vertex.normals[ 0 ]        = m_normals[ vny * 3 ];
		vertex.normals[ 1 ]        = m_normals[ vny * 3 + 1 ];
		vertex.normals[ 2 ]        = m_normals[ vny * 3 + 2 ];
		
		// Adding vertex to ibo
		m_ibos[ index * 3 + 1 ]    = AddVertex( y, &vertex );

		// Setting vertex position
		vertex.position[ 0 ]       = m_vertexCoords[ z * 3 ];
		vertex.position[ 1 ]       = m_vertexCoords[ z * 3 + 1 ];
		vertex.position[ 2 ]       = m_vertexCoords[ z * 3 + 2 ];
		
		// Setting vertex uv's
		vertex.texCoords[ 0 ]      = m_textureCoords[ vtz * 2 ];
		vertex.texCoords[ 1 ]      = 1 - m_textureCoords[ vtz * 2 + 1 ]; // Texture fix, coordinates are backwards
		
		// Setting vertex normals
		vertex.normals[ 0 ]        = m_normals[ vnz * 3 ];
		vertex.normals[ 1 ]        = m_normals[ vnz * 3 + 1 ];
		vertex.normals[ 2 ]        = m_normals[ vnz * 3 + 2 ];
		
		// Adding vertex to ibo
		m_ibos[ index * 3 + 2 ]    = AddVertex( z, &vertex );
	}

	int ModelLoaderOBJ::AddVertex( int hash, const Vertex *pVertex )
	{
		int index = -1;
		std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find( hash );

		if ( iter == m_vertexCache.end() )
		{
			// Vertex hash doesn't exist in the cache.
			index = static_cast<int>( m_vbos.size() );
			m_vbos.push_back( *pVertex );
			m_vertexCache.insert( std::make_pair( hash, std::vector<int>( 1, index )));
		}
		else
		{
			// One or more vertices have been hashed to this entry in the cache.
			const std::vector<int> &vertices = iter->second;
			const Vertex *pCachedVertex = 0;
			bool found = false;

			for ( std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i )
			{
				index = *i;
				pCachedVertex = &m_vbos[index];

				if ( memcmp( pCachedVertex, pVertex, sizeof( Vertex )) == 0 )
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				index = static_cast<int>(m_vbos.size());
				m_vbos.push_back(*pVertex);
				m_vertexCache[hash].push_back(index);
			}
		}

		return index;
	}

	void ModelLoaderOBJ::BuildTriangleSet()
	{
		// Group the model's triangles based on material type.

		TriangleSet *pMesh = 0;
		int materialId     = -1;
		int numMeshes      = 0;

		// Count the number of meshes.
		for ( int i = 0; i < static_cast<int>( m_attributeBuffer.size() ); ++i )
		{
			if ( m_attributeBuffer[i] != materialId )
			{
				materialId = m_attributeBuffer[ i ];
				++numMeshes;
			}
		}

		// Allocate memory for the meshes and reset counters.
		m_numberOfMeshes = numMeshes;		
		numMeshes        = 0;
		materialId       = -1;
		m_triangles.resize( m_numberOfMeshes );

		// Build the meshes. One mesh for each unique material.
		for ( int i = 0; i < static_cast<int>( m_attributeBuffer.size() ); ++i )
		{
			if ( m_attributeBuffer[ i ] != materialId )
			{
				materialId        = m_attributeBuffer[ i ];
				pMesh             = &m_triangles[ numMeshes++ ];            
				pMesh->pMaterial  = &m_materials[ materialId ];
				pMesh->StartIndex = i * 3;
				++pMesh->TriangleCount;
			}
			else
			{
				++pMesh->TriangleCount;
			}
		}

		// Sort the meshes based on its material alpha. Fully opaque meshes
		// towards the front and fully transparent towards the back.
		std::sort(m_triangles.begin(), m_triangles.end(), MeshCompFunc);
	}

	void ModelLoaderOBJ::GenerateNormals()
	{
		const int *pTriangle = 0;
		Vertex *pVertex0     = 0;
		Vertex *pVertex1     = 0;
		Vertex *pVertex2     = 0;
		float edge1[ 3 ]     = { 0.0f, 0.0f, 0.0f };
		float edge2[ 3 ]     = { 0.0f, 0.0f, 0.0f };
		float normal[ 3 ]    = { 0.0f, 0.0f, 0.0f };
		float length         = 0.0f;
		int totalVertices    =  GetNumberOfVertices();
		int totalTriangles   = GetNumberOfTriangles();

		// Initialize all the vertex normals.
		for ( int i = 0; i < totalVertices; ++i )
		{
			pVertex0			   = &m_vbos[ i];
			pVertex0->normals[ 0 ] = 0.0f;
			pVertex0->normals[ 1 ] = 0.0f;
			pVertex0->normals[ 2 ] = 0.0f;
		}

		// Calculate the vertex normals.
		for ( int i = 0; i < totalTriangles; ++i )
		{
			pTriangle = &m_ibos[ i * 3 ];

			pVertex0 = &m_vbos[ pTriangle[ 0 ]];
			pVertex1 = &m_vbos[ pTriangle[ 1 ]];
			pVertex2 = &m_vbos[ pTriangle[ 2 ]];

			// Calculate triangle face normal.

			edge1[ 0 ]  = pVertex1->position[ 0 ] - pVertex0->position[ 0 ];
			edge1[ 1 ]  = pVertex1->position[ 1 ] - pVertex0->position[ 1 ];
			edge1[ 2 ]  = pVertex1->position[ 2 ] - pVertex0->position[ 2 ];
				   	    					 	 					    
			edge2[ 0 ]  = pVertex2->position[ 0 ] - pVertex0->position[ 0 ];
			edge2[ 1 ]  = pVertex2->position[ 1 ] - pVertex0->position[ 1 ];
			edge2[ 2 ]  = pVertex2->position[ 2 ] - pVertex0->position[ 2 ];

			normal[ 0 ] = ( edge1[ 1 ] * edge2[ 2 ] ) - ( edge1[ 2 ] * edge2[ 1 ] );
			normal[ 1 ] = ( edge1[ 2 ] * edge2[ 0 ] ) - ( edge1[ 0 ] * edge2[ 2 ] );
			normal[ 2 ] = ( edge1[ 0 ] * edge2[ 1 ] ) - ( edge1[ 1 ] * edge2[ 0 ] );

			// Accumulate the normals.

			pVertex0->normals[ 0 ] += normal[ 0 ];
			pVertex0->normals[ 1 ] += normal[ 1 ];
			pVertex0->normals[ 2 ] += normal[ 2 ];
							   	 			    
			pVertex1->normals[ 0 ] += normal[ 0 ];
			pVertex1->normals[ 1 ] += normal[ 1 ];
			pVertex1->normals[ 2 ] += normal[ 2 ];
							   	 			    
			pVertex2->normals[ 0 ] += normal[ 0 ];
			pVertex2->normals[ 1 ] += normal[ 1 ];
			pVertex2->normals[ 2 ] += normal[ 2 ];
		}

		// Normalize the vertex normals.
		for ( int i = 0; i < totalVertices; ++i )
		{
			pVertex0 = &m_vbos[ i ];

			length = 1.0f / sqrtf(pVertex0->normals[ 0 ] * pVertex0->normals[ 0 ] +
				pVertex0->normals[1] * pVertex0->normals[ 1 ] +
				pVertex0->normals[2] * pVertex0->normals[ 2 ]);

			pVertex0->normals[ 0 ] *= length;
			pVertex0->normals[ 1 ] *= length;
			pVertex0->normals[ 2 ] *= length;
		}

		m_hasNormals = true;
	}

	void ModelLoaderOBJ::CountGeometry( FILE * pFile )
	{
		m_hasTextureCoords = false;
		m_hasNormals = false;
		m_numberOfVertexCoords = 0;
		m_numberOfTextureCoords = 0;
		m_numberOfNormals = 0;
		m_numberOfTriangles = 0;

		int v = 0;
		int vt = 0;
		int vn = 0;
		char buffer[ 256 ] = { 0 };
		std::string name;

		while ( fscanf( pFile, "%s", buffer) != EOF)
		{
			switch (buffer[0])
			{
			case 'f':   // v, v//vn, v/vt, v/vt/vn.
				fscanf( pFile, "%s", buffer );

				if ( strstr( buffer, "//" ) ) // v//vn
				{
					sscanf(buffer, "%d//%d", &v, &vn);
					fscanf(pFile, "%d//%d", &v, &vn);
					fscanf(pFile, "%d//%d", &v, &vn);
					++m_numberOfTriangles;

					while (fscanf(pFile, "%d//%d", &v, &vn) > 0)
						++m_numberOfTriangles;
				}
				else if (sscanf(buffer, "%d/%d/%d", &v, &vt, &vn) == 3) // v/vt/vn
				{
					fscanf(pFile, "%d/%d/%d", &v, &vt, &vn);
					fscanf(pFile, "%d/%d/%d", &v, &vt, &vn);
					++m_numberOfTriangles;

					while (fscanf(pFile, "%d/%d/%d", &v, &vt, &vn) > 0)
						++m_numberOfTriangles;
				}
				else if (sscanf(buffer, "%d/%d", &v, &vt) == 2) // v/vt
				{
					fscanf(pFile, "%d/%d", &v, &vt);
					fscanf(pFile, "%d/%d", &v, &vt);
					++m_numberOfTriangles;

					while (fscanf(pFile, "%d/%d", &v, &vt) > 0)
						++m_numberOfTriangles;
				}
				else // v
				{
					fscanf(pFile, "%d", &v);
					fscanf(pFile, "%d", &v);
					++m_numberOfTriangles;

					while (fscanf(pFile, "%d", &v) > 0)
						++m_numberOfTriangles;
				}
				break;

			case 'm':   // mtllib
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s %s", buffer, buffer);
				name = m_directoryPath;
				name += buffer;
				ImportMaterials( name.c_str() );
				break;

			case 'v':   // v, vt, or vn
				switch (buffer[1])
				{
				case '\0':
					fgets(buffer, sizeof(buffer), pFile);
					++m_numberOfVertexCoords;
					break;

				case 'n':
					fgets(buffer, sizeof(buffer), pFile);
					++m_numberOfNormals;
					break;

				case 't':
					fgets(buffer, sizeof(buffer), pFile);
					++m_numberOfTextureCoords;

				default:
					break;
				}
				break;

			default:
				fgets(buffer, sizeof(buffer), pFile);
				break;
			}
		}

		m_hasPositions = m_numberOfVertexCoords > 0;
		m_hasNormals = m_numberOfNormals > 0;
		m_hasTextureCoords = m_numberOfTextureCoords > 0;

		// Allocate memory for the OBJ model data.
		m_vertexCoords.resize( m_numberOfVertexCoords * 3 );
		m_textureCoords.resize( m_numberOfTextureCoords * 2 );
		m_normals.resize( m_numberOfNormals * 3 );
		m_ibos.resize( m_numberOfTriangles * 3 );
		m_attributeBuffer.resize( m_numberOfTriangles );

		// Define a default material if no materials were loaded.
		if (m_numberOfMaterials == 0)
		{
			MaterialData defaultMaterial =
			{
				0.2f, 0.2f, 0.2f, 1.0f,
				0.8f, 0.8f, 0.8f, 1.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
				0.0f,
				1.0f,
				std::string("default"),
				std::string(),
				std::string()
			};

			m_materials.push_back( defaultMaterial );
			m_materialCache[ defaultMaterial.Name ] = 0;
		}
	}

	void ModelLoaderOBJ::LoadGeometry( FILE *pFile )
	{
		int v[3]           = { 0 };
		int vt[3]          = { 0 };
		int vn[3]          = { 0 };
		int numVertices    = 0;
		int numTexCoords   = 0;
		int numNormals     = 0;
		int numTriangles   = 0;
		int activeMaterial = 0;
		char buffer[256]   = { 0 };
		std::string name;
		std::map<std::string, int>::const_iterator iter;

		while (fscanf(pFile, "%s", buffer) != EOF)
		{
			switch (buffer[0])
			{
			case 'f': // v, v//vn, v/vt, or v/vt/vn.
				v[0]  = v[1]  = v[2]  = 0;
				vt[0] = vt[1] = vt[2] = 0;
				vn[0] = vn[1] = vn[2] = 0;

				fscanf(pFile, "%s", buffer);

				if (strstr(buffer, "//")) // v//vn
				{
					sscanf(buffer, "%d//%d", &v[0], &vn[0]);
					fscanf(pFile, "%d//%d", &v[1], &vn[1]);
					fscanf(pFile, "%d//%d", &v[2], &vn[2]);

					v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
					v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
					v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

					vn[0] = (vn[0] < 0) ? vn[0] + numNormals - 1 : vn[0] - 1;
					vn[1] = (vn[1] < 0) ? vn[1] + numNormals - 1 : vn[1] - 1;
					vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

					// Tried to create just one function for this but failed
					AddTrianglePosNormal(numTriangles++, activeMaterial,
						v[0], v[1], v[2], vn[0], vn[1], vn[2]);

					v[1] = v[2];
					vn[1] = vn[2];

					while (fscanf(pFile, "%d//%d", &v[2], &vn[2]) > 0)
					{
						v[2]  = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;
						vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

						AddTrianglePosNormal( numTriangles++, activeMaterial,
							v[0], v[1], v[2], vn[0], vn[1], vn[2] );

						v[1]  = v[2];
						vn[1] = vn[2];
					}
				}
				else if (sscanf(buffer, "%d/%d/%d", &v[0], &vt[0], &vn[0]) == 3) // v/vt/vn
				{
					fscanf(pFile, "%d/%d/%d", &v[1], &vt[1], &vn[1]);
					fscanf(pFile, "%d/%d/%d", &v[2], &vt[2], &vn[2]);

					v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
					v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
					v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

					vt[0] = (vt[0] < 0) ? vt[0] + numTexCoords - 1 : vt[0] - 1;
					vt[1] = (vt[1] < 0) ? vt[1] + numTexCoords - 1 : vt[1] - 1;
					vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;

					vn[0] = (vn[0] < 0) ? vn[0] + numNormals - 1 : vn[0] - 1;
					vn[1] = (vn[1] < 0) ? vn[1] + numNormals - 1 : vn[1] - 1;
					vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

					AddTrianglePosTexCoordNormal(numTriangles++, activeMaterial,
						v[0], v[1], v[2], vt[0], vt[1], vt[2], vn[0], vn[1], vn[2]);

					v[1]  = v[2];
					vt[1] = vt[2];
					vn[1] = vn[2];

					while (fscanf(pFile, "%d/%d/%d", &v[2], &vt[2], &vn[2]) > 0)
					{
						v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;
						vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;
						vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

						AddTrianglePosTexCoordNormal(numTriangles++, activeMaterial,
							v[0], v[1], v[2], vt[0], vt[1], vt[2], vn[0], vn[1], vn[2]);

						v[1] = v[2];
						vt[1] = vt[2];
						vn[1] = vn[2];
					}
				}
				else if (sscanf(buffer, "%d/%d", &v[0], &vt[0]) == 2) // v/vt
				{
					fscanf(pFile, "%d/%d", &v[1], &vt[1]);
					fscanf(pFile, "%d/%d", &v[2], &vt[2]);

					v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
					v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
					v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

					vt[0] = (vt[0] < 0) ? vt[0] + numTexCoords - 1 : vt[0] - 1;
					vt[1] = (vt[1] < 0) ? vt[1] + numTexCoords - 1 : vt[1] - 1;
					vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;

					AddTrianglePosTexCoord(numTriangles++, activeMaterial,
						v[0], v[1], v[2], vt[0], vt[1], vt[2]);

					v[1] = v[2];
					vt[1] = vt[2];

					while (fscanf(pFile, "%d/%d", &v[2], &vt[2]) > 0)
					{
						v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;
						vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;

						AddTrianglePosTexCoord(numTriangles++, activeMaterial,
							v[0], v[1], v[2], vt[0], vt[1], vt[2]);

						v[1] = v[2];
						vt[1] = vt[2];
					}
				}
				else // v
				{
					sscanf(buffer, "%d", &v[0]);
					fscanf(pFile, "%d", &v[1]);
					fscanf(pFile, "%d", &v[2]);

					v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
					v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
					v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

					AddTrianglePos(numTriangles++, activeMaterial, v[0], v[1], v[2]);

					v[1] = v[2];

					while (fscanf(pFile, "%d", &v[2]) > 0)
					{
						v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

						AddTrianglePos(numTriangles++, activeMaterial, v[0], v[1], v[2]);

						v[1] = v[2];
					}
				}
				break;

			case 'u': // usemtl
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s %s", buffer, buffer);
				name = buffer;
				iter = m_materialCache.find(buffer);
				activeMaterial = (iter == m_materialCache.end()) ? 0 : iter->second;
				break;

			case 'v': // v, vn, or vt.
				switch (buffer[1])
				{
				case '\0': // v
					fscanf(pFile, "%f %f %f",
						&m_vertexCoords[3 * numVertices],
						&m_vertexCoords[3 * numVertices + 1],
						&m_vertexCoords[3 * numVertices + 2]);
					++numVertices;
					break;

				case 'n': // vn
					fscanf(pFile, "%f %f %f",
						&m_normals[3 * numNormals],
						&m_normals[3 * numNormals + 1],
						&m_normals[3 * numNormals + 2]);
					++numNormals;
					break;

				case 't': // vt
					fscanf(pFile, "%f %f",
						&m_textureCoords[2 * numTexCoords],
						&m_textureCoords[2 * numTexCoords + 1]);
					++numTexCoords;
					break;

				default:
					break;
				}
				break;

			default:
				fgets(buffer, sizeof(buffer), pFile);
				break;
			}
		}
	}

	bool ModelLoaderOBJ::ImportMaterials(const char *szFilePath)
	{
		FILE *pFile = fopen( szFilePath, "r" );

		if ( !pFile ) return false;

		MaterialData *pMaterial = 0;
		int illum = 0;
		int numMaterials = 0;
		char buffer[256] = {0};

		// Count the number of materials in the MTL file.
		while (fscanf(pFile, "%s", buffer) != EOF)
		{
			switch (buffer[0])
			{
			case 'n': // newmtl
				++numMaterials;
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s %s", buffer, buffer);
				break;

			default:
				fgets(buffer, sizeof(buffer), pFile);
				break;
			}
		}

		rewind(pFile);

		m_numberOfMaterials = numMaterials;
		numMaterials = 0;
		m_materials.resize(m_numberOfMaterials);

		// Load the materials in the MTL file.
		while (fscanf(pFile, "%s", buffer) != EOF)
		{
			switch (buffer[0])
			{
			case 'N': // Ns
				fscanf(pFile, "%f", &pMaterial->Shininess);

				// Wavefront .MTL file shininess is from [0,1000].
				// Scale back to a generic [0,1] range.
				pMaterial->Shininess /= 1000.0f;
				break;

			case 'K': // Ka, Kd, or Ks
				switch (buffer[1])
				{
				case 'a': // Ka
					fscanf(pFile, "%f %f %f",
						&pMaterial->Ambient[0],
						&pMaterial->Ambient[1],
						&pMaterial->Ambient[2]);
					pMaterial->Ambient[3] = 1.0f;
					break;

				case 'd': // Kd
					fscanf(pFile, "%f %f %f",
						&pMaterial->Diffuse[0],
						&pMaterial->Diffuse[1],
						&pMaterial->Diffuse[2]);
					pMaterial->Diffuse[3] = 1.0f;
					break;

				case 's': // Ks
					fscanf(pFile, "%f %f %f",
						&pMaterial->Specular[0],
						&pMaterial->Specular[1],
						&pMaterial->Specular[2]);
					pMaterial->Specular[3] = 1.0f;
					break;

				default:
					fgets(buffer, sizeof(buffer), pFile);
					break;
				}
				break;

			case 'T': // Tr
				switch (buffer[1])
				{
				case 'r': // Tr
					fscanf(pFile, "%f", &pMaterial->Alpha);
					pMaterial->Alpha = 1.0f - pMaterial->Alpha;
					break;

				default:
					fgets(buffer, sizeof(buffer), pFile);
					break;
				}
				break;

			case 'd':
				fscanf(pFile, "%f", &pMaterial->Alpha);
				break;

			case 'i': // illum
				fscanf(pFile, "%d", &illum);

				if (illum == 1)
				{
					pMaterial->Specular[0] = 0.0f;
					pMaterial->Specular[1] = 0.0f;
					pMaterial->Specular[2] = 0.0f;
					pMaterial->Specular[3] = 1.0f;
				}
				break;

			case 'm': // map_Kd, map_bump
				if(strstr(buffer, "map_Kd") != 0)
				{
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%s %s", buffer, buffer);
					pMaterial->DiffuseFilename = buffer;
				}
				else if(strstr(buffer, "map_bump") != 0)
				{
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%s %s", buffer, buffer);
					pMaterial->BumpMapFilename = buffer;
				}
				else if(strstr(buffer, "map_height") != 0)
				{
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%s %s", buffer, buffer);
					pMaterial->HeightMapFilename = buffer;
				}
				else if( strstr(buffer, "map_Ke") != 0 )
				{
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%s %s", buffer, buffer);
					pMaterial->EmissiveFilename = buffer;
				}
				else if( strstr(buffer, "map_Ks") != 0 )
				{
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%s %s", buffer, buffer);
					pMaterial->SpecularFilename = buffer;
				}
				else
				{
					fgets(buffer, sizeof(buffer), pFile);
				}
				break;

			case 'n': // newmtl
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s %s", buffer, buffer);

				pMaterial = &m_materials[numMaterials];
				pMaterial->Ambient[0] = 0.2f;
				pMaterial->Ambient[1] = 0.2f;
				pMaterial->Ambient[2] = 0.2f;
				pMaterial->Ambient[3] = 1.0f;
				pMaterial->Diffuse[0] = 0.8f;
				pMaterial->Diffuse[1] = 0.8f;
				pMaterial->Diffuse[2] = 0.8f;
				pMaterial->Diffuse[3] = 1.0f;
				pMaterial->Specular[0] = 0.0f;
				pMaterial->Specular[1] = 0.0f;
				pMaterial->Specular[2] = 0.0f;
				pMaterial->Specular[3] = 1.0f;
				pMaterial->Shininess = 0.0f;
				pMaterial->Alpha = 1.0f;
				pMaterial->Name = buffer;
				pMaterial->DiffuseFilename.clear();
				pMaterial->BumpMapFilename.clear();

				m_materialCache[pMaterial->Name] = numMaterials;
				++numMaterials;
				break;

			default:
				fgets(buffer, sizeof(buffer), pFile);
				break;
			}
		}

		fclose(pFile);
		return true;
	}

	void ModelLoaderOBJ::Unload()
	{
		m_hasPositions	   = false;
		m_hasTextureCoords = false;
		m_hasNormals	   = false;
		m_hasTangents	   = false;

		m_numberOfVertexCoords = 0;
		m_numberOfTextureCoords = 0;
		m_numberOfNormals = 0;
		m_numberOfTriangles = 0;
		m_numberOfMaterials = 0;
		m_numberOfMeshes = 0;

		m_center[0] = m_center[1] = m_center[2] = 0.0f;
		m_width = m_height = m_length = m_radius = 0.0f;

		m_directoryPath.clear();

		m_triangles.clear();
		m_materials.clear();
		m_vbos.clear();
		m_ibos.clear();
		m_attributeBuffer.clear();

		m_vertexCoords.clear();
		m_textureCoords.clear();
		m_normals.clear();

		m_materialCache.clear();
		m_vertexCache.clear();

		// Clearing Textures
		//
		auto i = m_modelTextures.begin();

		while (i != m_modelTextures.end())
		{
			glDeleteTextures(1, &i->second);
			++i;
		}

		m_modelTextures.clear();
	}

	void ModelLoaderOBJ::Draw()
	{
		const Solidus::TriangleSet *pMesh = 0;
		const Solidus::MaterialData *pMaterial = 0;
		const Solidus::Vertex *pVertices = 0;
		std::map<std::string, GLuint>::const_iterator iter;

		for (int i = 0; i < GetNumberOfTriangleSets(); ++i)
		{
			pMesh = &GetTriangle(i);
			pMaterial = pMesh->pMaterial;
			pVertices = GetVertexBuffer();

			/*
			############################################################################
				BUFFER BINDING MAGIC!
			############################################################################
			*/

			//glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,  pMaterial->Ambient);
			//glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,  pMaterial->Diffuse);
			//glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, pMaterial->Specular);
			//glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,pMaterial->Shininess * 128.0f);

			// Load textures
			iter = m_modelTextures.find( pMaterial->DiffuseFilename );

			if ( iter == m_modelTextures.end() )
			{
				glDisable( GL_TEXTURE_2D );
			}
			else
			{
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, iter->second );
			}

			if ( HasPositions() )
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glVertexPointer( 3, GL_FLOAT, sizeof( Solidus::Vertex ),
					pVertices->position );
			}

			if ( HasTextureCoords() )
			{
				glEnableClientState( GL_TEXTURE_COORD_ARRAY );
				glTexCoordPointer( 2, GL_FLOAT, sizeof( Solidus::Vertex ),
					pVertices->texCoords );
			}

			if ( HasNormals() )
			{
				glEnableClientState( GL_NORMAL_ARRAY );
				glNormalPointer( GL_FLOAT, sizeof( Solidus::Vertex ),
					pVertices->normals );
			}

			glDrawElements(GL_TRIANGLES, pMesh->TriangleCount * 3, GL_UNSIGNED_INT, // When using short the portal_gun does not render properly
				GetIndexBuffer() + pMesh->StartIndex);

			if ( HasNormals() )
			{
				glDisableClientState(GL_NORMAL_ARRAY);
			}

			if ( HasTextureCoords() )
			{
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}

			if ( HasPositions() )
			{
				glDisableClientState(GL_VERTEX_ARRAY);	
			}
		}
	}

	void ModelLoaderOBJ::GenerateTangents()
	{
		const int *pTriangle = 0;
		Vertex *pVertex0     = 0;
		Vertex *pVertex1     = 0;
		Vertex *pVertex2     = 0;
		float edge1[ 3 ]     = { 0.0f, 0.0f, 0.0f };
		float edge2[ 3 ]     = { 0.0f, 0.0f, 0.0f };
		float texEdge1[ 2 ]  = { 0.0f, 0.0f };
		float texEdge2[ 2 ]  = { 0.0f, 0.0f };
		float tangent[ 3 ]   = { 0.0f, 0.0f, 0.0f };
		float bitangent[ 3 ] = { 0.0f, 0.0f, 0.0f };
		float determinant            = 0.0f;
		float nDotT          = 0.0f;
		float bDotB          = 0.0f;
		float length		 = 0.0f;
		int totalVertices	 = GetNumberOfVertices();
		int totalTriangles   = GetNumberOfTriangles();

		// Initialize all the vertex tangents and bitangents.
		for( int i = 0; i < totalVertices; ++i )
		{
			pVertex0 = &m_vbos[ i ];

			pVertex0->tangent[ 0 ]   = 0.0f;
			pVertex0->tangent[ 1 ]   = 0.0f;
			pVertex0->tangent[ 2 ]   = 0.0f;
			pVertex0->tangent[ 3 ]   = 0.0f;

			pVertex0->bitangent[ 0 ] = 0.0f;
			pVertex0->bitangent[ 1 ] = 0.0f;
			pVertex0->bitangent[ 2 ] = 0.0f;
		}

		// Calculate the vertex tangents and bitangents.
		for( int i = 0; i < totalTriangles; ++i )
		{
			pTriangle = &m_ibos[ i * 3 ];

			pVertex0 = &m_vbos[ pTriangle[ 0 ]];
			pVertex1 = &m_vbos[ pTriangle[ 1 ]];
			pVertex2 = &m_vbos[ pTriangle[ 2 ]];

			// Calculate the triangle face tangent and bitangent.
			edge1[ 0 ] = pVertex1->position[ 0 ] - pVertex0->position[ 0 ];
			edge1[ 1 ] = pVertex1->position[ 1 ] - pVertex0->position[ 1 ];
			edge1[ 2 ] = pVertex1->position[ 2 ] - pVertex0->position[ 2 ];
  
			edge2[ 0 ] = pVertex2->position[ 0 ] - pVertex0->position[ 0 ];
			edge2[ 1 ] = pVertex2->position[ 1 ] - pVertex0->position[ 1 ];
			edge2[ 2 ] = pVertex2->position[ 2 ] - pVertex0->position[ 2 ];

			texEdge1[ 0 ] = pVertex1->texCoords[ 0 ] - pVertex0->texCoords[ 0 ];
			texEdge1[ 1 ] = pVertex1->texCoords[ 1 ] - pVertex0->texCoords[ 1 ];

			texEdge2[ 0 ] = pVertex2->texCoords[ 0 ] - pVertex0->texCoords[ 0 ];
			texEdge2[ 1 ] = pVertex2->texCoords[ 1 ] - pVertex0->texCoords[ 1 ];

			determinant = texEdge1[ 0 ] * texEdge2[ 1 ] - texEdge2[ 0 ] * texEdge1[ 1 ];

			if ( fabs( determinant ) < 1e-6f )
			{
				tangent[ 0 ]   = 1.0f;
				tangent[ 1 ]   = 0.0f;
				tangent[ 2 ]   = 0.0f;

				bitangent[ 0 ] = 0.0f;
				bitangent[ 1 ] = 1.0f;
				bitangent[ 2 ] = 0.0f;
			}
			else
			{
				determinant = 1.0f / determinant;

				tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * determinant;
				tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * determinant;
				tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * determinant;

				bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * determinant;
				bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * determinant;
				bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * determinant;
			}

			// Accumulate the tangents and bitangents.

			pVertex0->tangent[ 0 ]   += tangent[ 0 ];
			pVertex0->tangent[ 1 ]   += tangent[ 1 ];
			pVertex0->tangent[ 2 ]   += tangent[ 2 ];
			pVertex0->bitangent[ 0 ] += bitangent[ 0 ];
			pVertex0->bitangent[ 1 ] += bitangent[ 1 ];
			pVertex0->bitangent[ 2 ] += bitangent[ 2 ];

			pVertex1->tangent[0] += tangent[0];
			pVertex1->tangent[1] += tangent[1];
			pVertex1->tangent[2] += tangent[2];
			pVertex1->bitangent[0] += bitangent[0];
			pVertex1->bitangent[1] += bitangent[1];
			pVertex1->bitangent[2] += bitangent[2];

			pVertex2->tangent[0] += tangent[0];
			pVertex2->tangent[1] += tangent[1];
			pVertex2->tangent[2] += tangent[2];
			pVertex2->bitangent[0] += bitangent[0];
			pVertex2->bitangent[1] += bitangent[1];
			pVertex2->bitangent[2] += bitangent[2];
		}

		// Orthogonalize and normalize the vertex tangents.
		for (int i = 0; i < totalVertices; ++i)
		{
			pVertex0 = &m_vbos[i];

			// Gram-Schmidt orthogonalize tangent with normal.
			nDotT = pVertex0->normals[0] * pVertex0->tangent[0] +
				pVertex0->normals[1] * pVertex0->tangent[1] +
				pVertex0->normals[2] * pVertex0->tangent[2];

			pVertex0->tangent[0] -= pVertex0->normals[0] * nDotT;
			pVertex0->tangent[1] -= pVertex0->normals[1] * nDotT;
			pVertex0->tangent[2] -= pVertex0->normals[2] * nDotT;

			// Normalize the tangent.

			length = 1.0f / sqrtf(pVertex0->tangent[0] * pVertex0->tangent[0] +
				pVertex0->tangent[1] * pVertex0->tangent[1] +
				pVertex0->tangent[2] * pVertex0->tangent[2]);

			pVertex0->tangent[0] *= length;
			pVertex0->tangent[1] *= length;
			pVertex0->tangent[2] *= length;

			// Calculate the handedness of the local tangent space.
			// The bitangent vector is the cross product between the triangle face
			// normal vector and the calculated tangent vector. The resulting
			// bitangent vector should be the same as the bitangent vector
			// calculated from the set of linear equations above. If they point in
			// different directions then we need to invert the cross product
			// calculated bitangent vector. We store this scalar multiplier in the
			// tangent vector's 'w' component so that the correct bitangent vector
			// can be generated in the normal mapping shader's vertex shader.
			//
			// Normal maps have a left handed coordinate system with the origin
			// located at the top left of the normal map texture. The x coordinates
			// run horizontally from left to right. The y coordinates run
			// vertically from top to bottom. The z coordinates run out of the
			// normal map texture towards the viewer. Our handedness calculations
			// must take this fact into account as well so that the normal mapping
			// shader's vertex shader will generate the correct bitangent vectors.
			// Some normal map authoring tools such as Crazybump
			// (http://www.crazybump.com/) includes options to allow you to control
			// the orientation of the normal map normal's y-axis.

			bitangent[0] = (pVertex0->normals[1] * pVertex0->tangent[2]) - 
				(pVertex0->normals[2] * pVertex0->tangent[1]);
			bitangent[1] = (pVertex0->normals[2] * pVertex0->tangent[0]) -
				(pVertex0->normals[0] * pVertex0->tangent[2]);
			bitangent[2] = (pVertex0->normals[0] * pVertex0->tangent[1]) - 
				(pVertex0->normals[1] * pVertex0->tangent[0]);

			bDotB = bitangent[0] * pVertex0->bitangent[0] + 
				bitangent[1] * pVertex0->bitangent[1] + 
				bitangent[2] * pVertex0->bitangent[2];

			pVertex0->tangent[3] = (bDotB < 0.0f) ? 1.0f : -1.0f;

			pVertex0->bitangent[0] = bitangent[0];
			pVertex0->bitangent[1] = bitangent[1];
			pVertex0->bitangent[2] = bitangent[2];
		}

		m_hasTangents = true;
	}

	void ModelLoaderOBJ::ReverseWinding()
	{
		int swap = 0;

		// Reverse face winding.
		for( int i = 0; i < static_cast<int>( m_ibos.size() ); i += 3 )
		{
			swap			= m_ibos[ i + 1 ];
			m_ibos[ i + 1 ] = m_ibos[i + 2];
			m_ibos[ i + 2 ] = swap;
		}

		float *pNormal  = 0;
		float *pTangent = 0;

		// Invert normals and tangents.
		for( int i = 0; i < static_cast<int>( m_vbos.size() ); ++i )
		{
			pNormal       =  m_vbos[ i ].normals;
			pNormal[ 0 ]  = -pNormal[ 0 ];
			pNormal[ 1 ]  = -pNormal[ 1 ];
			pNormal[ 2 ]  = -pNormal[ 2 ];

			pTangent      =  m_vbos[ i ].tangent;
			pTangent[ 0 ] = -pTangent[ 0 ];
			pTangent[ 1 ] = -pTangent[ 1 ];
			pTangent[ 2 ] = -pTangent[ 2 ];
		}
	}
}