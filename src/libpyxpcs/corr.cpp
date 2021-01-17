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

py::array_t<float> Multitau2(py::array_t<py::object> pixels)
{
    py::list pi = py::cast<py::list>(pixels[0]);

    // size_t no_of_frames = kwargs["frames"].cast<int>();
    // size_t no_of_pixels = kwargs["pixels"].cast<int>();
    // size_t dpl = kwargs["delays_per_level"].cast<int>();

    // size_t w = kwargs["rows"].cast<int>();
    // size_t h = kwargs["cols"].cast<int>();
    
    // auto _frames = frames.unchecked<1>();
    // for (int i = 0; i < 10; i++)
    // {
    //     printf("%d\n", _frames[i]);
    // }

    return py::array_t<float>(10);
}

py::array_t<float> Multitau(py::array_t<short> frames, 
              py::list pixels, 
              py::list values,
              py::dict config) 
{
    auto _frames = frames.unchecked<1>();

    size_t return_dims = 3;
    size_t no_of_frames = config["frames"].cast<int>();
    size_t no_of_pixels = config["pixels"].cast<int>();
    size_t dpl = config["delays_per_level"].cast<int>();

    size_t w = config["rows"].cast<int>();
    size_t h = config["cols"].cast<int>();

    size_t max_level = CalculateLevelMax(no_of_frames, dpl);

    vector<tuple<int,int> > delays_per_level = DelayPerLevel(no_of_frames, dpl, max_level);

    // float **result = new float[3 * pixels * delays_per_level.size()]
    size_t g2_size = 3* no_of_pixels * delays_per_level.size();
    float* result = new float[g2_size];
    
    for (int i = 0; i < (3 * no_of_pixels * delays_per_level.size()); i++) {
        result[i] = 0.0f;
    }

    printf("pixels = %d\n", no_of_pixels);
    printf("width = %d\n", w);
    printf("height = %d\n", h);
    printf("max_level = %d\n", max_level);


    // for (int i = 0; i < no_of_frames; i++)
    // {
    //     printf("frame# = %d\n", _frames[i]);

    //     py::array_t<int> pi = py::cast<py::array>(pixels[i]);
    //     py::array_t<float> pv = py::cast<py::array>(values[i]);

    //     auto _pi =  pi.mutable_unchecked<1>();
    //     auto _pv = pv.mutable_unchecked<1>();

        
    //     for (int j = 0; j < no_of_pixels; j++) 
    //     {
    //         printf("%d - %f\n", _pi[j], _pv[j]);
    //     }

    // }


    // for (int i = 0; i < no_of_pixels; i++)
    // {
    //     py::array_t<int> tt = py::cast<py::array>(times[i]);
    //     py::array_t<float> vv = py::cast<py::array>(vals[i]);

    //     auto _times =  tt.mutable_unchecked<1>();
    //     auto _vals = vv.mutable_unchecked<1>();

    //     int last_level = 0;

    //     int last_frame = frames;
    //     int last_index = _pixels.shape(0);
    //     int tau_index = 0;
    //     int g2_index = 0;
    //     int ip_index = 0;
    //     int if_index = 0;

    //     for (auto it = delays_per_level.begin() ; it != delays_per_level.end(); ++it)
    //     {
    //         tuple<int, int> tau_level = *it;
    //         int level = get<0>(tau_level);
    //         int tau = get<1>(tau_level);   
        
    //         // Multitau averaging step
    //         if (last_level != level)
    //         {   
    //             // Handle odd event case.
    //             if (last_frame % 2)
    //                 last_frame -= 1;

    //             last_frame = last_frame / 2;

    //             int index = 0;
    //             int cnt = 0;
                    
    //             int i0, i1;
    //             i0 = _times(cnt) / 2.0;

    //             if (last_index > 0 && i0 < last_frame) 
    //             {
    //                 _times(index) = i0;
    //                 cnt = 1;
    //             }

    //             while (cnt < last_index) 
    //             {
    //                 i1 = _times(cnt) / 2.0;

    //                 if (i1 >= last_frame) break;

    //                 if (i1 == i0) 
    //                     _vals(index) += _vals(cnt);
    //                 else 
    //                 {
    //                     _times(++index) = i1;
    //                     _vals(index) = _vals(cnt);
    //                 }

    //                 i0 = i1;
    //                 cnt++;
    //             }

    //             last_index = index + 1;

    //             for (int k = 0 ; k < last_index; k++) 
    //                 _vals(k) /= 2.0f;
    //         }

    //         if (level > 0)
    //             tau = tau / pow(2, level);

    //         g2_index = tau_index * pixels + _pixels(i);
    //         ip_index =  tau_index * pixels + _pixels(i) + g2_size;
    //         if_index =  tau_index * pixels + _pixels(i) + (2 * g2_size);

    //         for (int r = 0; r < last_index; r++)
    //         {
    //             int src = _times(r);
    //             if (src < (last_frame - tau) ) {

    //                 result[ip_index] += _vals(r);
    //                 int low = 0;
    //                 for (; low < _times.shape(0); low++) 
    //                 {   
    //                     printf("low %d\n", low);
    //                     if (_times(low) >= (src + tau) )  break;
    //                 }
    //                 if (low != _times.shape(0)) 
    //                 {
    //                   int pos = low - _times(0);
    //                   if (pos < last_index && _times(pos) == (src+tau))
    //                     result[g2_index] += _vals(r) * _vals(pos);
    //                 }
                 
    //             }

    //             if (src >= tau && src < last_frame) {
    //               result[if_index] += _vals(r);
    //             }
    //         }

    //         if ( (last_frame - tau) > 0) {
    //             result[g2_index] /= (last_frame - tau);
    //             result[ip_index] /= (last_frame - tau);
    //             result[if_index] /= (last_frame - tau);
    //         }
            
    //         last_level = level;
    //         tau_index++;
    //     }
    // }

    // py::capsule free_when_done(result, [](void *f) {
    //     float *test = reinterpret_cast<float *>(f);
    //   x

    //  return py::array_t<double>(
    //     {a, b, c},
    //     // {100*100*8, 100*8, 8},
    //     result,
    //     free_when_done
    // );

    // return py::array_t<float>(
    //     {return_dims, pixels, delays_per_level.size()},
    //     {pixels*delays_per_level.size()*sizeof(float), delays_per_level.size()*sizeof(float), sizeof(float)},
    //     result,
    //     free_when_done
    // );

    return py::array_t<float>(10);
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


PYBIND11_MODULE(libpyxpcs, m) {
    m.doc() = R"pbdoc(
        XPCS Analysis Toolkit
        -----------------------

        .. currentmodule:: pyxpcs

        .. autosummary::
           :toctree: _generate

           multitau
    )pbdoc";

    m.def("multitau", &Multitau, "Multi-thread version of multitau");
    m.def("multitau2", &Multitau2, "Multi-thread version of multitau");
    m.def("callback", &callback, "");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}

