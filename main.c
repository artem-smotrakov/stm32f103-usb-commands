#include "main.h"

void delay(int millis) {
    while (millis-- > 0) {
        volatile int x = 5971;
        while (x-- > 0) {
            __asm("nop");
        }
    }
}

int get_string_length(const char* s) {
    int len=0;
    while (s[len] != 0) {
        len++;
    }

    return len;
}

int is_equal(const char* one, const char* two) {
    int i;

    if (get_string_length(one) != get_string_length(two)) {
        return -1;
    }

    for (i=0; i<get_string_length(one); i++) {
        if (one[i] != two[i]) {
            return -1;
        }
    }

    return 0;
}

void init_output(void) {
    // GPIO structure for port initialization
    GPIO_InitTypeDef GPIO_Settings;

    // enable clock on APB2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // configure port A1 for driving an LED
    GPIO_Settings.GPIO_Pin = GPIO_Pin_1;
    GPIO_Settings.GPIO_Mode = GPIO_Mode_Out_PP;    // output push-pull mode
    GPIO_Settings.GPIO_Speed = GPIO_Speed_50MHz;   // highest speed
    GPIO_Init(GPIOA, &GPIO_Settings) ;             // initialize port
}

// TODO: check a flag, and call init_output() in not initialized
void turn_on_pa1(void) {
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void turn_off_pa1(void) {
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void usart_init() {
    GPIO_InitTypeDef GPIO_Settings;
    USART_InitTypeDef USART_Settrings;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_StructInit(&GPIO_Settings);
    GPIO_Settings.GPIO_Pin = GPIO_Pin_9;
    GPIO_Settings.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Settings.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA , &GPIO_Settings);

    GPIO_Settings.GPIO_Pin = GPIO_Pin_10;
    GPIO_Settings.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Settings.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA , &GPIO_Settings);

    USART_StructInit(&USART_Settrings);
    USART_Settrings.USART_BaudRate = 9600;
    USART_Settrings.USART_WordLength = USART_WordLength_8b;
    USART_Settrings.USART_StopBits = USART_StopBits_1;
    USART_Settrings.USART_Parity = USART_Parity_No;
    USART_Settrings.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Settrings.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_Settrings);

    USART_Cmd(USART1, ENABLE);

    __enable_irq();
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_EnableIRQ(USART1_IRQn);
}

// send one byte through USART
void usart_send(char chr) {
    while(!(USART1->SR & USART_SR_TC));
    USART1->DR = chr;
}

// send a string through USART
void usart_send_string(char *s) {
    int i=0;
    while (s[i]) {
        usart_send(s[i++]);
    }
}

void handle_command(char *command) {
    if (is_equal(command, A1_ON) == 0) {
        turn_on_pa1();
    } else if (is_equal(command, A1_OFF) == 0) {
        turn_off_pa1();
    } else {
        usart_send_string(UNKNOWN_COMMAND);
        usart_send_string(CRLF);
    }
}

char usart_buf[COMMAND_MAX_LENGTH];
unsigned short usart_buf_length=0;

void USART1_IRQHandler() {
    unsigned char received;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        received = USART_ReceiveData(USART1);

        if (received == CR) {
            usart_buf[usart_buf_length] = 0;
            usart_send_string(CRLF);

            handle_command(usart_buf);
            usart_buf_length = 0;
        } else if (received == LF) {
            // ignore
        } else {
            if (usart_buf_length == COMMAND_MAX_LENGTH) {
                usart_send_string(CRLF);
                usart_send_string(COMMAND_TOO_LONG);
                usart_send_string(CRLF);
                usart_buf_length = 0;
                return;

            }

            usart_buf[usart_buf_length++] = received;

            // echo
            usart_send(received);
        }
    }
}

int main(void) {

    // initialize output ports
    init_output();

    // initialize USART
    usart_init();

    // main loop
    while (1) {
        // do nothing
    }
}
