#ifndef LIBPROPERTY_PROPERTY_HPP_c
#define LIBPROPERTY_PROPERTY_HPP_c

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
#endif

#include "property_impl.hpp"

#include <utility> // for std::forward

// only call in class scope!
#define LIBPROPERTY_PROPERTY_WITH_STORAGE(type, name, host, getter, setter) \
  ::libproperty::rw_property<                                               \
      ::meta::type_<struct LIBPROPERTY__TAG_NAME(name)>,              \
      ::meta::type_<host>,                                            \
      ::meta::value_<decltype(&host::getter), &host::getter>,         \
      ::meta::value_<decltype(&host::setter), &host::setter>,         \
      type> name;                                                           \
  auto static constexpr LIBPROPERTY__FUNC_NAME(decltype(type_tag(name))) {  \
    return &host::name;                                                     \
  }                                                                         \
  /* so that we can have the missing semicolon... */                        \
  static_assert(true, "")

#define LIBPROPERTY_PROPERTY(name, host, getter, setter) \
  LIBPROPERTY_PROPERTY_WITH_STORAGE(char, name, host, getter, setter)

namespace libproperty {

template <typename TypeTag,
          typename Host,
          typename Getter,
          typename Setter,
          typename ValueType>
struct rw_property {
  using getter = Getter;
  using setter = Setter;
  using type_tag = TypeTag;
  using host = typename Host::type;

  friend host;

  operator decltype(auto)() const {
    using namespace ::libproperty::impl;
    return (get_host<host>(this).*getter::value)();
  }
  template <typename X>
  decltype(auto) operator=(X&& x) {
    using namespace ::libproperty::impl;
    return (get_host<host>(this).*setter::value)(std::forward<X>(x));
  }

private: // for the use of host, not for nobody's!
  ValueType value; // possibly unused.

  /// disallow copying for non-friend users of the class - this doesn't have a
  /// value, but if copied, it can get really, really bad (stack corruption).
  rw_property() = default;
  rw_property& operator=(rw_property const&) = default;
  rw_property(rw_property const&) = default;
  ~rw_property() = default;
  rw_property& operator=(rw_property&&) = default;
  rw_property(rw_property&&) = default;
};

template <typename T>
constexpr auto type_tag(T const&) {
  using meta::type_c;
  return type_c<typename T::type_tag>;
}
}  // property

#endif
