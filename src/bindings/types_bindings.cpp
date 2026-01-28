#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <optional>
#include <stdexcept>
#include "types/path.hpp"
#include "types/simulationresult.hpp"
#include "types/state.hpp"

namespace py = pybind11; 

//-----------------SimulationResult

//converts a row-major array of value (spot) from simulationresult into a np.matrix
    static py::array spot_view(py::object self, const SimulationResult& res) {
        if (!res.pathbundle) throw std::runtime_error("SimulationResult.spot_view : no reference to paths was found");

        const auto& b = *res.pathbundle; 
        const ssize_t n_rows = static_cast<ssize_t>(b.n_paths);
        const ssize_t n_cols = static_cast<ssize_t>(b.n_steps+1);

        py::array_t<float> out({n_rows, n_cols});
        auto r = out.mutable_unchecked<2>();

        for (ssize_t p = 0; p < n_rows; ++p) {
            ssize_t t = 0;
            for (const State& st : b.paths[p]) {
                r(p, t++) = st.spot();
            }
        }
        return out;

    }


    static py::array var_view(py::object self, const SimulationResult& res) {
        if (!res.pathbundle) throw std::runtime_error("SimulationResult.spot_view : no reference to paths was found");

        const auto& b = *res.pathbundle; 
        const ssize_t n_rows = static_cast<ssize_t>(b.n_paths);
        const ssize_t n_cols = static_cast<ssize_t>(b.n_steps+1);

        py::array_t<float> out({n_rows, n_cols});
        auto r = out.mutable_unchecked<2>();

        for (ssize_t p = 0; p < n_rows; ++p) {
            ssize_t t = 0;
            for (const State& st : b.paths[p]) {
                if (!st.vol().has_value()) throw std::runtime_error("Error : no values for volatility");
                r(p, t++) = st.vol().value();
            }
        }
        return out;

    }

namespace qe::pybind {

void bind_types(py::module_& m) {
    py::class_<State>(m, "_State")
        .def(py::init<>())
        .def("_spot", &State::spot)
        .def("_vol", &State::vol);

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
        .def_property_readonly("var", [](py::object self) {
            const auto& r = self.cast<const SimulationResult&>();
            return var_view(self, r);
        });

    }
} // namespace qe::pybind
