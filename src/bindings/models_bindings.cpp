#include <cstring>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <optional>
#include "models/black_scholes/black_scholes.hpp"
#include "models/dupire/dupire.hpp"
#include "models/heston/heston.hpp"
#include "models/ir_models/vasicek.h"
#include "models/model.hpp"
#include "surface/local_vol.hpp"

namespace py = pybind11; 

namespace qe::pybind {

void bind_models(py::module_& m) {

    py::class_<Model, std::shared_ptr<Model>>(m, "_Model");

    py::class_<BlackScholes, Model, std::shared_ptr<BlackScholes>>(m, "_BlackScholes")
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

    
    py::class_<Dupire, Model, std::shared_ptr<Dupire>>(m, "_Dupire")
        .def(py::init<float, float, std::shared_ptr<LocalVolatilitySurface>>(),
            py::arg("r"),
            py::arg("q"),
            py::arg("loc_vol_surface")
        );

    py::class_<Vasicek, Model, std::shared_ptr<Vasicek>>(m, "_Vasicek")
        .def(py::init<float, float, float>(), 
            py::arg("a"),
            py::arg("b"),
            py::arg("sigma"));
}

} // namespace qe::pybind
