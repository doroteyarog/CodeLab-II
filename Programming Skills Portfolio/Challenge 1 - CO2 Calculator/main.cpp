#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

// Prompts user for a positive distance value with basic input validation
double getDistanceTravelled() {
    double distance = 0.0;
    while (true) {
        std::cout << "Enter the journey distance in miles: ";
        if (std::cin >> distance && distance >= 0.0) {
            return distance;
        }
        std::cout << "Invalid input. Please enter a positive number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Prompts user to select Diesel or Petrol
char getFuelType() {
    char fuelChoice;
    while (true) {
        std::cout << "Select fuel type (D for Diesel, P for Petrol): ";
        std::cin >> fuelChoice;
        fuelChoice = std::toupper(fuelChoice);

        if (fuelChoice == 'D' || fuelChoice == 'P') {
            return fuelChoice;
        }
        std::cout << "Invalid choice. Please enter 'D' or 'P'.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Calculates emissions based on: 9.66 L / 100 miles
// Diesel: 2.68 kg CO2/L | Petrol: 2.31 kg CO2/L
double calculateEmissions(double distanceMiles, char fuelType) {
    const double LITRES_PER_100_MILES = 9.66;
    const double DIESEL_CO2_PER_LITRE = 2.68;
    const double PETROL_CO2_PER_LITRE = 2.31;

    double litresUsed = (distanceMiles / 100.0) * LITRES_PER_100_MILES;
    double emissionFactor = (fuelType == 'D') ? DIESEL_CO2_PER_LITRE : PETROL_CO2_PER_LITRE;

    return litresUsed * emissionFactor;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "        Vehicle CO2 Calculator          \n";
    std::cout << "========================================\n";

    double distance = getDistanceTravelled();
    char fuelType = getFuelType();
    double totalEmissions = calculateEmissions(distance, fuelType);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n--- Journey Summary ---\n";
    std::cout << "Distance: " << distance << " miles\n";
    std::cout << "Fuel Type: " << (fuelType == 'D' ? "Diesel" : "Petrol") << "\n";
    std::cout << "Estimated CO2 Emissions: " << totalEmissions << " kg\n";

    return 0;
}