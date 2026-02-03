# BareMetal-Drivers
这里是给所有单片机项目的系列基础库

## ToDo清单

### 其他驱动

预计编写项目涉及到的更多的相关驱动库





## 已完成清单

| **模块/目录**        | **完成文件**                                                 | **状态** | **详细说明**                                                 |
| -------------------- | ------------------------------------------------------------ | -------- | ------------------------------------------------------------ |
| **Application**      | `app.c/h`, `boot.c/h`, `system_clock.c/h`                    | 已完成   | 包含应用入口逻辑、系统启动引导序列以及主系统时钟的配置与初始化。 |
| **Config**           | `ah_no.h`, `cfbd_define.h`, `lib_settings.h`                 | 已完成   | 项目全局宏定义、库运行参数设置以及硬件环境相关的基础类型定义。 |
| **GPIO**             | `gpio.h`, `gpio-stm-impl.c/h`                                | 已完成   | GPIO 硬件抽象层接口（HAL）及其在 STM32 平台上的具体寄存器级实现。 |
| **Graphic (Base)**   | `arc.c/h`, `circle.c/h`, `ellipse.c/h`, `line.c/h`, `point.h`, `rectangle.c/h`, `size.c/h`, `triangle.c/h` | 已完成   | 核心绘图原始语算法，支持点、线、圆、弧、矩形、三角形等几何图形的计算与渲染。 |
| **Graphic (Device)** | `graphic_device.c/h`, `oled_graphic_device.c/h`              | 已完成   | 图形显示设备抽象层，定义了通用显示接口并实现了 OLED 设备的具体适配。 |
| **Graphic (Other)**  | `benchmark.c/h`, `test_base_graphic.c/h`, `test_widget.c/h`  | 已完成   | 提供图形渲染性能压力测试工具以及基础绘图与组件功能的快速验证脚本。 |
| **Resource**         | `config.h`, `ascii6x8.source.c`, `ascii8x16.source.c`        | 已完成   | 内置资源管理配置及常用点阵字体（6x8, 8x16 ASCII）的数据源。  |
| **Widget (Core)**    | `text.c/h`, `text_config.c/h`, `margin.h`, `image.c/h`, `helpers.h` | 已完成   | UI 框架基础组件，涵盖支持布局边距的文本渲染系统和位图图像显示功能。 |
| **Widget (UI)**      | `animation.c/h`, `menu.c/h`, `menu_indicator.c/h`, `menu_item.c/h` | 已完成   | 高级 UI 组件库，包含可配置的动画引擎、多级菜单管理、菜单项逻辑及交互指示器。 |
| **IIC**              | `iic.c/h`, `iic_error.h`, `i2c_stm_impl.c/h`                 | 已完成   | I2C 软件协议栈、详尽的错误码定义以及基于 STM32 硬件外设的底层后端。 |
| **OLED (Core)**      | `oled.c/h`, `cache_config.h`, `external_impl_driver.h`, `oled_iic.c/h` | 已完成   | OLED 驱动核心框架，包含显存缓存策略、驱动接口协议适配及 I2C 数据传输封装。 |
| **OLED Driver**      | `ssd1306.c/h`, `ssd1309.c/h`, `device_interface.h`           | 已完成   | 针对主流 SSD1306 和 SSD1309 控制芯片的具体命令集实现与接口定义。 |

