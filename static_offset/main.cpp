#include "property.hpp"
#include <iostream>

class Test
{
    private:
        float m_width;

        void setWidth(const float value)
        {
            m_width = value;
        }

        const float getWidth()
        {
            return m_width;
        }

        const float getArea()
        {
            return m_width * height;
        }

    public:
        float height;

        rw_property<float, Test, &Test::getWidth, &Test::setWidth> width{this};
};

template<> intptr_t rw_property<float, Test, &Test::getWidth, &Test::setWidth>::offset = 0;

int main()
{
    Test test;
    test.width = 10;

    std::cout << test.width;
}
