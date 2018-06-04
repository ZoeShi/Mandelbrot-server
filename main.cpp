#include <iostream>
#include <json/value.h>
#include <fstream>
#include "json.hpp"
#include "Mandelbrot.h"
#include <vector>
#define ASIO_DISABLE_THREADS
#include "server_http.hpp"
#include <sstream>
#include <omp.h>
#include <memory>

struct InputParameters {
    double real_from;
    double real_to;
    double imaginary_from;
    double imaginary_to;
    double interval;
    int max_iteration;
};

class Main {
private:
    InputParameters params;

    void json_to_input_parameters(const nlohmann::json &j) {
        params.real_from = j["realfrom"];
        params.real_to = j["realto"];
        params.imaginary_from = j["imaginaryfrom"];
        params.imaginary_to = j["imaginaryto"];
        params.interval = j["interval"];
        params.max_iteration = j["maxiteration"];
    }

    void input_json_string_to_params(const std::string &input) {
        // Transform input to lower case, so that the JSON key names are case insensitive
        std::string input_lower(input);
        transform(input_lower.begin(), input_lower.end(), input_lower.begin(), tolower);

        // Load into stream and parse into json object
        std::stringstream mandelbrot_json(input_lower);
        nlohmann::json j;
        mandelbrot_json >> j;
        json_to_input_parameters(j);
    }

    std::string response_vector_to_json_string(const std::vector<int> &iterations_vector) {
        nlohmann::json output_json;
        output_json["response"] = iterations_vector;

        std::stringstream output_json_strstream;
        output_json_strstream << output_json << std::endl;
        return output_json_strstream.str();
    }

    std::vector<int> run_mandelbrot_loops(int x_steps, int y_steps) {
        std::vector<std::vector<int>> iterations_vectors(x_steps);

        // Run mandelbrot function on each complex number (= point x, y)
#pragma omp parallel for
        for (int x = 0; x < x_steps; x++) {
            double real = params.real_from + x * params.interval;
            for (int y = 0; y < y_steps; y++) {
                double imaginary = params.imaginary_to - y * params.interval;
                auto iterations = Mandelbrot::calc_iterations(std::complex<double>(real, imaginary), params.max_iteration);
                iterations_vectors[x].push_back(iterations);
            }
        }

        // Copy together computed lines into one big array
        std::vector<int> iterations_vector;
        for (auto &v : iterations_vectors) {
            iterations_vector.insert(end(iterations_vector), begin(v), end(v));
        }
        return iterations_vector;
    }

    int php_range_count(double start, double end, double step) {
        double __calc_size = ((end - start) / step) + 1;
        int size = std::trunc(__calc_size);
        return size;
    }

public:
    std::string process(const std::string &input) {
        // Read input parameters from JSON
        input_json_string_to_params(input);

        // Calc number of steps in each direction (PHP-like!)
        int x_steps = php_range_count(params.real_from, params.real_to, params.interval);
        int y_steps = php_range_count(params.imaginary_from, params.imaginary_to, params.interval);

        // Get vector with results
        auto iterations_vector = run_mandelbrot_loops(x_steps, y_steps);

        // Get response JSON via json parser into stream and then into string
        auto output_json = response_vector_to_json_string(iterations_vector);
        return output_json;
    }
};

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

int main() {
    HttpServer server;
    server.config.port = 8080;

    server.resource["/"]["POST"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        // Get POST data from client, process and return reply
        auto input = request->content.string();
        Main m;
        std::string reply = m.process (input);

        *response << "HTTP/1.1 200 OK\r\nContent-Length: " << reply.length() << "\r\n\r\n"
                  << reply;
    };

    server.on_error = [](std::shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
        // Handle errors here
        // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
    };

    std::thread server_thread([&server]() {
        // Start server
        server.start();
    });

    server_thread.join();
}

