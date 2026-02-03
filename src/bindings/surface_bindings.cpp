#include <memory>
#include <pybind11/attr.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include <vector>
#include <stdexcept>
#include <optional>
#include "schemes/schemes.hpp"
#include "engine/montecarlo.hpp"
#include "surface/local_vol.hpp"

namespace py = pybind11;

namespace qe::pybind {

void bind_surface(py::module_& m) {

    py::class_<LocalVolatilitySurface>(m, "_LocalVolatilitySurface")
        .def(py::init([](py::array_t<double, py::array::c_style | py::array::forcecast> t,
                         py::array_t<double, py::array::c_style | py::array::forcecast> s,
                         py::array_t<double, py::array::c_style | py::array::forcecast> V) {

                            if (s.ndim() != 1) throw std::runtime_error("LocalVolatilitySurface : spot value array must be 1D");
                            if (t.ndim() != 1) throw std::runtime_error("LocalVolatilitySurface : time array must be 1D");
                            if (V.ndim() != 2) throw std::runtime_error("LocalVolatilitySurface : volatility matrix must be 2D");

                            size_t len_s = s.size();
                            size_t len_t = t.size();
                            size_t rows = V.shape(0);
                            size_t cols = V.shape(1);
                            
                            const double* ptr_s = s.data();
                            const double* ptr_t = t.data();
                            const double* ptr_V = V.data();

                            std::vector<double> times(ptr_t, ptr_t + len_t);
                            std::vector<double> spots(ptr_s, ptr_s + len_s);
                            std::vector<double> sigma(ptr_V, ptr_V+ rows*cols);

                            return LocalVolatilitySurface{times, spots, sigma};

                         }))
        .def(
            "_sigma",
            &LocalVolatilitySurface::sigma,
            py::arg("t"),
            py::arg("s")
        );
}

} // namespace qe::pybind
