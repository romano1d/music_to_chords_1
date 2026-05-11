#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <complex>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// преобразование фурье
vector<complex<double>> furi_transform(vector<double> signal){
    int size_signal = signal.size();
    vector<complex<double>> spectrum(size_signal);
    const double PI = acos(-1.0);


    
    for (int f = 0; f < size_signal; f++){
        complex<double> sum(0,  0);

        for (int i = 0; i < size_signal; i++){
            double t = 1.0 * i / size_signal;
            double item = (-1) * 2 * PI * f * t;
            complex<double> twiddle(cos(item), sin(item)); // возведение в экспоненту

            sum += signal[i] * twiddle;
        }
        spectrum[f] = sum;
    }

    return spectrum;
}


// функция разности между частотами
double delta(double frec) {
    return frec * 0.0577 / 2.0;
}


// определение одного пика частоты
vector<double> detect_frec(vector<complex<double>> spectrum, double N){

    int size_spectrum = spectrum.size();
    vector<double> frec;
    vector<double> ampls(size_spectrum);

    for (int i = 0; i < size_spectrum; i++){
        ampls[i] = abs(spectrum[i]);
    }

    // бегаем по спектру и определяем максимум
    double ampl_max = 0;
    int i_max = 0;
    for (int i = 0; i < size_spectrum / 2; i++) {
        if (ampls[i] > ampl_max) {
            ampl_max = ampls[i];
            i_max = i;
        }
    }
    frec.push_back(i_max * N / size_spectrum); // N / size_spectrum - это время прохода одного промежуточка (частота)

    return frec;
}
// определение нескольких пиков частот
vector<double> detect_multy_frec(vector<complex<double>> spectrum, double N){

    int size_spectrum = spectrum.size();
    vector<double> frec;
    vector<double> ampls(size_spectrum);

    for (int i = 0; i < size_spectrum; i++){
        ampls[i] = abs(spectrum[i]);
    }

    // бегаем по спектру и определяем максимумы
    double ampl_max = 0;
    int i_max = 0;
    for (int i = 0; i < size_spectrum / 2; i++) {
        if (ampls[i] > ampl_max) {
            ampl_max = ampls[i];
        }
    }

    for (int i = 0; i < size_spectrum / 2; i++) {
        if (ampls[i] > ampl_max * 0.20) {
            frec.push_back(i * N / size_spectrum);
        }
    }


    // усредняем близкие частоты
    vector<double> frec_final;
    int i = 0;
    while (i < frec.size()) {
        int count = 1;
        double sum = frec[i];
        while (i + 1 < frec.size() && abs(frec[i] - frec[i + 1]) < 5) {
            sum += frec[i + 1];
            i++;
            count++;
        }
        frec_final.push_back(sum / count);
        i++;
    }

    // // вырезаем четные гармоники (условия: неповторяемость и на хождение в малой октаве (130 - 246 Гц))
    // vector<double> frec_final_final;
    // for (int i = 0; i < frec_final.size(); i++) {
    //     if (frec_final[i] > 130 - delta(130) && frec_final[i] < 246 + delta(246)) {
    //         frec_final_final.push_back(frec_final[i]);
    //     }
    // }


    return frec_final;
}


// превращаем набор частот в список всевозможных гармоник
vector<double> get_all_harmonics(vector<double> frec) {

    vector<double> result;

    for (double item : frec) {
        for (int i = 1; i < 10; i *= 2) {
            if (item * i < 1000.0) {
                result.push_back(item * i);
            }
            if (item / i > 50.0) {
                result.push_back(item / i);
            }
        }
    }

    sort(result.begin(), result.end());
    result.erase(unique(result.begin(), result.end()), result.end()); // unique перемещает итератор на начало мусорной зоны (повторяющихся элементов) и затем erase их удаляет

    return result;

}


// определение названия ноты по частоте
string note_name(double f){
    ifstream file("frec_to_note.txt");
    
    string line;
    while (getline(file, line)) {
        
        stringstream ss(line);
        double frec;
        string name;
        ss >> frec >> name;
        
        if (abs(frec - f) < delta(f)){
            file.close();
            return name;
        }
    }
    
    file.close();
    return "Nope";
}


// определение аккорда по частотам
string chord_definition(vector<double> frec) {
    ifstream file("chords.json");
    json chords;
    file >> chords;
    
    for (json& chord : chords) {
        vector<double> freqs = chord["freqs"];
        string name = chord["name"];
        int N = freqs.size();
        int k = 0;
        for (double& i : freqs) {
            for (double& j : get_all_harmonics(frec)) {
                if (abs(i - j) < delta(j)) {
                    k++;
                    break;
                }
            }
        }

        if (k == N) {
            return name;
        }
    }



    return "No such chord in the list";

}








// Сохранение спектра в файл
void save_spectrum(vector<complex<double>> spectrum, double sampleRate) {
    ofstream file("spectrum.txt");
    int N = spectrum.size();
    
    for (int i = 0; i < N / 2; i++) {
        double freq = i * sampleRate / N;
        double amp = abs(spectrum[i]);
        file << freq << " " << amp << "\n";
    }
    
    file.close();
}
// Сохранение найденных частот
void save_frequencies(vector<double> frec) {

    ofstream file("frequencies.txt");

    for (int i = 0; i < frec.size(); i++) {
        file << frec[i] << " " << note_name(frec[i]) << "\n";
    }

    file.close();
}
// Сохранение названия аккорда
void save_chord_name(vector<double> frec) {

    ofstream file("chord_name.txt");
    
    file << "Chord name: "<< chord_definition(frec) << "\n";

    file.close();
}











void example_1() {
    const double PI = acos(-1.0);
    
    // Создаём искусственный сигнал для тестирования
    double sampleRate = 5000.0;  // 5000 отсчётов в секунду
    double frequency = 440.0;      // Нота Ля (440 Гц)
    double duration = 1.0;         // 1 секунда
    
    int numSamples = sampleRate * duration;
    vector<double> signal(numSamples);

    for (int i = 0; i < numSamples; i++) {
        double t = 1.0 * i / sampleRate;  // время в секундах
        signal[i] = sin(2.0 * PI * frequency * t);
    }

    vector<complex<double>> spectrum = furi_transform(signal);

    vector<double> frec = detect_frec(spectrum, sampleRate);

    for (int i = 0; i < frec.size(); i++) {
        cout << frec[i] << endl;
    }

    save_spectrum(spectrum, sampleRate);
    save_frequencies(frec);
}

void example_2() {
    const double PI = acos(-1.0);
    
    // Создаём искусственный сигнал для тестирования
    double sampleRate = 5000.0;  // 5000 отсчётов в секунду
    double frequency = 440.0;      // Нота Ля (440 Гц)
    double duration = 1.0;         // 1 секунда
    
    int numSamples = sampleRate * duration;
    vector<double> signal(numSamples);

    for (int i = 0; i < numSamples; i++) {
        double t = 1.0 * i / sampleRate;  // время в секундах
        signal[i] = sin(2.0 * PI * frequency * t) + sin(2.0 * PI * (2 *frequency) * t) + sin(2.0 * PI * (3 *frequency) * t);
    }

    vector<complex<double>> spectrum = furi_transform(signal);

    vector<double> frec = detect_multy_frec(spectrum, sampleRate);

    for (int i = 0; i < frec.size(); i++) {
        cout << frec[i] << endl;
    }

    save_spectrum(spectrum, sampleRate);
    save_frequencies(frec);
}

void analyze_signal_file() {
    const double PI = acos(-1.0);
    
    ifstream file("signal.txt");
    
    // Read sample rate from first line
    double sampleRate;
    file >> sampleRate;

    // Read number of samples (optional)
    int numSamples;
    file >> numSamples;
    
    // Read signal
    vector<double> signal;
    double value;
    while (file >> value) {
        signal.push_back(value);
    }
    file.close();
    
    cout << "Sample rate: " << sampleRate << " Hz" << endl;
    cout << "Signal loaded: " << signal.size() << " samples" << endl;
    cout << "Duration: " << signal.size() / sampleRate << " sec" << endl;
    
    // Compute spectrum
    vector<complex<double>> spectrum = furi_transform(signal);
    
    // Detect frequencies
    vector<double> frec = detect_multy_frec(spectrum, sampleRate);
    
    cout << "\nDetected frequencies:" << endl;
    for (int i = 0; i < frec.size(); i++) {
        cout << frec[i] << " --> " << note_name(frec[i]) << endl;
    }
    cout << "Chord name: "<< chord_definition(frec) << endl;
    
    save_spectrum(spectrum, sampleRate);
    save_frequencies(frec);
    save_chord_name(frec);
}



// void analyze_signal_file_long() {
//     const double PI = acos(-1.0);
    
//     ifstream file("signal.txt");
    
//     // Read sample rate from first line
//     double sampleRate;
//     file >> sampleRate;

//     // Read number of samples (optional)
//     int numSamples;
//     file >> numSamples;
    
//     // Read signal
//     vector<double> signal;
//     double value;
//     while (file >> value) {
//         signal.push_back(value);
//     }
//     file.close();
    
//     cout << "Sample rate: " << sampleRate << " Hz" << endl;
//     cout << "Signal loaded: " << signal.size() << " samples" << endl;
//     cout << "Duration: " << signal.size() / sampleRate << " sec" << endl;
    

//     for (int t = 0; t < floor(signal.size() / sampleRate) - 1; t++) {

//         vector<double> slice(signal.begin() + 4410 * t, signal.begin() + 4410 * (t + 1));
//         vector<complex<double>> spectrum = furi_transform(slice);
//         vector<double> frec = detect_multy_frec(spectrum, sampleRate);

//         ofstream file("frec_from_t.txt");

//         string name = chord_definition(frec);
//         if (name == "No such chord in the list") {
//             file << detect_frec(spectrum, sampleRate)[0] << " " << note_name(detect_frec(spectrum, sampleRate)[0]) << "\n";;
//         } else {

//         }
//     }

    
//     cout << "\nDetected frequencies:" << endl;
//     for (int i = 0; i < frec.size(); i++) {
//         cout << frec[i] << " --> " << note_name(frec[i]) << endl;
//     }
//     cout << "Chord name: "<< chord_definition(frec) << endl;
    
//     save_spectrum(spectrum, sampleRate);
//     save_frequencies(frec);
//     save_chord_name(frec);
// }






int main(){

    analyze_signal_file();

    return 0;
}





// g++ furi.cpp -o furi.exe -I. -std=c++17
// ./furi.exe