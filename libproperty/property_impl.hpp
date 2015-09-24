#ifndef LIBPROPERTY_PROPERTY_IMPL_HPP_
#define LIBPROPERTY_PROPERTY_IMPL_HPP_

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

#include "meta.hpp"
#include <cstddef> // for std::size_t

#define LIBPROPERTY__TAG_NAME(name) _libproperty__##name##_prop_tag
#define LIBPROPERTY__FUNC_NAME _libproperty__offset_get

namespace libproperty {
namespace impl {

template <typename T>
auto constexpr type_tag(T const&) {
  using meta::type_c;
  return type_c<typename T::type_tag>;
}

/**
 * Alternative implementation of offsetof, but with a member pointer instead of
 * a name. At this point, the host type is fully defined.
 */
template <typename Host, typename PointerToMemberType>
auto constexpr offset_of(PointerToMemberType member_ptr) {
  return reinterpret_cast<std::size_t>(&((Host*)0->*member_ptr));
}

template <typename Host, typename Property>
auto constexpr offset_of_property(Property const* property) {
  auto const tag = type_tag(*property);
  auto const member_ptr = Host::LIBPROPERTY__FUNC_NAME(tag);
  return offset_of<Host>(member_ptr);
}

/**
 * @param property should be the 'this' pointer of an rw_property or
 * compatible class.
 */
template <typename Host, typename Property>
constexpr Host const& get_host(Property const* property) {
  auto const raw_property_ptr = reinterpret_cast<char const*>(property);
  auto const raw_host_ptr =
      raw_property_ptr - offset_of_property<Host>(property);
  auto const host_ptr = reinterpret_cast<Host const*>(raw_host_ptr);
  return *host_ptr;
}

/**
 * Cast const back in, then cast it out again, so main functions remain const.
 */
template <typename Host, typename Property>
Host& get_host(Property* property) {
  auto const const_property_ptr = const_cast<Property const*>(property);
  auto const& const_host_ref = get_host<Host>(const_property_ptr);
  return const_cast<Host&>(const_host_ref);
}

}  // impl
} // property



#endif
