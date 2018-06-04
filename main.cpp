#include <iostream>
#include <json/value.h>
#include <fstream>
#include "json.hpp"
#include "Mandelbrot.h"
#include <vector>
#define ASIO_DISABLE_THREADS
#include <cxxhttp/httpd.h>
#include <sstream>


std::string process (const std::string &input) {
    std::stringstream mandelbrot_json(input);

    nlohmann::json j;
    mandelbrot_json >> j;

    double real_from = j["RealFrom"];
    double real_to = j["RealTo"];
    double imaginary_from = j["ImaginaryFrom"];
    double imaginary_to = j["ImaginaryTo"];
    double intervall = j["Intervall"];
    int maxiteration = j["MaxIteration"];
    std::vector<int> iterations_vector;

    // To ensure the interval is inclusive (avoid round error) use epsilon value for comparison
    auto epsilon = intervall / 2.0;
    for (double i = real_from; i - epsilon < real_to; i += intervall) {
        for (double k = imaginary_to; k + epsilon > imaginary_from; k -= intervall) {
            auto iterations = Mandelbrot::calc_iterations (std::complex<double>(i, k), maxiteration);
            iterations_vector.push_back(iterations);
        }
    }
    std::cout << std::endl;

    std::stringstream output_json_strstream("output.json");

    nlohmann::json output_json;

    output_json["response"] = iterations_vector;

    //output_json_strstream << std::setw(4) << output_json << std::endl;
    output_json_strstream << output_json << std::endl;
    return output_json_strstream.str ();
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