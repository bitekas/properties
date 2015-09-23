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

#define LIBPROPERTY__TAG_NAME(name) _proplib__##name##_prop_tag
#define LIBPROPERTY__FUNC_NAME _proplib__offset_get

namespace libproperty {
namespace impl {
/**
 * Alternative implementation of offsetof, but with a member pointer instead of
 * a name. At this point, the host type is fully defined.
 */
template <typename Host, typename T>
auto constexpr member_pointer_to_offset(T ptr) {
  return reinterpret_cast<std::size_t>(&((Host*)0->*ptr));
};

template <typename Host, typename Property>
Host const& get_host(Property const* property) {
  return *reinterpret_cast<Host const*>(
             reinterpret_cast<char const*>(property) -
             member_pointer_to_offset<Host>(Host::LIBPROPERTY__FUNC_NAME(
                 meta::type_c<typename Property::type_tag>)));
}

template <typename Host, typename Property>
Host& get_host(Property* property) {
  return const_cast<Host&>(
      get_host<Host>(const_cast<Property const*>(property)));
};

}  // impl
} // property



#endif
