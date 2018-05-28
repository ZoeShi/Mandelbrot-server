//
// Created by l on 28.05.18.
//


#include "Mandelbrot.h"

int Mandelbrot::calc_iterations(const std::complex<double> &c, int precision) {
    // Initial iteration: z0 = 0
    std::complex<double> z(0, 0);
    int k;
    for (k = 0; k < precision; k++) {
        // Next iteration: z(n+1) = z(n)^2 + c
        z = std::pow(z, 2) + c;
        // Is it still part of the Mandelbrot set? |z(n+1)| <= 2
        if (std::abs(z) > 2)
            break;
    }
    return k;
}
