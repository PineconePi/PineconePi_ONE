;******************************************************************************************************************************************
; 文件名称:    startup_SWM2400.s
; 功能说明:    SWM2400单片机的启动文件
; 技术支持:    http://www.synwit.com.cn/e/tool/gbook/?bid=1
; 注意事项:
; 版本日期: V1.0.0        2016年1月30日
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

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
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
        

        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =__iar_program_start
        BX      R0
        
        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler


        PUBWEAK GPIOA0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA0_IRQHandler
        B GPIOA0_IRQHandler

        PUBWEAK GPIOA1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA1_IRQHandler
        B GPIOA1_IRQHandler

        PUBWEAK GPIOA2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA2_IRQHandler
        B GPIOA2_IRQHandler

        PUBWEAK GPIOA3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA3_IRQHandler
        B GPIOA3_IRQHandler

        PUBWEAK GPIOA4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA4_IRQHandler
        B GPIOA4_IRQHandler

        PUBWEAK GPIOA5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA5_IRQHandler
        B GPIOA5_IRQHandler

        PUBWEAK GPIOA6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA6_IRQHandler
        B GPIOA6_IRQHandler

        PUBWEAK GPIOA7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA7_IRQHandler
        B GPIOA7_IRQHandler

        PUBWEAK GPIOB0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB0_IRQHandler
        B GPIOB0_IRQHandler

        PUBWEAK GPIOB1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB1_IRQHandler
        B GPIOB1_IRQHandler

        PUBWEAK GPIOB2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB2_IRQHandler
        B GPIOB2_IRQHandler

        PUBWEAK GPIOB3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB3_IRQHandler
        B GPIOB3_IRQHandler

        PUBWEAK GPIOB4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB4_IRQHandler
        B GPIOB4_IRQHandler

        PUBWEAK GPIOB5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB5_IRQHandler
        B GPIOB5_IRQHandler

        PUBWEAK GPIOB6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB6_IRQHandler
        B GPIOB6_IRQHandler

        PUBWEAK GPIOB7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB7_IRQHandler
        B GPIOB7_IRQHandler

        PUBWEAK GPIOC0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC0_IRQHandler
        B GPIOC0_IRQHandler

        PUBWEAK GPIOC1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC1_IRQHandler
        B GPIOC1_IRQHandler

        PUBWEAK GPIOC2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC2_IRQHandler
        B GPIOC2_IRQHandler

        PUBWEAK GPIOC3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC3_IRQHandler
        B GPIOC3_IRQHandler

        PUBWEAK GPIOC4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC4_IRQHandler
        B GPIOC4_IRQHandler

        PUBWEAK GPIOC5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC5_IRQHandler
        B GPIOC5_IRQHandler

        PUBWEAK GPIOC6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC6_IRQHandler
        B GPIOC6_IRQHandler

        PUBWEAK GPIOC7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC7_IRQHandler
        B GPIOC7_IRQHandler

        PUBWEAK GPIOM0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOM0_IRQHandler
        B GPIOM0_IRQHandler

        PUBWEAK GPIOM1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOM1_IRQHandler
        B GPIOM1_IRQHandler

        PUBWEAK GPIOM2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOM2_IRQHandler
        B GPIOM2_IRQHandler

        PUBWEAK GPIOM3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOM3_IRQHandler
        B GPIOM3_IRQHandler

        PUBWEAK GPIOM4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOM4_IRQHandler
        B GPIOM4_IRQHandler

        PUBWEAK GPIOM5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOM5_IRQHandler
        B GPIOM5_IRQHandler

        PUBWEAK GPIOM6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOM6_IRQHandler
        B GPIOM6_IRQHandler

        PUBWEAK GPIOM7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOM7_IRQHandler
        B GPIOM7_IRQHandler

        PUBWEAK DMA_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_IRQHandler
        B DMA_IRQHandler

        PUBWEAK LCD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
LCD_IRQHandler
        B LCD_IRQHandler

        PUBWEAK NORFLC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
NORFLC_IRQHandler
        B NORFLC_IRQHandler

        PUBWEAK CAN_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN_IRQHandler
        B CAN_IRQHandler

        PUBWEAK TIMR_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMR_IRQHandler
        B TIMR_IRQHandler

        PUBWEAK WDT_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
WDT_IRQHandler
        B WDT_IRQHandler

        PUBWEAK PWM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PWM_IRQHandler
        B PWM_IRQHandler

        PUBWEAK UART0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART0_IRQHandler
        B UART0_IRQHandler

        PUBWEAK UART1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART1_IRQHandler
        B UART1_IRQHandler

        PUBWEAK UART2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART2_IRQHandler
        B UART2_IRQHandler

        PUBWEAK UART3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART3_IRQHandler
        B UART3_IRQHandler

        PUBWEAK I2C0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C0_IRQHandler
        B I2C0_IRQHandler

        PUBWEAK I2C1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_IRQHandler
        B I2C1_IRQHandler

        PUBWEAK SPI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI0_IRQHandler
        B SPI0_IRQHandler

        PUBWEAK ADC0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC0_IRQHandler
        B ADC0_IRQHandler

        PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_IRQHandler
        B RTC_IRQHandler

        PUBWEAK ANAC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ANAC_IRQHandler
        B ANAC_IRQHandler

        PUBWEAK SDIO_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SDIO_IRQHandler
        B SDIO_IRQHandler

        PUBWEAK GPIOA_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA_IRQHandler
        B GPIOA_IRQHandler

        PUBWEAK GPIOB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB_IRQHandler
        B GPIOB_IRQHandler

        PUBWEAK GPIOC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC_IRQHandler
        B GPIOC_IRQHandler

        PUBWEAK GPIOM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOM_IRQHandler
        B GPIOM_IRQHandler

        PUBWEAK GPION_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPION_IRQHandler
        B GPION_IRQHandler

        PUBWEAK GPIOP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOP_IRQHandler
        B GPIOP_IRQHandler

        PUBWEAK ADC1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC1_IRQHandler
        B ADC1_IRQHandler

        PUBWEAK FPU_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FPU_IRQHandler
        B FPU_IRQHandler

        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI1_IRQHandler
        B SPI1_IRQHandler


        END
