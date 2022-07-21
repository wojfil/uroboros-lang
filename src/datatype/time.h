/*
    This file is part of Uroboros.
    Uroboros is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Uroboros is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Uroboros. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIME_H
#define TIME_H

#include "period.h"
#include <vector>


struct Time
{
   // time can be defined in 4 forms:
   enum TimeType
   {
      tt_YearMonth = 0,     // contains month and year
      tt_Date = 1,          // contains day, month and year
      tt_ShortClock = 2,    // contains day, month, year, hour, minute
      tt_Clock = 3          // contains day, month, year, hour, minute, second
      // these numbers are important and do not delete them
   };

   Time();
   Time(_tnum mo, _tnum ye);
   Time(_tnum da, _tnum mo, _tnum ye);
   Time(_tnum da, _tnum mo, _tnum ye, _tnum ho, _tnum mi);
   Time(_tnum da, _tnum mo, _tnum ye, _tnum ho, _tnum mi, _tnum sec);

   _tnum year;
   _tnum day;
   _tnum month;
   _tnum hour;
   _tnum minute;
   _tnum second;
   TimeType type;

   _str toString() const;
   void initClock(const _boo& withSeconds, const _tnum& recentChange);

   void addYears(const _tnum& y);
   void addMonths(const _tnum& m);
   void addWeeks(const _tnum& w);
   void addDays(const _tnum& d);
   void addHours(const _tnum& h);
   void addMinutes(const _tnum& m);
   void addSeconds(const _tnum& s);

   void setYear(const _tnum& y);
   void setMonth(const _tnum& m);
   void setDay(const _tnum& d);
   void setHour(const _tnum& h);
   void setMinute(const _tnum& m);
   void setSecond(const _tnum& s);

   Time toDate() const;
   _tnum getWeekDay() const;
   _boo equalsExactly(const Time& tim) const;
   _boo isEmpty() const;
   void setValue(const Time& tim);

   Time& operator += (const Period& per);
   Time& operator -= (const Period& per);

   _boo operator == (const Time& tim) const;
   _boo operator != (const Time& tim) const;
   _boo operator < (const Time& tim) const;
   _boo operator > (const Time& tim) const;
   _boo operator <= (const Time& tim) const;
   _boo operator >= (const Time& tim) const;

   Period operator - (const Time& tim) const;

};

_tnum toTimeNumber(const Number& num);
_str monthToString(const _tnum& month);
_str weekdayToString(const _tnum& wday);

inline _str fillTimeUnit(const _tnum& val);
inline _boo isLeapYear(const _tnum& year);
_tnum daysInMonth(const _tnum& month, const _tnum& year);
inline Period timeDifference(const Time& min, const Time& max);
inline _tnum daysInYears(const _tnum& min, const _tnum& max);
inline void decrementMonth(Period& p, const Time& t, const _boo& addDays);
inline void decrementDay(Period& p, const Time& t);
inline void shortClockTillMidnight(Period& p, const Time& t);
inline void clockTillMidnight(Period& p, const Time& t);
std::vector<Time> sortedAndUniqueTimeList(const std::vector<Time>& base);

#endif /* TIME_H */
