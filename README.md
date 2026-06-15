# 无线汽车 OBD 仪表从机端 (STM32F4 + GC9A01 圆屏 + LVGL)

本项目是基于 **STM32F401CCU6** 的无线汽车 OBD 仪表**显示从机**。它通过 **ESP-01 WiFi 模块** 接收来自 [无线 OBD 仪表主机](https://github.com/htc-01/STM32-OBD2-WiFi-Bridge)（或本地 `E:\stm32\car` 主机项目）的车辆实时数据，并在 **240×240 GC9A01 圆形液晶屏** 上以 LVGL 表盘形式展示。

> 🚗 **系统定位**：
> - `car` / [STM32-OBD2-WiFi-Bridge](https://github.com/htc-01/STM32-OBD2-WiFi-Bridge) → **主机**：负责 CAN/OBD2 数据采集与无线发送。
> - `car111plus` → **从机**：负责无线接收数据并驱动圆形仪表盘显示。

---

## ✨ 功能特性

- **240×240 圆形仪表显示**：基于 GC9A01 圆屏，单表盘大字体显示，清晰直观。
- **6 项核心 OBD2 参数轮显**：

  | 参数 | 单位 | 正常颜色 | 报警阈值 |
  |------|------|----------|----------|
  | 发动机负载 (LOAD) | % | 绿色 | > 80 |
  | 冷却液温度 (TMP) | °C | 青色 | > 110 |
  | 发动机转速 (RPM) | rpm | 黄色 | > 6000 |
  | 车速 (SPD) | km/h | 绿色 | > 120 |
  | 空气流量 (MAF) | g/s | 洋红 | > 400 |
  | 节气门开度 (THR) | % | 橙色 | > 80 |

- **无线接收数据**：通过 USART1 连接 ESP-01，接收 `[LOAD=... TMP=... RPM=... SPD=... MAF=... THR=...]` 格式数据帧。
- **动态表盘动画**：使用 LVGL `lv_arc` 弧形控件，数值变化时平滑刷新，减少闪烁。
- **越限报警闪烁**：当数值超过量程上限时，屏幕红/黑闪烁提示。
- **页面记忆功能**：当前显示的仪表页面通过 **AT24C02 EEPROM** 保存，掉电不丢失。
- **电源管理**：支持短按按键切换页面，长按按键熄屏/唤醒，降低功耗。
- **开机自检画面**：启动时依次检测 MCU、时钟、I2C、AT24C02、SPI、GC9A01、USART1、LVGL、Flash、Gauge。

---

## 🛠️ 硬件架构

```
┌─────────────────┐      WiFi      ┌─────────┐      UART 460800       ┌──────────────┐
│   汽车OBD2口    │                  │ ESP-01  │◄───USART1───────────►│  STM32F401   │
│   (ECU)         │◄───CAN bus────►│ WiFi模组 │   无线数据帧          │   CCU6       │
└─────────────────┘                └─────────┘                       │              │
                                                                     │  主频 84MHz  │
                                                                     │  HSE 25MHz   │
                                                                     └──────┬───────┘
                                                                            │ SPI1 + DMA
                                                                            ▼
                                                                     ┌──────────────┐
                                                                     │  GC9A01      │
                                                                     │ 240×240 圆屏  │
                                                                     └──────────────┘
                                                                            │ I2C1
                                                                            ▼
                                                                     ┌──────────────┐
                                                                     │  AT24C02     │
                                                                     │   EEPROM     │
                                                                     └──────────────┘
```

### 引脚分配

| 功能 | 引脚 | 说明 |
|------|------|------|
| USART1_TX | PA9 | 连接 ESP-01 RX（接收无线数据） |
| USART1_RX | PA10 | 连接 ESP-01 TX |
| ESP_EN | PA11 | ESP-01 使能控制 |
| SPI1_SCK | PA5 | GC9A01 时钟 |
| SPI1_MISO | PA6 | GC9A01 MISO |
| SPI1_MOSI | PA7 | GC9A01 MOSI |
| DC | PB0 | GC9A01 数据/命令选择 |
| CS | PB1 | GC9A01 片选 |
| screen_RST | PB2 | GC9A01 复位 |
| key | PA1 | 用户按键（短按切页，长按开关屏） |
| AT24C02_VCC | PB4 | AT24C02 电源控制 |
| AT24C02_GND | PB5 | AT24C02 地控制 |
| AT24C02_SCL | PB6 | I2C1 时钟 |
| AT24C02_SDA | PB7 | I2C1 数据 |
| SWDIO | PA13 | 调试 |
| SWCLK | PA14 | 调试 |

> ⚠️ **硬件提示**：ESP-01 峰值电流可达 300mA 以上，建议独立供电并加足够去耦电容。STM32F401 的 GPIO 不建议直接承担大电流负载。

---

## 📁 项目结构

```
.
├── Core/
│   ├── Inc/
│   │   ├── main.h                  # 主头文件（引脚定义）
│   │   ├── gc9a01.h                # GC9A01 圆屏驱动头文件
│   │   ├── gauge.h                 # 表盘绘制接口
│   │   ├── at24c02.h               # AT24C02 EEPROM 接口
│   │   ├── flash_storage.h         # 内部 Flash 存储接口
│   │   ├── lv_port_disp.h          # LVGL 显示移植接口
│   │   ├── lv_port_tick.h          # LVGL 时基接口
│   │   └── stm32f4xx_hal_conf.h    # HAL 库配置
│   └── Src/
│       ├── main.c                  # 主程序：初始化 + 数据解析 + 按键 + LVGL 主循环
│       ├── gc9a01.c                # GC9A01 SPI 驱动实现
│       ├── gauge.c                 # 自绘表盘（备用/非 LVGL）
│       ├── gauge_flash_data.c      # 表盘 Flash 数据
│       ├── at24c02.c               # AT24C02 EEPROM 读写
│       ├── flash_storage.c         # 内部 Flash 页面记忆
│       ├── lv_port_disp.c          # LVGL 显示刷新（SPI + DMA）
│       ├── lv_port_tick.c          # LVGL 1ms 时基
│       ├── font.c                  # 字体数据
│       ├── stm32f4xx_hal_msp.c     # HAL MSP 初始化
│       ├── stm32f4xx_it.c          # 中断服务程序
│       └── system_stm32f4xx.c      # 系统时钟配置
├── GUI/                            # LVGL / NXP GUI Guider 生成文件
│   ├── gui_guider.c/h              # GUI 框架初始化
│   ├── custom.c/h                  # 自定义表盘逻辑与电源管理
│   ├── events_init.c/h             # 事件初始化
│   ├── widgets_init.c/h            # 控件初始化
│   ├── setup_scr_speed.c           # 速度/主表盘页面
│   ├── setup_scr_record.c          # 记录/图表页面
│   ├── images/                     # 图片资源
│   └── guider_fonts/               # 自定义字体
├── Middlewares/LVGL/               # LVGL v9.x 图形库源码
├── Drivers/
│   ├── CMSIS/                      # ARM CMSIS 核心库
│   └── STM32F4xx_HAL_Driver/       # STM32F4 HAL 驱动库
├── cmake/stm32cubemx/              # CMake 子项目（CubeMX 生成）
├── build/                          # 构建输出目录
├── startup_stm32f401xc.s           # 启动汇编文件
├── STM32F401XX_FLASH.ld            # 链接脚本
├── CMakeLists.txt                  # CMake 主配置（含 LVGL 裁剪）
├── CMakePresets.json               # CMake 预设
├── car111plus.ioc                  # STM32CubeMX 工程文件
└── README.md                       # 本文件
```

---

## 🔌 通信协议

### 无线 / 串口输入帧格式

从机端通过 USART1 接收来自主机（或 ESP-01 透传）的数据帧：

```
[LOAD=<负载> TMP=<温度> RPM=<转速> SPD=<车速> MAF=<空流> THR=<节气门>]
```

**示例**：
```
[LOAD=23 TMP=89 RPM=2150 SPD=65 MAF=12 THR=34]
```

- 帧头：`[`
- 帧尾：`]`
- 字段以空格分隔
- 数据通过 **ESP-01 WiFi 透传** → USART1 → STM32F401 接收

接收代码位于 `main.c` 的 `HAL_UART_RxCpltCallback()` 与 `ParseRxData()` 中，采用中断逐字节接收，检测到 `]` 后标记数据就绪。

---

## 🚀 快速开始

### 环境要求

- **IDE**：CLion / VS Code + Cortex-Debug / 任何支持 CMake 的 IDE
- **工具链**：`arm-none-eabi-gcc` 或 ARM Clang
- **构建工具**：CMake >= 3.22
- **烧录器**：ST-Link V2 / J-Link / DAP-Link

### 构建与烧录

```bash
# 克隆仓库
git clone <repo-url>
cd car111plus

# 配置并构建（以 GCC 为例）
cmake --preset=default
cmake --build build/Debug

# 烧录（使用 OpenOCD 示例）
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program build/Debug/car111plus.elf verify reset exit"
```

或直接在 CLion 中选择 `STM32_GCC` / `STM32_ARMClang` 预设进行构建。

### 首次上电流程

1. 将 ESP-01 的 TX/RX 交叉连接到 STM32F401 的 USART1（PA10/PA9）。
2. 上电后屏幕显示开机自检信息，随后进入主表盘。
3. 当主机（`car` 或 [STM32-OBD2-WiFi-Bridge](https://github.com/htc-01/STM32-OBD2-WiFi-Bridge)）开始发送数据帧后，屏幕实时更新。
4. 短按按键切换仪表页面，长按按键熄屏/唤醒。

---

## ⚙️ 核心代码逻辑

### 1. 数据接收与解析 (`main.c`)

- USART1 以 **460800 波特率** 中断接收。
- 帧格式为 `[KEY=VALUE ...]`，解析后调用 `gauge_set_value()` 更新 6 个内部变量。
- UI 刷新限制在 **约 30 fps**，避免 LVGL 渲染过载。

### 2. 表盘显示 (`GUI/custom.c`)

- 初始化时隐藏 GUI Guider 默认控件，自定义创建 `lv_arc` 弧形表盘、标题、数值、单位标签。
- 根据当前页面配置动态调整弧形的范围、颜色、数值。
- 数值超过报警阈值时颜色变红；超过量程上限时触发红/黑全屏闪烁。

### 3. 页面记忆 (`at24c02.c` / `flash_storage.c`)

- 当前页面索引同时写入 **AT24C02 EEPROM** 和 **STM32 内部 Flash** 双保险。
- 开机时优先从 AT24C02 恢复，失败则回退到内部 Flash。
- 页面切换或熄屏时延迟保存，避免 I2C 阻塞 UI。

### 4. 电源管理 (`custom.c`)

- 短按按键（< 1s）：切换下一页仪表。
- 长按按键（≥ 1s）：熄屏并保存页面；再次长按唤醒。

---

## 📝 配置与调参

关键参数集中在以下文件：

| 参数 | 位置 | 默认值 | 说明 |
|------|------|--------|------|
| `RX_BUF_SIZE` | `main.c` | 128 | 串口接收缓冲区大小 |
| `REFRESH_INTERVAL_MS` | `main.c` | 100 | 最小 UI 刷新间隔 |
| 波特率 | `MX_USART1_UART_Init()` | 460800 | 需与主机/ESP-01 一致 |
| 报警阈值 | `GUI/custom.c` / `Core/Src/gauge.c` | 见上表 | 可自定义 |
| 表盘量程 | `GUI/custom.c` / `Core/Src/gauge.c` | 见上表 | 可自定义 |

---

## 🤝 关联项目

- **主机端**：`E:\stm32\car` 或 GitHub [htc-01/STM32-OBD2-WiFi-Bridge](https://github.com/htc-01/STM32-OBD2-WiFi-Bridge)
  - 负责连接汽车 OBD2 CAN 总线，采集发动机数据并通过 WiFi/串口发送。
- **显示端**：`car111plus`（本项目）
  - 负责接收数据帧并驱动 GC9A01 圆形液晶屏显示。

---

## 📜 许可证

本项目基于 STM32CubeMX 生成的 HAL 库代码，MCU 相关驱动版权归 STMicroelectronics 所有。用户添加的应用层代码（GC9A01 驱动、表盘逻辑、通信协议等）以 MIT 许可证开源。

LVGL 图形库遵循其自身的 MIT 许可证，详见 `Middlewares/LVGL/LICENCE.txt`。

---

## 🙋 常见问题

**Q: 屏幕黑屏无显示？**
> A: 检查 GC9A01 的接线（SPI 线序、DC/CS/RST）、3.3V 供电是否足够，以及 `GC9A01_Init()` 是否成功执行。

**Q: 有数据但表盘不更新？**
> A: 确认主机与从机波特率一致（默认 460800），并检查 ESP-01 是否已正确配置为透传模式。可在 `main.c` 的 `ParseRxData()` 中加串口打印调试。

**Q: 页面记忆失效？**
> A: 检查 AT24C02 是否正确上电（PB4 = VCC、PB5 = GND），以及 I2C 上拉电阻是否接入。

**Q: 屏幕刷新卡顿？**
> A: LVGL 已限制在约 30 fps，若仍卡顿可尝试降低 SPI 分频或进一步裁剪 LVGL 功能模块（见 `CMakeLists.txt` 中的 `list(FILTER ... EXCLUDE ...)` 配置）。

---

> 🚗 **项目状态**：个人 DIY 项目，持续迭代中。欢迎提交 Issue 或 PR 改进显示效果、增加新 PID 支持或优化无线稳定性！
