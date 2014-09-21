//
//  RuntimeException.cpp
//  integral
//
//  Copyright (C) 2013, 2014  André Pereira Henriques
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

#include "RuntimeException.h"

namespace integral {
    const char * const RuntimeException::kUnknownExceptionMessage_ = "[integral] unknown exception thrown";
    const char * const RuntimeException::kInvalidStackExceptionMessage_ = "[integral] invalid Lua stack";
    
    RuntimeException::RuntimeException(const std::string &fileName, unsigned lineNumber, const std::string &functionName, const std::string &errorMessage) : std::runtime_error(errorMessage + " at " + fileName + ":" + std::to_string(lineNumber) + ":(" + functionName + ")") {}
}