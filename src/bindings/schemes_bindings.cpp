#include <memory>
#include <pybind11/pybind11.h>
#include "models/black_scholes/black_scholes.hpp"
#include "models/dupire/dupire.hpp"
#include "models/heston/heston.hpp"
#include "models/model.hpp"
#include "schemes/euler.h"
#include "schemes/eulerheston.hpp"
#include "schemes/qe.hpp"

namespace py = pybind11;

namespace qe::pybind {

void bind_schemes(py::module_& m) {

    py::class_<Scheme, std::shared_ptr<Scheme>>(m, "_Scheme");

    py::class_<EulerHeston, Scheme, std::shared_ptr<EulerHeston>>(m, "_EulerHeston")
        .def(py::init<Heston>(),
        py::arg("model"));
    
    py::class_<QE, Scheme, std::shared_ptr<QE>>(m, "_QE")
        .def(py::init<Heston, float>(),
        py::arg("model"),
        py::arg("psi_c"));


    py::class_<Euler, Scheme, std::shared_ptr<Euler>>(m, "_Euler")
        .def(py::init<std::shared_ptr<Model>>(),
        py::arg("model"));

}

} // namespace qe::pybind
