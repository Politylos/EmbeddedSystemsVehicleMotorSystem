/*
 * sensors.c
 *
 *  Created on: 23 May 2022
 *      Author: Baker
 */


#include "sensors.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ti/drivers/I2C.h>
#include "Board.h"
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/swi.h>
#include <ti/sysbios/knl/Task.h>
#include "sensors.h"
#include "Interface/GraphFunctions.h"
#include "Interface/GraphData.h"
#include "bmi160.h"
#include "opt3001.h"
#include "i2cDriver.h"
#include "drivers/Motor/Estop.h"
#include <driverlib/adc.h>
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>



double LightSecond=0;
double AccelSecond = 0;
Swi_Handle swiAccelSensorFilterHandle;
Swi_Handle swiLightSensorFilterHandle;
Swi_Handle swiCurrentSensorFilterHandle;

I2C_Handle i2c;
I2C_Params i2cParams;          // Structure for I2C parameters
/* I2C initialisation function */
void i2c_Open(void)
{

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2c = I2C_open(Board_I2C2, &i2cParams);
    if (i2c == NULL)
    {
        System_abort("Error initialising I2C\n");
    }
}

#define SENSOR_LIGHT_BUFFERSIZE 5
double opt3001_light_values[SENSOR_LIGHT_BUFFERSIZE];
/*
 * Task thread to read light values from opt3001 at 2 Hz
 */
void readOptFxn(UArg arg0, UArg arg1)
{
    int graphpoint=0;
    int sendtoGraph = 0;
    bool result;
    uint8_t opt3001_buffer_position = 0;
    uint16_t opt3001_value;
    double opt3001_value_converted;

    result = opt3001_init();
    if (!result)
    {
        System_printf("opt3001 failed to initialise\n");
        System_flush();
    }
    else
    {
        System_printf("opt3001 initialised!\n");
        System_flush();
    }

    while (1)
    {
        // Read light value from opt3001
        result = opt3001_read(&opt3001_value);
        if (result)
        {
            sendtoGraph++;
            opt3001_convert(opt3001_value, &opt3001_value_converted);
            opt3001_light_values[opt3001_buffer_position] =
                     opt3001_value_converted;
            if (++opt3001_buffer_position >= SENSOR_LIGHT_BUFFERSIZE)
            {
                opt3001_buffer_position = 0;
                // Post Swi for filtering
                Swi_post(swiLightSensorFilterHandle);
            }
            if (sendtoGraph>=2){
                sendtoGraph=0;
                if (opt3001_buffer_position-1  < 0){
                    ArrayUpdate(LightData,(opt3001_light_values[opt3001_buffer_position]+opt3001_light_values[4])/2);
                    LightSecond= (opt3001_light_values[opt3001_buffer_position]+opt3001_light_values[4])/2;
                }else{
                LightSecond= (opt3001_light_values[opt3001_buffer_position]+opt3001_light_values[opt3001_buffer_position-1])/2;
                ArrayUpdate(LightData,(opt3001_light_values[opt3001_buffer_position]+opt3001_light_values[opt3001_buffer_position-1])/2);
                }

            }
        }
        else
        {
            System_printf("opt3001 not read\n");
            System_flush();
        }
        Task_sleep(500);
    }
}

double opt3001_filteredValue = 0;
//opt3001_filteredValue = 0;
/*
 * Swi for light sensor filtering
 */
void lightSensorFilterFxn(void)
{
    uint8_t buffer_position;
    double light_value_average = 0;


    // TODO protect opt3001_light_values
    for (buffer_position = 0; buffer_position < SENSOR_LIGHT_BUFFERSIZE; buffer_position++)
    {
        light_value_average += opt3001_light_values[buffer_position];
    }

    // TODO protect opt3001_filteredValue
    opt3001_filteredValue = light_value_average / SENSOR_LIGHT_BUFFERSIZE;
}



#define SENSOR_ACCEL_BUFFERSIZE 10
#define SENSOR_ACCEL_BUFFERSIZE_Filtered 20
bmi160_accel_values_t bmi160_accel_values[SENSOR_ACCEL_BUFFERSIZE];
double Sensor_Filtered_Buffer[SENSOR_ACCEL_BUFFERSIZE_Filtered];
int FilteredAccelPos = 0;
/*
 * Task thread to read acceleration values from bmi160 sensor at 200 Hz
 */
void readAccFxn(UArg arg0, UArg arg1)
{
    bool result;    // Return value from functions
    bmi160_accel_values_t bmi160_current_values;    // Struct for accel values
    uint8_t bmi160_buffer_position = 0;            // Buffer position for filter

    result = bmi160_init();
    if (!result)
    {
        System_printf("bmi160 failed to initialise\n");
        System_flush();
    }
    else
    {
        System_printf("bmi160 initialised!\n");
        System_flush();
    }

    while (1)
    {
        // Read 3 axis values from bmi160
        result = bmi160_getAccel(&bmi160_current_values);
        // Check if result is valid
        if (result)
        {
            // Copy new values into buffer
            bmi160_accel_values[bmi160_buffer_position].acc_x =
                    bmi160_current_values.acc_x;
            bmi160_accel_values[bmi160_buffer_position].acc_y =
                    bmi160_current_values.acc_y;
            bmi160_accel_values[bmi160_buffer_position].acc_z =
                    bmi160_current_values.acc_z;

            // Debug values (cannot output doubles.......)
//            System_printf("Acc %d: x: %d  y: %d  z: %d\n",
//                          bmi160_buffer_position,
//                          (int)bmi160_accel_values[bmi160_buffer_position].acc_x,
//                          (int)bmi160_accel_values[bmi160_buffer_position].acc_y,
//                          (int)bmi160_accel_values[bmi160_buffer_position].acc_z);
//            System_flush();

            // Increment buffer position, roll over if required
            if (++bmi160_buffer_position >= SENSOR_ACCEL_BUFFERSIZE)
            {
                bmi160_buffer_position = 0;
                // Post Swi for filtering
                Swi_post(swiAccelSensorFilterHandle);
                if (FilteredAccelPos == 0){
                    AccelSecond=CalcAvg(Sensor_Filtered_Buffer,SENSOR_ACCEL_BUFFERSIZE_Filtered);
                    ArrayUpdate(AccelData,AccelSecond);
                }
            }
        }
        else
        {
            System_printf("bmi160 not read\n");
            System_flush();
        }
        Task_sleep(5); // 200 Hz
    }
}

/*
 * Swi for accel sensor filtering
 */
double bmi160_filteredValue = 0;
void accelSensorFilterFxn(void)
{
    uint8_t buffer_position;
    double acc_x_average = 0;
    double acc_y_average = 0;
    double acc_z_average = 0;


    // TODO protect bmi160_accel_values
    for (buffer_position = 0; buffer_position < SENSOR_ACCEL_BUFFERSIZE;
            buffer_position++)
    {
        acc_x_average += bmi160_accel_values[buffer_position].acc_x;
        acc_y_average += bmi160_accel_values[buffer_position].acc_y;
        acc_z_average += bmi160_accel_values[buffer_position].acc_z;
    }

    // Calculate averages for each axis
    acc_x_average /= SENSOR_ACCEL_BUFFERSIZE;
    acc_y_average /= SENSOR_ACCEL_BUFFERSIZE;
    acc_z_average /= SENSOR_ACCEL_BUFFERSIZE;

    // Calculate absolute sum of axes
    // TODO protect bmi160_filteredValue
    Sensor_Filtered_Buffer[FilteredAccelPos] = fabs(acc_x_average) + fabs(acc_y_average)
            + fabs(acc_z_average);
    FilteredAccelPos=(FilteredAccelPos+1)%SENSOR_ACCEL_BUFFERSIZE_Filtered;
    if (MaxAccel  <= Sensor_Filtered_Buffer[FilteredAccelPos]){
        EstopPower();


    }


}

/*
 * Initialise the ADC for current readings
 */
void adc_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    // ISEN B current sensor AIN0 PE_3
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    ADCSequenceConfigure(ADC1_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE, 2, 0, ADC_CTL_IE | ADC_CTL_CH0 | ADC_CTL_END);
    ADCSequenceEnable(ADC1_BASE, 2);
    ADCIntClear(ADC1_BASE, 2);

    // ISEN C current sensor AIN4 PD_7
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_7);
    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_IE | ADC_CTL_CH4 | ADC_CTL_END);
    ADCSequenceEnable(ADC1_BASE, 1);
    ADCIntClear(ADC1_BASE, 1);
}

/*
 * Task thread to read current values from ADC
 */
#define SENSOR_CURRENT_BUFFERSIZE 5
double adc_current_values[SENSOR_CURRENT_BUFFERSIZE];
void readCurrentFxn(UArg arg0, UArg arg1)
{
    // ADC full scale voltage 3.3V (datasheet p1861)
    // ADC full resolution 12 bits 4096 (datasheet p1862)
    // ADC volts = (ADC_FULLSCALE / RESOLUTION) * adc_value
    // Raw values from ADC
    uint8_t adc_current_buffer_pos = 0;
    uint32_t adcRawValue1[1], adcRawValue2[1];
    double adcVolts1, adcVolts2;
    double adcCurrentB, adcCurrentC, adcCurrentTotal;

    // Initialise ADC for current reading
    adc_init();

    // Spin the wheels
    while (1)
    {
        // Start ADC conversions
        ADCProcessorTrigger(ADC1_BASE, 1);
        ADCProcessorTrigger(ADC1_BASE, 2);

        // Wait for ADC conversions to complete
        while(!ADCIntStatus(ADC1_BASE, 1, false));
        while(!ADCIntStatus(ADC1_BASE, 2, false));

        // Clear the ADC conversion complete interrupts
        ADCIntClear(ADC1_BASE, 1);
        ADCIntClear(ADC1_BASE, 2);

        // Read the ADC value registers
        ADCSequenceDataGet(ADC1_BASE, 1, adcRawValue1);
        ADCSequenceDataGet(ADC1_BASE, 2, adcRawValue2);

        // Convert ADC values to volts
        adcVolts1 = ((double)adcRawValue1[0] * 3.3) / 4096.0;
        adcVolts2 = ((double)adcRawValue2[0] * 3.3) / 4096.0;

        // Calculate current of ADC value
        adcCurrentB = ((3.3 / 2.0) - adcVolts2) / (10 * 0.007);
        adcCurrentC = ((3.3 / 2.0) - adcVolts1) / (10 * 0.007);

        // Combine 2 phases
        adcCurrentTotal = (adcCurrentB + adcCurrentC) * 1.5;

        // Copy value to buffer
        adc_current_values[adc_current_buffer_pos] = adcCurrentTotal;

        if (++adc_current_buffer_pos >= SENSOR_CURRENT_BUFFERSIZE)
        {
            adc_current_buffer_pos = 0;
            Swi_post(swiCurrentSensorFilterHandle);
        }

        Task_sleep(6); // 166 Hz
    }
}

double current_filteredValue = 0;

/*
 * Swi for current sensor filtering
 */
void currentSensorFilterFxn(void)
{
    uint8_t buffer_position;
    double current_value_average = 0;


    // TODO protect current_light_values
    for (buffer_position = 0; buffer_position < SENSOR_CURRENT_BUFFERSIZE; buffer_position++)
    {
        current_value_average += adc_current_values[buffer_position];
    }

    // TODO protect current_filteredValue
    current_filteredValue = current_value_average / SENSOR_CURRENT_BUFFERSIZE;
}

