#ifndef OMFGUTIL_DETAIL_MACROS_H
#define OMFGUTIL_DETAIL_MACROS_H

//NOTE: It's important that these are defined on a single-line
//since all references to __LINE__ must evaluate to the same value.

#define foreach( i, c )\
  typedef __typeof__( c ) T_##__LINE__; T_##__LINE__& C_##__LINE__ = (c); for( T_##__LINE__::iterator i = C_##__LINE__.begin(); i != C_##__LINE__.end(); ++i )
  
#define foreach_delete( i, c )\
  typedef __typeof__( c ) T_##__LINE__; T_##__LINE__& C_##__LINE__ = (c); for( T_##__LINE__::iterator i = C_##__LINE__.begin(), next; (i != C_##__LINE__.end()) && (next = i, ++next, true); i = next )
  
#define const_foreach( i, c )\
  typedef __typeof__( c ) T_##__LINE__; T_##__LINE__& C_##__LINE__ = (c); for( T_##__LINE__::const_iterator i = C_##__LINE__.begin(); i != C_##__LINE__.end(); ++i )

#define reverse_foreach( i, c )\
  typedef __typeof__( c ) T_##__LINE__; T_##__LINE__& C_##__LINE__ = (c); for( T_##__LINE__::reverse_iterator i = C_##__LINE__.rbegin(); i != C_##__LINE__.rend(); ++i )
  
#define forrange( i, b, e )\
  typedef __typeof__( b ) T_##__LINE__; T_##__LINE__ E_##__LINE__ = (e); for( T_##__LINE__ i = (b); i != E_##__LINE__; ++i )

#define forrange_delete( i, b, e )\
  typedef __typeof__( b ) T_##__LINE__; T_##__LINE__ E_##__LINE__ = (e); for( T_##__LINE__ i = (b), next; (i != E_##__LINE__) && (next = i, ++next, true); i = next )

#endif //OMFGUTIL_DETAIL_MACROS_H
