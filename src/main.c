// Morse code puzzle game
// Written by Aryan Afrouzi and Aung Khant Min, November 2021

//RED RS PC1
//BROWN RW PC0
//BLACK E PC3
//ORANGE D4 PC10
//GREEN D5 PC12
//YELLOW D6 PC11
//BLUE D7 PC2

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"
#include "LiquidCrystal.h"

char MorseTranslate(char button_input[], unsigned int size);
void pwm_test(uint16_t period, uint16_t prescale);
void flash_question(char question[], unsigned int size);


void pwm_test(uint16_t period, uint16_t prescale)
{

    __TIM2_CLK_ENABLE();                                           // enable timer 2
    TIM_HandleTypeDef pwmTimerInstance;                            // this variable stores an instance of the timer
    InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale); // initialize the timer instance
    InitializePWMChannel(&pwmTimerInstance, TIM_CHANNEL_1);        // initialize one channel (can use others for motors, etc)

    for (uint32_t i = 0; i < period; i++)
    {
        SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, i);
        HAL_Delay(10);
    }
}

char MorseTranslate(char button_input[], unsigned int size)
{
    // morse code translation table:
    // 1 = dot
    // 2 = dash
    // 0 = none (default)
    char button_input_array[26][4] = {
        {1, 2},
        {2, 1, 1, 1},
        {2, 1, 2, 1},
        {2, 1, 1},
        {1},
        {1, 1, 2, 1},
        {2, 2, 1},
        {1, 1, 1, 1},
        {1, 1},
        {1, 2, 2, 2},
        {2, 1, 2},
        {1, 2, 1, 1},
        {2, 2},
        {2, 1},
        {2, 2, 2},
        {1, 2, 2, 1},
        {2, 2, 1, 2},
        {1, 2, 1},
        {1, 1, 1},
        {2},
        {1, 1, 2},
        {1, 1, 1, 2},
        {1, 2, 2},
        {2, 1, 1, 2},
        {2, 1, 2, 2},
        {2, 2, 1, 1}};

    for (unsigned int k = 0; k < 26; ++k)
    {
        bool match = true;
        // find the character that matches the given pattern
        for (unsigned int i = 0; i < size; ++i)
        {
            if (button_input_array[k][i] != button_input[i])
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            return (char)('a' + k);
        }
    }
    return 'a';
}

void flash_question(char question[], unsigned int size) { 

    char button_input_array[26][4] = {
        {1, 2},
        {2, 1, 1, 1},
        {2, 1, 2, 1}, 
        {2, 1, 1}, //d
        {1},
        {1, 1, 2, 1},
        {2, 2, 1}, //g
        {1, 1, 1, 1},
        {1, 1},
        {1, 2, 2, 2},
        {2, 1, 2},
        {1, 2, 1, 1},
        {2, 2},
        {2, 1},
        {2, 2, 2},
        {1, 2, 2, 1},
        {2, 2, 1, 2},
        {1, 2, 1},
        {1, 1, 1},
        {2},
        {1, 1, 2},
        {1, 1, 1, 2},
        {1, 2, 2},
        {2, 1, 1, 2},
        {2, 1, 2, 2},
        {2, 2, 1, 1}};

    unsigned int LONG_SIGNAL = 1500; // long signal (dash)
    unsigned int SHORT_SIGNAL = 300; // short signal (dot) 
    unsigned int CHARACTER_DIFF_SIGNAL = 600; // signal for in between characters
    unsigned int BUFFER_SIGNAL = 200; // signal for in between flashes

    for (unsigned int k = 0; k < size; ++k) { 
        for (unsigned int i = 0; i < 4; ++i) { 
            // for long signal
            if (button_input_array[(int)(question[k] - 'a')][i] == 2) { 
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, true);
                HAL_Delay(LONG_SIGNAL);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, false); 

            // for short signal
            } else if (button_input_array[(int)(question[k] - 'a')][i] == 1) { 
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, true);
                HAL_Delay(SHORT_SIGNAL);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, false);        
            }
            HAL_Delay(BUFFER_SIGNAL); 
        }
        HAL_Delay(SHORT_SIGNAL);
    }
}

int main(void)
{
    HAL_Init(); // initialize the Hardware Abstraction Layer

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LED, for example)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the rotary encoder inputs, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)

    InitializePin(GPIOC, GPIO_PIN_8, GPIO_MODE_INPUT, GPIO_PULLUP, 0);
    InitializePin(GPIOC, GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_PULLUP, 0);
    InitializePin(GPIOC, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLUP, 0);
    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM2); // Servo pin

    InitializePin(GPIOC, GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); // Blue LED
    InitializePin(GPIOC, GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); // Green LED
    InitializePin(GPIOC, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); // Red LED

    LiquidCrystal(GPIOC, GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_3, GPIO_PIN_10, GPIO_PIN_12, GPIO_PIN_11, GPIO_PIN_2);

    SerialSetup(9600);


    // MORSE CODE VARIABLES: 
    unsigned int LONG_PRESS = 200; // how long a long press is in milliseconds
    unsigned int NUM_CHAR = 4;     // max number of dots and dashes in a character

    unsigned int user_input_index = 0; // index for user_input

    char user_input_char[NUM_CHAR];         // user input for one character/letter in dots and dashes (1 - dot; 2 - dash)
    unsigned int user_input_char_index = 0; // index for user_input_char


    // GAME CONTROLS:   
    unsigned int NUM_CHARACTERS_IN_ANSWER = 3; // the number of characters in the expectedd answer

    char correct_answer[NUM_CHARACTERS_IN_ANSWER]; // correct answer in plain text
    char user_input[NUM_CHARACTERS_IN_ANSWER];     // user input answer in plain text

    bool question_display_mode = 0; // 0 - display question in plain text on lcd screen; 1 - flash question in morse code

    unsigned int NUM_CHARACTERS_IN_QUESTION = 2;
    char question_to_display[NUM_CHARACTERS_IN_QUESTION + 1]; // question to display

    question_to_display[0] = 'f';
    question_to_display[1] = 'l';
    question_to_display[2] = 'u';
    question_to_display[3] = 'f';
    question_to_display[4] = 'f';
    question_to_display[5] = 'y';
    // question_to_display[2] = '\0';

    for (unsigned int k = 0; k < NUM_CHAR; ++k)
    {
        user_input_char[k] = 0;
    }

    correct_answer[0] = 'd';
    correct_answer[1] = 'o';
    correct_answer[2] = 'g';

    bool game_is_won = false;
    bool game_is_reset = true;

    int time1; //time when button first pressed
    int time2; //time when button let go
    int final; //difference between time 2 and time 1

    char button_pressed = 0; // the button pressed: 0 for morse code button, 1 for next character button, 2 for reset button

    while (true)
    {

        // if the game is rest / just starting, output the question
        if (game_is_reset) { 
            if (question_display_mode) {
                flash_question(question_to_display, NUM_CHARACTERS_IN_QUESTION);
            } else { 
                char buff[100];
                sprintf(buff, question_to_display);
                SerialPuts(buff); 

                setCursor(0, 0);
                print(question_to_display);
                HAL_Delay(2000);
                clear();           
            }
            game_is_reset = false;
        }
        
        while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5))
        {
        }

        // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, true);
        // HAL_Delay(100);
        // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, false);

        // wait for a button press (active low)
        while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) || !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) || !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5))
        {
            // morse code button
            if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8))
            {

                button_pressed = 0;

                while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8))
                {
                };
                time1 = HAL_GetTick();

                // button released
                while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8))
                {
                };

                time2 = HAL_GetTick();
                final = time2 - time1;

                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, true);
                HAL_Delay(100);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, false);

                // setCursor(0,0);
                // print("test");


                char buff[100];
                if (final > LONG_PRESS)
                {
                    sprintf(buff, "Long Press\n");
                    SerialPuts(buff);

                    setCursor(0, 1);
                    print("Long Press");
                    HAL_Delay(500);
                    clear();

                    user_input_char[user_input_char_index] = 2;
                    user_input_char_index++;
                }
                else
                {
                    sprintf(buff, "Short Press\n");
                    SerialPuts(buff);

                    setCursor(0, 1);
                    print("Short Press");
                    HAL_Delay(500);
                    clear();

                    user_input_char[user_input_char_index] = 1;
                    user_input_char_index++;
                }
            }

            else if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6))
            {

                button_pressed = 1;

                clear();
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, true);
                HAL_Delay(100);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, false);
            }

            // reset button
            else if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5))
            {

                button_pressed = 2;

                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, true);
                HAL_Delay(100);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, false);
            }
        }

        // reset button
        if (button_pressed == 2)
        {
            // rest everything
            for (unsigned int k = 0; k < NUM_CHAR; ++k)
            {
                user_input_char[k] = 0;
            }
            for (unsigned int k = 0; k < NUM_CHARACTERS_IN_ANSWER; ++k)
            {
                user_input[k] = 0;
            }
            user_input_index = 0;
            user_input_char_index = 0;
            game_is_reset = true;
        }

        if (button_pressed == 1)
        {
            user_input[user_input_index] = MorseTranslate(user_input_char, NUM_CHAR);
            char buff[2];
            sprintf(buff, "%c\n", user_input[user_input_index]);
            SerialPuts(buff);

            setCursor(0,0);
            char str[2] = "\0";
            str[0] = user_input[user_input_index];
            print(str);
            HAL_Delay(750);
            clear();

            // reset user input array for current character
            for (unsigned int k = 0; k < NUM_CHAR; ++k)
            {
                user_input_char[k] = 0;
            }
            user_input_char_index = 0;

            // go to next character
            ++user_input_index;

        }

        if (user_input_index == NUM_CHARACTERS_IN_ANSWER)
        {

            // see if the input answer matches the correct answer
            bool match = true;

            for (unsigned int k = 0; k < NUM_CHARACTERS_IN_ANSWER; ++k)
            {
                if (user_input[k] != correct_answer[k])
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                char buff[20];
                sprintf(buff, "Congrats!\n");
                printf(user_input_index);
                SerialPuts(buff);

                setCursor(0,4);
                print("CONGRATS!");
                HAL_Delay(2000);
                clear();

                // OPEN DOOR
                pwm_test(24, 8);

                game_is_won = true;
                game_is_reset = true;

                while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5)) { }    

                while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5)) { }   

                pwm_test(75, 8);

                while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5)) { }    

                while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5)) { } 

                pwm_test(24, 8);

            }
            else
            {
                char buff[30];
                sprintf(buff, "Wrong. Input reset!\n");
                printf(user_input_index);
                SerialPuts(buff);

                setCursor(0,0);
                print("Wrong. Reset!");
                HAL_Delay(1000);
                clear();

                // reset the user input
                for (unsigned int k = 0; k < NUM_CHAR; ++k)
                {
                    user_input_char[k] = 0;
                }
                for (unsigned int k = 0; k < NUM_CHARACTERS_IN_ANSWER; ++k)
                {
                    user_input[k] = 0;
                }
                user_input_index = 0;
                user_input_char_index = 0;
                
                game_is_reset = true;
            }
        }
    }
}

// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}
