#include <iostream>
#include <map>
#include <math.h>
#include <vector>
#include <tuple>

#include <omp.h>


#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

using std::cout;
using std::endl;
using std::vector;
using std::tuple;
using std::get;


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

py::array_t<float> Multitau_2(py::list rows, py::dict config) 
{
    return py::array_t<float>(10);   
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
    
    printf("G2 size = %d\n", g2_size);

    float* result = new float[3 * g2_size];

    for (int i = 0; i < (3 * g2_size); i++) {
        result[i] = 0.0f;
    }

    auto _valid_pixels = valid_pixels.mutable_unchecked<1>();
    //default(none) schedule(dynamic, 20) shared(_valid_pixels, delays_per_level, no_of_frames,g2_size, no_of_pixels, rows, values, result)
    
    #pragma omp parallel for
    for (int i = 0; i < _valid_pixels.size(); i++)
    {   
        int pixno = _valid_pixels(i);
        
        // printf("%d\n", pixno);

        py::array_t<int> tt = py::cast<py::array>(rows[pixno]);
        py::array_t<int> vv = py::cast<py::array>(values[pixno]);

        // auto _times =  tt.mutable_unchecked<1>();
        // auto _vals = vv.mutable_unchecked<1>();

        // int last_level = 0;

        // int last_frame = no_of_frames;
        // int last_index = _times.size();

        // int tau_index = 0;
        // int g2_index = 0;
        // int ip_index = 0;
        // int if_index = 0;

        // for (auto it = delays_per_level.begin() ; it != delays_per_level.end(); ++it)
        // {
        //     tuple<int, int> tau_level = *it;
        //     int level = get<0>(tau_level);
        //     int tau = get<1>(tau_level);   

        //     // printf("level = %d, tau = %d\n", level, tau);

        //     // Multitau averaging step
        //     if (last_level != level)
        //     {   
        //         // Handle odd event case.
        //         if (last_frame % 2)
        //             last_frame -= 1;

        //         last_frame = last_frame / 2;
        //         // printf("Last_frame %d\n", last_frame);

        //         int index = 0;
        //         int cnt = 0;
                    
        //         int i0, i1;
        //         i0 = _times(cnt) / 2.0;

        //         if (last_index > 0 && i0 < last_frame) 
        //         {
        //             _times(index) = i0;
        //             cnt = 1;
        //         }

        //         while (cnt < last_index) 
        //         {
        //             i1 = _times(cnt) / 2.0;

        //             if (i1 >= last_frame) break;

        //             if (i1 == i0) 
        //                 _vals(index) += _vals(cnt);
        //             else 
        //             {
        //                 _times(++index) = i1;
        //                 _vals(index) = _vals(cnt);
        //             }

        //             i0 = i1;
        //             cnt++;
        //         }

        //         last_index = index + 1;

        //         for (int k = 0 ; k < last_index; k++) 
        //             _vals(k) /= 2.0f;
        //     }

        //     if (level > 0)
        //         tau = tau / pow(2, level);

        //     g2_index =  tau_index * no_of_pixels + pixno;
        //     ip_index =  (tau_index * no_of_pixels + pixno) +  g2_size;
        //     if_index =  (tau_index * no_of_pixels + pixno) + (2 * g2_size);

        //     // printf("Running to last_index of %d\n", last_index);
        //     for (int r = 0; r < last_index; r++)
        //     {
                
        //         int src = _times(r);

        //         if (src < (last_frame - tau) ) {

        //             result[ip_index] += _vals(r);
                    
        //             int low = 0;
        //             for (; low < _times.size(); low++) 
        //             {   
        //                 if (_times(low) >= (src + tau) )  break;
        //             }

        //             if (low != _times.size()) 
        //             {
        //               int pos = low - _times(0);
        //               if (pos < last_index && _times(pos) == (src+tau))
        //                 result[g2_index] += _vals(r) * _vals(pos);
        //             }
                 
        //         }

        //         if (src >= tau && src < last_frame) {
        //           result[if_index] += _vals(r);
        //         }
        //     }

        //     if ( (last_frame - tau) > 0) {
        //         result[g2_index] /= (last_frame - tau);
        //         result[ip_index] /= (last_frame - tau);
        //         result[if_index] /= (last_frame - tau);
        //     }
            
        //     last_level = level;
        //     tau_index++;
        // }
    }

    py::capsule free_when_done(result, [](void *f) {
        float *test = reinterpret_cast<float *>(f);
    });

    // //  return py::array_t<double>(
    // //     {a, b, c},
    // //     // {100*100*8, 100*8, 8},
    // //     result,
    // //     free_when_done
    // // );

    printf("Done\n");
    size_t return_dims = 3;
    return py::array_t<float>(
        {return_dims, delays_per_level.size(), no_of_pixels},
        {delays_per_level.size()*no_of_pixels*sizeof(float), no_of_pixels*sizeof(float), sizeof(float)},
        result,
        free_when_done
    );

    // return py::array_t<float>(10);
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

py::array_t<double> callback(size_t a, size_t b, size_t c)
{
    size_t size = a * b * c;
    double *result = new double[size];

    for (int i = 0; i < size; i++) {
        result[i] = (double) i;
    }

    // memory when destroyed:
    py::capsule free_when_done(result, [](void *f) {
        double *result = reinterpret_cast<double *>(f);
        delete[] result;
    });

    return py::array_t<double>(
        {a, b, c},
        // {100*100*8, 100*8, 8},
        result,
        free_when_done
    );

}

// Passing in an array of doubles
void twice(py::array_t<int> valid_pixels, py::list rows) {
    py::gil_scoped_acquire acquire;

    // py::buffer_info info = xs.request();
    // auto ptr = static_cast<double *>(info.ptr);

    // int n = 1;
    // for (auto r: info.shape) {
    //   n *= r;
    // }

    // #pragma omp parallel for
    // for (int i = 0; i <n; i++) {
    //     *ptr++ *= 2;
    // }

    auto _valid_pixels = valid_pixels.mutable_unchecked<1>();
    
    // py::array_t<int> tt = py::cast<py::array>(rows[pixno]);
    // py::array_t<int> vv = py::cast<py::array>(values[pixno]);

        // auto _times =  tt.mutable_unchecked<1>();
        // auto _vals = vv.mutable_unchecked<1>();

        // int last_level = 0;

        // int last_frame = no_of_frames;
        // int last_index = _times.size();

        // int tau_index = 0;
        // int g2_index = 0;
        // int ip_index = 0;
        // int if_index = 0;
    // std::vector<py::array_t<int>* > ptrs;
    std::vector<pybind11::detail::unchecked_mutable_reference<int, 1> > ptrs;
    for (int i = 0; i < _valid_pixels.size(); i++)
    {
        py::array_t<int> tt = py::cast<py::array>(rows[i]);
        auto _times =  tt.mutable_unchecked<1>();
        // printf("Valid pixel %d and frame# at 0 %d\n", i, _times[0]);
        ptrs.push_back(_times);
    }



    // printf("%d\n", *(ptrs[5]);
    auto _times = ptrs.at(5);
    printf("value = %d\n", _times[0]);

    // auto _times = ptrs[4]->mutable_unchecked<1>();
    // auto _times = ptrs[0];
    // printf("value = %d\n", _times[1]);

    //default(none) schedule(dynamic, 20) shared(_valid_pixels, delays_per_level, no_of_frames,g2_size, no_of_pixels, rows, values, result)
    // #pragma omp parallel for
    // for (int i = 0; i < _valid_pixels.size(); i++)
    // {   
    //     int pixno = _valid_pixels(i);
    //     // auto ptr = static_cast<int *>(rows[pixno]);
    //     // py::array_t<int> tt = py::cast<py::array>(rows[pixno]);
    //     // py::array_t<int> vv = py::cast<py::array>(values[pixno]);

    //     auto _times =  ptrs[i]->mutable_unchecked<1>();
    //     // auto _vals = vv.mutable_unchecked<1>();

    //             // auto ptr = static_cast<int *>(info.ptr);
    //     printf("%d , %d\n", omp_get_thread_num(), _times[0]);
    // }
}


// PYBIND11_PLUGIN(code) {
//   pybind11::module m("libpyxpcs", "Multi-thread version of multitau");
//   m.def("multitau", [](py::array_t<int> valid_pixels, py::list rows, py::list values, py::dict config) {
//       /* Release GIL before calling into C++ code */
//       py::gil_scoped_release release;
//       return Multitau(valid_pixels, rows, values, config);
//     });

//   return m.ptr();
// }


PYBIND11_MODULE(libpyxpcs, m) {
    m.doc() = R"pbdoc(
        XPCS Analysis Toolkit
        -----------------------

        .. currentmodule:: pyxpcs

        .. autosummary::
           :toctree: _generate

           multitau
    )pbdoc";

    // m.def("multitau", &Multitau, "Multi-thread version of multitau");
    m.def("multitau", [](py::array_t<int> valid_pixels, py::list rows, py::list values, py::dict config) {
        py::gil_scoped_release release;
        return Multitau(valid_pixels, rows, values, config);
    });
    m.def("twice", [](py::array_t<int> valid_pixels, py::list rows) {
      /* Release GIL before calling into C++ code */
      py::gil_scoped_release release;
      return twice(valid_pixels, rows);
    });

    // m.def("multitau_test", &Multitau_2, "Multi-thread version of multitau");
    // m.def("callback", &callback, "");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}

