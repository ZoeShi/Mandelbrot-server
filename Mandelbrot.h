//
// Created by l on 28.05.18.
//

#ifndef MANDELBROT_SERVER_MANDELBROT_H
#define MANDELBROT_SERVER_MANDELBROT_H


#include <complex>

struct Mandelbrot {
    static int calc_iterations (const std::complex<double> &c, const int precision);
};

#endif //MANDELBROT_SERVER_MANDELBROT_H




