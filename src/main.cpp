#include "simlib.h"
#include <iostream>
#include <iomanip>

using namespace std;

class cell{
private:
public:
    Integrator temperature;
    void Out(){
        printf("%-1.1f", temperature.Value());
    }
    cell(): temperature(0., 21.) {};
    ~cell();
};

cell::~cell(){}

#define X_SIZE 16
#define Y_SIZE 16
#define Z_SIZE 1

// needs to be stored on heap because it doesn't work if its just array
cell *room[X_SIZE][Y_SIZE][Z_SIZE];

void Sample(){
    printf("######################## %-9.3f ########################\n", T.Value());

    for (size_t x = 0, y = 0, z = 0;;)
    {
        auto current = room[x][y][z];
        current->Out();

        if(x == X_SIZE - 1)
            cout << endl;
        else
            cout << ' ';

        if (++x >= X_SIZE)
            x = 0;
        else continue;
        if (++y >= Y_SIZE)
            y = 0;
        else continue;
        if (++z >= Z_SIZE)
            break;
    }
}

Sampler S(Sample,10);

#define heat_transef_equastion(T1, T2) (-0.1 * ((T1) - (T2)))

int main(int argc, char const *argv[])
{
    for (size_t x = 0, y = 0, z = 0;;)
    {
        room[x][y][z] = new cell;

        if (++x >= X_SIZE)
            x = 0;
        else continue;
        if (++y >= Y_SIZE)
            y = 0;
        else continue;
        if (++z >= Z_SIZE)
            break;
    }
    for (size_t x = 0, y = 0, z = 0;;)
    {
        cell *current = room[x][y][z];

        current->temperature.SetInput(
            heat_transef_equastion(current->temperature, x != 0 ? room[x-1][y][z]->temperature : current->temperature) +
            heat_transef_equastion(current->temperature, x != X_SIZE - 1 ? room[x+1][y][z]->temperature : current->temperature) +
            heat_transef_equastion(current->temperature, y != 0 ? room[x][y-1][z]->temperature : current->temperature) +
            heat_transef_equastion(current->temperature, y != Y_SIZE - 1 ? room[x][y+1][z]->temperature : current->temperature) - 0.0);

        current->temperature.Init(21.0 + double(x) / X_SIZE - double(y) /Y_SIZE + double(z) / Z_SIZE);
        current->Out();

        if(x == X_SIZE - 1)
            cout << endl;
        else
            cout << ' ';

        if (++x >= X_SIZE)
            x = 0;
        else continue;
        if (++y >= Y_SIZE)
            y = 0;
        else continue;
        if (++z >= Z_SIZE)
            break;
    }
    
    Init(0, 100.00001);
    SetStep(1e-10,0.5);
    Run();
    return 0;
}
