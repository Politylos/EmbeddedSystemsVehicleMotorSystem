#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


#define FIRST_STEP 0
#define SEQUENCE_NUMBER 0
#define ADC_PRIORITY 0
#define RESOLUTION 4095
#define REF_VOLTAGE 3.3

// Function prototypes
//void StartADCSampling();
//double GetCurrentValue();

/*
 */
void StartADCSampling() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    //

    ADCSequenceConfigure(ADC1_BASE, SEQUENCE_NUMBER, ADC_TRIGGER_PROCESSOR, ADC_PRIORITY);

    //

    //
    ADCSequenceStepConfigure(ADC1_BASE, SEQUENCE_NUMBER, FIRST_STEP, ADC_CTL_IE | ADC_CTL_CH0 | ADC_CTL_END);

    //
    // Since sample sequence 0 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC1_BASE, SEQUENCE_NUMBER);


    //
    ADCIntClear(ADC1_BASE, SEQUENCE_NUMBER);
}

/*
 * Gets the most recent VIout sample stored by ADC sampler and calculates current value from that.
 */
double GetCurrentValue() {

    uint32_t pui32ADC1Value[1];
    double current_value = 1.25;
    // Trigger the ADC conversion.
    ADCProcessorTrigger( ADC1_BASE, SEQUENCE_NUMBER );
    // Wait for conversion to be completed.
    while(!ADCIntStatus( ADC1_BASE, SEQUENCE_NUMBER , false) ) { }
    //Clear ADC Interrupt
    ADCIntClear( ADC1_BASE, SEQUENCE_NUMBER );
    // Read ADC FIFO buffer from sample sequence.
    ADCSequenceDataGet( ADC1_BASE, SEQUENCE_NUMBER , pui32ADC1Value );


    // Convert digital value to current reading (VREF- is 0, so it can be ignored)
    current_value = ((REF_VOLTAGE/2)- (pui32ADC1Value[1]))/(10*0.007);


    return current_value;
}
