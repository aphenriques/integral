//
//  type_manager.cpp
//  integral
//
//  Copyright (C) 2013  André Pereira Henriques
//  aphenriques (at) outlook (dot) com
//
//  This file is part of integral.
//
//  integral is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  integral is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with integral.  If not, see <http://www.gnu.org/licenses/>.
//

#include "type_manager.h"
#include <typeindex>
#include <lua.hpp>

namespace integral {
    namespace type_manager {
        const char * const gkTypeMangerRegistryKey = "integral_TypeManagerRegistryKey";
        const char * const gkTypeIndexKey = "integral_TypeIndexKey";
        const char * const gkVersionKey = "integral_VersionKey";
    }
}
