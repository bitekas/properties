#include "../libproperty/property.hpp"

#include <cassert>
#include <cstddef>
#include <utility>
#include <iostream>
#include <string>

using namespace meta;

/** consumes 8 bytes, because of alignment */
template <int I>
struct property_test {
  using self_ = property_test;
public:
  int value_;

  int const& get_value() const { return value_; }
  int const& set_value(int const& x) { return value_ = x; }

  LIBPROPERTY_PROPERTY(prop1, self_, get_value, set_value);
  LIBPROPERTY_PROPERTY(prop2, self_, get_value, set_value);
  LIBPROPERTY_PROPERTY(prop3, self_, get_value, set_value);
};
/** consumes 8 bytes, because of alignment */
static_assert(sizeof(property_test<0>) == sizeof(int) + sizeof(int),
              "External property tester is supposed to be only its value and"
              " the property alignment!");

template <typename T>
struct property_with_storage_test {
  using self_ = property_with_storage_test;
public:
  T const& get_value() const { return prop1.value; }
  T const& set_value(T const& x) { return prop1.value = x; }

  LIBPROPERTY_PROPERTY_WITH_STORAGE(T, prop1, self_, get_value, set_value);
};
static_assert(sizeof(property_with_storage_test<char>) == sizeof(char),
              "Supposed to be equal in size as what it's storing!");
static_assert(sizeof(property_with_storage_test<int>) == sizeof(int),
              "Supposed to be equal in size as what it's storing!");
static_assert(sizeof(property_with_storage_test<long>) == sizeof(long),
              "Supposed to be equal in size as what it's storing!");

struct my_class {

  int const& my_getter() const { return property.value; }
  int const& my_setter(std::string const& x) {
    return property.value = atoi(x.c_str());
  }
  int const& my_int_setter(int x) { return property.value = x; }
  LIBPROPERTY_PROPERTY_WITH_STORAGE(
      int, property, my_class, my_getter, my_setter);
  LIBPROPERTY_PROPERTY(as_int, my_class, my_getter, my_int_setter);
};

struct offset_of_test {
  int i;
  size_t j;
};

auto y = [](auto x) {
  std::cout << x << std::endl;
};

int main() {
  {
    using namespace libproperty::impl;
    assert(offset_of<offset_of_test>(&offset_of_test::j) ==
           offsetof(offset_of_test, j));
    assert(offset_of<offset_of_test>(&offset_of_test::i) ==
           offsetof(offset_of_test, i));
  }
  {
    property_test<0> x;
    property_test<0> y;
    x.prop1 = 5;
    y = x;
    std::cout << "sizeof property_test: " << sizeof(x) << " " << x.prop1 << " "
              << y.prop2 << " prop1 offset:"
              << libproperty::impl::offset_of_property<decltype(x)>(&x.prop1)
              << " prop2 offset: "
              << libproperty::impl::offset_of_property<decltype(x)>(&x.prop2)
              << '\n';
  }
  {
    property_with_storage_test<int> x;
    x.prop1 = 5;
    auto y = x;
    std::cout << "sizeof property_test: " << sizeof(x) << " "
              << x.prop1 << " " << y.prop1 << '\n';
  }
  {
    my_class a;
    a.property = "5";
    std::cout << "should print 8: " << a.property + 3 << '\n';
    std::cout << "sizeof(a): " << sizeof(a)
              << " == sizeof(int): " << sizeof(int) << '\n';
  }
}
