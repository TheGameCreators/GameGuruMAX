#ifndef _H_GGTERRAIN_DATA_STRUCTURES
#define _H_GGTERRAIN_DATA_STRUCTURES

// optimized for fast adding/removing without preserving order
template<class T> class UnorderedArray
{
	public:
		UnorderedArray() {}
		~UnorderedArray() { if ( m_pItems ) delete [] m_pItems; }

		void Clear() { m_iNumItems = 0; }

		void Resize( uint32_t size ) 
		{
			if ( m_pItems ) delete [] m_pItems;
			m_pItems = 0;
			m_iArraySize = size; 
			if ( m_iArraySize ) m_pItems = new T[ m_iArraySize ];
			if ( size < m_iNumItems ) m_iNumItems = size;
		}

		void AddItem( T item )
		{
			if ( m_iArraySize < m_iNumItems+1 )
			{
				if ( m_iArraySize <= 1 ) m_iArraySize = 2;
				else m_iArraySize = m_iArraySize + m_iArraySize/2;
				T* newArray = new T[ m_iArraySize ];
				if ( m_pItems )
				{
					memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
					delete [] m_pItems;
				}
				m_pItems = newArray;
			}

			m_pItems[ m_iNumItems ] = item;
			m_iNumItems++;
		}

		void RemoveIndex( uint32_t index )
		{
			if ( m_iNumItems == 0 ) return;
			m_iNumItems--;
			if ( m_iNumItems > 0 ) m_pItems[ index ] = m_pItems[ m_iNumItems ];
		}

		T PopItem()
		{
			if ( m_iNumItems == 0 ) return 0;
			m_iNumItems--;
			return m_pItems[ m_iNumItems ];
		}

		void RemoveItem( T item )
		{
			if ( m_iNumItems == 0 ) return;
			for( int i = 0; i < (int)m_iNumItems; i++ )
			{
				if ( m_pItems[ i ] == item )
				{
					m_iNumItems--;
					m_pItems[ i ] = m_pItems[ m_iNumItems ];
				}
			}
		}

		uint32_t NumItems() { return m_iNumItems; }
		T& operator[](uint32_t i) { return m_pItems[ i ]; }
		T* GetItemPtr(uint32_t i) { return &(m_pItems[ i ]); }

	protected:
		T* m_pItems = 0;
		uint32_t m_iNumItems = 0;
		uint32_t m_iArraySize = 0;
};

// ordered does not mean sorted, items maintain the order they were added
template<class T> class OrderedArray
{
	public:
		OrderedArray() {}
		~OrderedArray() { if ( m_pItems ) delete [] m_pItems; }

		void Clear() { m_iNumItems = 0; }

		void AddItem( T item )
		{
			if ( m_iArraySize < m_iNumItems+1 )
			{
				if ( m_iArraySize <= 1 ) m_iArraySize = 2;
				else m_iArraySize = m_iArraySize + m_iArraySize/2;
				T* newArray = new T[ m_iArraySize ];
				if ( m_pItems )
				{
					memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
					delete [] m_pItems;
				}
				m_pItems = newArray;
			}

			m_pItems[ m_iNumItems ] = item;
			m_iNumItems++;
		}

		void RemoveIndex( uint32_t index )
		{
			if ( m_iNumItems == 0 ) return;
			m_iNumItems--;
			for( uint32_t i = index; i < m_iNumItems; i++ ) m_pItems[ i ] = m_pItems[ i+1 ];
		}

		void RemoveGaps()
		{
			if ( m_iNumItems == 0 ) return;

			uint32_t index2 = 0;
			for( uint32_t i = 0; i < m_iNumItems; i++ ) 
			{
				if ( m_pItems[ i ] )
				{
					m_pItems[ index2 ] = m_pItems[ i ];
					index2++;
				}
			}

			m_iNumItems = index2;
		}

		uint32_t NumItems() { return m_iNumItems; }
		T& operator[](uint32_t i) { return m_pItems[ i ]; }
		T GetItem(uint32_t i) { return m_pItems[ i ]; }

	protected:
		T* m_pItems = 0;
		uint32_t m_iNumItems = 0;
		uint32_t m_iArraySize = 0;
};

// items can only be added/removed from the end
template<class T> class StackArray
{
	public:
		StackArray( uint32_t size=2 ) { m_iMaxSize = size; if ( m_iMaxSize ) m_pItems = new T[ m_iMaxSize ]; }
		~StackArray() { if ( m_pItems ) delete [] m_pItems; }

		void Resize( uint32_t size ) 
		{ 
			if ( size < m_iNumItems ) size = m_iNumItems;
			if ( size < 2 ) size = 2;
			if ( m_iMaxSize != size )
			{
				m_iMaxSize = size;
				T* newArray = new T[ m_iMaxSize ];
				if ( m_pItems )
				{
					memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
					delete [] m_pItems;
				}
				m_pItems = newArray;
			}
		}

		void Clear() { m_iNumItems = 0; }

		void PushItem( T item )
		{
			if ( m_iMaxSize < m_iNumItems+1 )
			{
				if ( m_iMaxSize <= 1 ) m_iMaxSize = 2;
				else m_iMaxSize = m_iMaxSize + m_iMaxSize/2;
				T* newArray = new T[ m_iMaxSize ];
				if ( m_pItems )
				{
					memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
					delete [] m_pItems;
				}
				m_pItems = newArray;
			}

			m_pItems[ m_iNumItems ] = item;
			m_iNumItems++;
		}

		T PopItem()
		{
			if ( m_iNumItems == 0 ) return 0;
			m_iNumItems--;
			return m_pItems[ m_iNumItems ];
		}

		uint32_t NumItems() { return m_iNumItems; }
		T& operator[](uint32_t i) { return m_pItems[ i ]; }
		T* GetItemPtr(uint32_t i) { return &m_pItems[ i ]; }
		
	protected:
		T* m_pItems = 0;
		uint32_t m_iNumItems = 0;
		uint32_t m_iMaxSize = 0;
};

#endif // _H_GGTERRAIN_DATA_STRUCTURES