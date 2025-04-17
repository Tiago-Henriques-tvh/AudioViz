#include "lib.h"
#include <iostream>
#include <cmath>

// Initialize any necessary variables, such as for MP3/WAV file reading
static SNDFILE *sndFile = nullptr;
static mpg123_handle *mpg123 = nullptr;
static int channels = 0;
static int rate = 0;

int init_audio(const char *file_path) {
    // Open the audio file using the appropriate library (libsndfile for WAV, mpg123 for MP3)
    if (strstr(file_path, ".mp3")) {
        // Initialize mpg123 for MP3 files
        if (mpg123_init() != MPG123_OK) {
            std::cerr << "Error initializing mpg123" << std::endl;
            return -1;
        }
        
        mpg123 = mpg123_new(NULL, NULL);
        if (mpg123_open(mpg123, file_path) != MPG123_OK) {
            std::cerr << "Error opening MP3 file" << std::endl;
            return -1;
        }

        mpg123_getformat(mpg123, &rate, &channels, NULL);
    } else if (strstr(file_path, ".wav")) {
        // Open WAV file using libsndfile
        SF_INFO sfinfo;
        sndFile = sf_open(file_path, SFM_READ, &sfinfo);
        if (!sndFile) {
            std::cerr << "Error opening WAV file" << std::endl;
            return -1;
        }
        rate = sfinfo.samplerate;
        channels = sfinfo.channels;
    }

    return 0; // Return success
}

void get_audio_intensities(float intensities[MAX_BARS]) {
    // This function will read audio data and calculate intensities (RMS) for visualization
    if (mpg123) {
        // Process MP3 data with mpg123
        size_t bytes;
        unsigned char buffer[4096];
        int32_t samples[2048];
        
        mpg123_read(mpg123, buffer, sizeof(buffer), &bytes);
        // Convert to PCM data and calculate intensity (RMS)
        // [Simplified: Real implementation would decode and process the audio here]
        float rms = 0;
        for (int i = 0; i < 2048; i++) {
            rms += samples[i] * samples[i];
        }
        rms = sqrt(rms / 2048);
        
        // Map the RMS value to the intensities array
        for (int i = 0; i < MAX_BARS; i++) {
            intensities[i] = rms / 1000.0f; // Normalize to fit the visualizer range
        }
    } else if (sndFile) {
        // Process WAV data with libsndfile
        short buffer[2048]; // Audio buffer
        int readCount = sf_read_short(sndFile, buffer, 2048);
        if (readCount <= 0) {
            return; // End of file or error
        }

        // Calculate RMS intensity
        float rms = 0;
        for (int i = 0; i < readCount; i++) {
            rms += buffer[i] * buffer[i];
        }
        rms = sqrt(rms / readCount);

        // Map the RMS value to the intensities array
        for (int i = 0; i < MAX_BARS; i++) {
            intensities[i] = rms / 1000.0f; // Normalize to fit the visualizer range
        }
    }
}

void cleanup_audio() {
    // Clean up and close the audio files
    if (mpg123) {
        mpg123_close(mpg123);
        mpg123_delete(mpg123);
        mpg123_exit();
    }

    if (sndFile) {
        sf_close(sndFile);
    }
}

int play_audio(const char *file_path) {
    // Implement audio playback logic (e.g., using a playback library or custom logic)
    return 0;
}

void stop_audio() {
    // Implement stop functionality, e.g., stopping the audio playback thread
}
