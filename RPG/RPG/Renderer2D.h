#pragma once

#ifndef _RENDERER2D_H_
#define _RENDERER2D_H_

namespace Solidus
{	
	class Map;	
	class Entity;
	class Tile;	
	class Renderer2D
	{
	public:		
		static void		   Create( void );
		static void		   Destroy( void );		
		void			   DrawLevel( const Map& level ) const;		
		void			   DrawEntity( Entity* entity ) const;
		static Renderer2D& Instance();
	private:
		// Setting the entity as non-copyable
		//
		Renderer2D( void ) {}
		Renderer2D( Renderer2D const& ) {};
		Renderer2D& operator=( Renderer2D const& ) {};

		// DATA
		//
		static std::unique_ptr < Renderer2D > s_instance;
	};
}
#endif