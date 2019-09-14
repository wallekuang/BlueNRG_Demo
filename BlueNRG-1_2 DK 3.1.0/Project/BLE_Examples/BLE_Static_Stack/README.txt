----------------------------------------------------------------------
Instructions to build an application that uses the BLE Static Stack project
----------------------------------------------------------------------

----------------------------
How to compile application
----------------------------

- Remove bluenrg1_stack_lib.a library from the set of files to be compiled
- Include bluenrg1_stack_init_if.c and libbluenrg1_static_stack.a in your project
  (from Library\Bluetooth_LE\library\static_stack).
- Define NO_SMART_POWER_MANAGEMENT symbol for C compiler.
- Define MEMORY_FLASH_APP_OFFSET for linker, depending on the Flash size occupied by the BlueNRG
  static stack image. E.g. if static stack image uses the Flash till address 0x1005a438, offset
  must be equal to 0x1A800, which is the offset of the first available Flash location aligned to
  the start of a page.
- Define MEMORY_RAM_APP_OFFSET for linker with the offset in RAM equal to the first available
  address in RAM. E.g. 0x404, aligned to a 32-bit word. See MEMORY_RAM_APP_OFFSET  used inside 
  BLE_SensorDemo_Fixed_Stack project to know which RAM offset must be used by your application
  when using the Static Stack included insided the BlueNRG-1 DK.

----------------------------
How to build a bluenrg static stack library
----------------------------
The BlueNRG static stack is provided inside the BlueNRG1 DK in binary format (i.e. at a fixed
address in Flash) and usually there is no need to build it again.
If you want to customize and build it, the following steps need to be followed.

------
Prerequisites
------
Install GNU ARM Embedded Toolchain and add the binary folder in the system PATH. The utility
create_sym_lib.exe in BlueNRG-1 SDK Utility folder needs the following utilities:
- arm-none-eabi-readelf 
- arm-none-eabi-gcc
- arm-none-eabi-ar
Open the Windows Command Prompt and try to invoke those commands to check if they are correctly
installed.

------
Build steps
------
- Build "BLE Static Stack" project. If necessary, change MEMORY_FLASH_APP_SIZE (for linker) to increase
  (or possibly reduce) the flash reserved for the BlueNRG Stack.
- If MEMORY_FLASH_APP_SIZE has been changed, change also APP_OFFSET (for C preprocessor), or
  RESET_MANAGER_SIZE (if using the Reset Manager to support OTA firmware update). This macro points
  to the base of the flash where the application resides and it should be equal to MEMORY_FLASH_APP_SIZE +
  MEMORY_FLASH_APP_OFFSET.
- Use create_sym_lib.exe utility to generate the required library with symbols to be referenced by the
  application. See post-build step used in BLE Static Stack project.
- Take a note of the first available address in RAM (excluding CSTACK) from map file. (e.g. 0x20000404).
  It has to be used when defining MEMORY_RAM_APP_OFFSET inside application project.
   
