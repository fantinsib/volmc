#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <optional>
#include "types/path.hpp"
#include "types/state.hpp"

namespace py = pybind11; 

void bind_models(py::module_& m) {
    py::class_<State>(m, "State")
        .def(py::init<>())
        .def("spot", &State::spot)
        def("vol", &State::vol);

    py::class_<Path>(m, "Path")
        .def(py::init<>())
        .def("end_state", Path::end_state)
        .def("__len__", Path::size)
        def("len", Path::size);
}