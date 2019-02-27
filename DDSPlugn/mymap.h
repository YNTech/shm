#ifndef __MYMAP_H__
#define __MYMAP_H__

#include <map>
#include "windows.h"
		// TEMPLATE CLASS map
template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty>,
	class _Alloc = allocator<pair<const _Kty, _Ty> > >
	class mymap: public std::map<_Kty, _Ty, _Pr, _Alloc>
	{	// ordered red-black tree of {key, mapped} values, unique keys
public:
	mymap():map(),m_isLocked(false)
	{	
	}
	mapped_type& operator[](const key_type& _Keyval)
	{	// find element matching _Keyval or insert with default mapped
		while(m_isLocked)
			Sleep(5);
		m_isLocked = true;
		iterator _Where = this->lower_bound(_Keyval);
		if (_Where == this->end()
			|| this->comp(_Keyval, this->_Key(_Where._Mynode())))
			_Where = this->insert(_Where,
			value_type(_Keyval, mapped_type()));
		m_isLocked = false;
		return ((*_Where).second);
	}
private:
	volatile bool m_isLocked;
	};
#endif

