#pragma once

#ifndef _TRIE_H_
#define _TRIE_H_

#include <vector>
#include <string>

namespace Solidus
{
	const int MAXN = 256;

	class Trie
	{
	public:
		// CTOR
		//
		Trie();
		~Trie();

		// FUNCTIONS
		//		
		void AddWord( const char * s, const int k );
		int CountPreffixes( const char * s );
		int CountWords( const char * s );		
		std::vector< int > Search( const char *s ) ;
		bool Find( const char * s );

		// PUBLIC MEMBERS
		//		
		std::vector< int > d_id;
	private:
		Trie * m_child[ MAXN ];
		int m_prefixes;
		int m_words;		
	};
}

#endif