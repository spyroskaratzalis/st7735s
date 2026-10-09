#include "DEV_Config.h"
#include "GUI_BMP.h"
#include "GUI_Paint.h"
#include "KEY_APP.h"
#include "LCD_1in44.h"

#include "test.h"

#include <signal.h> //signal()
#include <stdio.h>  //printf()
#include <stdlib.h> //exit()sudo apt install p7zip-full
#include <time.h>

#define KEY_UP_BIT       (1 << 0)
#define KEY_DOWN_BIT     (1 << 1)
#define KEY_LEFT_BIT     (1 << 2)
#define KEY_RIGHT_BIT    (1 << 3)
#define KEY_PRESS_BIT    (1 << 4)
#define KEY1_BIT         (1 << 5)
#define KEY2_BIT         (1 << 6)
#define KEY3_BIT         (1 << 7)

uint8_t read_all_keys() {
    uint8_t mask = 0;
    if(!DEV_Digital_Read(KEY_UP_PIN)) mask |= KEY_UP_BIT;
    if (!DEV_Digital_Read(KEY_DOWN_PIN))    mask |= KEY_DOWN_BIT;
    if (!DEV_Digital_Read(KEY_LEFT_PIN))    mask |= KEY_LEFT_BIT;
    if (!DEV_Digital_Read(KEY_RIGHT_PIN))   mask |= KEY_RIGHT_BIT;
    if (!DEV_Digital_Read(KEY_PRESS_PIN))   mask |= KEY_PRESS_BIT;
    if (!DEV_Digital_Read(KEY1_PIN))        mask |= KEY1_BIT;
    if (!DEV_Digital_Read(KEY2_PIN))        mask |= KEY2_BIT;
    if (!DEV_Digital_Read(KEY3_PIN))        mask |= KEY3_BIT;
    return mask;
}
// extern LCD_DIS sLCD_DIS;
/************************************
When using the button
Please execute
    sudo nano /boot/config.txt
Add at the end
    gpio=6,19,5,26,13,21,20,16=pu
*************************************/
void KEY_1in44_test(void) {
    // Exception handling:ctrl + c
    // signal(SIGINT, Handler_1in44_LCD);

    /* Module Init */
    // if(DEV_ModuleInit() != 0){
    //     DEV_ModuleExit();
    //     exit(0);
    // }
    // printf("Use this routine to add gpio=6,19,5,26,13,21,20,16=pu at the end of /boot/config.txt\r\n");
    // LCD_SCAN_DIR LCD_ScanDir = SCAN_DIR_DFT;//SCAN_DIR_DFT = D2U_L2R

    // /* LCD Init */
    // printf("1.44inch LCD KEY demo...\r\n");
    // LCD_1in44_Init(LCD_ScanDir);
    // LCD_1in44_Clear(WHITE);
    UWORD *BlackImage;
    UWORD Imagesize = LCD_HEIGHT * LCD_WIDTH;
    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }

    /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    printf("LCD_WIDTH = %d   LCD_HEIGHT  = %d\r\n", LCD_WIDTH, LCD_HEIGHT);

    Paint_NewImage(BlackImage, LCD_WIDTH, LCD_HEIGHT, 0, WHITE, 16);
    Paint_Clear(WHITE);

    /* Monitor button */
    printf("Listening KEY\r\n");
    KEY_Listen(BlackImage);

    /* Module Exit */
    free(BlackImage);
    BlackImage = NULL;
    // DEV_ModuleExit();
}
