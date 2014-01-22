//
//  SubMatch.h
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

#ifndef SUBMATCH_H
#define SUBMATCH_H

#include <string>
#include <memory>
#include <regex>

// Forward declaration
class Match;

class SubMatch : public std::csub_match {
public:
    SubMatch(const Match &match, unsigned index);

private:
    // the matching string data must be stored because std::csub_match and std::cmatch rely on it, and there is no guarantee that it would be kept in the lua stack
    const std::shared_ptr<const std::string> sharedData_;
};

#endif
