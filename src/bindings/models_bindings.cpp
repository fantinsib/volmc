#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <optional>
#include "models/blackscholes/blackscholes.hpp"


namespace py = pybind11; 

void bind_models(py::module_& m) {

    py::class_<BlackScholes>(m, "_BlackScholes")

}