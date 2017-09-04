//
//  Match.cpp
//  integral
//
//  Copyright (C) 2013, 2014, 2016  André Pereira Henriques
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

#include "Match.hpp"

Match::Match(const char * string, const std::regex &pattern, Mode mode) : sharedData_(new std::string(string)) {
    std::cmatch match;
    switch (mode) {
        case Mode::EXACT:
            std::regex_match(sharedData_->c_str(), match, pattern);
            break;
        case Mode::PARTIAL:
            std::regex_search(sharedData_->c_str(), match, pattern);
            break;
    }
    std::cmatch::operator=(std::move(match));
}

