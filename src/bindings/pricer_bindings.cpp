#include "engine/engine.hpp"
#include "engine/montecarlo.hpp"
#include "instruments/instrument.h"
#include "pricing/pricer.h"
#include "types/marketstate.h"
#include <memory>
#include <optional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace qe::pybind {


void bind_pricer(py::module_& m){

    py::class_<MarketState>(m, "_MarketState")
        .def(py::init<double, double, std::optional<double>>(), 
            py::arg("S0"),
            py::arg("r"),
            py::arg("v0") = py::none());

    py::class_<Pricer>(m, "_Pricer")
        .def(py::init<std::shared_ptr<Instrument>, std::shared_ptr<MonteCarlo>>(),
            py::arg("instrument"),
            py::arg("engine"))
        .def("_compute_price", 
            [](const Pricer& self, const MarketState& marketstate, int n_steps, int n_paths)
            {return self.compute_price(marketstate, n_steps, n_paths);});


}

}
