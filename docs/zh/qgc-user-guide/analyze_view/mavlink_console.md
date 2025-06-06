# MAVLink 控制台(分析视图)

The MAVLink Console (**Analyze > Mavlink Console**) allows you to connect to the PX4 [System Console](https://docs.px4.io/main/en/debug/system_console.html) and send commands.

:::info
它仅在桌面版本上支持 (Windows, Linux, Mac OS)。
PX4 SITL and ArduPilot are not supported.
:::

:::tip
This is a very useful feature for developers as it allows deep access to the system. In particular, if you are connected via Wifi, you can have this same level of access while the vehicle is flying.
:::

![Analyze View MAVLink Console](../../../assets/analyze/mavlink_console.jpg)

The view does not display any output except in response to commands.
Once the vehicle is connected, you can enter commands in the bar provided (for a full list of available commands enter: `?`).

Command output is displayed in the view above the command bar.
Click **Show Latest** to jump to the bottom of the command output.
