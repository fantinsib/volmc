#include "instruments/instrument.h"
#include "options/options.hpp"
#include "payoff/payoff.h"
#include "types/simulationresult.hpp"
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>




namespace py = pybind11;

namespace qe::pybind {


void bind_options(py::module_& m){

    py::class_<OptionContract>(m, "_OptionContract")
        .def(py::init<double, double>(),
            py::arg("K"),
            py::arg("T"));

    py::class_<Payoff, std::shared_ptr<Payoff>>(m, "_Payoff");

    py::class_<CallPayoff, Payoff, std::shared_ptr<CallPayoff>>(m, "_CallPayoff")
        .def(py::init<>());
        
    py::class_<PutPayoff, Payoff, std::shared_ptr<PutPayoff>>(m, "_PutPayoff")
        .def(py::init<>());

    py::class_<DigitalCallPayoff, Payoff, std::shared_ptr<DigitalCallPayoff>>(m, "_DigitalCallPayoff")
        .def(py::init<>());

    py::class_<DigitalPutPayoff, Payoff, std::shared_ptr<DigitalPutPayoff>>(m, "_DigitalPutPayoff")
        .def(py::init<>());

    py::enum_<Direction>(m, "_Direction")
        .value("Up", Direction::Up)
        .value("Down", Direction::Down)
        .export_values();

    py::enum_<Nature>(m, "_Nature")
        .value("In", Nature::In)
        .value("Out", Nature::Out)
        .export_values();

    py::class_<Barrier, Payoff, std::shared_ptr<Barrier>>(m, "_Barrier")
        .def(py::init<double, Direction, Nature, Payoff&>(),
            py::arg("H"),
            py::arg("direction"),
            py::arg("nature"),
            py::arg("payoff"));

    py::class_<Instrument, std::shared_ptr<Instrument>>(m, "_Instrument")
        .def(py::init<OptionContract, std::shared_ptr<Payoff>>())
        .def("_compute_payoff",
            [](const Instrument& self, const SimulationResult& res){
                return self.compute_payoff(res);
            },
            py::arg("simulation_result"));


}
}



