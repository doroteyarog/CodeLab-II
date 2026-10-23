#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

struct Joke {
    std::string setup;
    std::string punchline;
};

// Reads randomJokes.txt and parses setup and punchline separated by '?'
std::vector<Joke> loadJokes(const std::string& filename) {
    std::vector<Joke> jokes;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << "\n";
        return jokes;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t qPos = line.find('?');
        if (qPos != std::string::npos) {
            Joke j;
            j.setup = line.substr(0, qPos + 1);
            j.punchline = line.substr(qPos + 1);
            jokes.push_back(j);
        }
    }
    return jokes;
}

int main() {
    std::vector<Joke> jokes = loadJokes("randomJokes.txt");
    if (jokes.empty()) {
        std::cerr << "No jokes found or file failed to load.\n";
        return 1;
    }

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, jokes.size() - 1);

    std::cout << "=== Virtual Alexa Joke Machine ===\n";
    std::cout << "Say 'Alexa, tell me a joke' to hear a joke, or 'quit' to exit.\n\n";

    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "quit" || input == "Quit") {
            std::cout << "Goodbye!\n";
            break;
        }

        if (input == "Alexa, tell me a joke") {
            size_t randomIndex = dist(rng);
            std::cout << "\nAlexa: " << jokes[randomIndex].setup << "\n";

            std::cout << "(Press [Enter] for the punchline...)";
            std::string temp;
            std::getline(std::cin, temp);

            std::cout << jokes[randomIndex].punchline << "\n\n";
        } else {
            std::cout << "Sorry, I only respond to 'Alexa, tell me a joke' (or 'quit' to exit).\n";
        }
    }

    return 0;
}