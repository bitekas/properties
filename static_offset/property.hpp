//----------------------------------------------------------------------------------//
//                              An implementation of class properties
///    \project     Properties
///    \author      Viktor Korsun 
///    \date        19.09.2015.
///    \company     Zeptolab
//----------------------------------------------------------------------------------//

#include <cstdint>
#include <iostream>

#ifndef PROPERTY__PROPERTY_HPP
#define PROPERTY__PROPERTY_HPP

template <typename T, 
          typename h, 
          const T (h::*getter)(), 
          void (h::*setter)(const T value)
         >
class rw_property 
{
public:
    rw_property(h* host)
    {
        if (offset == 0)
            offset = reinterpret_cast<intptr_t>(this) - reinterpret_cast<intptr_t>(host);
    }                           

    rw_property<T, h, getter, setter>& operator = (const T& value)
    {
        ((reinterpret_cast<h*>(this - offset))->*setter)(value);
        return *this;
    }

    operator const T () 
    {
        return ((reinterpret_cast<h*>(this - offset))->*getter)();
    }

    static intptr_t offset;
};
#endif
