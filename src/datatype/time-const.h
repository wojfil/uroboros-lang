#ifndef TIME_CONST_H_INCLUDED
#define TIME_CONST_H_INCLUDED

#include "primitives.h"


namespace uro
{

// _tnum is a data structure used for every element of Time and Period
// let it be "int" by default
// it can be anything, but necessarily non-unsigned
typedef _int _tnum;

inline constexpr _tnum TNUM_FIRST_YEAR = 1601;
inline constexpr _tnum TNUM_LAST_YEAR = 30000;

inline constexpr _tnum TNUM_FEBRUARY = 2;
inline constexpr _tnum TNUM_DAYS_IN_NORMAL_YEAR = 365;
inline constexpr _tnum TNUM_DAYS_IN_LEAP_YEAR = 366;
inline constexpr _tnum TNUM_MONTHS_IN_YEAR = 12;
inline constexpr _tnum TNUM_HOURS_IN_DAY = 24;
inline constexpr _tnum TNUM_MINUTES_IN_HOUR = 60;
inline constexpr _tnum TNUM_SECONDS_IN_MINUTE = 60;




}

#endif // TIME_CONST_H_INCLUDED
