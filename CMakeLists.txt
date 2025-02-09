# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Definir a placa (pico_w) para usar o Raspberry Pi Pico W
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Incluir o SDK do Raspberry Pi Pico
include(pico_sdk_import.cmake)

# Nome do projeto
project(display C CXX ASM)

# Inicialização do SDK do Raspberry Pi Pico
pico_sdk_init()

# Definir o executável, incluindo os arquivos do código principal e as bibliotecas
add_executable(display 
    src/display.c
    inc/ssd1306.c
    inc/font.c
)

# Definir o nome e a versão do programa
pico_set_program_name(display "display")
pico_set_program_version(display "0.1")

# Habilitar o uso de UART e USB
pico_enable_stdio_uart(display 1)
pico_enable_stdio_usb(display 1)

# Adicionar a biblioteca padrão e a biblioteca de I2C ao build
target_link_libraries(display 
    pico_stdlib
    hardware_i2c
)

# Incluir diretórios de cabeçalhos
target_include_directories(display PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    inc  # Adicione a pasta 'inc' para incluir os arquivos de cabeçalho
)

# Adicionar saídas extras, se necessário
pico_add_extra_outputs(display)

# Habilitar o monitor serial
enable_stdio_uart(display 1)
