import machine
import utime

# 定義LED控制的GPIO pin
RED_PIN_1 = 0
YELLOW_PIN_1 = 1
GREEN_PIN_1 = 2

RED_PIN_2 = 3
YELLOW_PIN_2 = 4
GREEN_PIN_2 = 5

# 函數：控制單一燈號的開關
def control_light(pin, state):
    pin = machine.Pin(pin, machine.Pin.OUT)
    pin.value(state)

# 函數：紅燈閃爍
def flash_red_light(pin, duration, interval):
    end_time = utime.ticks_add(utime.ticks_ms(), duration * 1000)
    while utime.ticks_diff(end_time, utime.ticks_ms()) > 0:
        control_light(pin, 1)
        utime.sleep(0.5)
        control_light(pin, 0)
        utime.sleep(0.5)
        utime.sleep(interval)

# 交通燈模式 1
def traffic_light_mode_1():
    while True:
        control_light(RED_PIN_1, 1)  # 紅燈 10s
        utime.sleep(10)
        
        flash_red_light(RED_PIN_1, 0.5, 0.5)  # 紅燈閃爍 0.5s，每次閃爍間隔0.5s
        
        control_light(GREEN_PIN_1, 1)  # 綠燈 3s
        utime.sleep(3)
        
        control_light(YELLOW_PIN_1, 1)  # 黃燈 0.5s
        utime.sleep(0.5)
        control_light(YELLOW_PIN_1, 0)

# 交通燈模式 2
def traffic_light_mode_2():
    while True:
        control_light(RED_PIN_2, 1)  # 紅燈 5s
        utime.sleep(5)
        
        flash_red_light(RED_PIN_2, 0.5, 0.5)  # 紅燈閃爍 0.5s，每次閃爍間隔0.5s
        
        control_light(GREEN_PIN_2, 1)  # 綠燈 4s
        utime.sleep(4)
        
        control_light(YELLOW_PIN_2, 1)  # 黃燈 0.5s
        utime.sleep(0.5)
        control_light(YELLOW_PIN_2, 0)

# 主程式
traffic_light_mode_1()  # 啟動交通燈模式 1
# 或者使用以下方式啟動交通燈模式 2
# traffic_light_mode_2()
