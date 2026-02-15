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
            .def(py::init<double>(), py::arg("K"))
            .def("_compute", &CallPayoff::compute, py::arg("S"));

        py::class_<PutPayoff, Payoff, std::shared_ptr<PutPayoff>>(m, "_PutPayoff")
            .def(py::init<double>(), py::arg("K"))
            .def("_compute", &PutPayoff::compute, py::arg("S"));
            
        py::class_<DigitalCallPayoff, Payoff, std::shared_ptr<DigitalCallPayoff>>(m, "_DigitalCallPayoff")
            .def(py::init<double>(), py::arg("K"))
            .def("_compute", &DigitalCallPayoff::compute, py::arg("S"));
        
        py::class_<DigitalPutPayoff, Payoff, std::shared_ptr<DigitalPutPayoff>>(m, "_DigitalPutPayoff")
            .def(py::init<double>(), py::arg("K"))
            .def("_compute", &DigitalPutPayoff::compute, py::arg("S"));
            

    py::class_<Instrument>(m, "_Instrument")
        .def(py::init<OptionContract, std::shared_ptr<Payoff>>())
        .def("_compute_payoff",
            [](const Instrument& self, const SimulationResult& res){
                return self.compute_payoff(res);
            },
            py::arg("simulation_result"));


}
}



