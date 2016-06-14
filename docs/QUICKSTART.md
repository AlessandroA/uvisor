# Quick-Start Guide for uVisor on mbed OS

This guide will help you get started with uVisor on mbed OS.

The uVisor creates sandboxed environments and resources protection for application built for ARM Cortex-M3 and Cortex-M4 devices. Here we will show you how to enable the uVisor and configure a secure box to get hold of some exclusive resources (memory, peripherals, interrupts). For more information on the uVisor design philosophy, please check out our the uVisor [introductory document](docs/README.md).

To get a basic `blinky` application running on mbed OS with uVisor enabled, you will need the following:

* A platform and a toolchain supported by uVisor on mbed OS. You can verify this on [the official list](docs/README.md#supported-platforms). Please note that some platforms supported by uVisor might have still not been deployed to mbed OS. If you want to port your platform to uVisor and enable it on mbed OS, please follow the [uVisor porting guide](docs/PORTING.md).
* git. It will be used to download the mbed code-base.
* The mbed command-line tools, mbed-cli. You can run `pip install mbed-cli` to install them.

For the remainder of this guide we will assume that you are developing in `~/code`. Most of the instructions/code that you will see are platform-independent, but some of them will be specific to the NXP Freedom K64F board.

## Start with the `blinky` app

To create a new mbed application called `uvisor-example`just run the following commands:

```bash
cd ~/code
mbed new uvisor-example
```

The mbed-cli tools will automatically fetch the mbed code-base for you. By default, git will be used to track your code changes, so your application will be ready to be pushed to a git server, if you want to.

Once the import process is finished, create a `source` folder:
```bash
mkdir ~/code/uvisor-example/source
```
and place a new file `main.cpp` in it:

```C
/* ~/code/uvisor-example/source */

#include "mbed.h"
#include "rtos.h"

DigitalOut led(LED1);

int main(void)
{
    while (true) {
        led = !led;
        Thread::wait(500);
    }
}
```

This simple application just blinks an LED from the main thread, which is created by default by the OS.

In the next sections you will see:

* How to [enable uVisor](#enable-uvisor) on the `blinky` app.
* How to [create a new secure box](#create-a-new-secure-box) with exclusive access to a timer interrupt, the UART peripheral, and a private structure in SRAM.

## Enable uVisor

If you enable uVisor in the `blinky` app as it was written above, you will not get any particular security feature. All code and resources share the same security context, which we call the *main box*.

A lot happens under the hood, though. All the user code now runs in unprivileged mode, and the systems services like the `NVIC` APIs or the OS SVCalls are routed through the uVisor.

To enable the uVisor on the app, just add the following line in the `main.cpp` file:

```C
/* ~/code/uvisor-example/source */

#include "mbed.h"
#include "rtos.h"

UVISOR_CONFIG_SET_MODE(UVISOR_ENABLED);

...
```

Assuming that you 
