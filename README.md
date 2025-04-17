# AudioViz - Audio Visualizer for Android

**AudioViz** is an Android app that displays a real-time audio visualizer (similar to an equalizer) based on the intensity of audio from WAV and MP3 files. The app is powered by C++ for audio processing and visualization logic, integrated with Android using Java Native Interface (JNI).

This project allows you to visualize audio intensities (calculated using RMS) as dynamic bars on your Android device.

## Features

- **WAV and MP3 File Support**: AudioViz supports both WAV and MP3 formats for audio playback and visualization.
- **Real-Time Visualization**: Visualize the intensity of the audio in real-time using bars, with the intensity dynamically updating as the audio plays.
- **Native C++ Processing**: Audio processing and RMS intensity calculations are performed in C++ for better performance.
- **Cross-Language Integration**: Uses JNI to call C++ functions from Kotlin/Java, leveraging the power of C++ in an Android environment.

## Screenshots

_(Optional: You can add screenshots of the app's visualization UI here)_

---

## How It Works

The app consists of two parts:

1. **C++ Code (Native Library)**: The core audio processing, such as decoding the audio file (MP3 or WAV), calculating the audio's RMS (Root Mean Square) intensity, and generating visualizer data (bars), is written in C++.

2. **Kotlin/Android UI**: The UI is developed using Android’s standard views and layout tools. The C++ library is accessed via JNI, where Kotlin calls native C++ functions to get audio intensities and update the visualizer in real-time.

### Core Components:

- **Audio Processing**: Using `libsndfile` for WAV files and `mpg123` for MP3 files, the C++ code extracts the RMS intensity of audio data in real-time.
- **Visualization**: The intensity data is used to update a custom `View` in Android, displaying bars that represent the audio's volume levels.
- **JNI Interface**: JNI (Java Native Interface) allows Kotlin to interact with the native C++ code, making the app both efficient and responsive.

---

## Installation

### Prerequisites

Before building the project, you will need the following:

- **Android Studio** (to build and run the Android app)
- **C++ Libraries**:
  - `libsndfile` (for reading WAV files)
  - `libmpg123` (for reading MP3 files)

### Dependencies

#### On Ubuntu:

```bash
sudo apt-get install libsndfile1-dev libmpg123-dev
```

#### On macOS:

```bash
brew install libsndfile mpg123
```

### Building and Running the App

1. **Clone the Repository**: Download the project to your local machine.

2. **Open in Android Studio**: Open the project in **Android Studio**.

3. **Build the Project**:

   - Android Studio should automatically download dependencies. You can use the `Build` option to compile both the Java and C++ components of the project.

4. **Run the App**:
   - Connect your Android device or use an emulator.
   - Select the device and click **Run** to install and launch the app.

---

## Setting Up JNI and C++ Code

The app's core audio processing is implemented in C++. The C++ code interacts with Kotlin through JNI (Java Native Interface). Here's a breakdown of the setup:

### `lib.h` (C Header File)

```cpp
#ifndef AUDIO_VIZ_H
#define AUDIO_VIZ_H

#include <sndfile.h>
#include <mpg123.h>

#define MAX_BARS 50

#ifdef __cplusplus
extern "C" {
#endif

int init_audio(const char *file_path);
void get_audio_intensities(float intensities[MAX_BARS]);
void cleanup_audio();
int play_audio(const char *file_path);
void stop_audio();

#ifdef __cplusplus
}
#endif

#endif // AUDIO_VIZ_H
```

### `lib.cpp` (C++ Implementation)

This file includes functions that initialize the audio file (MP3/WAV), calculate intensities, and clean up resources.

```cpp
#include "lib.h"
#include <iostream>
#include <cmath>

static SNDFILE *sndFile = nullptr;
static mpg123_handle *mpg123 = nullptr;
static int channels = 0;
static int rate = 0;

int init_audio(const char *file_path) {
    if (strstr(file_path, ".mp3")) {
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
        SF_INFO sfinfo;
        sndFile = sf_open(file_path, SFM_READ, &sfinfo);
        if (!sndFile) {
            std::cerr << "Error opening WAV file" << std::endl;
            return -1;
        }
        rate = sfinfo.samplerate;
        channels = sfinfo.channels;
    }

    return 0;
}

void get_audio_intensities(float intensities[MAX_BARS]) {
    // Audio processing logic to calculate RMS intensities
}

void cleanup_audio() {
    if (mpg123) {
        mpg123_close(mpg123);
        mpg123_delete(mpg123);
        mpg123_exit();
    }

    if (sndFile) {
        sf_close(sndFile);
    }
}
```

### JNI Integration

The JNI interface maps C++ functions to Kotlin methods:

```cpp
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_audio_viz_MainActivity_initAudio(JNIEnv *env, jobject thiz, jstring file_path) {
    const char *path = env->GetStringUTFChars(file_path, 0);
    return init_audio(path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_audio_viz_MainActivity_getAudioIntensities(JNIEnv *env, jobject thiz, jfloatArray intensities) {
    jfloat *intensityArray = env->GetFloatArrayElements(intensities, NULL);
    get_audio_intensities(intensityArray);
    env->ReleaseFloatArrayElements(intensities, intensityArray, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_audio_viz_MainActivity_cleanupAudio(JNIEnv *env, jobject thiz) {
    cleanup_audio();
}
```

### Kotlin (MainActivity)

In `MainActivity.kt`, you'll define native methods and handle UI updates.

```kotlin
class MainActivity : AppCompatActivity() {

    companion object {
        init {
            System.loadLibrary("audio_viz")
        }
    }

    // Native methods
    external fun initAudio(filePath: String): Int
    external fun getAudioIntensities(intensities: FloatArray)
    external fun cleanupAudio()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val filePath = "path_to_audio_file"
        val intensities = FloatArray(50)

        // Initialize the audio
        initAudio(filePath)

        // Get intensities for visualization
        getAudioIntensities(intensities)

        // Update UI (visualizer view) with intensities
        // (e.g., drawing bars on a custom view)

        // Cleanup resources
        cleanupAudio()
    }
}
```

---

## Contributing

Feel free to fork the project and submit pull requests. If you'd like to enhance the visualizer, consider adding:

- **Frequency band visualization** using FFT (Fast Fourier Transform).
- **Customizations for the visualizer**, such as color schemes and animation effects.

For bugs or feature requests, please open an issue.

---

## License

This project is licensed under the [MIT License](LICENSE).
