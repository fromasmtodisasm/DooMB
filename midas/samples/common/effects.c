/*      effects.c
 *
 * A small sound effects example with the DLL API
 *
 * Copyright 1996,1997 Housemarque Inc.
 *
 * This file is part of MIDAS Digital Audio System, and may only be
 * used, modified and distributed under the terms of the MIDAS
 * Digital Audio System license, "license.txt". By continuing to use,
 * modify or distribute this file you indicate that you have
 * read the license and understand and accept it fully.
*/

#if defined(__NT__) || defined(__WINDOWS__) || defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdio.h>
#ifndef __linux__
#include <conio.h>
#endif
#include <stdlib.h>

#if defined( __DOS__) || defined(__DJGPP__)
#include <dos.h>
#endif

#include "midasdll.h"

/* Use delay() for delays in DOS, Sleep() in Win32 and usleep() in Linux: */
#if defined(__DOS__) || defined(__DJGPP__)
    #define DELAY(x) delay(x)
#else
    #ifdef __linux__
        #include <unistd.h>
        #define DELAY(x) usleep(1000*x)
    #else
        #define DELAY(x) Sleep(x)
    #endif
#endif


/****************************************************************************\
*
* Function:     void MIDASerror(void)
*
* Description:  Handles a MIDAS error - displays an error message and exits
*
\****************************************************************************/

void MIDASerror(void)
{
    int         error;

    error = MIDASgetLastError();
    printf("\nMIDAS error: %s\n", MIDASgetErrorMessage(error));
    if ( !MIDASclose() )
    {
        printf("\nBIG PANIC! MIDASclose Failed: %s\n", MIDASgetErrorMessage(
            MIDASgetLastError()));
    }
    exit(EXIT_FAILURE);
}


#define SWEEPSTEPS 16


static MIDASsample sample1, sample2;
static MIDASsamplePlayHandle playHandles[SWEEPSTEPS];
static MIDASsamplePlayHandle laugh;


int main(void)
{
    int         pan;
    unsigned    i;
    unsigned    laughChannel;

    MIDASstartup();

    setbuf(stdout, NULL);

    /* Flag that we don't have a module, effects or streams playing: */
    sample1 = sample2 = 0;
    for ( i = 0; i < 16; i++ )
        playHandles[i] = 0;
    laugh = 0;

    /* Decrease the size of buffer used: */
    MIDASsetOption(MIDAS_OPTION_MIXBUFLEN, 150);
    MIDASsetOption(MIDAS_OPTION_MIXBUFBLOCKS, 4);

    /* Initialize MIDAS and start background playback (at 100 polls
       per second): */
    if ( !MIDASinit() )
        MIDASerror();
    if ( !MIDASstartBackgroundPlay(100) )
        MIDASerror();

    /* Open five channels for our effects:  */
    if ( !MIDASopenChannels(5) )
        MIDASerror();

    /* Allocate four channels for automatic sound effects: */
    if ( !MIDASallocAutoEffectChannels(4) )
        MIDASerror();

    /* Allocate one channel for the laugh: */
    if ( (laughChannel = MIDASallocateChannel()) == MIDAS_ILLEGAL_CHANNEL )
        MIDASerror();

    /* Load our samples: */
#ifdef __linux__
    if ( (sample1 = MIDASloadRawSample("../data/explosi1.pcm",
        MIDAS_SAMPLE_8BIT_MONO, MIDAS_LOOP_NO)) == 0 )
        MIDASerror();
    if ( (sample2 = MIDASloadRawSample("../data/laugh1.pcm",
        MIDAS_SAMPLE_8BIT_MONO, MIDAS_LOOP_YES)) == 0 )
        MIDASerror();
#else
    if ( (sample1 = MIDASloadRawSample("..\\data\\explosi1.pcm",
        MIDAS_SAMPLE_8BIT_MONO, MIDAS_LOOP_NO)) == 0 )
        MIDASerror();
    if ( (sample2 = MIDASloadRawSample("..\\data\\laugh1.pcm",
        MIDAS_SAMPLE_8BIT_MONO, MIDAS_LOOP_YES)) == 0 )
        MIDASerror();
#endif    

    /* Start the laugh at volume 0: */
    if ( (laugh = MIDASplaySample(sample2, laughChannel, 0, 16000, 0,
        MIDAS_PAN_SURROUND)) == 0 )
        MIDASerror();

    /* Fade laugh in: */
    for ( i = 0; i <= 64; i++ )
    {
        if ( !MIDASsetSampleVolume(laugh, i) )
            MIDASerror();
        DELAY(20);
    }

    /* Play machine gun fire, sweep panning from left to right: */
    pan = MIDAS_PAN_LEFT;
    for ( i = 0; i < SWEEPSTEPS; i++ )
    {
        if ( (playHandles[1] = MIDASplaySample(sample1, MIDAS_CHANNEL_AUTO,
            0, 22050, 32, pan)) == 0 )
            MIDASerror();

        pan += (MIDAS_PAN_RIGHT - MIDAS_PAN_LEFT) / SWEEPSTEPS;

        DELAY(200);
    }

    /* Wait for a while for all gun sounds to stop: */
    DELAY(500);

    /* Stop the gun sounds: */
    for ( i = 0; i < SWEEPSTEPS; i++ )
    {
        if ( !MIDASstopSample(playHandles[i]) )
            MIDASerror();
    }

    /* Fade laugh out: */
    for ( i = 64; i > 0; i-- )
    {
        if ( !MIDASsetSampleVolume(laugh, i) )
            MIDASerror();
        DELAY(30);
    }

    /* Stop laugh: */
    if ( !MIDASstopSample(laugh) )
        MIDASerror();

    /* Deallocate channels: */
    if ( !MIDASfreeChannel(laughChannel) )
        MIDASerror();
    if ( !MIDASfreeAutoEffectChannels() )
        MIDASerror();

    /* Free the samples: */
    if ( !MIDASfreeSample(sample1) )
        MIDASerror();
    if ( !MIDASfreeSample(sample2) )
        MIDASerror();

    /* Stop MIDAS: */
    if ( !MIDASstopBackgroundPlay() )
        MIDASerror();
    if ( !MIDASclose() )
        MIDASerror();

    return 0;
}
