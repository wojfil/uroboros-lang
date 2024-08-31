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

#pragma once

#include "../../tokens.h"

namespace perun2::parse
{

p_bool parseOneToken(Perun2Process& p2, const Tokens& tks, p_genptr<p_bool>& result);
p_bool parseOneToken(Perun2Process& p2, const Tokens& tks, p_genptr<p_num>& result);
p_bool parseOneToken(Perun2Process& p2, const Tokens& tks, p_genptr<p_str>& result);
p_bool parseOneToken(Perun2Process& p2, const Tokens& tks, p_genptr<p_nlist>& result);
p_bool parseOneToken(Perun2Process& p2, const Tokens& tks, p_genptr<p_tlist>& result);
p_bool parseOneToken(Perun2Process& p2, const Tokens& tks, p_genptr<p_list>& result);
p_bool parseOneToken(Perun2Process& p2, const Tokens& tks, p_genptr<p_tim>& result);
p_bool parseOneToken(Perun2Process& p2, const Tokens& tks, p_genptr<p_per>& result);
p_bool parseOneToken(Perun2Process& p2, const Tokens& tks, p_defptr& result);

}
