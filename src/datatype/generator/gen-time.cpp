/*
    This file is part of Perun2.
    Perun2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Peruns2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Perun2. If not, see <http://www.gnu.org/licenses/>.
*/

#include "gen-time.h"
#include "gen-generic.h"
#include "../generator.h"
#include "../../os.h"


namespace perun2::gen
{

IncreasedTime::IncreasedTime (_genptr<_tim>& tim, _genptr<_per>& per)
   : time(std::move(tim)), period(std::move(per)) { }

_tim IncreasedTime::getValue()
{
   _tim t = this->time->getValue();
   t += this->period->getValue();
   return t;
}

DecreasedTime::DecreasedTime(_genptr<_tim>& tim, _genptr<_per>& per)
   : time(std::move(tim)), period(std::move(per)) {};

_tim DecreasedTime::getValue()
{
   _tim t = this->time->getValue();
   t -= this->period->getValue();
   return t;
}

_tim v_Now::getValue()
{
   return os_now();
}

_tim v_Today::getValue()
{
   return os_today();
}

_tim v_Yesterday::getValue()
{
   return os_yesterday();
}

_tim v_Tomorrow::getValue()
{
   return os_tomorrow();
}

TimeDate::TimeDate(_genptr<_tim>& val) 
   : UnaryOperation<_tim>(val) { }

_tim TimeDate::getValue() 
{
   return this->value->getValue().toDate();
}

}
