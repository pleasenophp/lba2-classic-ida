//──────────────────────────────────────────────────────────────────────────
#ifndef	LIB_OBJECT
#define LIB_OBJECT


//──────────────────────────────────────────────────────────────────────────
#ifndef	_BUILDING_LIB

#ifdef	YAZ_WIN32

#pragma library ("NT_OBJ.LIB")

#else//	YAZ_WIN32

#pragma library ("DOS_OBJ.LIB")

#endif//YAZ_WIN32

#endif//_BUILDING_LIB

//──────────────────────────────────────────────────────────────────────────
#include <SYSTEM\ADELINE.H>

//──────────────────────────────────────────────────────────────────────────
#include <OBJECT\AFF_OBJ.H>

//──────────────────────────────────────────────────────────────────────────
#endif//LIB_OBJECT

//──────────────────────────────────────────────────────────────────────────
