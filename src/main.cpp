#include "simlib.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

class termostat: Condition{
private:
    double ON_temperature;
    double OFF_temperature;
    Variable target_temperature;
public:
    Variable output;
    termostat(double on, double off, Integrator temperature_sensor):
        ON_temperature(on),
        OFF_temperature(off),
        target_temperature(on),
        output(0),
        Condition(target_temperature - temperature_sensor)
        {};
    void Action(){
        if(Up()){
            output = 1.0;
            target_temperature = OFF_temperature;
        }else{
            output = 0.0;
            target_temperature = OFF_temperature;
        }
    }
    ~termostat();
};

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
        Input equation = 0.0 * current->temperature;
        if(x != 0)
            equation = equation + heat_transef_equastion(current->temperature, room[x-1][y][z]->temperature);
        if(x != X_SIZE - 1)
            equation = equation + heat_transef_equastion(current->temperature, room[x+1][y][z]->temperature);
        if(y != 0)
            equation = equation + heat_transef_equastion(current->temperature, room[x][y-1][z]->temperature);
        if(y != Y_SIZE - 1)
            equation = equation + heat_transef_equastion(current->temperature, room[x][y+1][z]->temperature);
        current->temperature.SetInput(equation);

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
