#ifndef PROPLIB_PSEUDOHANA_HPP_
#define PROPLIB_PSEUDOHANA_HPP_

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

namespace pseudohana {
template <typename T>
struct type_t { using type = T; };
template <typename T>
type_t<T> type_;

template <typename Type, Type Value>
struct value_t {
  using type = Type;
  static constexpr type value{Value};
  constexpr operator Type () const { return Value; };
  static constexpr type get() { return Value; }
};
template <typename Type, Type Value>
value_t<Type, Value> value_;

template <bool Value>
using bool_t = value_t<bool, Value>;
using true_t = bool_t<true>;
using false_t = bool_t<false>;

template <bool Value> bool_t<Value> bool_;
true_t true_;
false_t false_;

constexpr true_t operator!(false_t) { return {}; };
constexpr false_t operator!(true_t) { return {}; };

template <typename T, typename S>
constexpr auto operator==(type_t<T>, type_t<S>) {
  return bool_<std::is_same<T, S>::value>;
};

template <typename T, typename S>
constexpr auto operator!=(type_t<T> x, type_t<S> y) { return !(x == y); }

template <typename T1, T1 V1, typename T2, T2 V2>
constexpr auto operator==(value_t<T1, V1>, value_t<T2, V2>) {
  return bool_<type_<T1> == type_<T2> && V1 == V2>;
};

} // pseudohana

#endif
