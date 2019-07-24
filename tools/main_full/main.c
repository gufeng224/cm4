/* Scheduler include files. */
#include "mosWrapper.h"

#include "drvAudio.h"
#include "drvMBX.h"
#include "drvINTC.h"
#include "halUART.h"
#include "halCPUINT.h"
#include "halCPU.h"
#include "halBDMA.h"
#include "halTimer.h"

#include "apiVOC.h"
#include "voc_task.h"
#include "mbx_task.h"
#include "ms_version.h"

/* Hardware register addresses. */
#define mainVTOR 					( * ( uint32_t * ) 0xE000ED08 )
#define mainNVIC_AUX_ACTLR			( * ( uint32_t * ) 0xE000E008 )

__attribute__((section(".plate_number"))) static unsigned long plateNum = 0;
unsigned long  plateCheck = 0;

/*
 * Configure the hardware as necessary to run this demo.
 */
static void prvSetupHardware( void );

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

/*-----------------------------------------------------------*/
extern void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority );
extern void vRegisterCLICommandsAudioTest( void );
extern void vRegisterSampleCLICommands( void );
/*-----------------------------------------------------------*/
extern unsigned long __vq_start__;
extern unsigned long __vq_end__;
extern unsigned long __vp_start__;
extern unsigned long __vp_end__;

int main( void )
{
  /* Configure the hardware ready to run the demo. */
  prvSetupHardware();
    // print version
  UartSendTrace("Version : %s %s\r\n",MVXV_CHANGELIST,MVXV_EXT);
  UartSendTrace("vp section start = 0x%x\n", (unsigned long)&__vp_start__);
  UartSendTrace("vp section size = 0x%x\n", (unsigned long)&__vp_end__-(unsigned long)&__vp_start__);
  UartSendTrace("vq section start = 0x%x\n", (unsigned long)&__vq_start__);
  UartSendTrace("vq section size = 0x%x\n", (unsigned long)&__vq_end__-(unsigned long)&__vq_start__);
  //vRegisterSampleCLICommands();
  //vRegisterCLICommandsAudioTest();
	/* Should not get here. */
  MbxTaskStart();
  VocTaskStart();

#if !defined(CONFIG_SYS_UART_DISABLE)
  vUARTCommandConsoleStart(256, ( tskIDLE_PRIORITY + 1 ));
#endif

  vTaskStartScheduler();
  for(;;);

}

static void prvSetupHardware( void )
{
extern unsigned long __Vectors[];

	mainVTOR = ( uint32_t ) __Vectors;

#if !defined(CONFIG_SYS_UART_DISABLE) && !defined(CONFIG_SYS_UART_EXT)
  MHal_UART_Init();
#endif

  MHal_Timer_Reset();

  MDrv_INTC_Init();

  MHal_BDMA_Init();

  MDrv_CPU_SetFreq(E_CPU_FREQ_HIGH);

  MDrv_AUD_Init();

  MDrv_MBX_Init();

  MApi_VOC_VDInit();
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

#if( CONFIG_BENCH == 1 )
double squareroot(double x)
{
    double it = x;
    while (fabs(it*it - x) > 1e-13) {
        it = it - (it*it-x)/(2*it);
    }
    return it;
}

__attribute__((section(".bench"))) double sum_real = 0;
void benchmark(void)
{
    const int num_iter = 100000;
    int i;
    TickType_t t = xTaskGetTickCount();
    for (i = 0; i < num_iter; i++) {
        sum_real += squareroot(10000.0);
    }
    TickType_t ttot = xTaskGetTickCount() - t;
    UartSendTrace("%d milliseconds\n\r", ttot);
}
#endif

void vApplicationIdleHook( void )
{
#if( CONFIG_BENCH == 1 )
  benchmark();
#endif

  if (plateCheck < 2 && plateNum != 0x87878787)
  {
    if (plateCheck == 0)
      plateNum = 0x87878787;
    else
      UartSendTrace("Warning: Bin end corruption\n");
    plateCheck++;
  }
}



void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;

}


