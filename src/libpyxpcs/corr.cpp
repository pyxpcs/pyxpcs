#include <iostream>
#include <map>
#include <math.h>
#include <vector>
#include <tuple>
#include <omp.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "row.h"

namespace py = pybind11;

using std::cout;
using std::endl;
using std::vector;
using std::tuple;
using std::get;

PYBIND11_MAKE_OPAQUE(std::vector<int>);
PYBIND11_MAKE_OPAQUE(std::vector<float>);

vector<tuple<int, int>> DelayPerLevel(int frame_count, int dpl, int max_delay)
{
    vector< tuple<int, int> > result;

    int ll_dpl = 0;

    for (int i=0; i<= max_delay; i++)
    {
        int step = (int) pow(2.0, i);

        int dpll = i == 0 ? dpl * 2 : dpl;

        for (int j=0; j<dpll; j++) 
        {
            
            if ( (ll_dpl+step + pow(2.0, i)) > frame_count) break;

            result.push_back(std::make_tuple(i, ll_dpl + step));
            ll_dpl = ll_dpl + step;
        }
    }   

    return result;
}


int CalculateLevelMax(int frame_count, int dpl) 
{
    if (frame_count < dpl * 2) return 0;

    return (int) (floor(log2(frame_count) - log2(1.0 + 1.0/(double)(dpl))) - log2(dpl));
}

py::array_t<float> Multitau(py::array_t<int> valid_pixels, py::list rows, py::list values, py::dict config) 
{

    py::gil_scoped_acquire acquire;

    size_t no_of_frames = config["frames"].cast<int>();
    size_t no_of_pixels = config["pixels"].cast<int>();
    size_t dpl = config["delays_per_level"].cast<int>();

    size_t w = config["rows"].cast<int>();
    size_t h = config["cols"].cast<int>();

    size_t max_level = CalculateLevelMax(no_of_frames, dpl);

    vector<tuple<int,int> > delays_per_level = DelayPerLevel(no_of_frames, dpl, max_level);

    // float **result = new float[3 * pixels * delays_per_level.size()]
    size_t g2_size = no_of_pixels * delays_per_level.size();
    
    float* result = new float[3 * g2_size];

    for (int i = 0; i < (3 * g2_size); i++) {
        result[i] = 0.0f;
    }

    auto _valid_pixels = valid_pixels.mutable_unchecked<1>();
    //default(none) schedule(dynamic, 20) shared(_valid_pixels, delays_per_level, no_of_frames,g2_size, no_of_pixels, rows, values, result)
    // #pragma omp parallel for
    for (int i = 0; i < _valid_pixels.size(); i++)
    {   
        int pixno = _valid_pixels(i);
        

        py::array_t<int> tt = py::cast<py::array>(rows[pixno]);
        py::array_t<int> vv = py::cast<py::array>(values[pixno]);

        auto _times =  tt.mutable_unchecked<1>();
        auto _vals = vv.mutable_unchecked<1>();

        int last_level = 0;

        int last_frame = no_of_frames;
        int last_index = _times.size();

        int tau_index = 0;
        int g2_index = 0;
        int ip_index = 0;
        int if_index = 0;

        for (auto it = delays_per_level.begin() ; it != delays_per_level.end(); ++it)
        {
            tuple<int, int> tau_level = *it;
            int level = get<0>(tau_level);
            int tau = get<1>(tau_level);   

            // printf("level = %d, tau = %d\n", level, tau);

            // Multitau averaging step
            if (last_level != level)
            {   
                // Handle odd event case.
                if (last_frame % 2)
                    last_frame -= 1;

                last_frame = last_frame / 2;
                // printf("Last_frame %d\n", last_frame);

                int index = 0;
                int cnt = 0;
                    
                int i0, i1;
                i0 = _times(cnt) / 2.0;

                if (last_index > 0 && i0 < last_frame) 
                {
                    _times(index) = i0;
                    cnt = 1;
                }

                while (cnt < last_index) 
                {
                    i1 = _times(cnt) / 2.0;

                    if (i1 >= last_frame) break;

                    if (i1 == i0) 
                        _vals(index) += _vals(cnt);
                    else 
                    {
                        _times(++index) = i1;
                        _vals(index) = _vals(cnt);
                    }

                    i0 = i1;
                    cnt++;
                }

                last_index = index + 1;

                for (int k = 0 ; k < last_index; k++) 
                    _vals(k) /= 2.0f;
            }

            if (level > 0)
                tau = tau / pow(2, level);

            g2_index =  tau_index * no_of_pixels + pixno;
            ip_index =  (tau_index * no_of_pixels + pixno) +  g2_size;
            if_index =  (tau_index * no_of_pixels + pixno) + (2 * g2_size);

            // printf("Running to last_index of %d\n", last_index);
            for (int r = 0; r < last_index; r++)
            {
                
                int src = _times(r);

                if (src < (last_frame - tau) ) {

                    result[ip_index] += _vals(r);
                    
                    int low = 0;
                    for (; low < _times.size(); low++) 
                    {   
                        if (_times(low) >= (src + tau) )  break;
                    }

                    if (low != _times.size()) 
                    {
                      int pos = low - _times(0);
                      if (pos < last_index && _times(pos) == (src+tau))
                        result[g2_index] += _vals(r) * _vals(pos);
                    }
                 
                }

                if (src >= tau && src < last_frame) {
                  result[if_index] += _vals(r);
                }
            }

            if ( (last_frame - tau) > 0) {
                result[g2_index] /= (last_frame - tau);
                result[ip_index] /= (last_frame - tau);
                result[if_index] /= (last_frame - tau);
            }
            
            last_level = level;
            tau_index++;
        }
    }

    py::capsule free_when_done(result, [](void *f) {
        float *test = reinterpret_cast<float *>(f);
    });

    size_t return_dims = 3;
    return py::array_t<float>(
        {return_dims, delays_per_level.size(), no_of_pixels},
        {delays_per_level.size()*no_of_pixels*sizeof(float), no_of_pixels*sizeof(float), sizeof(float)},
        result,
        free_when_done
    );
}

int call(py::list list_of_arrays)
{
    for (int i = 0; i < list_of_arrays.size(); i++) {
        py::array_t<int> casted_array = py::cast<py::array>(list_of_arrays[i]);
        auto _casted_array = casted_array.unchecked<2>();

        for (int i = 0; i < _casted_array.shape(0); ++i) {
            for (int j = 0; j < _casted_array.shape(1); ++j) {
                std::cout << _casted_array(i, j) << " ";
            }
            std::cout << '\n';
        }
    }

    return 0;
}

void test(Row &list) {
    for (auto entry: list.indxPtr) {
        std::cout << entry <<std::endl;
    }
}

PYBIND11_MODULE(libpyxpcs, m) {
    m.doc() = R"pbdoc(
        XPCS Analysis Toolkit
        -----------------------

        .. currentmodule:: pyxpcs

        .. autosummary::
           :toctree: _generate

           multitau
    )pbdoc";

    py::class_<IntList>(m, "IntList")
        .def(py::init<>())
        .def("pop_back", &IntList::pop_back)
        /* There are multiple versions of push_back(), etc. Select the right ones. */
        .def("push_back", (void (IntList::*)(const int &)) &IntList::push_back)
        .def("back", (int &(IntList::*)()) &IntList::back)
        .def("__len__", [](const IntList &v) { return v.size(); })
        .def("__iter__", [](IntList &v) {
            return py::make_iterator(v.begin(), v.end());
        }, py::keep_alive<0, 1>());
        
    py::class_<FloatList>(m, "FloatList")
        .def(py::init<>())
        .def("pop_back", &FloatList::pop_back)
        /* There are multiple versions of push_back(), etc. Select the right ones. */
        .def("push_back", (void (FloatList::*)(const float &)) &FloatList::push_back)
        .def("back", (float &(FloatList::*)()) &FloatList::back)
        .def("__len__", [](const FloatList &v) { return v.size(); })
        .def("__iter__", [](FloatList &v) {
            return py::make_iterator(v.begin(), v.end());
        }, py::keep_alive<0, 1>());

    py::class_<Row>(m, "Row")
        .def(py::init<>())
        .def_readwrite("indxPtr", &Row::indxPtr)
        .def_readwrite("valPtr", &Row::valPtr);

    m.def("multitau", &Multitau, "Multi-thread version of multitau");
    m.def("test", &test);
    
    // m.def("multitau", [](py::array_t<int> valid_pixels, py::list rows, py::list values, py::dict config) {
    //     // py::gil_scoped_release release;
    //     return Multitau(valid_pixels, rows, values, config);
    // });
    // m.def("twice", [](py::array_t<int> valid_pixels, py::list rows) {
    //   /* Release GIL before calling into C++ code */
    //   py::gil_scoped_release release;
    //   return twice(valid_pixels, rows);
    // });

    // m.def("multitau_test", &Multitau_2, "Multi-thread version of multitau");
    // m.def("callback", &callback, "");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}

