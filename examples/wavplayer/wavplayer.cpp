//#include "dsp.h"

#include "portaudio.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <memory>
#include <stdint.h>
#include <array>

struct WavHeader {
    // ref: http://soundfile.sapp.org/doc/WaveFormat/
    uint32_t ChunkId;
    uint32_t ChunkSize;
    uint32_t Format;
    uint32_t Subchunk1D;
    uint32_t SubChunk1Size;
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    uint32_t SubChunk2D;
    uint32_t SubChunk2Size;
};

const int NUM_SECONDS = 5;
const int FRAMES_PER_BUFFER =128;
const int SAMPLE_RATE = 48000;

constexpr uint32_t bufferSize = SAMPLE_RATE * 10;

struct paTestData {
    float buffer[bufferSize];
    uint32_t n = 0;
    uint32_t length = bufferSize;
};

#if 0
template <typename T, typename R>
R audioCallback(dsp::InputBuffer in, dsp::OutputBuffer out, dsp::Blocksize size) {
}
#endif

static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    float *out = (float*)outputBuffer; 
    (void) inputBuffer;/* Prevent unused variable warning. */
    unsigned long i = 0;
    paTestData * data = (paTestData *) userData;
    
    for(i=0; i<framesPerBuffer;i++ )
    {
        //*out++ = float(data->buffer[data->n])/32768.f; 
        //*out++ = float(data->buffer[data->n])/32768.f; 
        *out++ = data->buffer[data->n]; 
        data->n = (data->n + 1) % data->length;
        *out++ = data->buffer[data->n];
        data->n = (data->n + 1) % data->length;
    }
    return paContinue;
}

static void StreamFinished( void* userData )
{
    printf( "Stream Completed.\n");
}

int main() {

    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    paTestData data; 
    WavHeader wf;



    auto e = fopen("sines2.wav", "r"); 
   if (!e) {
        printf("Error!\n"); 
    }
 
    
    auto ret = fread(&wf, sizeof(wf), 1, e);    
    if (ret == 1)
        printf("File successfully read.\n");
    else {
        if (feof(e))
            printf("EOF.\n");
        else if (ferror(e))
            printf("Error reading %s", "sines2.wav");
    }

    printf("Wave info\n");

    printf("NumChannels: %d\n", wf.NumChannels);
    printf("SampleRate: %d\n", wf.SampleRate);
    printf("BitDepth: %d\n", wf.BitsPerSample);
    printf("ChunkSize %d\n", wf.ChunkSize);
    printf("Subchunk1Size %d\n", wf.SubChunk1Size);
    printf("Subchunk2Size %d\n", wf.SubChunk2Size);
    fseek(e, 0, SEEK_END);
    auto size = ftell(e);
    rewind(e);
    fseek(e,44, SEEK_SET);

    printf("Length: %f\n seconds", static_cast<float>(size)/wf.BitsPerSample/wf.SampleRate/wf.NumChannels*8);

    ret = fread(&data.buffer[0], sizeof(float), wf.ChunkSize, e);
    data.length = size;
    fclose(e);

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto error;
    }
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              patestCallback,
              &data );
    if( err != paNoError ) goto error;

    err = Pa_SetStreamFinishedCallback( stream, &StreamFinished );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    printf("Play for %d seconds.\n", NUM_SECONDS );
    Pa_Sleep( NUM_SECONDS * 1000 );

    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    Pa_Terminate();
    printf("Test finished.\n");

    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;

}
