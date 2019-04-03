;******************************************************************************************************************************************
; 文件名称:	startup_SWM2400.s
; 功能说明:	SWM2400单片机的启动文件
; 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
; 注意事项:
; 版本日期: V1.0.0		2016年1月30日
; 升级记录:
;
;
;******************************************************************************************************************************************
; @attention
;
; THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
; REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
; FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
; OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
; -ECTION WITH THEIR PRODUCTS.
;
; COPYRIGHT 2012 Synwit Technology
;******************************************************************************************************************************************

; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp               ; Top of Stack
                DCD     Reset_Handler              ; Reset Handler
                DCD     NMI_Handler                ; NMI Handler
                DCD     HardFault_Handler          ; Hard Fault Handler
                DCD     MemManage_Handler          ; MPU Fault Handler
                DCD     BusFault_Handler           ; Bus Fault Handler
                DCD     UsageFault_Handler         ; Usage Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler
                DCD     DebugMon_Handler           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     PendSV_Handler             ; PendSV Handler
                DCD     SysTick_Handler            ; SysTick Handler

                ; External Interrupts
                DCD     GPIOA0_IRQHandler
                DCD     GPIOA1_IRQHandler
                DCD     GPIOA2_IRQHandler
                DCD     GPIOA3_IRQHandler
                DCD     GPIOA4_IRQHandler
                DCD     GPIOA5_IRQHandler
                DCD     GPIOA6_IRQHandler
                DCD     GPIOA7_IRQHandler
				DCD     GPIOB0_IRQHandler
                DCD     GPIOB1_IRQHandler
                DCD     GPIOB2_IRQHandler
                DCD     GPIOB3_IRQHandler
                DCD     GPIOB4_IRQHandler
                DCD     GPIOB5_IRQHandler
                DCD     GPIOB6_IRQHandler
                DCD     GPIOB7_IRQHandler
                DCD     GPIOC0_IRQHandler
                DCD     GPIOC1_IRQHandler
                DCD     GPIOC2_IRQHandler
                DCD     GPIOC3_IRQHandler
                DCD     GPIOC4_IRQHandler
                DCD     GPIOC5_IRQHandler
                DCD     GPIOC6_IRQHandler
                DCD     GPIOC7_IRQHandler                                 
                DCD     GPIOM0_IRQHandler
                DCD     GPIOM1_IRQHandler
                DCD     GPIOM2_IRQHandler
                DCD     GPIOM3_IRQHandler
                DCD     GPIOM4_IRQHandler
                DCD     GPIOM5_IRQHandler
                DCD     GPIOM6_IRQHandler
                DCD     GPIOM7_IRQHandler                                            
				DCD     DMA_IRQHandler
                DCD     LCD_IRQHandler
                DCD     NORFLC_IRQHandler
				DCD		CAN_IRQHandler
                DCD     TIMR_IRQHandler
                DCD     WDT_IRQHandler
                DCD     PWM_IRQHandler
                DCD     UART0_IRQHandler
				DCD     UART1_IRQHandler
				DCD     UART2_IRQHandler
				DCD     UART3_IRQHandler
				DCD     0
				DCD     I2C0_IRQHandler
                DCD     I2C1_IRQHandler
                DCD     SPI0_IRQHandler
                DCD     ADC0_IRQHandler
                DCD     RTC_IRQHandler
                DCD     ANAC_IRQHandler
                DCD     SDIO_IRQHandler
                DCD     GPIOA_IRQHandler
                DCD     GPIOB_IRQHandler
                DCD     GPIOC_IRQHandler
                DCD     GPIOM_IRQHandler
                DCD     GPION_IRQHandler
                DCD     GPIOP_IRQHandler
                DCD     ADC1_IRQHandler
                DCD     FPU_IRQHandler
				DCD     SPI1_IRQHandler
                         
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors



                AREA    |.text|, CODE, READONLY


Reset_Handler    PROC
                 EXPORT  Reset_Handler             [WEAK]
        IMPORT  __main
                 LDR     R0, =__main
                 BX      R0
                 ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                [WEAK]
                B       .
                ENDP

HardFault_Handler PROC
                EXPORT  HardFault_Handler          [WEAK]
                B       .
                ENDP

MemManage_Handler PROC
                EXPORT  MemManage_Handler          [WEAK]
                B       .
                ENDP

BusFault_Handler PROC
                EXPORT  BusFault_Handler           [WEAK]
                B       .
                ENDP

UsageFault_Handler PROC
                EXPORT  UsageFault_Handler         [WEAK]
                B       .
                ENDP

SVC_Handler     PROC
                EXPORT  SVC_Handler                [WEAK]
                B       .
                ENDP

DebugMon_Handler PROC
                EXPORT  DebugMon_Handler           [WEAK]
                B       .
                ENDP

PendSV_Handler  PROC
                EXPORT  PendSV_Handler             [WEAK]
                B       .
                ENDP

SysTick_Handler PROC
                EXPORT  SysTick_Handler            [WEAK]
                B       .
                ENDP

GPIOA0_IRQHandler PROC
                EXPORT  GPIOA0_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOA1_IRQHandler PROC
                EXPORT  GPIOA1_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOA2_IRQHandler PROC
                EXPORT  GPIOA2_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOA3_IRQHandler PROC
                EXPORT  GPIOA3_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOA4_IRQHandler PROC
                EXPORT  GPIOA4_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOA5_IRQHandler PROC
                EXPORT  GPIOA5_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOA6_IRQHandler PROC
                EXPORT  GPIOA6_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOA7_IRQHandler PROC
                EXPORT  GPIOA7_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOB0_IRQHandler PROC
                EXPORT  GPIOB0_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOB1_IRQHandler PROC
                EXPORT  GPIOB1_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOB2_IRQHandler PROC
                EXPORT  GPIOB2_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOB3_IRQHandler PROC
                EXPORT  GPIOB3_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOB4_IRQHandler PROC
                EXPORT  GPIOB4_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOB5_IRQHandler PROC
                EXPORT  GPIOB5_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOB6_IRQHandler PROC
                EXPORT  GPIOB6_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOB7_IRQHandler PROC
                EXPORT  GPIOB7_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOC0_IRQHandler PROC
                EXPORT  GPIOC0_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOC1_IRQHandler PROC
                EXPORT  GPIOC1_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOC2_IRQHandler PROC
                EXPORT  GPIOC2_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOC3_IRQHandler PROC
                EXPORT  GPIOC3_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOC4_IRQHandler PROC
                EXPORT  GPIOC4_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOC5_IRQHandler PROC
                EXPORT  GPIOC5_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOC6_IRQHandler PROC
                EXPORT  GPIOC6_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOC7_IRQHandler PROC
                EXPORT  GPIOC7_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOM0_IRQHandler PROC
                EXPORT  GPIOM0_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOM1_IRQHandler PROC
                EXPORT  GPIOM1_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOM2_IRQHandler PROC
                EXPORT  GPIOM2_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOM3_IRQHandler PROC
                EXPORT  GPIOM3_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOM4_IRQHandler PROC
                EXPORT  GPIOM4_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOM5_IRQHandler PROC
                EXPORT  GPIOM5_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOM6_IRQHandler PROC
                EXPORT  GPIOM6_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOM7_IRQHandler PROC
                EXPORT  GPIOM7_IRQHandler            [WEAK]
                B       .
                ENDP

DMA_IRQHandler PROC
                EXPORT  DMA_IRQHandler            [WEAK]
                B       .
                ENDP

LCD_IRQHandler PROC
                EXPORT  LCD_IRQHandler            [WEAK]
                B       .
                ENDP

NORFLC_IRQHandler PROC
                EXPORT  NORFLC_IRQHandler            [WEAK]
                B       .
                ENDP

CAN_IRQHandler PROC
                EXPORT  CAN_IRQHandler            [WEAK]
                B       .
                ENDP

TIMR_IRQHandler PROC
                EXPORT  TIMR_IRQHandler            [WEAK]
                B       .
                ENDP

WDT_IRQHandler PROC
                EXPORT  WDT_IRQHandler            [WEAK]
                B       .
                ENDP

PWM_IRQHandler PROC
                EXPORT  PWM_IRQHandler            [WEAK]
                B       .
                ENDP

UART0_IRQHandler PROC
                EXPORT  UART0_IRQHandler            [WEAK]
                B       .
                ENDP

UART1_IRQHandler PROC
                EXPORT  UART1_IRQHandler            [WEAK]
                B       .
                ENDP

UART2_IRQHandler PROC
                EXPORT  UART2_IRQHandler            [WEAK]
                B       .
                ENDP

UART3_IRQHandler PROC
                EXPORT  UART3_IRQHandler            [WEAK]
                B       .
                ENDP

I2C0_IRQHandler PROC
                EXPORT  I2C0_IRQHandler            [WEAK]
                B       .
                ENDP

I2C1_IRQHandler PROC
                EXPORT  I2C1_IRQHandler            [WEAK]
                B       .
                ENDP

SPI0_IRQHandler PROC
                EXPORT  SPI0_IRQHandler            [WEAK]
                B       .
                ENDP

ADC0_IRQHandler PROC
                EXPORT  ADC0_IRQHandler            [WEAK]
                B       .
                ENDP

RTC_IRQHandler PROC
                EXPORT  RTC_IRQHandler            [WEAK]
                B       .
                ENDP

ANAC_IRQHandler PROC
                EXPORT  ANAC_IRQHandler            [WEAK]
                B       .
                ENDP

SDIO_IRQHandler PROC
                EXPORT  SDIO_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOA_IRQHandler PROC
                EXPORT  GPIOA_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOB_IRQHandler PROC
                EXPORT  GPIOB_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOC_IRQHandler PROC
                EXPORT  GPIOC_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOM_IRQHandler PROC
                EXPORT  GPIOM_IRQHandler            [WEAK]
                B       .
                ENDP

GPION_IRQHandler PROC
                EXPORT  GPION_IRQHandler            [WEAK]
                B       .
                ENDP

GPIOP_IRQHandler PROC
                EXPORT  GPIOP_IRQHandler            [WEAK]
                B       .
                ENDP

ADC1_IRQHandler PROC
                EXPORT  ADC1_IRQHandler            [WEAK]
                B       .
                ENDP

FPU_IRQHandler PROC
                EXPORT  FPU_IRQHandler            [WEAK]
                B       .
                ENDP

SPI1_IRQHandler PROC
                EXPORT  SPI1_IRQHandler            [WEAK]
                B       .
                ENDP

                ALIGN


;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                IF      :DEF:__MICROLIB
                
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit
                
                ELSE
                
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
                 
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF

                END
