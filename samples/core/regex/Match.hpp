//
//  Match.hpp
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

#ifndef Match_hpp
#define Match_hpp

#include <memory>
#include <regex>
#include <string>
#include "SubMatch.hpp"

class Match : public std::cmatch {
public:
    enum class Mode {
        EXACT,
        PARTIAL
    };

    Match(const char * string, const std::regex &pattern, Mode mode);
    inline SubMatch getSubMatch(unsigned index) const;
    inline const std::shared_ptr<const std::string> & getSharedData() const;

private:
    // the matching string data must be stored because std::csub_match and std::cmatch rely on it, and there is no guarantee that lua would not collect it
    const std::shared_ptr<const std::string> sharedData_;
};

//-

inline SubMatch Match::getSubMatch(unsigned index) const {
    return SubMatch(*this, index);
}

inline const std::shared_ptr<const std::string> & Match::getSharedData() const {
    return sharedData_;
}

#endif
