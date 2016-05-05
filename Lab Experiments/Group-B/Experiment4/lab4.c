#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/debug.h"
#include"driverlib/sysctl.h"
#include"driverlib/adc.h"

#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/uart.h"

#include"inc/hw_ints.h"
#include"driverlib/interrupt.h"
#include"utils/uartstdio.h"

#include "driverlib/timer.h"

#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

uint32_t ui32ADC0Value[4];

volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;
int i,j;
char t1,t2,st1,st2;
int mode = 0; //0 for monitor mode
uint32_t setTemp = 20;
int flag=0;

// initalizing the timer
void initTimer()
{
    uint32_t ui32Period;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    
    ui32Period = (SysCtlClockGet() / 1);
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    
    TimerEnable(TIMER0_BASE, TIMER_A);
}

// function to be called on tiemr interrupts
void Timer0IntHandler(void)
{
    
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    
    
    /*for(i=0;i<50;i++)
     {
     UARTCharPut(UART0_BASE, '\b');
     }*/
    
    //UARTCharPut(UART0_BASE, 'p');
    //setting the text to be shown on the display
    if(mode==0)//Monitor
    {
        t1 = ui32TempValueC/10+'0';
        t2 = ui32TempValueC%10+'0';
        
        st1 = setTemp/10 + '0';
        st2 = setTemp%10 + '0';
        UARTCharPut(UART0_BASE, '\r');
        UARTCharPut(UART0_BASE, '\n');
        UARTCharPut(UART0_BASE, 'C');
        UARTCharPut(UART0_BASE, 'u');
        UARTCharPut(UART0_BASE, 'r');
        UARTCharPut(UART0_BASE, 'r');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 'n');
        UARTCharPut(UART0_BASE, 't');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 'T');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 'm');
        UARTCharPut(UART0_BASE, 'p');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, t1);
        UARTCharPut(UART0_BASE, t2);
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 167);
        UARTCharPut(UART0_BASE, 'C');
        
        UARTCharPut(UART0_BASE, ',');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 'S');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 't');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 'T');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 'm');
        UARTCharPut(UART0_BASE, 'p');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, st1);
        UARTCharPut(UART0_BASE, st2);
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 167);
        UARTCharPut(UART0_BASE, 'C');
        
        if(ui32TempValueC<setTemp)
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 2);
        }
    }
    else if(mode==1)
    {
        //UARTCharPut(UART0_BASE, 'r');
        
        /*for(j=0;j<60;i++)
         {
         UARTCharPut(UART0_BASE, '\b');
         }*/
        UARTCharPut(UART0_BASE, '\r');
        UARTCharPut(UART0_BASE, '\n');
        UARTCharPut(UART0_BASE, 'E');
        UARTCharPut(UART0_BASE, 'n');
        UARTCharPut(UART0_BASE, 't');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 'r');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 't');
        UARTCharPut(UART0_BASE, 'h');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 't');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 'm');
        UARTCharPut(UART0_BASE, 'p');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 'r');
        UARTCharPut(UART0_BASE, 'a');
        UARTCharPut(UART0_BASE, 't');
        UARTCharPut(UART0_BASE, 'u');
        UARTCharPut(UART0_BASE, 'r');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, ':');
        UARTCharPut(UART0_BASE, ' ');
        mode=2;
        
    }
    else if(mode==3)
    {
        /*for(j=0;j<60;i++)
         {
         UARTCharPut(UART0_BASE, '\b');
         }*/
        
        st1 = setTemp/10 + '0';
        st2 = setTemp%10 + '0';
        UARTCharPut(UART0_BASE, '\r');
        UARTCharPut(UART0_BASE, '\n');
        UARTCharPut(UART0_BASE, 'S');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 't');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 'T');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 'm');
        UARTCharPut(UART0_BASE, 'p');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 'u');
        UARTCharPut(UART0_BASE, 'p');
        UARTCharPut(UART0_BASE, 'd');
        UARTCharPut(UART0_BASE, 'a');
        UARTCharPut(UART0_BASE, 't');
        UARTCharPut(UART0_BASE, 'e');
        UARTCharPut(UART0_BASE, 'd');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 't');
        UARTCharPut(UART0_BASE, 'o');
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, st1);
        UARTCharPut(UART0_BASE, st2);
        UARTCharPut(UART0_BASE, ' ');
        UARTCharPut(UART0_BASE, 167);
        UARTCharPut(UART0_BASE, 'C');
        mode=0;
    }
}

// function to be called on uart interrupts
void UARTIntHandler(void)
{
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
    UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
    if(mode==0 && UARTCharsAvail(UART0_BASE) && UARTCharGetNonBlocking(UART0_BASE)=='S')
    {
        mode=1;
        setTemp=0;
        
    }
    else if (mode==2)
    {
        
        while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
        {
            char a = UARTCharGetNonBlocking(UART0_BASE);
            UARTCharPut(UART0_BASE, a);
            if(a=='|')
                mode=3;
            else
                setTemp=setTemp*10+(a-'0');
        }
        
        
        
    }
    
}


int main(void)
{
    
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);// Sysclock set at 40MHz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);// Enable AD0 Peripheral
    
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);// use ADC0, sample sequencer 1, want the processor to trigger the sequence and we want to use the highest priority
    
    
    //code will average all four samples of sensor data on sequencer 1 to sample the temperature, so all four sequencer steps will measure the temperature sensor.
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    // configure interrupt flag to be set when sample is done, tell ADC logic this is last conversion on sequencer
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS | ADC_CTL_IE|ADC_CTL_END);
    
    // enabling adc sequencing
    ADCSequenceEnable(ADC0_BASE, 1);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    
    
    initTimer();
    
    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART0); //enable the UART interrupt
    // enabling uart interrupts
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts
    
    while(1)
    {
        ADCIntClear(ADC0_BASE, 1);
        ADCProcessorTrigger(ADC0_BASE, 1);
        while(!ADCIntStatus(ADC0_BASE, 1, false))
        {
        }
        
        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
        ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
        // converting sensor data to discernable temperatures
        ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
        ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
        
        //UARTCharPut(UART0_BASE, 'E');
        //UARTprintf("Enter text: ");
    }
    
}