SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\ProgramData\Oracle\Java\javapath;C:\Program Files (x86)\Kingston SSD Toolbox;C:\Program Files (x86)\Common Files\Intel\Shared Files\cpp\bin\Intel64;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\7-Zip;C:\Users\Admin\.dnx\bin;C:\Program Files\Microsoft DNX\Dnvm\;C:\Program Files\Microsoft SQL Server\120\Tools\Binn\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files (x86)\Skype\Phone\;C:\Program Files (x86)\Calibre2\;D:\CooCox\CoIDE_V2Beta\gcc\bin;D:\CooCox\CoIDE_V2Beta\bin;D:\Program Files (x86)\GNU Tools ARM Embedded\5.4 2016q3\bin
SET CPU_TYPE=STM32F411RETx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=26.02.2017
SET CPU_CLOCK=0x00F42400
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\system_stm32f4xx.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\objects\startup_stm32f411xe._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\26.02.2017 Gpio_try.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Objects\26.02.2017 Gpio_try.axf" --i32combined --output ".\Objects\26.02.2017 Gpio_try.hex"
