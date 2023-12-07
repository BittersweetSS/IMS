#include "simlib.h"
#include <iostream>
#include <iomanip>

class cell
{
private:
public:
    simlib3::Integrator temperature;

    cell();
    ~cell();
};

cell::cell(/* args */)
{
}

cell::~cell()
{
}

#define X_SIZE 100
#define Y_SIZE 100
#define Z_SIZE 1

int main(int argc, char const *argv[])
{
    cell room[X_SIZE][Y_SIZE][Z_SIZE];

    for (size_t x, y, z = 0;;)
    {
        std::cout << std::setw(3);
        std::cout <<"(" << x << "," << y << "," << z << ")\n";
        if (++x >= X_SIZE)
            x = 0;
        else continue;
        if (++y >= Y_SIZE)
            y = 0;
        else continue;
        if (++z >= Z_SIZE)
            break;
    }
    

    return 0;
}
