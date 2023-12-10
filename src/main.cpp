#include "simlib.h"
#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

class termostat: public Condition{
private:
    double ON_temperature;
    double OFF_temperature;
    Variable target_temperature;
public:
    Variable output;
    termostat(double on, double off, Input temperature_sensor):
        ON_temperature(on),
        OFF_temperature(off),
        target_temperature(on),
        output(1),
        Condition(target_temperature - temperature_sensor)
        {};
    void Action(){
        printf("Event");
        if(Up()){
            output = 1.0;
            target_temperature = ON_temperature;
        }else{
            output = 0.0;
            target_temperature = OFF_temperature;
        }
    }
};

const double Mass = 80;           // in kg
const double SpecificHeatCapacity = 4186.0; // in J/(kg·°C) water
const double HeatLossCoefficient = 0.004;
const double RadiatorPower = 1500; // in W
const double initialTemperature = 21; // in C

class Radiator : public aContiBlock {
    private:
    public:
    Integrator temperature;
    Input heaterPower;
    Input outsideTemperature;

    Radiator(Input heaterPower, Input outsideTemperature, double Mass, double specificHeatCapacity, double heatLossCoefficient) :
    heaterPower(heaterPower), outsideTemperature(outsideTemperature),
    temperature((heaterPower /Mass * specificHeatCapacity) - ((heatLossCoefficient * (temperature - outsideTemperature)) / Mass * specificHeatCapacity)) {}

     double Value() override {
    return temperature.Value();
}

};

class cell {
private:
    bool shouldConsumePower;
    bool isSensor;

public:
    Radiator *radiator;
    Integrator temperature;
    Input temp;

    cell(Radiator* Radiator, bool consumePower, bool isSensor, Input temp) : 
    temp(temp), 
    temperature(temp,0.), 
    radiator(Radiator),
    shouldConsumePower(consumePower),
    isSensor(isSensor)
    {}
    
    void Out(){
        printf("%-1.1f", temperature.Value());
        if (shouldConsumePower){
            printf("<-Radiator");
        }
    }

    ~cell();
};


#define X_SIZE 16
#define Y_SIZE 16
#define Z_SIZE 1

// needs to be stored on heap because it doesn't work if its just array
cell *room[X_SIZE][Y_SIZE][Z_SIZE];
termostat *thermostat;

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
    printf("\n");
}

Sampler S(Sample,10);
#define heat_transfer_equation(T1, T2) (-0.1 * ((T1) - (T2)))

int main(int argc, char const *argv[])
{  
    Radiator radiator(0., 0., Mass, SpecificHeatCapacity, HeatLossCoefficient);
    for (size_t x = 0, y = 0, z = 0;;)
    {
        bool consumePower = (x == 15 && y == 6 && z == 0);
        bool isSensor = (x == 0 && y == 7 && z == 0);
        room[x][y][z] = new cell(&radiator, consumePower,isSensor,0.);

        if (++x >= X_SIZE)
            x = 0;
        else continue;
        if (++y >= Y_SIZE)
            y = 0;
        else continue;
        if (++z >= Z_SIZE)
            break;
    }
    thermostat = new termostat(25.,18.,room[0][7][0]->temperature.Value());
    for (size_t x = 0, y = 0, z = 0;;)
    {
        cell *current = room[x][y][z];
        Input equation = 0.0 * current->temperature;
        
        if(x != 0)
            equation = equation + heat_transfer_equation(current->temperature, room[x-1][y][z]->temperature);
        if(x != X_SIZE - 1)
            equation = equation + heat_transfer_equation(current->temperature, room[x+1][y][z]->temperature);
        if(y != 0)
            equation = equation + heat_transfer_equation(current->temperature, room[x][y-1][z]->temperature);
        if(y != Y_SIZE - 1)
            equation = equation + heat_transfer_equation(current->temperature, room[x][y+1][z]->temperature);
        if (x == 0 || x == X_SIZE - 1 || y == 0 || y == Y_SIZE - 1) {
                Input rimHeatTransfer = heat_transfer_equation(current->temperature, 0.);
                equation = equation + HeatLossCoefficient * rimHeatTransfer;
            }

        if (x == 15 && y == 6 && z == 0) {
            current->radiator->heaterPower.Set(RadiatorPower);
            current->radiator->outsideTemperature.Set(equation);
            current->radiator->temperature.Init(initialTemperature);

            current->temperature.SetInput( thermostat->output * HeatLossCoefficient * current->radiator->temperature.Value());
            current->temperature.Init(current->radiator->temperature.Value());
        } else {
            
            current->temperature.SetInput(equation);
            current->temperature.Init(initialTemperature + double(x) / X_SIZE - double(y) / Y_SIZE + double(z) / Z_SIZE);
        }

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
   
    Init(0.);
    SetStep(1e-10,0.5);
    Run();
    return 0;
}
