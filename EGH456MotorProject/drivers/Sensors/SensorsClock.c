#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <ti/sysbios/knl/Clock.h>

#include "drivers/i2cOptDriver.h"
#include "opt3001.h"
Clock_Struct clkLight;
Clock_Handle clk2Handle;
uint16_t lightbits;
int calcPeriod(int freq){
    double time = 1/freq*1000000;
    return (int) time/Clock_tickPeriod;
}
bool InitSensors(){
    bool light =  InitOPT3001();
    return light;
}
bool InitSensorsHWI(){
    Clock_Params clkParams;
    /* init Light clock*/
    Clock_Params_init(&clkParams);

        clkParams.period = calcPeriod(2);
        clkParams.startFlag = TRUE;
        clkParams.arg = &lightbits;

        /* Construct a periodic Clock Instance with period = 5 system time units */
        Clock_construct(&clkLight, (Clock_FuncPtr)sensorOpt3001Read,
                       100, &clkParams);
        return true;

}
