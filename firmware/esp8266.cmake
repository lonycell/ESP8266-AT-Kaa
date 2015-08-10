include(CMakeForceCompiler)

#change this to path where you've installed xtensa toolchain
set(ESP_TOOLCHAIN_DIR /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin)

set (ESP_SDK_BASE /opt/Espressif/esp-rtos-sdk)

set(CMAKE_C_COMPILER ${ESP_TOOLCHAIN_DIR}/xtensa-lx106-elf-gcc)

set(CMAKE_C_LINK_EXECUTABLE "${ESP_TOOLCHAIN_DIR}/xtensa-lx106-elf-ld <LINK_FLAGS> <OBJECTS>")

set(CMAKE_AR ${ESP_TOOLCHAIN_DIR}/xtensa-lx106-elf-ar)

set(CMAKE_RANLIB ${ESP_TOOLCHAIN_DIR}/xtensa-lx106-elf-ranlib)

set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> cru <TARGET> <LINK_FLAGS> <OBJECTS>")

set(CMAKE_LIBRARY_PATH ${ESP_SDK_BASE}/lib/)

set(ESP8266_LIBDIR ${ESP_SDK_BASE}/lib ${ESP_SDK_BASE}/ld)

set(ESP8266_SRC user/user_main.c
                driver/uart.c
                src/cmd.c
                src/cmd_parse.c
                src/kaa_client.c
                src/profile_upd.c
                src/log_upload.c
                )

set(ESP8266_LIBS "-lfreertos -lhal -lgcc -lpp -lphy -lnet80211 -lwpa -llwip -lmain -lssl")

set(ESP8266_INCDIRS 
    ${ESP_SDK_BASE}/extra_include 
    ${ESP_SDK_BASE}/include 
    ${ESP_SDK_BASE}/include/lwip 
    ${ESP_SDK_BASE}/include/lwip/ipv4 
    ${ESP_SDK_BASE}/include/lwip/ipv6 
    ${ESP_SDK_BASE}/include/espressif/
    )

set(ESP8266_LINKSCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/ld/eagle.app.v6.ld)

set(ESP8266_LDFLAGS "-L${CMAKE_CURRENT_SOURCE_DIR} -L${ESP_TOOLCHAIN_DIR}/../lib/gcc/xtensa-lx106-elf/4.8.2/ -L${ESP_SDK_BASE}/lib -T${ESP8266_LINKSCRIPT} -nostdlib -u call_user_start -static --start-group ${ESP8266_LIBS} -lesp-platform -lkaac_s --end-group --gc-sections -Map map")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DESP8266_PLATFORM -Os -Wno-pointer-sign -Wno-comment -Wno-implicit-function-declaration -Wpointer-arith -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -D__ets__ -DICACHE_FLASH")


