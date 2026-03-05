# libproperty

A C++14 property library that adds getter/setter semantics to class members with **zero memory overhead** and only **one indirection call** of runtime cost (which is even inlined with `-O3`).

This version is a Proof of Concept while the recommended to use production-ready fork can be found here: https://github.com/atomgalaxy/libproperty.

## What Is a Property?

A property is a class member that behaves like a field at the call site but executes user-defined logic on every access. Reading a property calls a **getter**; writing to it calls a **setter**. The caller uses plain value syntax - assignment, arithmetic, passing to functions - while the class author controls what happens behind the scenes.

### Call semantics

```cpp
obj.prop = value;          // invokes the setter: obj.set(value)
auto x = obj.prop;         // invokes the getter: obj.get()
func(obj.prop);            // invokes the getter, passes result to func
obj.prop = obj.prop + 1;   // getter then setter
```

The property itself is not a value - it is an access point. Every read is a function call; every write is a function call. There is no "direct" access that bypasses the getter or setter.

### Effects

Because every access goes through a function, properties enable side effects that plain fields cannot:

- **Validation** - reject out-of-range values at the point of assignment.
- **Notification** - trigger observers, UI updates, or dirty flags on write.
- **Computed values** - return a derived result on read without storing it.
- **Access control** - expose a read-only or write-only view of internal state.
- **Lazy initialization** - defer expensive work until the value is first read.
- **Logging / instrumentation** - record every access for debugging or profiling.

Properties are a first-class feature in C#, Kotlin, Swift, Python (via descriptors), JavaScript/TypeScript (via getters/setters), and many other languages. C++ has no built-in property mechanism, which is the gap this library fills.

## Motivation

Languages like C# have first-class property support. In C++, the typical workaround is either a pair of `get_`/`set_` methods (verbose call sites) or a wrapper object that stores a back-pointer to the host (extra memory per property). libproperty eliminates both problems: properties look and feel like plain data members at the call site, yet add no bytes to the class layout.

## Properties and code porting

The absence of properties in C++ is one of the major pain points when porting code - automatically or semi-manually - from higher-level languages to C++. A C# or Kotlin class that relies on property getters and setters for validation, notification, or lazy initialization has no direct C++ equivalent. Transpilers and migration tools must either rewrite every property access into explicit `get_`/`set_` call pairs (changing the API surface and breaking the look-and-feel of the original code) or introduce a heavyweight wrapper that adds per-property memory overhead. Neither option preserves both the original semantics and the original syntax. libproperty solves all of these problems: properties that compile down to the same call semantics as the source language, keep the field-like syntax at the call site, add zero bytes to the class layout, and have only one indicrection call that is usually inlined.

## Key Features

- **Zero memory overhead** - a class with a property is exactly the same size as the underlying value it stores. Verified at compile time with `static_assert`.
- **One indirection call overhead** - each property access resolves to a single getter or setter call on the host. With `-O3` the offset arithmetic is fully inlined.
- **Header-only** - drop the `libproperty/` directory into your project and `#include "libproperty/property.hpp"`. No build step, no linking.
- **No external dependencies** - ships a minimal metaprogramming layer (`meta.hpp`) inspired by [Boost.Hana](https://www.boost.org/doc/libs/release/libs/hana/doc/html/index.html); no Boost install required.
- **Safe by default** - property objects cannot be copied or moved out of their host class. Copy and move constructors are private, preventing accidental extraction that would corrupt the offset arithmetic.

## How It Works

Traditional property wrappers store a pointer back to the host object so they can forward to the real getter/setter. That pointer costs memory.

libproperty replaces the stored pointer with a **compile-time offset calculation**. Each property knows its own byte offset within the host class. When a property is accessed, it recovers the host through pointer arithmetic:

```
host_ptr = reinterpret_cast<Host*>(
    reinterpret_cast<char*>(this) - compile_time_offset);
```

Because the offset is a compile-time constant, the compiler folds it into the generated code and no extra storage is required. The only runtime cost is the getter/setter call itself.

## Quick Start

```cpp
#include "libproperty/property.hpp"
#include <iostream>
#include <string>

struct my_class {
  // Getter: returns the stored value.
  int const& my_getter() const { return property.value; }

  // Setter: accepts a string and converts it to int.
  int const& my_setter(std::string const& x) {
    return property.value = atoi(x.c_str());
  }

  LIBPROPERTY_PROPERTY_WITH_STORAGE(
      int, property, my_class, my_getter, my_setter);
};

int main() {
  my_class a;
  a.property = "5";                 // calls my_setter
  std::cout << a.property + 3;      // calls my_getter, prints 8
  std::cout << sizeof(a);           // prints sizeof(int) - zero overhead
}
```

## API

The macro names (`LIBPROPERTY_PROPERTY_WITH_STORAGE`, `LIBPROPERTY_PROPERTY`, etc.) are intentionally verbose. This is a proof of concept, and the explicit naming is chosen to make the purpose and mechanics of each macro immediately clear to readers exploring the code for the first time.

### `LIBPROPERTY_PROPERTY_WITH_STORAGE(type, name, host, getter, setter)`

Declares a property member named `name` that **stores a value** of the given `type`. The getter and setter are member functions of `host` and can read/write `name.value` directly.

Use this when the property owns the underlying data.

### `LIBPROPERTY_PROPERTY(name, host, getter, setter)`

Declares a property member named `name` that does **not** provide meaningful storage. It allocates a minimal `char` for addressability but the getter and setter are expected to operate on other members of the host class.

Use this when the property is a view or alias over data stored elsewhere in the class.

## Examples

### Property with storage (zero overhead)

```cpp
template <typename T>
struct container {
  using self_ = container;

  T const& get() const { return data.value; }
  T const& set(T const& x) { return data.value = x; }

  LIBPROPERTY_PROPERTY_WITH_STORAGE(T, data, self_, get, set);
};

static_assert(sizeof(container<int>) == sizeof(int), "zero overhead");
```

### Multiple properties without storage

```cpp
struct sensor {
  using self_ = sensor;

  int raw_;

  int const& raw() const { return raw_; }
  int const& set_raw(int const& x) { return raw_ = x; }

  // Both properties alias the same underlying int.
  LIBPROPERTY_PROPERTY(input,  self_, raw, set_raw);
  LIBPROPERTY_PROPERTY(output, self_, raw, set_raw);
};
```

Because these properties carry only a `char` for addressability, the total size of `sensor` is `sizeof(int)` plus alignment padding - no back-pointers.

### Mixed: storage + alias

```cpp
struct my_class {
  int const& my_getter() const { return property.value; }
  int const& my_setter(std::string const& x) {
    return property.value = atoi(x.c_str());
  }
  int const& my_int_setter(int x) { return property.value = x; }

  LIBPROPERTY_PROPERTY_WITH_STORAGE(
      int, property, my_class, my_getter, my_setter);

  // as_int shares storage with property but accepts int directly.
  LIBPROPERTY_PROPERTY(as_int, my_class, my_getter, my_int_setter);
};
```

## Building and Testing

```sh
make test
```

Requires a C++14-capable compiler. Tested with Clang (`-std=c++14 -O3`).

## Project Structure

```
libproperty/
  property.hpp       - public API: macros and rw_property template
  property_impl.hpp  - offset calculation and host recovery
  meta.hpp           - lightweight type/value wrappers (Hana-inspired)
tests/
  usage_test.cpp     - examples and static_assert size checks
Makefile             - build and test rules
```

## Design Notes

### Compile-time offset resolution

The central trick is turning a pointer-to-member into a `std::size_t` byte offset without any runtime state. The type chain is:

1. **Tag type** - the `LIBPROPERTY_PROPERTY*` macro generates a unique empty struct inside the host (e.g. `_libproperty__property_prop_tag`). This struct exists only as a compile-time discriminator and occupies no storage.

2. **Tag-to-member-pointer dispatch** - the macro also generates a `static constexpr` function on the host that accepts the tag wrapped in `meta::type_<Tag>` and returns the corresponding pointer-to-member (`&Host::name`). The return type is deduced as the exact pointer-to-member type, e.g. `rw_property<...> Host::*`.

3. **Offset computation** - `offset_of` takes that pointer-to-member and resolves it to a `std::size_t` byte offset via null-pointer arithmetic:

   ```cpp
   template <typename Host, typename PointerToMemberType>
   auto constexpr offset_of(PointerToMemberType member_ptr) -> std::size_t {
     return reinterpret_cast<std::size_t>(&((Host*)0->*member_ptr));
   }
   ```

   The expression `&((Host*)0->*member_ptr)` applies the member pointer to a null base, producing the member's absolute address which, relative to zero, is exactly its byte offset. The result is a `std::size_t` constant that the compiler can fold away entirely at `-O3`.

4. **Getter/setter encoding** - the getter and setter member-function pointers are encoded as compile-time values using `meta::value_<decltype(&Host::fn), &Host::fn>`. This wraps each pointer in a type so it can be passed as a template parameter to `rw_property` and retrieved via `Getter::value` / `Setter::value` with no storage cost.

5. **Host recovery** - when a property is accessed, `get_host` subtracts the resolved `std::size_t` offset from the property's `this` pointer and `reinterpret_cast`s back to `Host*`. The entire sequence - tag lookup, offset computation, pointer subtraction - collapses to a constant subtract-and-cast under optimization, leaving only the user's getter or setter call as the single indirection.

### Copy safety

All copy/move constructors of `rw_property` are private (the host is declared `friend`). This prevents users from copying a property out of its host, which would make the offset arithmetic point to garbage.

## References

- [Boost.Hana](https://www.boost.org/doc/libs/release/libs/hana/doc/html/index.html) - Louis Dionne's metaprogramming library; the `meta.hpp` layer is a minimal implementation of similar ideas was the motivation for the present library.

## Copyright
Idea, proof of concept: Viktor Korsun, 2015.

Production implementation and improvements: Gašper Ažman, 2015.
This library originated from Viktor Korsun's "Implementing properties" talk at CppCon 2015. After the talk Gašper and Viktor had a discussion on how to reduce the memory overhead from 1 pointer size to zero, and later that night Gašper implemented the present PoC. A further improved version is available as a fork here: https://github.com/atomgalaxy/libproperty which is the recommended version for production use.

## License

MIT - see the license headers in the source files for the full text.
