//
//  Match.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
