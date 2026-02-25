#include <cmath>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <optional>
#include <span>
#include <stdexcept>
#include "types/path.hpp"
#include "types/simulationresult.hpp"
#include "types/state.hpp"

namespace py = pybind11; 

//-----------------SimulationResult

//converts a row-major array of value (spot) from simulationresult into a np.matrix
    static py::array spot_view(py::object self, const SimulationResult& res) {
        
        const std::vector<double>& paths =res.get_paths();
        if (paths.size() == 0) throw std::runtime_error("Error : no paths were found in the the SimulationResult");

        const ssize_t n_rows = static_cast<ssize_t>(res.get_npaths());
        const ssize_t n_cols = static_cast<ssize_t>(res.get_nsteps()+1);

        py::array_t<double> out({n_rows, n_cols});
        auto r = out.mutable_unchecked<2>();

    for (ssize_t i = 0; i < n_rows; ++i) {
        const size_t offset = static_cast<size_t>(i) * static_cast<size_t>(n_cols);
        std::span<const double> row(paths.data() + offset, static_cast<size_t>(n_cols));

        ssize_t j = 0;
        for (double st : row) {
            r(i, j++) = st;
            }
        }
        return out;

    }

    static py::array vol_view(py::object self, const SimulationResult& res) {
        
        const std::vector<double>& paths =res.get_vol();
        if (paths.size() == 0) throw std::runtime_error("Error : no paths were found in the the SimulationResult");

        const ssize_t n_rows = static_cast<ssize_t>(res.get_npaths());
        const ssize_t n_cols = static_cast<ssize_t>(res.get_nsteps()+1);

        py::array_t<double> out({n_rows, n_cols});
        auto r = out.mutable_unchecked<2>();

    for (ssize_t i = 0; i < n_rows; ++i) {
        const size_t offset = static_cast<size_t>(i) * static_cast<size_t>(n_cols);
        std::span<const double> row(paths.data() + offset, static_cast<size_t>(n_cols));

        ssize_t j = 0;
        for (double st : row) {
            r(i, j++) = st;
            }
        }
        return out;

    }


namespace qe::pybind {

void bind_types(py::module_& m) {
    py::class_<State>(m, "_State")
        .def(py::init<>())
        .def("_spot", [](const State& s) {return s.at(0);})
        .def("_vol", [](const State& s) {return s.at(1);});

    py::class_<Path>(m, "_Path")
        .def(py::init<>())
        .def("end_state", &Path::end_state, py::return_value_policy::reference_internal)
        .def("__len__", &Path::size)
        .def("len", &Path::size)
        .def("at", &Path::at,
            py::arg("i"), py::return_value_policy::reference_internal);


    py::class_<SimulationResult>(m, "_SimulationResult")
        .def_property_readonly("spot", [](py::object self) {
            const auto& r = self.cast<const SimulationResult&>();
            return spot_view(self, r);
        })
        .def_property_readonly("vol", [](py::object self) {
            const auto& r = self.cast<const SimulationResult&>();
            return vol_view(self, r);
        });

    }
} // namespace qe::pybind
