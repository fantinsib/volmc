#include <pybind11/pybind11.h>
#include "bindings.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_volmc, m) {
    m.doc() = "Quadratic Exponential Monte Carlo engine";

    qe::pybind::bind_types(m);
    qe::pybind::bind_models(m);
    qe::pybind::bind_schemes(m);
    qe::pybind::bind_engine(m);
}
