#pragma once
#include <pybind11/pybind11.h>

namespace qe::pybind {

void bind_types(pybind11::module_& m);
void bind_models(pybind11::module_& m);
void bind_schemes(pybind11::module_& m);
void bind_engine(pybind11::module_& m);

} // namespace qe::pybind