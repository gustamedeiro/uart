#include <stdlib.h> 
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/irq.h"

// Definições de pinos
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define PIN_LED_G 11   // LED RGB Verde
#define PIN_LED_B 12   // LED RGB Azul
#define PIN_LED_R 13   // LED RGB Vermelho
#define PIN_BUTTON_A 5
#define PIN_BUTTON_B 6

#define NUM_PIXELS 25  // A matriz tem 25 LEDs (5x5)
bool led_buffer[NUM_PIXELS];  // Armazena os estados dos LEDs da matriz WS2812

// Definição de estado dos LEDs e botões
bool ledGreenState = false;
bool ledBlueState = false;
volatile bool buttonAState = false;
volatile bool buttonBState = false;

// Função para acender a matriz de LEDs com base no número
void atualizar_matriz(int numero) {
    // Define o padrão de LED para cada número de 0 a 9 em uma matriz 5x5
    const bool numeros[10][NUM_PIXELS] = {
        {1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1}, // 0
        {0,1,1,1,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,0,0, 0,0,1,0,0}, // 1
        {1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1}, // 2
        {1,1,1,1,1, 0,0,0,0,1, 1,1,1,0,0, 0,0,0,0,1, 1,1,1,1,1}, // 3
        {1,0,0,0,0, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1}, // 4
        {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1}, // 5
        {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1}, // 6
        {0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 0,0,0,0,1, 1,1,1,1,1}, // 7
        {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}, // 8
        {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}  // 9
    };

    // Preenche o buffer com os LEDs correspondentes ao número
    for (int i = 0; i < NUM_PIXELS; i++) {
        led_buffer[i] = numeros[numero][i];
    }

    set_one_led();  // Atualiza a matriz de LEDs
}

// Função para enviar o sinal dos LEDs para a matriz WS2812
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Função que converte valores RGB em um único valor de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Função para acender a matriz WS2812 com os LEDs definidos
void set_one_led() {
    uint32_t color = urgb_u32(200, 200, 200);  // Cor padrão
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (led_buffer[i]) {
            put_pixel(color);  // Acende o LED
        } else {
            put_pixel(0);  // Apaga o LED
        }
    }
}

// Funções de interrupção
void buttonA_irq() {
    buttonAState = true;
    ledGreenState = !ledGreenState;
    gpio_put(PIN_LED_G, ledGreenState);  // Alterna o LED verde

    // Enviar informações para o Serial Monitor
    uart_puts(uart0, "LED Verde: ");
    uart_puts(uart0, ledGreenState ? "ON" : "OFF");
    uart_puts(uart0, "\n");
}

void buttonB_irq() {
    buttonBState = true;
    ledBlueState = !ledBlueState;
    gpio_put(PIN_LED_B, ledBlueState);  // Alterna o LED azul

    // Enviar informações para o Serial Monitor
    uart_puts(uart0, "LED Azul: ");
    uart_puts(uart0, ledBlueState ? "ON" : "OFF");
    uart_puts(uart0, "\n");
}

// Função para atualizar o display com o estado dos LEDs
void updateDisplay() {
    ssd1306_clear(&ssd);
    ssd1306_draw_string(&ssd, "LED Verde: ", 0, 0);
    ssd1306_draw_string(&ssd, ledGreenState ? "ON" : "OFF", 0, 16);
    ssd1306_draw_string(&ssd, "LED Azul: ", 0, 32);
    ssd1306_draw_string(&ssd, ledBlueState ? "ON" : "OFF", 0, 48);
    ssd1306_send_data(&ssd);
}

int main() {
    // Inicialização I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_t ssd;
    ssd1306_init(&ssd, 128, 64, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração dos pinos de botões e LEDs
    gpio_init(PIN_BUTTON_A);
    gpio_set_dir(PIN_BUTTON_A, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_A);
    gpio_init(PIN_BUTTON_B);
    gpio_set_dir(PIN_BUTTON_B, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_B);
    
    gpio_init(PIN_LED_R);
    gpio_set_dir(PIN_LED_R, GPIO_OUT);
    gpio_init(PIN_LED_G);
    gpio_set_dir(PIN_LED_G, GPIO_OUT);
    gpio_init(PIN_LED_B);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);

    // Configuração de interrupção para os botões
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &buttonA_irq);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &buttonB_irq);

    // Configuração da UART
    uart_init(uart0, 9600);
    gpio_set_function(0, GPIO_FUNC_UART);  // TX
    gpio_set_function(1, GPIO_FUNC_UART);  // RX

    while (true) {
        // Verifica se há dados para serem lidos no Serial Monitor
        if (uart_is_readable(uart0)) {
            char input = uart_getc(uart0);
            
            // Se for um número entre 0 e 9, acende o correspondente na matriz WS2812
            if (input >= '0' && input <= '9') {
                int numero = input - '0';  // Converte o caractere para o número
                ssd1306_clear(&ssd);
                ssd1306_draw_string(&ssd, "Numero Recebido:", 0, 0);
                ssd1306_draw_string(&ssd, &input, 0, 16);
                ssd1306_send_data(&ssd);

                // Atualiza a matriz WS2812 com o número correspondente
                atualizar_matriz(numero);
            }
        }

        // Verificar o estado do botão A e alternar LED verde
        if (buttonAState) {
            updateDisplay();
            sleep_ms(200);  // Debouncing
            buttonAState = false;  // Resetar estado após operação
        }

        // Verificar o estado do botão B e alternar LED azul
        if (buttonBState) {
            updateDisplay();
            sleep_ms(200);  // Debouncing
            buttonBState = false;  // Resetar estado após operação
        }

        sleep_ms(100);  // Pausa para evitar loop excessivo
    }
}
