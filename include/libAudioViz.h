#ifndef AUDIO_VIZ_H
#define AUDIO_VIZ_H

// Include necessary libraries for audio processing (libsndfile, mpg123, etc.)
#include <sndfile.h>
#include <mpg123.h>

// Define the maximum number of intensity bars
#define MAX_BARS 50

// C-style interface to make it compatible with JNI
#ifdef __cplusplus
extern "C" {
#endif

// Function to initialize and process the audio file (WAV/MP3)
int init_audio(const char *file_path);

// Function to get the current audio intensities (RMS values) for visualization
void get_audio_intensities(float intensities[MAX_BARS]);

// Function to clean up resources after processing
void cleanup_audio();

// Optionally, add functions to pause, resume, stop audio, or handle file types
int play_audio(const char *file_path); // Plays audio (can be called from Kotlin)
void stop_audio(); // Stop the audio

#ifdef __cplusplus
}
#endif

#endif // AUDIO_VIZ_H
