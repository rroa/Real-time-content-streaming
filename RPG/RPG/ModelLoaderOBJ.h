#pragma once

#include "Vertex.h"
#include "MaterialDataOBJ.h"
#include "Triangles.h"
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include "Vector3.h"

namespace Solidus
{
	class ModelLoaderOBJ
	{
	public:
		// CTOR
		//
		ModelLoaderOBJ();
		~ModelLoaderOBJ();

		/* 
		############################################################################
			INLINE GETTERS 
		############################################################################
		*/
		//
		inline void				  GetCenter( float &x, float &y, float &z ) const { x = m_center[0]; y = m_center[1]; z = m_center[2]; }
		inline float			  GetWidth() const				     { return m_width; }
		inline float			  GetHeight() const				     { return m_height; }
		inline float			  GetLength() const				     { return m_length; }
		inline float			  GetRadius() const				     { return m_radius; }
		
		// Indices
		//
		inline const int*         GetIndexBuffer() const			 { return &m_ibos[0]; }
		inline int				  GetIndexSize() const				 { return static_cast<int>(sizeof(int)); }

		// Materials
		//
		inline const MaterialData&    GetMaterial( int i ) const		 { return m_materials[ i ]; }

		// Mesh
		//
		inline const TriangleSet& GetTriangle( int i ) const	     { return m_triangles[ i ]; }

		// Buffers
		//
		inline int				  GetNumberOfIndices() const         { return m_numberOfTriangles * 3; }
		inline int				  GetNumberOfMaterials() const		 { return m_numberOfMaterials; }
		inline int				  GetNumberOfTriangleSets() const	 { return m_numberOfMeshes; }
		inline int				  GetNumberOfTriangles() const		 { return m_numberOfTriangles; }
		inline int				  GetNumberOfVertices() const		 { return static_cast<int>( m_vbos.size() ); }

		// File Paths
		//
		inline const std::string& GetPath() const				     { return m_directoryPath; }

		// Vertices's
		//
		const Vertex&			  GetVertex(int i) const;
		inline const Vertex*	  GetVertexBuffer() const			 { return &m_vbos[ 0 ]; }
		inline int				  GetVertexSize() const			     { return static_cast<int>(sizeof(Vertex)); }

		// Buffer Bools
		//
		inline bool				  HasNormals() const				 { return m_hasNormals; }
		inline bool				  HasPositions() const				 { return m_hasPositions; }
		inline bool				  HasTangents() const				 { return m_hasTangents; }
		inline bool				  HasTextureCoords() const			 { return m_hasTextureCoords; }		

		/*
		############################################################################
		PUBLIC FUNCTIONS
		############################################################################
		*/
		void					  Draw();
		void					  Unload();
		bool					  Load( const char *szFilePath, bool rebuildNormals = false );		
		void					  ReverseWinding();
		void					  Normalize( float scaleTo = 1.0f, bool center = true );		
		
		/* 
		############################################################################
		FRIENDS
		############################################################################
		*/
		friend class Renderer; // TODO: Temporary fix-up until render pipeline is re-designed
	private:
		// FUNCTIONS
		//
		void					  CountGeometry( FILE *pFile );
		void					  LoadGeometry( FILE *pFile );
		bool					  ImportMaterials( const char *szFilePath );
		void					  Scale( float scaleFactor, float offset[3] );

		// TODO: Merge all this functions into one
		// Functions for face check on the obj file
		void					  AddTrianglePos( int index, int material, int x, int y, int z );
		void					  AddTrianglePosNormal( int index, int material, int x, int y, int z,int nx, int ny, int nz );
		void					  AddTrianglePosTexCoord( int index, int material, int x, int y, int z, int vtx, int vty, int vtz );
		void					  AddTrianglePosTexCoordNormal( int index, int material, int x, int y, int z, int vtx, int vty, int vtz, int vnx, int vny, int vnz );		
		
		void					  BuildTriangleSet();
		void					  GenerateNormals();
		void					  GenerateTangents();

		int						  AddVertex( int hash, const Vertex *pVertex );
		void					  Bounds( Vector3 center, float& width, float& height,float& length, float& radius ) const;		
		void					  LoadMaterials();

		// MEMBERS
		//
		bool					         m_hasPositions;
		bool					         m_hasTextureCoords;
		bool					         m_hasNormals;
		bool					         m_hasTangents;
								         
		int						         m_numberOfVertexCoords;
		int						         m_numberOfTextureCoords;
		int						         m_numberOfNormals;
		int						         m_numberOfTriangles;
		int						         m_numberOfMaterials;
		int						         m_numberOfMeshes;
								         
		Vector3					         m_center;
		float					         m_width;
		float					         m_height;
		float					         m_length;
		float					         m_radius;
								         
		std::string				         m_directoryPath;
								         
		std::vector< TriangleSet >       m_triangles;
		std::vector< MaterialData >      m_materials;
		std::vector< Vertex >	         m_vbos;
		std::vector< int >		         m_ibos;
		std::vector< int >		         m_attributeBuffer;
		std::vector< float >	         m_vertexCoords;
		std::vector< float >	         m_textureCoords;
		std::vector< float >	         m_normals;

		std::map<std::string, int>       m_materialCache;
		std::map<int, std::vector<int> > m_vertexCache;
		std::map<std::string, GLuint>    m_modelTextures;

		// 
		//
		GLuint vertexbuffer;
		GLuint elementbuffer;
		GLuint tangentbuffer;
		GLuint bitangentbuffer;
		GLuint vertexArrayID;
		GLuint matrixID;
		GLuint viewMatrixID;
		GLuint modelMatrixID;
		GLuint textureID;
		GLuint lightID;
		GLuint programId;
	};
}