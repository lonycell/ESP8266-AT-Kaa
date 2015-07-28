#
# Copyright 2014 CyberVision, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 2.8.8)

project (C-SDK-sample)
enable_language(C)

find_package (OpenSSL REQUIRED)

set(KAA_SDK_ROOT /usr/local/kaa-esp8266)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Os -Wall -Wextra")

set(APP_NAME ESP8266-AT-Kaa)

include(${CMAKE_TOOLCHAIN_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${KAA_SDK_ROOT}/include
                    ${ESP8266_INCDIRS}
                   )

link_directories(${KAA_SDK_ROOT}/lib
                 ${ESP8266_LIBDIR}
                )

set (SRC
    src/kaa_demo.c
    )

add_library(esp-platform STATIC ${ESP8266_SRC})
add_executable(${APP_NAME} blank.c)
add_dependencies(${APP_NAME} esp-platform ${APP_NAME}_s kaac_s)
add_library(${APP_NAME}_s STATIC ${SRC})

target_link_libraries(${APP_NAME} ${APP_NAME}_s esp-platform)

add_custom_command(TARGET ${APP_NAME}
                   POST_BUILD
                   COMMAND esptool.py elf2image -o ./ ${APP_NAME}
                   & echo "Making firmware image"
                  )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ESP8266_LDFLAGS} -L${KAA_SDK_ROOT}/lib -L./ -o ${APP_NAME}")
