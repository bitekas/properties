Libproperty
===========

An experimental design for a property library for C++.

This library borrows ideas from Louis Dionne's boost::hana library. Since it's
not in boost yet, I implemented a local version from memory.

Usage:
------

* See [test](tests/usage_test.cpp) for examples.

### Quick example: ###

```c++
#include "libproperty/property.hpp"
#include <iostream>
#include <string>
struct my_class {

  int const& my_getter() const { return property.value; }
  int const& my_setter(std::string const& x) {
    return property.value = atoi(x.c_str());
  }
  LIBPROPERTY_PROPERTY_WITH_STORAGE(
      int, property, my_class, my_getter, my_setter);
};

int main() {
  my_class a;
  a.property = "5";
  std::cout << "should print 8: " << a.property + 3 << '\n';
  std::cout << "sizeof(a): " << sizeof(a)
            << " == sizeof(int): " << sizeof(int) << '\n';
}
```

Public interface:
-----------------
For the public interface, see [the main source file](libproperty/property.hpp).


Other nifty features:
---------------------

Classes that use properties don't inflate (classes that just use the
getter/setter capabilities do inflate - one needs at least one byte per class).

   auto x = y.property;

does not work - the copy constructor is private, as are assignment operators for
the property type. It is therefore hard to copy it out of the class, where
accessing it would cause extreme havoc (don't do that!).

