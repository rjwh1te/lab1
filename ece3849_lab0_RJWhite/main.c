/**
 * main.c
 *
 * ECE 3849 Lab 0 Starter Project
 * Gene Bogdanov    10/18/2017
 *
 * This version is using the new hardware for B2017: the EK-TM4C1294XL LaunchPad with BOOSTXL-EDUMKII BoosterPack.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "Crystalfontz128x128_ST7735.h"
#include <stdio.h>

#include "buttons.h"

uint32_t gSystemClock; // [Hz] system clock frequency
volatile uint32_t gTime = 8345; // time in hundredths of a second

volatile uint32_t buttonDis = 0; // value of gbutton

int main(void)
{
    IntMasterDisable();

    // Enable the Floating Point Unit, and permit ISRs to use it
    FPUEnable();
    FPULazyStackingEnable();

    // Initialize the system clock to 120 MHz
    gSystemClock = SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, 120000000);

    Crystalfontz128x128_Init(); // Initialize the LCD display driver
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP); // set screen orientation

    ButtonInit();
    IntMasterEnable();

    tContext sContext;
    GrContextInit(&sContext, &g_sCrystalfontz128x128); // Initialize the grlib graphics context
    GrContextFontSet(&sContext, &g_sFontFixed6x8); // select font

    uint32_t time;  // local copy of gTime

    uint32_t frames; // time parsed into fractions of a second
    uint32_t seconds; // time parsed into seconds
    uint32_t minutes; // time parsed into minutes

    char str[50];   // string buffer
    // full-screen rectangle
    tRectangle rectFullScreen = {0, 0, GrContextDpyWidthGet(&sContext)-1, GrContextDpyHeightGet(&sContext)-1};

    while (true) {
        GrContextForegroundSet(&sContext, ClrBlack);
        GrRectFill(&sContext, &rectFullScreen); // fill screen with black
        time = gTime; // read shared global only once

        frames = time % 100; // extract frames (1/100th seconds)
        seconds = (time / 100) % 60; // extract seconds
        minutes = time / 6000; // extract minutes



        snprintf(str, sizeof(str), "Time = %02u:%02u:%02u\n", minutes, seconds, frames); // convert time to string
        GrContextForegroundSet(&sContext, ClrYellow); // yellow text
        GrStringDraw(&sContext, str, /*length*/ -1, /*x*/ 0, /*y*/ 0, /*opaque*/ false);

        snprintf(str, sizeof(str), "Buttons: %09d\n", buttonDis & 0x1FF); // prints 9 most lsb of gbutton
        GrStringDraw(&sContext, str, /*length*/ -1, /*x*/ 0, /*y*/ 20, /*opaque*/ false); // prints it lower

        GrFlush(&sContext); // flush the frame buffer to the LCD
    }
}
