#include "stdafx.h"
#include "Trie.h"

#include <algorithm>
#include "ReportErrors.h"
#include "Console.h"
#include "Vector4.h"

namespace Solidus
{
	Trie::Trie()
		: m_prefixes( 0 )
		, m_words( 0 )
	{		
		for( int i = 0; i < MAXN; ++i )
		{
			m_child[ i ] = NULL;
		}
		/*std::generate( m_child, m_child + MAXN, [] () { return 0; }  );*/
	}	

	Trie::~Trie()
	{
		for( int i = 0; i < MAXN; ++i )
		{
			delete m_child[ i ];
		}	
	}

	void Trie::AddWord( const char * s, const int k )
	{
		if( *s == '\0' ) 
		{
			m_words++;
			return;
		}

		Trie *t = m_child[ *s ];
		if( m_child[ *s ] == NULL ) 
		{
			t = m_child[ *s ] = new Trie;
			t->m_prefixes = 1;
		} 
		else 
		{
			t->m_prefixes++;
		}

		t->d_id.push_back( k );
		t->AddWord( s + 1, k );
	}

	int Trie::CountPreffixes( const char * s )
	{
		if( *s == '\0' ) return m_prefixes;
		
		Trie *t = m_child[ *s ];
		if( t == NULL ) return 0;

		return t->CountPreffixes( s + 1 );
	}

	int Trie::CountWords( const char * s )
	{
		if( *s == '\0' ) return m_words;
		
		Trie *t = m_child[ *s ];
		if( t == NULL ) return 0;

		return t->CountWords( s + 1 );
	}

	std::vector<int> Trie::Search( const char *s )
	{
		int N = strlen( s );
		Trie *t = new Trie;
		
		for(int i = 0; i < N; ++i) 
		{
			if(i == 0)
				t = m_child[ s[ i ]];
			else
				t = (t != NULL) ? t->m_child[ s[ i ]]: NULL;

			if(t == NULL) return std::vector< int >( 0 );
		}

		return t->d_id;   
	}
}