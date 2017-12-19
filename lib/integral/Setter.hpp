//
//  Setter.hpp
//  integral
//
//  Copyright (C) 2017  André Pereira Henriques
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

#ifndef integral_Setter_hpp
#define integral_Setter_hpp

namespace integral {
    namespace detail {
        template<typename T, typename R>
        class Setter {
        public:
            inline Setter(R T::* attribute);

            inline void operator()(T &object, const R &value) const;

        private:
            R T::* const attribute_;
        };

        //--

        template<typename T, typename R>
        inline Setter<T, R>::Setter(R T::* attribute) : attribute_(attribute) {}

        template<typename T, typename R>
        inline void Setter<T, R>::operator()(T &object, const R &value) const {
            object.*attribute_ = value;
        }
    }
}

#endif
