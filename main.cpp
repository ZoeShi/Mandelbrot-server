#include <iostream>
#include <json/value.h>
#include <fstream>
#include "json.hpp"
#include "Mandelbrot.h"
#include <vector>


// std::cout<<mandelbrot;

int main() {
    std::ifstream mandelbrot_json("mandelbrot.json");

    nlohmann::json j;
    mandelbrot_json >> j;

    double real_from = j["RealFrom"];
    double real_to = j["RealTo"];
    double imaginary_from = j["ImaginaryFrom"];
    double imaginary_to = j["ImaginaryTo"];
    double intervall = j["Intervall"];
    double maxiteration = j["MaxIteration"];
    std::vector<int> iterations_vector;

    for (double k = imaginary_to; k > imaginary_from;k -= intervall) {
        for (double i = real_from; i < real_to; i += intervall) {
            auto iterations = Mandelbrot::calc_iterations (std::complex<double>(i, k), maxiteration);
            iterations_vector.push_back(iterations);
        }
    }

    std::ofstream output_json("output.json");

    nlohmann::json j_vec(iterations_vector);
    output_json << std::setw(4) << j_vec << std::endl;
}
