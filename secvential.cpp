#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <chrono>
#include <algorithm>

// Functie ajutatoare pentru curatarea cuvintelor (scoate semnele de punctuatie si le face litere mici)
std::string cleanWord(std::string word) {
    std::string clean = "";
    for (char c : word) {
        if (isalpha(c)) {
            clean += tolower(c);
        }
    }
    return clean;
}

int main(int argc, char** argv) {
    std::string filename = "input.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Eroare la deschiderea fisierului!" << std::endl;
        return 1;
    }

    std::map<std::string, int> word_freq;
    std::string word;

    auto start_time = std::chrono::high_resolution_clock::now();

    while (file >> word) {
        std::string cleaned = cleanWord(word);
        if (!cleaned.empty()) {
            word_freq[cleaned]++;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // Calculam numarul total de cuvinte pentru verificare
    long total_words = 0;
    for (auto const& [w, count] : word_freq) {
        total_words += count;
    }

    std::cout << "RULARE SECVENTIALA" << std::endl;
    std::cout << "Cuvinte unice gasite: " << word_freq.size() << std::endl;
    std::cout << "Total cuvinte procesate: " << total_words << std::endl;
    std::cout << "Timp executie: " << elapsed.count() << " secunde" << std::endl;

    return 0;
}