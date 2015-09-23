#ifndef PROPLIB_PSEUDOHANA_HPP_c
#define PROPLIB_PSEUDOHANA_HPP_c

#include <utility> // for std::is_same

#if 0
The MIT License (MIT)

Copyright (c) 2015 Gašper Ažman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

The original idea for this: Louis Dionne, the boost::hana library.
#endif

namespace meta {
template <typename T>
struct type_ { using type = T; };
template <typename T>
type_<T> type_c;

template <typename Type, Type Value>
struct value_ {
  using type = Type;
  static constexpr type value{Value};
  constexpr operator Type () const { return Value; };
  static constexpr type get() { return Value; }
};
template <typename Type, Type Value>
value_<Type, Value> value_c;

template <bool Value>
using bool_ = value_<bool, Value>;
using true_ = bool_<true>;
using false_ = bool_<false>;

template <bool Value> bool_<Value> bool_c;
true_ true_c;
false_ false_c;

constexpr true_ operator!(false_) { return {}; };
constexpr false_ operator!(true_) { return {}; };

template <typename T, typename S>
constexpr auto operator==(type_<T>, type_<S>) {
  return bool_c<std::is_same<T, S>::value>;
};

template <typename T, typename S>
constexpr auto operator!=(type_<T> x, type_<S> y) { return !(x == y); }

template <typename T1, T1 V1, typename T2, T2 V2>
constexpr auto operator==(value_<T1, V1>, value_<T2, V2>) {
  return bool_c<type_c<T1> == type_c<T2> && V1 == V2>;
};

} // meta

#endif
