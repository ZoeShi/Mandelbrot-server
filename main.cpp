#include <iostream>
#include <json/value.h>
#include <fstream>
#include "json.hpp"
#include "Mandelbrot.h"
#include <vector>
#define ASIO_DISABLE_THREADS
#include <cxxhttp/httpd.h>
#include <sstream>
#include <omp.h>

/*
 * real_from = -2
real_to = 1
imaginary_from = -1
imaginary_to = 1
intervall = 0.007
x_steps = 430
y_steps = 287

 */

int php_range_count (double start, double end, double step) {
    double __calc_size = ((end - start) / step) + 1;
    //int size = std::round (__calc_size);
    int size = std::trunc (__calc_size);
    return size;
}

std::vector<int> run_mandelbrot_loops(double real_from, double imaginary_to, double interval, int max_iteration,
                                      int x_steps, int y_steps);

std::string process (const std::string &input) {

    std::string input_lower (input);
    std::transform(input_lower.begin(), input_lower.end(), input_lower.begin(), ::tolower);

    std::cout << input_lower << std::endl;

    std::stringstream mandelbrot_json(input_lower);

    nlohmann::json j;
    mandelbrot_json >> j;

    double real_from = j["realfrom"];
    double real_to = j["realto"];
    double imaginary_from = j["imaginaryfrom"];
    double imaginary_to = j["imaginaryto"];
    double interval = j["interval"];
    int max_iteration = j["maxiteration"];


    std::cout << "real_from = " << real_from << "\n";
    std::cout << "real_to = " << real_to << "\n";
    std::cout << "imaginary_from = " << imaginary_from << "\n"; 
    std::cout << "imaginary_to = " << imaginary_to << "\n";
    std::cout << "interval = " << interval << "\n";

    int x_steps = php_range_count(real_from, real_to, interval);
    int y_steps = php_range_count(imaginary_from, imaginary_to, interval);
    std::cout << "x_steps = " << x_steps << "\n";
    std::cout << "y_steps = " << y_steps << "\n";

    std::vector<int> iterations_vector;
    iterations_vector = run_mandelbrot_loops(real_from, imaginary_to, interval, max_iteration, x_steps, y_steps);

    std::cout.flush();

    std::stringstream output_json_strstream;

    nlohmann::json output_json;

    output_json["response"] = iterations_vector;

    //output_json_strstream << std::setw(4) << output_json << std::endl;
    output_json_strstream << output_json << std::endl;
    return output_json_strstream.str ();
}

std::vector<int> run_mandelbrot_loops(double real_from, double imaginary_to, double interval, int max_iteration,
                                      int x_steps, int y_steps) {
    std::vector<std::vector<int>> iterations_vectors (x_steps);

// #pragma omp parallel for
    for (int x = 0; x < x_steps; x++) {
        double real = real_from + x * interval;
        for (int y = 0; y < y_steps; y++) {
            double imaginary = imaginary_to - y * interval;
            auto iterations = Mandelbrot::calc_iterations (std::complex<double>(real, imaginary), max_iteration);
            iterations_vectors[x].push_back(iterations);
        }
    }

    std::vector<int> iterations_vector;
    for(auto &v : iterations_vectors) {
        iterations_vector.insert(end(iterations_vector), begin(v), end(v));
    }
    return iterations_vector;
}


using namespace cxxhttp;

static void main_servlet (http::sessionData &session, std::smatch &) {
    std::string input = session.content;
    std::string reply = process (input);
    session.reply(200, reply);
}

static http::servlet servlet("/", ::main_servlet, "POST");

int main(int argc, char *argv[]) {
    return cxxhttp::main(argc, argv);
}