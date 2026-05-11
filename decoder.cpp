#include <iostream>
#include <sndfile.h>
#include <vector>
#include <fstream>

int main() {
    SF_INFO info;
    info.format = 0;
    
    SNDFILE* file = sf_open("C.wav", SFM_READ, &info);
    
    // основная информация сигнала
    int sampleRate = info.samplerate;
    int sampleRate_wanted = sampleRate / 10;
    int channels = info.channels;
    int frames = info.frames;
    int frames_wanted = frames / 10;
    
    std::cout << "Sample rate wanted: " << sampleRate_wanted << " Hz" << std::endl;
    std::cout << "Sample rate: " << sampleRate << " Hz" << std::endl;
    std::cout << "Channels: " << channels << std::endl;
    std::cout << "Frames: " << frames << std::endl;
    std::cout << "Duration: " << (double)frames / sampleRate << " sec" << std::endl;
    
    // чтение файла
    std::vector<float> buffer(frames * channels);
    sf_read_float(file, buffer.data(), frames * channels);
    sf_close(file);
    
    // переход к 1 каналу
    std::vector<double> signal(frames);
    if (channels == 1) {
        for (int i = 0; i < frames; i++) {
            signal[i] = buffer[i];
        }
    } else {
        // среднее левого и правого каналов
        for (int i = 0; i < frames; i++) {
            signal[i] = (buffer[i * 2] + buffer[i * 2 + 1]) / 2.0;
        }
    }
    
    // усредняем каждые 10 значений сигнала
    std::vector<double> signal_squeezed(frames_wanted);
    
    for (int i = 0; i < frames_wanted; i++) {
        double sum = 0;
        for (int j = 0; j < 10; j++) {
            sum += signal[i * 10 + j];
        }
        signal_squeezed[i] = sum / 10.0;
    }
    




    // saving samplerate, number of samples, all samples
    std::ofstream signalFile("signal.txt");

    signalFile << sampleRate_wanted << "\n";

    signalFile << frames_wanted << "\n";

    for (int i = 0; i < frames_wanted; i++) {
        signalFile << signal_squeezed[i] << "\n";
    }
    signalFile.close();
    


    return 0;
}


// g++ decoder.cpp -o decoder.exe -lsndfile
// ./decoder.exe 