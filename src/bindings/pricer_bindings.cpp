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
        .def(py::init<MarketState, int, int, std::shared_ptr<MonteCarlo>>(),
            py::arg("marketstate"),
            py::arg("n_steps"),
            py::arg("n_paths"),
            py::arg("engine"))
        .def("_compute_price", 
            [](const Pricer& self, std::shared_ptr<Instrument> instrument)
            {return self.compute_price(instrument);});


}

}
