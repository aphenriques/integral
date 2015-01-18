//
//  CallerException.h
//  integral
//
//  Copyright (C) 2014  André Pereira Henriques
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

#ifndef integral_CallerException_h
#define integral_CallerException_h

#include <stdexcept>
#include <string>

namespace integral {
    class CallerException : public std::runtime_error {
    public:
        inline CallerException(const std::string &errorMessage);
    };
    
    //--
    
    inline CallerException::CallerException(const std::string &errorMessage) : std::runtime_error(errorMessage) {}
}

#endif
