#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

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
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::string filename = "input.txt";
    double start_time = MPI_Wtime();

    // 1. Aflam dimensiunea totala a fisierului
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        if (rank == 0) std::cerr << "Eroare la deschiderea fisierului!" << std::endl;
        MPI_Finalize();
        return 1;
    }
    long file_size = file.tellg();

    // 2. Calcularea intervalului local de bytes pentru fiecare proces
    long low_bound = rank * file_size / size;
    long high_bound = (rank + 1) * file_size / size;
    if (rank == size - 1) high_bound = file_size; // Ultimul proces ia pana la capat

    file.seekg(low_bound);

    // Corectare la limita: daca nu suntem la inceput, sarim peste primul cuvant fragmentat
    // (il va citi procesul dinaintea noastra)
    if (rank != 0) {
        char c;
        while (file.get(c) && !std::isspace(c)) {
            // Citim in gol pana la primul spatiu
        }
    }

    // 3. Procesare locala (Dictionarul procesului)
    std::map<std::string, int> local_freq;
    std::string word;

    while (file >> word) {
        std::string cleaned = cleanWord(word);
        if (!cleaned.empty()) {
            local_freq[cleaned]++;
        }
        // Daca pozitia curenta a depasit limita superioara, ne oprim.
        // Citirea completa a ultimului cuvant s-a facut deja (file >> word citeste pana la spatiu).
        long current_pos = file.tellg();
        if (current_pos >= high_bound || current_pos == -1) {
            break;
        }
    }
    file.close();

    // 4. Pregatim dictionarul local pentru a-l trimite la Procesul 0
    // (Il transformam intr-un text continuu "cuvant1 5 cuvant2 3 ...")
    std::stringstream ss;
    for (auto const& [w, count] : local_freq) {
        ss << w << " " << count << "\n";
    }
    std::string serialized_map = ss.str();
    int local_len = serialized_map.size();

    // 5. Adunam rezultatele finale in Procesul 0
    if (rank == 0) {
        std::map<std::string, int> global_freq = local_freq; // Includem ce a gasit P0

        for (int i = 1; i < size; i++) {
            int recv_len;
            // Primim lungimea textului
            MPI_Recv(&recv_len, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Primim dictionarul sub forma de text
            std::vector<char> buffer(recv_len + 1, '\0');
            MPI_Recv(buffer.data(), recv_len, MPI_CHAR, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Reconstruim dictionarul si adunam frecventele
            std::istringstream iss(buffer.data());
            std::string w;
            int c;
            while (iss >> w >> c) {
                global_freq[w] += c;
            }
        }

        double end_time = MPI_Wtime();

        long total_words = 0;
        for (auto const& [w, count] : global_freq) {
            total_words += count;
        }

        std::cout << "RULARE PARALELA (MPI)" << std::endl;
        std::cout << "Numar procese: " << size << std::endl;
        std::cout << "Cuvinte unice gasite: " << global_freq.size() << std::endl;
        std::cout << "Total cuvinte procesate: " << total_words << std::endl;
        std::cout << "Timp executie: " << end_time - start_time << " secunde" << std::endl;

    }
    else {
        // Procesele Worker (rank > 0) trimit datele catre Master (rank 0)
        MPI_Send(&local_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(serialized_map.c_str(), local_len, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}