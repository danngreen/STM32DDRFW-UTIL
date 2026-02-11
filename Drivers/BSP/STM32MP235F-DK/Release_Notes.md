---
pagetitle: Release Notes for STM32MP235F-DK BSP Drivers
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---
::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# Release Notes for <mark>STM32MP235F-DK BSP Drivers</mark>
Copyright &copy; 2024 STMicroelectronics\
    
[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>

# Purpose

The BSP (Board Specific Package) drivers are parts of the STM32Cube package based on the HAL drivers and provide a set of high level APIs relative to the hardware components and features in the Disco boards coming with the STM32Cube package for a given STM32 series.

The BSP drivers allow a quick access to the boards’ services using high level APIs and without any specific configuration as the link with the HAL and the external components is done in intrinsic within the drivers.

From project settings points of view, user has only to add the necessary driver’s files in the workspace and call the needed functions from examples. However some low level configuration functions are weak and can be overridden by the applications if user wants to change some BSP drivers default behavior.

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section1" checked aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">__V1.3.0 / 02-Feb-2026__</label>
<div>

## Main Changes

**v1.3.0 release** of STMP32MP235F-DK BSP

## Contents

- Enable/Disable ADC kernel clock using SCMI.


## Notes

stm32mp235f_disco_conf_template.h file must be copied in user application as >stm32mp235f_disco_conf.h with optional configuration update

</div>
:::


::: {.collapse}
<input type="checkbox" id="collapse-section0" aria-hidden="true">
<label for="collapse-section0" aria-hidden="true">__V1.2.0 / 30-May-2025__</label>
<div>

## Main Changes

**v1.2.0 release** of STMP32MP235F-DK BSP

## Contents

- Mass market of STMP32MP235F-DK.


## Notes

stm32mp235f_disco_conf_template.h file must be copied in user application as >stm32mp235f_disco_conf.h with optional configuration update

</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on <mark>STM32 Microcontrollers</mark> ,
visit: [[www.st.com](http://www.st.com/STM32)]{style="font-color: blue;"}
</footer>
