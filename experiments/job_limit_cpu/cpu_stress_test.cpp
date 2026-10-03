#include <cmath>

int main()
{
    volatile double result = 0.0;
    while (true)
    {
        result += std::sin(result);
    }

    return 0;
}
