#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <optional>
#include "models/black_scholes/black_scholes.hpp"
#include "models/dupire/dupire.hpp"
#include "models/heston/heston.hpp"
#include "models/model.hpp"

namespace py = pybind11; 

namespace qe::pybind {

void bind_models(py::module_& m) {

    py::class_<Model>(m, "_Model")
        .def(py::init());

    py::class_<BlackScholes>(m, "_BlackScholes")
        .def(py::init<float, float>(),
            py::arg("mu"),
            py::arg("sigma"));

    py::class_<Heston>(m, "_Heston")
        .def(py::init<float, float, float, float, float>(),
            py::arg("mu"),
            py::arg("kappa"),
            py::arg("theta"),
            py::arg("epsilon"),
            py::arg("rho"))

        .def("_feller_condition", &Heston::feller_condition);

    /*
    py::class_<Dupire>(m, "_Dupire")
        .def(py::init<float, float,  >(),
            py::arg("mu"),
            py::arg("sigma"));
*/
}

} // namespace qe::pybind
