#include "Application.h"
#include <iostream>

int main() {
    std::cout << "===========================================\n";
    std::cout << "    Ocean FFT Real-Time Simulator\n";
    std::cout << "    Using Phillips Spectrum + FFTW3\n";
    std::cout << "===========================================\n\n";

    // Create and initialize application
    Application app;
    
    if (!app.initialize(1920, 1080, "Ocean FFT Simulator")) {
        std::cerr << "Failed to initialize application\n";
        return -1;
    }

    // Run main loop
    app.run();

    std::cout << "\nExiting...\n";
    return 0;
}
