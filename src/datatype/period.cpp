/*
    This file is part of Perun2.
    Perun2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Perun2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Perun2. If not, see <http://www.gnu.org/licenses/>.
*/

#include "period.h"
#include "text/chars.h"
#include "numbers.h"
#include "text/strings.h"
#include <sstream>


namespace perun2
{

Period::Period() 
   : periodUnit(PeriodUnit::u_Years), periodType(PeriodType::pt_Unit) { }

Period::Period(const p_tnum val, const PeriodUnit unit)
   : periodUnit(unit), periodType(PeriodType::pt_Unit)
{
   switch (periodUnit) {
      case PeriodUnit::u_Years: {
         years = val;
         break;
      }
      case PeriodUnit::u_Months: {
         months = val;
         break;
      }
      case PeriodUnit::u_Weeks: {
         weeks = val;
         break;
      }
      case PeriodUnit::u_Days: {
         days = val;
         break;
      }
      case PeriodUnit::u_Hours: {
         hours = val;
         break;
      }
      case PeriodUnit::u_Minutes: {
         minutes = val;
         break;
      }
      case PeriodUnit::u_Seconds: {
         seconds = val;
         break;
      }
   }
}

p_nint Period::toSeconds() const
{
   p_nint secs = static_cast<p_nint>(seconds);
   const p_tnum d = days + years_ad + months_ad;

   if (years != TNUM_ZERO) {
      secs += NINT_SECONDS_IN_YEAR * static_cast<p_nint>(years);
   }
   if (months != TNUM_ZERO) {
      secs += NINT_SECONDS_IN_MONTH * static_cast<p_nint>(months);
   }
   if (weeks != TNUM_ZERO) {
      secs += NINT_SECONDS_IN_WEEK * static_cast<p_nint>(weeks);
   }
   if (d != TNUM_ZERO) {
      secs += NINT_SECONDS_IN_DAY * static_cast<p_nint>(d);
   }
   if (hours != TNUM_ZERO) {
      secs += NINT_SECONDS_IN_HOUR * static_cast<p_nint>(hours);
   }
   if (minutes != TNUM_ZERO) {
      secs += NINT_SECONDS_IN_MINUTE * static_cast<p_nint>(minutes);
   }

   return secs;
}

p_str Period::toString() const
{
   p_stream ss;
   p_bool first = true;

   const p_tnum y = years + years_sec;
   const p_tnum m = months + months_sec;

   if (y != TNUM_ZERO || years != TNUM_ZERO) {
      if (!first) ss << CHAR_SPACE;
      ss << y;
      if (y == TNUM_ONE || y == TNUM_MINUS_ONE)
         ss << L" year";
      else
         ss << L" years";
      if (first) first = false;
   }

   if (m != TNUM_ZERO || months != TNUM_ZERO) {
      if (!first) ss << CHAR_SPACE;
      ss << m;
      if (m == TNUM_ONE || m == TNUM_MINUS_ONE)
         ss << L" month";
      else
         ss << L" months";
      if (first) first = false;
   }

   if (weeks != TNUM_ZERO) {
      if (!first) ss << CHAR_SPACE;
      ss << weeks;
      if (weeks == TNUM_ONE || weeks == TNUM_MINUS_ONE)
         ss << L" week";
      else
         ss << L" weeks";
      if (first) first = false;
   }

   if (days != TNUM_ZERO) {
      if (!first) ss << CHAR_SPACE;
      ss << days;
      if (days == TNUM_ONE || days == TNUM_MINUS_ONE)
         ss << L" day";
      else
         ss << L" days";
      if (first) first = false;
   }

   if (hours != TNUM_ZERO) {
      if (!first) ss << CHAR_SPACE;
      ss << hours;
      if (hours == TNUM_ONE || hours == TNUM_MINUS_ONE)
         ss << L" hour";
      else
         ss << L" hours";
      if (first) first = false;
   }

   if (minutes != TNUM_ZERO) {
      if (!first) ss << CHAR_SPACE;
      ss << minutes;
      if (minutes == TNUM_ONE || minutes == TNUM_MINUS_ONE)
         ss << L" minute";
      else
         ss << L" minutes";
      if (first) first = false;
   }

   if (seconds != TNUM_ZERO) {
      if (!first) ss << CHAR_SPACE;
      ss << seconds;
      if (seconds == TNUM_ONE || seconds == TNUM_MINUS_ONE)
         ss << L" second";
      else
         ss << L" seconds";
      if (first) first = false;
   }

   return first
      ? STRING_NO_PERIOD
      : ss.str();
}

inline p_bool Period::isNegativeDifference() const
{
   return years < TNUM_ZERO
       || months < TNUM_ZERO
       || months_ad < TNUM_ZERO
       || years_ad < TNUM_ZERO
       || days < TNUM_ZERO
       || hours < TNUM_ZERO
       || minutes < TNUM_ZERO
       || seconds < TNUM_ZERO;
}

void Period::clear()
{
   periodType = PeriodType::pt_Unit;
   years = TNUM_ZERO;
   months = TNUM_ZERO;
   weeks = TNUM_ZERO;
   days = TNUM_ZERO;
   hours = TNUM_ZERO;
   minutes = TNUM_ZERO;
   seconds = TNUM_ZERO;
   years_sec = TNUM_ZERO;
   months_sec = TNUM_ZERO;
   years_ad = TNUM_ZERO;
   months_ad = TNUM_ZERO;
}

void Period::reverse()
{
   if (periodType == PeriodType::pt_Unit) {
      switch (periodUnit) {
         case PeriodUnit::u_Years: {
            years *= TNUM_MINUS_ONE;
            break;
         }
         case PeriodUnit::u_Months: {
            months *= TNUM_MINUS_ONE;
            break;
         }
         case PeriodUnit::u_Weeks: {
            weeks *= TNUM_MINUS_ONE;
            break;
         }
         case PeriodUnit::u_Days: {
            days *= TNUM_MINUS_ONE;
            break;
         }
         case PeriodUnit::u_Hours: {
            hours *= TNUM_MINUS_ONE;
            break;
         }
         case PeriodUnit::u_Minutes: {
            minutes *= TNUM_MINUS_ONE;
            break;
         }
         case PeriodUnit::u_Seconds: {
            seconds *= TNUM_MINUS_ONE;
            break;
         }
      }
   }
   else {
      years *= TNUM_MINUS_ONE;
      months *= TNUM_MINUS_ONE;
      weeks *= TNUM_MINUS_ONE;
      days *= TNUM_MINUS_ONE;
      hours *= TNUM_MINUS_ONE;
      minutes *= TNUM_MINUS_ONE;
      seconds *= TNUM_MINUS_ONE;
      years_sec *= TNUM_MINUS_ONE;
      months_sec *= TNUM_MINUS_ONE;
      years_ad *= TNUM_MINUS_ONE;
      months_ad *= TNUM_MINUS_ONE;
   }
}

inline void Period::checkDifference()
{
   if (isNegativeDifference()) {
      if (years > TNUM_ZERO || months > TNUM_ZERO || days > TNUM_ZERO || hours > TNUM_ZERO
       || minutes > TNUM_ZERO || seconds > TNUM_ZERO || months <= -TNUM_MONTHS_IN_YEAR || days <= -TNUM_DAYS_IN_FEBRUARY
       || hours <= -TNUM_HOURS_IN_DAY || minutes <= -TNUM_MINUTES_IN_HOUR || seconds <= -TNUM_SECONDS_IN_MINUTE) {
         periodType = PeriodType::pt_Mingled;
      }
   }
   else {
      if (years < TNUM_ZERO || months < TNUM_ZERO || days < TNUM_ZERO || hours < TNUM_ZERO
       || minutes < TNUM_ZERO || seconds < TNUM_ZERO || months >= TNUM_MONTHS_IN_YEAR || days >= TNUM_DAYS_IN_FEBRUARY
       || hours >= TNUM_HOURS_IN_DAY || minutes >= TNUM_MINUTES_IN_HOUR || seconds >= TNUM_SECONDS_IN_MINUTE) {
         periodType = PeriodType::pt_Mingled;
      }
   }
}

Period& Period::operator += (const Period& per)
{
   if (per.periodType == PeriodType::pt_Unit) {
      addUnit(per);
   }
   else {
      years += per.years;
      months += per.months;
      weeks += per.weeks;
      days += per.days;
      hours += per.hours;
      minutes += per.minutes;
      seconds += per.seconds;
      years_sec += per.years_sec;
      months_sec += per.months_sec;
      years_ad += per.years_ad;
      months_ad += per.months_ad;

      if (periodType == PeriodType::pt_Difference) {
         checkDifference();
      }
   }

   return *this;
}

Period& Period::operator -= (const Period& per)
{
   if (per.periodType == PeriodType::pt_Unit) {
      subtractUnit(per);
   }
   else {
      years -= per.years;
      months -= per.months;
      weeks -= per.weeks;
      days -= per.days;
      hours -= per.hours;
      minutes -= per.minutes;
      seconds -= per.seconds;
      years_sec -= per.years_sec;
      months_sec -= per.months_sec;
      years_ad -= per.years_ad;
      months_ad -= per.months_ad;

      if (periodType == PeriodType::pt_Difference) {
         checkDifference();
      }
   }

   return *this;
}

inline void Period::addUnit(const Period& per)
{
   if (periodType == PeriodType::pt_Unit) {
      switch (per.periodUnit) {
         case PeriodUnit::u_Years: {
            years += per.years;
            if (years != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Months: {
            months += per.months;
            if (months != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Weeks: {
            weeks += per.weeks;
            if (weeks != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Days: {
            days += per.days;
            if (days != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Hours: {
            hours += per.hours;
            if (hours != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Minutes: {
            minutes += per.minutes;
            if (minutes != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Seconds: {
            seconds += per.seconds;
            if (seconds != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
      }
   }
   else {
      switch (per.periodUnit) {
         case PeriodUnit::u_Years: {
            years += per.years;
            break;
         }
         case PeriodUnit::u_Months: {
            months += per.months;
            break;
         }
         case PeriodUnit::u_Weeks: {
            weeks += per.weeks;
            break;
         }
         case PeriodUnit::u_Days: {
            days += per.days;
            break;
         }
         case PeriodUnit::u_Hours: {
            hours += per.hours;
            break;
         }
         case PeriodUnit::u_Minutes: {
            minutes += per.minutes;
            break;
         }
         case PeriodUnit::u_Seconds: {
            seconds += per.seconds;
            break;
         }
      }

      if (periodType == PeriodType::pt_Difference) {
         checkDifference();
      }
   }
}

inline void Period::subtractUnit(const Period& per)
{
   if (periodType == PeriodType::pt_Unit) {
      switch (per.periodUnit) {
         case PeriodUnit::u_Years: {
            years -= per.years;
            if (years != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Months: {
            months -= per.months;
            if (months != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Weeks: {
            weeks -= per.weeks;
            if (weeks != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Days: {
            days -= per.days;
            if (days != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Hours: {
            hours -= per.hours;
            if (hours != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Minutes: {
            minutes -= per.minutes;
            if (minutes != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
         case PeriodUnit::u_Seconds: {
            seconds -= per.seconds;
            if (seconds != TNUM_ZERO && periodUnit != per.periodUnit) {
               periodType = PeriodType::pt_Mingled;
            }
            break;
         }
      }
   }
   else {
      switch (per.periodUnit) {
         case PeriodUnit::u_Years: {
            years -= per.years;
            break;
         }
         case PeriodUnit::u_Months: {
            months -= per.months;
            break;
         }
         case PeriodUnit::u_Weeks: {
            weeks -= per.weeks;
            break;
         }
         case PeriodUnit::u_Days: {
            days -= per.days;
            break;
         }
         case PeriodUnit::u_Hours: {
            hours -= per.hours;
            break;
         }
         case PeriodUnit::u_Minutes: {
            minutes -= per.minutes;
            break;
         }
         case PeriodUnit::u_Seconds: {
            seconds -= per.seconds;
            break;
         }
      }

      if (periodType == PeriodType::pt_Difference) {
         checkDifference();
      }
   }
}

Period& Period::operator *= (const Number& num)
{
   if (num.isNaN()) {
      clear();
      return *this;
   }

   const p_tnum n = static_cast<p_tnum>(num.toInt());

   if (periodType ==  PeriodType::pt_Unit) {
      switch (periodUnit) {
         case PeriodUnit::u_Years: {
            years *= n;
            break;
         }
         case PeriodUnit::u_Months: {
            months *= n;
            break;
         }
         case PeriodUnit::u_Weeks: {
            weeks *= n;
            break;
         }
         case PeriodUnit::u_Days: {
            days *= n;
            break;
         }
         case PeriodUnit::u_Hours: {
            hours *= n;
            break;
         }
         case PeriodUnit::u_Minutes: {
            minutes *= n;
            break;
         }
         case PeriodUnit::u_Seconds: {
            seconds *= n;
            break;
         }
      }
   }
   else {
      years *= n;
      months *= n;
      weeks *= n;
      days *= n;
      hours *= n;
      minutes *= n;
      seconds *= n;
      years_sec *= n;
      months_sec *= n;
      years_ad *= n;
      months_ad *= n;
      if (periodType == PeriodType::pt_Difference) {
         checkDifference();
      }
   }

   return *this;
}

Period& Period::operator /= (const Number& num)
{
   if (num.isNaN()) {
      clear();
      return *this;
   }

   const p_tnum n = static_cast<p_tnum>(num.toInt());

   if (n == TNUM_ZERO) {
      clear();
      return *this;
   }

   if (periodType ==  PeriodType::pt_Unit) {
      switch (periodUnit) {
         case PeriodUnit::u_Years: {
            years /= n;
            break;
         }
         case PeriodUnit::u_Months: {
            months /= n;
            break;
         }
         case PeriodUnit::u_Weeks: {
            weeks /= n;
            break;
         }
         case PeriodUnit::u_Days: {
            days /= n;
            break;
         }
         case PeriodUnit::u_Hours: {
            hours /= n;
            break;
         }
         case PeriodUnit::u_Minutes: {
            minutes /= n;
            break;
         }
         case PeriodUnit::u_Seconds: {
            seconds /= n;
            break;
         }
      }
   }
   else {
      years /= n;
      months /= n;
      weeks /= n;
      days /= n;
      hours /= n;
      minutes /= n;
      seconds /= n;
      years_sec /= n;
      months_sec /= n;
      years_ad /= n;
      months_ad /= n;
   }

   return *this;
}

Period Period::operator + (const Period& per) const
{
   Period result = *this;
   result += per;
   return result;
}

Period Period::operator - (const Period& per) const
{
   Period result = *this;
   result -= per;
   return result;
}

Period Period::operator - () const
{
   Period p;

   p.years = -years;
   p.months = -months;
   p.weeks = -weeks;
   p.days = -days;
   p.hours = -hours;
   p.minutes = -minutes;
   p.seconds = -seconds;
   p.years_sec = -years_sec;
   p.months_sec = -months_sec;
   p.years_ad = -years_ad;
   p.months_ad = -months_ad;

   p.periodType = periodType;
   p.periodUnit = periodUnit;

   return p;
}

p_bool Period::operator == (const Period& per) const
{
   switch (periodType) {
      case PeriodType::pt_Unit: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodUnitsCmp(per) == TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffUnitCmp(per, *this) == TNUM_ZERO;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) == TNUM_ZERO;
            }
         }
         break;
      }
      case PeriodType::pt_Difference: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodDiffUnitCmp(*this, per) == TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffsCmp(per) == TNUM_ZERO;
               break;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) == TNUM_ZERO;
            }
         }
         break;
      }
   }

   return periodCmp(per) == TNUM_ZERO;
}

p_bool Period::operator != (const Period& per) const
{
   switch (periodType) {
      case PeriodType::pt_Unit: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodUnitsCmp(per) != TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffUnitCmp(per, *this) != TNUM_ZERO;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) != TNUM_ZERO;
            }
         }
         break;
      }
      case PeriodType::pt_Difference: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodDiffUnitCmp(*this, per) != TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffsCmp(per) != TNUM_ZERO;
               break;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) != TNUM_ZERO;
            }
         }
         break;
      }
   }

   return periodCmp(per) != TNUM_ZERO;
}

p_bool Period::operator < (const Period& per) const
{
   switch (periodType) {
      case PeriodType::pt_Unit: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodUnitsCmp(per) < TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffUnitCmp(per, *this) < TNUM_ZERO;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) < TNUM_ZERO;
            }
         }
         break;
      }
      case PeriodType::pt_Difference: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodDiffUnitCmp(*this, per) < TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffsCmp(per) < TNUM_ZERO;
               break;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) < TNUM_ZERO;
            }
         }
         break;
      }
   }

   return periodCmp(per) < TNUM_ZERO;
}

p_bool Period::operator > (const Period& per) const
{
   switch (periodType) {
      case PeriodType::pt_Unit: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodUnitsCmp(per) > TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffUnitCmp(per, *this) > TNUM_ZERO;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) > TNUM_ZERO;
            }
         }
         break;
      }
      case PeriodType::pt_Difference: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodDiffUnitCmp(*this, per) > TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffsCmp(per) > TNUM_ZERO;
               break;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) > TNUM_ZERO;
            }
         }
         break;
      }
   }

   return periodCmp(per) > TNUM_ZERO;
}

p_bool Period::operator <= (const Period& per) const
{
   switch (periodType) {
      case PeriodType::pt_Unit: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodUnitsCmp(per) <= TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffUnitCmp(per, *this) <= TNUM_ZERO;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) <= TNUM_ZERO;
            }
         }
         break;
      }
      case PeriodType::pt_Difference: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodDiffUnitCmp(*this, per) <= TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffsCmp(per) <= TNUM_ZERO;
               break;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) <= TNUM_ZERO;
            }
         }
         break;
      }
   }

   return periodCmp(per) <= TNUM_ZERO;
}

p_bool Period::operator >= (const Period& per) const
{
   switch (periodType) {
      case PeriodType::pt_Unit: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodUnitsCmp(per) >= TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffUnitCmp(per, *this) >= TNUM_ZERO;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) >= TNUM_ZERO;
            }
         }
         break;
      }
      case PeriodType::pt_Difference: {
         switch (per.periodType) {
            case PeriodType::pt_Unit: {
               return periodDiffUnitCmp(*this, per) >= TNUM_ZERO;
            }
            case PeriodType::pt_Difference: {
               return periodDiffsCmp(per) >= TNUM_ZERO;
               break;
            }
            case PeriodType::pt_Mingled: {
               return periodCmp(per) >= TNUM_ZERO;
            }
         }
         break;
      }
   }

   return periodCmp(per) >= TNUM_ZERO;
}

// all 49 possibilities hardcoded
inline p_tnum Period::periodUnitsCmp(const Period& per) const
{
   switch (periodUnit) {
      case PeriodUnit::u_Years: {
         switch (per.periodUnit) {
            case PeriodUnit::u_Years:
               return years - per.years;
            case PeriodUnit::u_Months:
               return years * TNUM_MONTHS_IN_YEAR - per.months;
            case PeriodUnit::u_Weeks:
               return years * TNUM_DAYS_IN_NORMAL_YEAR - per.weeks * TNUM_DAYS_IN_WEEK;
            case PeriodUnit::u_Days:
               return years * TNUM_DAYS_IN_NORMAL_YEAR - per.days;
            case PeriodUnit::u_Hours:
               return years * TNUM_HOURS_IN_YEAR - per.hours;
            case PeriodUnit::u_Minutes:
               return years * TNUM_MINUTES_IN_YEAR - per.minutes;
            case PeriodUnit::u_Seconds:
               return years * TNUM_SECONDS_IN_YEAR - per.seconds;
         }
         break;
      }
      case PeriodUnit::u_Months: {
         switch (per.periodUnit) {
            case PeriodUnit::u_Years:
               return months - TNUM_MONTHS_IN_YEAR * per.years;
            case PeriodUnit::u_Months:
               return months - per.months;
            case PeriodUnit::u_Weeks:
               return months * TNUM_DAYS_IN_NORMAL_MONTH - TNUM_DAYS_IN_WEEK * per.weeks;
            case PeriodUnit::u_Days:
               return months * TNUM_DAYS_IN_NORMAL_MONTH - per.days;
            case PeriodUnit::u_Hours:
               return months * TNUM_HOURS_IN_MONTH - per.hours;
            case PeriodUnit::u_Minutes:
               return months * TNUM_MINUTES_IN_MONTH - per.minutes;
            case PeriodUnit::u_Seconds:
               return months * TNUM_SECONDS_IN_MONTH - per.seconds;
         }
         break;
      }
      case PeriodUnit::u_Weeks: {
         switch (per.periodUnit) {
            case PeriodUnit::u_Years:
               return weeks * TNUM_DAYS_IN_WEEK- TNUM_DAYS_IN_NORMAL_YEAR * per.years;
            case PeriodUnit::u_Months:
               return weeks * TNUM_DAYS_IN_WEEK- TNUM_DAYS_IN_NORMAL_MONTH * per.months;
            case PeriodUnit::u_Weeks:
               return weeks - per.weeks;
            case PeriodUnit::u_Days:
               return weeks * TNUM_DAYS_IN_WEEK- per.days;
            case PeriodUnit::u_Hours:
               return weeks * TNUM_HOURS_IN_WEEK - per.hours;
            case PeriodUnit::u_Minutes:
               return weeks * TNUM_MINUTES_IN_WEEK - per.minutes;
            case PeriodUnit::u_Seconds:
               return weeks * TNUM_SECONDS_IN_WEEK - per.seconds;
         }
         break;
      }
      case PeriodUnit::u_Days: {
         switch (per.periodUnit) {
            case PeriodUnit::u_Years:
               return days - TNUM_DAYS_IN_NORMAL_YEAR * per.years;
            case PeriodUnit::u_Months:
               return days - TNUM_DAYS_IN_NORMAL_MONTH * per.months;
            case PeriodUnit::u_Weeks:
               return days - TNUM_DAYS_IN_WEEK* per.weeks;
            case PeriodUnit::u_Days:
               return days - per.days;
            case PeriodUnit::u_Hours:
               return days * TNUM_HOURS_IN_DAY - per.hours;
            case PeriodUnit::u_Minutes:
               return days * TNUM_MINUTES_IN_DAY - per.minutes;
            case PeriodUnit::u_Seconds:
               return days * TNUM_SECONDS_IN_DAY - per.seconds;
         }
         break;
      }
      case PeriodUnit::u_Hours: {
         switch (per.periodUnit) {
            case PeriodUnit::u_Years:
               return hours - TNUM_HOURS_IN_YEAR * per.years;
            case PeriodUnit::u_Months:
               return hours - TNUM_HOURS_IN_MONTH * per.months;
            case PeriodUnit::u_Weeks:
               return hours - TNUM_HOURS_IN_WEEK * per.weeks;
            case PeriodUnit::u_Days:
               return hours - TNUM_HOURS_IN_DAY * per.days;
            case PeriodUnit::u_Hours:
               return hours - per.hours;
            case PeriodUnit::u_Minutes:
               return hours * TNUM_MINUTES_IN_HOUR - per.minutes;
            case PeriodUnit::u_Seconds:
               return hours * TNUM_SECONDS_IN_HOUR - per.seconds;
         }
         break;
      }
      case PeriodUnit::u_Minutes: {
         switch (per.periodUnit) {
            case PeriodUnit::u_Years:
               return minutes - TNUM_MINUTES_IN_YEAR * per.years;
            case PeriodUnit::u_Months:
               return minutes - TNUM_MINUTES_IN_MONTH * per.months;
            case PeriodUnit::u_Weeks:
               return minutes - TNUM_MINUTES_IN_WEEK * per.weeks;
            case PeriodUnit::u_Days:
               return minutes - TNUM_MINUTES_IN_DAY * per.days;
            case PeriodUnit::u_Hours:
               return minutes - TNUM_MINUTES_IN_HOUR * per.hours;
            case PeriodUnit::u_Minutes:
               return minutes - per.minutes;
            case PeriodUnit::u_Seconds:
               return minutes * TNUM_SECONDS_IN_MINUTE - per.seconds;
         }
         break;
      }
      case PeriodUnit::u_Seconds: {
         switch (per.periodUnit) {
            case PeriodUnit::u_Years:
               return seconds - TNUM_SECONDS_IN_YEAR * per.years;
            case PeriodUnit::u_Months:
               return seconds - TNUM_SECONDS_IN_MONTH * per.months;
            case PeriodUnit::u_Weeks:
               return seconds - TNUM_SECONDS_IN_WEEK * per.weeks;
            case PeriodUnit::u_Days:
               return seconds - TNUM_SECONDS_IN_DAY * per.days;
            case PeriodUnit::u_Hours:
               return seconds - TNUM_SECONDS_IN_HOUR * per.hours;
            case PeriodUnit::u_Minutes:
               return seconds - TNUM_SECONDS_IN_MINUTE * per.minutes;
            case PeriodUnit::u_Seconds:
               return seconds - per.seconds;
         }
         break;
      }
   }

   return TNUM_ZERO;
}

inline p_tnum Period::periodDiffsCmp(const Period& diff) const
{
   p_tnum value = days + months_ad + years_ad
      - diff.days - diff.months_ad - diff.years_ad;

   if (value != TNUM_ZERO) {
      return value;
   }

   value = hours - diff.hours;
   if (value != TNUM_ZERO) {
      return value;
   }

   value = minutes - diff.minutes;

   return value == TNUM_ZERO
      ? seconds - diff.seconds
      : value;
}

inline p_tnum Period::periodCmp(const Period& per) const
{
   p_tnum s = (hours - per.hours) * TNUM_SECONDS_IN_HOUR
      + (minutes - per.minutes) * TNUM_SECONDS_IN_MINUTE + seconds - per.seconds;
   p_tnum d = (s / TNUM_SECONDS_IN_DAY) + days - per.days + TNUM_DAYS_IN_WEEK* (weeks - per.weeks);
   s %= TNUM_SECONDS_IN_DAY;

   p_tnum y1 = years - per.years;
   p_tnum y2 = years_sec - per.years_sec;
   p_tnum m1 = months - per.months;
   p_tnum m2 = months_sec - per.months_sec;

   const p_bool noYear = (y1 == TNUM_ZERO) && (y2 == TNUM_ZERO);
   const p_bool noMonth = (m1 == TNUM_ZERO) && (m2 == TNUM_ZERO);

   if (noYear || noMonth) {
      d += months_ad - per.months_ad + years_ad - per.years_ad;

      if (noYear) {
         if (noMonth) {
            return d == TNUM_ZERO ? s : d;
         }

         if (m1 == -m2) {
            return d == TNUM_ZERO ? s : d;
         }

         d += m1 * TNUM_DAYS_IN_NORMAL_MONTH;
         return d == TNUM_ZERO ? s : d;
      }

      if (noMonth) {
         if (y1 == -y2) {
            return d == TNUM_ZERO ? s : d;
         }

         d += y1 * TNUM_DAYS_IN_NORMAL_YEAR;
         return d == TNUM_ZERO ? s : d;
      }
   }

   if (m2 == TNUM_ZERO && y2 == TNUM_ZERO) {
      d += months_ad - per.months_ad + years_ad - per.years_ad;
      if ((y1 * TNUM_MONTHS_IN_YEAR) == -m1) {
         return d == TNUM_ZERO ? s : d;
      }

      if (y1 >= TNUM_ZERO) {
         if (m1 >= TNUM_ZERO) {
            d += TNUM_DAYS_IN_NORMAL_YEAR * y1 + TNUM_DAYS_IN_NORMAL_MONTH * m1;
         }
         else {
            const p_tnum x = (-m1) / TNUM_MONTHS_IN_YEAR;
            const p_tnum mx = x > y1 ? y1 : x;

            d += mx == TNUM_ZERO
               ? (TNUM_DAYS_IN_NORMAL_YEAR * y1 + TNUM_DAYS_IN_NORMAL_MONTH * m1)
               : (TNUM_DAYS_IN_NORMAL_YEAR * (y1 - mx) + TNUM_DAYS_IN_NORMAL_MONTH * (m1 + TNUM_MONTHS_IN_YEAR * mx));
         }
      }
      else {
         if (m1 >= TNUM_ZERO) {
            const p_tnum x = m1 / TNUM_MONTHS_IN_YEAR;
            const p_tnum mx = x > (-y1) ? (-y1) : x;

            d += mx == TNUM_ZERO
               ? (TNUM_DAYS_IN_NORMAL_YEAR * y1 + TNUM_DAYS_IN_NORMAL_MONTH * m1)
               : (TNUM_DAYS_IN_NORMAL_YEAR * (y1 + mx) + TNUM_DAYS_IN_NORMAL_MONTH * (m1 - TNUM_MONTHS_IN_YEAR * mx));
         }
         else {
            d += TNUM_DAYS_IN_NORMAL_YEAR * y1 + TNUM_DAYS_IN_NORMAL_MONTH * m1;
         }
      }

      return d == TNUM_ZERO ? s : d;
   }

   // final
   p_bool ney = false;
   p_bool nem = false;

   if ((y1 > TNUM_ZERO && y2 < TNUM_ZERO && y1 >= (-y2)) ||
       (y1 < TNUM_ZERO && y2 > TNUM_ZERO && (-y1) >= y2))
   {
      ney = true;
      y1 += y2;
      y2 = TNUM_ZERO;
   }

   if ((m1 > TNUM_ZERO && m2 < TNUM_ZERO && m1 >= (-m2)) ||
       (m1 < TNUM_ZERO && m2 > TNUM_ZERO && (-m1) >= m2))
   {
      nem = true;
      m1 += m2;
      m2 = TNUM_ZERO;
   }

   if (!ney)
   {
      if ((m1 > TNUM_ZERO && y2 < TNUM_ZERO && m1 >= (-y2 * TNUM_MONTHS_IN_YEAR)) ||
         (m1 < TNUM_ZERO && y2 > TNUM_ZERO && (-m1) >= y2 * TNUM_MONTHS_IN_YEAR))
      {
         ney = true;
         m1 += y2 * TNUM_MONTHS_IN_YEAR;
         y2 = TNUM_ZERO;
      }
   }

   if (!nem) {
      if ((y1 > TNUM_ZERO && m2 < TNUM_ZERO && (y1 * TNUM_MONTHS_IN_YEAR) >= (-m2)))
      {
         const p_tnum a = ((-m2) / TNUM_MONTHS_IN_YEAR)
            + (((-m2) % TNUM_MONTHS_IN_YEAR) == TNUM_ZERO ? TNUM_ZERO : TNUM_ONE);

         y1 -= a;
         m1 += m2 + a * TNUM_MONTHS_IN_YEAR;
         m2 = TNUM_ZERO;
         nem = true;
      }
      else if (y1 < TNUM_ZERO && m2 > TNUM_ZERO && (-y1 * TNUM_MONTHS_IN_YEAR) >= m2)
      {
         const p_tnum a = (m2 / TNUM_MONTHS_IN_YEAR)
            + ((m2 % TNUM_MONTHS_IN_YEAR) == TNUM_ZERO ? TNUM_ZERO : TNUM_ONE);

         y1 += a;
         m1 -= m2 + TNUM_MONTHS_IN_YEAR * a;
         m2 = TNUM_ZERO;
         nem = true;
      }
   }

   if (!ney) {
      d += years_ad - per.years_ad;
   }
   if (!nem) {
      d += months_ad - per.months_ad;
   }

   if ((y1 * TNUM_MONTHS_IN_YEAR) == -m1) {
      return d == TNUM_ZERO ? s : d;
   }

   if (y1 >= TNUM_ZERO) {
      if (m1 >= TNUM_ZERO) {
         d += TNUM_DAYS_IN_NORMAL_YEAR * y1 + TNUM_DAYS_IN_NORMAL_MONTH * m1;
      }
      else {
         const p_tnum x = (-m1) / TNUM_MONTHS_IN_YEAR;
         const p_tnum mx = x > y1 ? y1 : x;

         d += mx == TNUM_ZERO
            ? (TNUM_DAYS_IN_NORMAL_YEAR * y1 + TNUM_DAYS_IN_NORMAL_MONTH * m1)
            : (TNUM_DAYS_IN_NORMAL_YEAR * (y1 - mx) + TNUM_DAYS_IN_NORMAL_MONTH * (m1 + TNUM_MONTHS_IN_YEAR * mx));
      }
   }
   else {
      if (m1 >= TNUM_ZERO) {
         const p_tnum x = m1 / TNUM_MONTHS_IN_YEAR;
         const p_tnum mx = x > (-y1) ? (-y1) : x;

         d += mx == TNUM_ZERO
            ? (TNUM_DAYS_IN_NORMAL_YEAR * y1 + TNUM_DAYS_IN_NORMAL_MONTH * m1)
            : (TNUM_DAYS_IN_NORMAL_YEAR * (y1 + mx) + TNUM_DAYS_IN_NORMAL_MONTH * (m1 - TNUM_MONTHS_IN_YEAR * mx));
      }
      else {
         d += TNUM_DAYS_IN_NORMAL_YEAR * y1 + TNUM_DAYS_IN_NORMAL_MONTH * m1;
      }
   }

   return d == TNUM_ZERO ? s : d;
}

inline p_tnum periodDiffUnitCmp(const Period& diff, const Period& unit)
{
   switch (unit.periodUnit) {
      case Period::u_Years: {
         const p_tnum v = diff.years + diff.years_sec - unit.years;
         if (v != TNUM_ZERO) {
            return v;
         }

         if (diff.months != TNUM_ZERO) return diff.months;
         if (diff.months_sec != TNUM_ZERO) return diff.months_sec;
         if (diff.months_ad != TNUM_ZERO) return diff.months_ad;
         if (diff.days != TNUM_ZERO) return diff.days;
         if (diff.hours != TNUM_ZERO) return diff.hours;
         if (diff.minutes != TNUM_ZERO) return diff.minutes;
         if (diff.seconds != TNUM_ZERO) return diff.seconds;

         return TNUM_ZERO;
      }
      case Period::u_Months: {
         const p_tnum v = TNUM_MONTHS_IN_YEAR * (diff.years + diff.years_sec)
            + diff.months + diff.months_sec - unit.months;

         if (v != TNUM_ZERO) {
            return v;
         }

         if (diff.days != TNUM_ZERO) return diff.days;
         if (diff.hours != TNUM_ZERO) return diff.hours;
         if (diff.minutes != TNUM_ZERO) return diff.minutes;
         if (diff.seconds != TNUM_ZERO) return diff.seconds;

         return TNUM_ZERO;
      }
      case Period::u_Weeks: {
         const p_tnum v = TNUM_DAYS_IN_NORMAL_YEAR * diff.years + diff.years_ad + TNUM_DAYS_IN_NORMAL_MONTH
            * diff.months + diff.months_ad + diff.days - TNUM_DAYS_IN_WEEK* unit.weeks;

         if (v != TNUM_ZERO) {
            return v;
         }

         if (diff.hours != TNUM_ZERO) return diff.hours;
         if (diff.minutes != TNUM_ZERO) return diff.minutes;
         if (diff.seconds != TNUM_ZERO) return diff.seconds;

         return TNUM_ZERO;
      }
      case Period::u_Days: {
         const p_tnum v = TNUM_DAYS_IN_NORMAL_YEAR * diff.years + diff.years_ad + TNUM_DAYS_IN_NORMAL_MONTH * diff.months
            + diff.months_ad + diff.days - unit.days;

         if (v != TNUM_ZERO) {
            return v;
         }

         if (diff.hours != TNUM_ZERO) return diff.hours;
         if (diff.minutes != TNUM_ZERO) return diff.minutes;
         if (diff.seconds != TNUM_ZERO) return diff.seconds;

         return TNUM_ZERO;
      }
      case Period::u_Hours: {
         const p_tnum v = TNUM_HOURS_IN_DAY * (TNUM_DAYS_IN_NORMAL_YEAR * diff.years + diff.years_ad
            + TNUM_DAYS_IN_NORMAL_MONTH * diff.months + diff.months_ad + diff.days)
            + diff.hours - unit.hours;

         if (v != TNUM_ZERO) {
            return v;
         }

         if (diff.minutes != TNUM_ZERO) return diff.minutes;
         if (diff.seconds != TNUM_ZERO) return diff.seconds;

         return TNUM_ZERO;
      }
      case Period::u_Minutes: {
         const p_tnum v = TNUM_MINUTES_IN_HOUR * (TNUM_HOURS_IN_DAY * (TNUM_DAYS_IN_NORMAL_YEAR * diff.years + diff.years_ad
            + TNUM_DAYS_IN_NORMAL_MONTH * diff.months + diff.months_ad + diff.days) + diff.hours)
            + diff.minutes - unit.minutes;

         return v == TNUM_ZERO ? diff.seconds : v;
      }
      case Period::u_Seconds: {
         return TNUM_SECONDS_IN_MINUTE * (TNUM_MINUTES_IN_HOUR * (TNUM_HOURS_IN_DAY * (TNUM_DAYS_IN_NORMAL_YEAR * diff.years + diff.years_ad
            + TNUM_DAYS_IN_NORMAL_MONTH * diff.months + diff.months_ad + diff.days)
            + diff.hours) + diff.minutes) + diff.seconds - unit.seconds;
      }
   }

   return TNUM_ZERO;
}

Period alignPeriod(Period value)
{
   if (value.seconds >= TNUM_SECONDS_IN_MINUTE) {
      value.minutes += value.seconds / TNUM_SECONDS_IN_MINUTE;
      value.seconds %= TNUM_SECONDS_IN_MINUTE;
   }

   if (value.minutes >= TNUM_MINUTES_IN_HOUR) {
      value.hours += value.minutes / TNUM_MINUTES_IN_HOUR;
      value.minutes %= TNUM_MINUTES_IN_HOUR;
   }

   if (value.hours >= TNUM_HOURS_IN_DAY) {
      value.days += value.hours / TNUM_HOURS_IN_DAY;
      value.hours %= TNUM_HOURS_IN_DAY;
   }

   if (value.days > TNUM_DAYS_IN_FEBRUARY 
      || value.years_sec != TNUM_ZERO || value.months_sec != TNUM_ZERO
      || value.years_ad != TNUM_ZERO || value.months_ad != TNUM_ZERO)
   {
      value.periodType = Period::pt_Mingled;
      return value;
   }

   value.periodType = Period::pt_Difference;
   return value;
}

}

