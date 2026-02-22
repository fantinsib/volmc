#include <memory>
#include <pybind11/attr.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <optional>
#include "schemes/schemes.hpp"
#include "engine/montecarlo.hpp"

namespace py = pybind11;

namespace qe::pybind {

void bind_engine(py::module_& m) {
    
    py::class_<MonteCarlo, std::shared_ptr<MonteCarlo>>(m, "_MonteCarlo")
        .def(py::init<std::shared_ptr<Scheme> >(),
            py::arg("scheme"),
            py::keep_alive<1,2>())
        .def("_generate", &MonteCarlo::generate,
            py::arg("S0"),
            py::arg("n"),
            py::arg("T"),
            py::arg("n_paths"),
            py::arg("v0") = py::none())
        .def("_configure", &MonteCarlo::configure,
            py::arg("seed"),
            py::arg("n_jobs")
        );
}

} // namespace qe::pybind
