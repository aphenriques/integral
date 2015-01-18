//
//  DefaultArgument.h
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

#ifndef integral_DefaultArgument_h
#define integral_DefaultArgument_h

#include <utility>
#include "ArgumentTag.h"

namespace integral {
    // "T": type of default argument
    // "I": lua index (starts with 1) of the default argument
    template<typename T, unsigned I>
    class DefaultArgument {
    public:
        using ArgumentTag = detail::ArgumentTag<T, I>;
        
        // Arguments are forwarded to typename T constructor
        template<typename ...A>
        inline DefaultArgument(A &&...arguments);
        
        // Avoids template constructor ambiguity
        DefaultArgument(const DefaultArgument<T, I> &) = default;
        DefaultArgument(DefaultArgument<T, I> &&) = default;
        
        inline const T & getArgument() const;
        
    private:
        T argument_;
    };
    
    //--
    
    template<typename T, unsigned I>
    template<typename ...A>
    inline DefaultArgument<T, I>::DefaultArgument(A &&...arguments) : argument_(std::forward<A>(arguments)...) {}
    
    template<typename T, unsigned I>
    inline const T & DefaultArgument<T, I>::getArgument() const {
        return argument_;
    }

}

#endif
