MicroPython port to SWM320 (Cortex-M4 MCU)
==========================================

## Pin
### 输出
``` python
from machine import Pin
​
led = Pin('PA5', Pin.OUT)
led.low()
led.high()
led.toggle()
led.value(1-led.value())
```

### 输入
``` python
key = Pin('PA4', Pin.IN)
key = Pin('PA4', Pin.IN, mode=Pin.PULL_UP)    # 可开启内部上拉、下拉
key.value()
```

### 中断
``` python
led = Pin('PA5', Pin.OUT)
​
key = Pin('PA4', Pin.IN, irq=Pin.FALL_EDGE, callback=lambda key: led.toggle())
```


## Timer
### 定时器
``` python
from machine import Pin, Timer
​
led = Pin('PA5', Pin.OUT)
​
tmr = Timer(0, 10000, callback=lambda tmr: led.toggle())
tmr.start()
```
第二个参数‘10000’是定时周期时长，单位为0.1ms

### 计数器
``` python
from machine import Pin, Timer
​
led = Pin('PA5', Pin.OUT)
​
ctr = Timer(2, 3, mode=Timer.MODE_COUNTER, pin='PA4', callback=lambda ctr: led.toggle())
ctr.start()
```
Timer4、Timer5没有Counter功能


## UART
``` python
from machine import UART
​
ser = UART(1, 57600, txd='PA9', rxd='PA10')    # 等效于下面一行
ser = UART(1, 57600, bits=UART.DATA_8BIT, parity=UART.PARITY_None, stop=UART.STOP_1BIT, txd='PA9', rxd='PA10')
​
ser.write('Hi from Synwit\n')
if ser.any(): s = ser.read()
```
UART0是micropython命令界面，用户程序中不要使用


## SPI
``` python
from machine import Pin, SPI
​
spi = SPI(0, 100000, mosi='PC4', miso='PC5', sclk='PC7')    # 等效于下面一行
spi = SPI(0, 100000, polarity=0, phase=0, bits=8, mosi='PC4', miso='PC5', sclk='PC7')
​
spi_cs = Pin('PC2', Pin.OUT)
spi_cs.high()
​
spi_cs.low()
spi.write(b'\x33\x78\xAF')
spi_cs.high()
```


## I2C
``` python
from machine import I2C
​
i2c = I2C(0, 100000, scl='PA10', sda='PA9')
​
i2c.writeto(0x6C, '\x33\x78\xAF')
```
端口A、C上的引脚有内部上拉，外部可不接上拉电阻；端口B上的引脚必须外接上拉电阻



## ADC
``` python
from machine import ADC
​
adc = ADC(0, chns=ADC.ADC_CH4)
​
for i in range(5): print(adc.read())
​
adc.chn_select(ADC.ADC_CH4|ADC.ADC_CH5|ADC.ADC_CH6|ADC.ADC_CH7)
​
for i in range(5): print(adc.read())
```


## PWM
### 独立输出
``` python
import time
from machine import PWM
​
pwm = PWM(0, 15000, 5000, pin='PA10')
pwm.start()
​
time.sleep_ms(50)
pwm.period(30000)
pwm.duty(15000)
```
PWM的计数时钟为15MHz，计数周期15000可得方波频率为1KHz

### 互补输出
``` python
import time
from machine import PWM
​
pwm = PWM(1, 1500, 500, mode=PWM.MODE_COMPL, pinA='PA9', pinB='PA11', deadzone=50)
pwm.start()
​
time.sleep_ms(50)
pwm.period(3000)
pwm.duty(1500)
```
死区最大取值63



## notice
1、PA12引脚没有外设功能映射功能，只能用作Pin
