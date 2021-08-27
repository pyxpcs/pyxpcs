#include <iostream>
#include <map>
#include <math.h>
#include <vector>
#include <tuple>
#include <omp.h>
#include <chrono>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "row.h"
#include "sparse_data.h"

namespace py = pybind11;

using std::cout;
using std::endl;
using std::vector;
using std::tuple;
using std::get;

PYBIND11_MAKE_OPAQUE(std::vector<int>);
PYBIND11_MAKE_OPAQUE(std::vector<float>);

vector<tuple<int, int>> DelaysPerLevel(int frame_count, int dpl, int max_delay)
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

// py::array_t<float> Multitau(py::array_t<int> valid_pixels, py::list rows, py::list values, py::dict config) 
// {

//     py::gil_scoped_acquire acquire;

//     size_t no_of_frames = config["frames"].cast<int>();
//     size_t no_of_pixels = config["pixels"].cast<int>();
//     size_t dpl = config["delays_per_level"].cast<int>();

//     size_t w = config["rows"].cast<int>();
//     size_t h = config["cols"].cast<int>();

//     size_t max_level = CalculateLevelMax(no_of_frames, dpl);

//     vector<tuple<int,int> > delays_per_level = DelayPerLevel(no_of_frames, dpl, max_level);

//     // float **result = new float[3 * pixels * delays_per_level.size()]
//     size_t g2_size = no_of_pixels * delays_per_level.size();
    
//     float* result = new float[3 * g2_size];

//     for (int i = 0; i < (3 * g2_size); i++) {
//         result[i] = 0.0f;
//     }

//     auto _valid_pixels = valid_pixels.mutable_unchecked<1>();
//     //default(none) schedule(dynamic, 20) shared(_valid_pixels, delays_per_level, no_of_frames,g2_size, no_of_pixels, rows, values, result)
//     // #pragma omp parallel for
//     for (int i = 0; i < _valid_pixels.size(); i++)
//     {   
//         int pixno = _valid_pixels(i);
        

//         py::array_t<int> tt = py::cast<py::array>(rows[pixno]);
//         py::array_t<int> vv = py::cast<py::array>(values[pixno]);

//         auto _times =  tt.mutable_unchecked<1>();
//         auto _vals = vv.mutable_unchecked<1>();

//         int last_level = 0;

//         int last_frame = no_of_frames;
//         int last_index = _times.size();

//         int tau_index = 0;
//         int g2_index = 0;
//         int ip_index = 0;
//         int if_index = 0;

//         for (auto it = delays_per_level.begin() ; it != delays_per_level.end(); ++it)
//         {
//             tuple<int, int> tau_level = *it;
//             int level = get<0>(tau_level);
//             int tau = get<1>(tau_level);   

//             // printf("level = %d, tau = %d\n", level, tau);

//             // Multitau averaging step
//             if (last_level != level)
//             {   
//                 // Handle odd event case.
//                 if (last_frame % 2)
//                     last_frame -= 1;

//                 last_frame = last_frame / 2;
//                 // printf("Last_frame %d\n", last_frame);

//                 int index = 0;
//                 int cnt = 0;
                    
//                 int i0, i1;
//                 i0 = _times(cnt) / 2.0;

//                 if (last_index > 0 && i0 < last_frame) 
//                 {
//                     _times(index) = i0;
//                     cnt = 1;
//                 }

//                 while (cnt < last_index) 
//                 {
//                     i1 = _times(cnt) / 2.0;

//                     if (i1 >= last_frame) break;

//                     if (i1 == i0) 
//                         _vals(index) += _vals(cnt);
//                     else 
//                     {
//                         _times(++index) = i1;
//                         _vals(index) = _vals(cnt);
//                     }

//                     i0 = i1;
//                     cnt++;
//                 }

//                 last_index = index + 1;

//                 for (int k = 0 ; k < last_index; k++) 
//                     _vals(k) /= 2.0f;
//             }

//             if (level > 0)
//                 tau = tau / pow(2, level);

//             g2_index =  tau_index * no_of_pixels + pixno;
//             ip_index =  (tau_index * no_of_pixels + pixno) +  g2_size;
//             if_index =  (tau_index * no_of_pixels + pixno) + (2 * g2_size);

//             // printf("Running to last_index of %d\n", last_index);
//             for (int r = 0; r < last_index; r++)
//             {
                
//                 int src = _times(r);

//                 if (src < (last_frame - tau) ) {

//                     result[ip_index] += _vals(r);
                    
//                     int low = 0;
//                     for (; low < _times.size(); low++) 
//                     {   
//                         if (_times(low) >= (src + tau) )  break;
//                     }

//                     if (low != _times.size()) 
//                     {
//                       int pos = low - _times(0);
//                       if (pos < last_index && _times(pos) == (src+tau))
//                         result[g2_index] += _vals(r) * _vals(pos);
//                     }
                 
//                 }

//                 if (src >= tau && src < last_frame) {
//                   result[if_index] += _vals(r);
//                 }
//             }

//             if ( (last_frame - tau) > 0) {
//                 result[g2_index] /= (last_frame - tau);
//                 result[ip_index] /= (last_frame - tau);
//                 result[if_index] /= (last_frame - tau);
//             }
            
//             last_level = level;
//             tau_index++;
//         }
//     }

//     py::capsule free_when_done(result, [](void *f) {
//         float *test = reinterpret_cast<float *>(f);
//     });

//     size_t return_dims = 3;
//     return py::array_t<float>(
//         {return_dims, delays_per_level.size(), no_of_pixels},
//         {delays_per_level.size()*no_of_pixels*sizeof(float), no_of_pixels*sizeof(float), sizeof(float)},
//         result,
//         free_when_done
//     );
// }

py::array_t<float> Multitau(SparseData* data, py::dict config)
{
    py::gil_scoped_acquire acquire;

    // const auto t0(std::chrono::steady_clock::now());
    
    size_t no_of_frames = config["frames"].cast<int>();
    size_t no_of_pixels = config["pixels"].cast<int>();
    size_t dpl = config["delays_per_level"].cast<int>();

    size_t w = config["rows"].cast<int>();
    size_t h = config["cols"].cast<int>();

    size_t max_level = CalculateLevelMax(no_of_frames, dpl);
   
    vector<int> validPixels = data->ValidPixels();
    vector<tuple<int,int> > delays_per_level = DelaysPerLevel(no_of_frames, dpl, max_level);

    // float *G2s = new float[no_of_pixels * delays_per_level.size()];
    // float *IPs = new float[no_of_pixels * delays_per_level.size()];
    // float *IFs = new float[no_of_pixels * delays_per_level.size()];
    size_t g2_size = no_of_pixels * delays_per_level.size();
    float* result = new float[3 * g2_size];
    for (int i = 0; i < (3 * g2_size); i++) {
        result[i] = 0.0f;
    }

    // long threads[12];
    // for (int i = 0; i < 12; i++)
    //     threads[i] = 0;

    #pragma omp parallel for default(none) schedule(dynamic, 20) shared(validPixels, delays_per_level, no_of_frames, no_of_pixels, g2_size, result, data)
    for (int i = 0; i < validPixels.size(); i++)
    {
        const auto t0(std::chrono::steady_clock::now());

        std::shared_ptr<Row> row = data->Pixel(validPixels.at(i));

        int ll = 0;

        int lastframe = no_of_frames;
        int lastIndex = row->indxPtr.size();
        int tauIndex = 0;
        int g2Index = 0;
        int ipIndex = 0;
        int ifIndex = 0;

        for (vector<tuple<int, int> >::iterator it = delays_per_level.begin() ;
                it != delays_per_level.end(); ++it)
        {
            // View of pixel data at two different tau values. 
            tuple<int, int> tau_level = *it;
            int level = get<0>(tau_level);
            int tau = get<1>(tau_level);
            
            if (ll != level)
            {   
                if (lastframe % 2)
                    lastframe -= 1;

                lastframe = lastframe / 2;

                int index = 0;
                int cnt = 0;
                
                int i0, i1;
                i0 = row->indxPtr[cnt] / 2.0;

                if (lastIndex > 0 && i0 < lastframe) {
                    row->indxPtr[index] = i0;
                    cnt = 1;
                }

                while (cnt < lastIndex) 
                {
                    i1 = row->indxPtr[cnt] / 2.0;
                    if (i1 >= lastframe) break;

                    if (i1 == i0) 
                    {
                        row->valPtr[index] += row->valPtr[cnt]; 
                    }
                    else 
                    {
                        row->indxPtr[++index] = i1;
                        row->valPtr[index] = row->valPtr[cnt];
                    }

                    i0 = i1;
                    cnt++;
                }

                lastIndex = index+1;

                for (int i = 0 ; i < lastIndex; i++) 
                    row->valPtr[i] /= 2.0f;
            }

            if (level > 0)
                tau = tau / pow(2, level);

            g2Index = tauIndex * no_of_pixels + validPixels[i]; 
            ipIndex = (tauIndex * no_of_pixels + validPixels[i]) +  g2_size;
            ifIndex = (tauIndex * no_of_pixels + validPixels[i]) + (2 * g2_size);

            for (int r = 0; r < lastIndex; r++)
            {
                int src = row->indxPtr[r];
                // int dst = src;
                
                if (src < (lastframe-tau)) {

                    result[ipIndex] += row->valPtr[r];
                    // int limit = min(lastIndex, src+tau+1);
                    auto lower = lower_bound(row->indxPtr.begin(), row->indxPtr.end(), src+tau); 
                    if (lower != row->indxPtr.end()) {
                      int pos = lower - row->indxPtr.begin();
                      if (pos < lastIndex && row->indxPtr[pos] == (src+tau))
                        result[g2Index] += row->valPtr[r] * row->valPtr[pos];
                    }
                   
                }

                if (src >= tau && src < lastframe) {
                  result[ifIndex] += row->valPtr.at(r);
                }
            }

            if ( (lastframe - tau) > 0) {
                result[g2Index] /= (lastframe-tau);
                result[ipIndex] /= (lastframe-tau);
                result[ifIndex] /= (lastframe-tau);
            }
            
            ll = level;
            tauIndex++;
        }
        // break;

        int tid = omp_get_thread_num();
        
        const auto t1(std::chrono::steady_clock::now());
        const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>( t1 - t0).count();
        // threads[tid] += duration_ms;
        // printf("Time for %d, %f\n", tid, duration_ms/1000);
    }

    // for (int i =0 ; i < 12; i++) {
    //     printf("Time for tid %d is %f\n", i, threads[i]/1000);
    // }

    // const auto t1(std::chrono::steady_clock::now());
    // const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>( t1 - t0).count();        
    // std::cout<<"Time "<<std::endl;
    // std::cout<<duration_ms/1000<<std::endl;
    // printf("Done\n");

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


std::shared_ptr<SparseData> SparseLIL(py::list indices, py::list values, const int& no_of_pixels, py::array mask)
{
    std::shared_ptr<SparseData> data = std::shared_ptr<SparseData>(new SparseData(no_of_pixels, 10));
    py::array_t<int> mask_ = py::cast<py::array>(mask);
    auto mask__ = mask_.unchecked<1>();

    int fno = 0;
    for (int i = 0; i < indices.size(); i++) {
        py::array_t<int> indices_ = py::cast<py::array>(indices[i]);
        py::array_t<float> values_ = py::cast<py::array>(values[i]);

        auto indices__ = indices_.unchecked<1>();
        auto values__ = values_.unchecked<1>();

        for (int j = 0; j < indices__.shape(0); j++) {
            
            if (mask__[indices__[j]] <= 0) {
                continue;
            }

            std::shared_ptr<Row> row = data->Pixel(indices__[j]);
            row->indxPtr.push_back(fno);
            row->valPtr.push_back(values__[j]);
        }
        fno++;
    }

    return data;
}

std::shared_ptr<SparseData> SparseLIL2(py::array indices, py::array frames, py::array values, const int& no_of_pixels)
{
    std::shared_ptr<SparseData> data = std::shared_ptr<SparseData>(new SparseData(no_of_pixels, 10));

    int fno = 0;
    py::array_t<int> indices_ = py::cast<py::array>(indices);
    py::array_t<int> frames_ = py::cast<py::array>(frames);
    py::array_t<int> values_ = py::cast<py::array>(values);

    auto indices__ = indices_.unchecked<1>();
    auto frames__ = frames_.unchecked<1>();
    auto values__ = values_.unchecked<1>();

    for (int i = 0; i < indices__.shape(0); i++) {
        std::shared_ptr<Row> row = data->Pixel(indices__[i]); 
        row->indxPtr.push_back(frames__[i]);
        row->valPtr.push_back(values__[i]);
    }

    return data;
}


std::shared_ptr<SparseData> SparseLIL3(py::array_t<unsigned short int> values)
{

    py::buffer_info buf1 = values.request();

    unsigned short int *ptr1 = (unsigned short int *) buf1.ptr;
    int frames = buf1.shape[0];
    int rows = buf1.shape[1];
    int cols = buf1.shape[2];

    printf("%d, %d, %d\n", frames, rows, cols);

    std::shared_ptr<SparseData> data = std::shared_ptr<SparseData>(new SparseData(rows*cols, 1000));


    for (long idx = 0; idx < frames; idx++) {
        for (long idy = 0; idy < rows; idy++) {
            for(long idz = 0; idz < cols; idz++) {


                int val = ptr1[idx*(rows*cols) + idy*cols + idz];
                if (val <= 0)
                    continue;
                
                // printf("pixel # %d\n", (idy*cols + idz));

                std::shared_ptr<Row> row = data->Pixel(idy*cols + idz); 
                row->indxPtr.push_back(idx);
                row->valPtr.push_back(val);
            }
        }
    }

    return data;
  
}

void test(std::shared_ptr<SparseData> data) {
    std::shared_ptr<Row> row = data->Pixel(0);

    for (auto entry: row->indxPtr) {
        std::cout << entry <<std::endl;
    }

    for (auto entry: row->valPtr) {
        std::cout << entry <<std::endl;
    }
}

py::array_t<double> add_arrays(py::array_t<double> input1, py::array_t<double> input2) {
  py::buffer_info buf1 = input1.request();
  py::buffer_info buf2 = input2.request();

  if (buf1.size != buf2.size) {
    throw std::runtime_error("Input shapes must match");
  }

  /*  allocate the buffer */
  py::array_t<double> result = py::array_t<double>(buf1.size);

  py::buffer_info buf3 = result.request();

  double *ptr1 = (double *) buf1.ptr,
         *ptr2 = (double *) buf2.ptr,
         *ptr3 = (double *) buf3.ptr;
  int X = buf1.shape[0];
  int Y = buf1.shape[1];
  int Z = buf1.shape[2];

  for (size_t idx = 0; idx < X; idx++) {
    for (size_t idy = 0; idy < Y; idy++) {
        for(size_t idz = 0; idz < Z; idz++) {
            ptr3[idx*Y + idy*Z + idz] = ptr1[idx*Y + idy*Z + idz] + ptr2[idx*Y + idy*Z + idz];
        }
    }
  }
 
  // reshape array to match input shape
  result.resize({X,Y,Z});

  return result;
}


PYBIND11_MODULE(libpyxpcs, m) {
    m.doc() = R"pbdoc(
        XPCS Analysis Toolkit
        -----------------------
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

    py::class_<Row, std::shared_ptr<Row>>(m, "Row")
        .def(py::init<>())
        .def("push", &Row::Push)
        .def_readwrite("indxPtr", &Row::indxPtr)
        .def_readwrite("valPtr", &Row::valPtr);

    py::class_<SparseData, std::shared_ptr<SparseData>>(m, "SparseData")
        .def(py::init<const int&, const int&>())
        // .def("Pixel", (std::shared_ptr<Row>  &(SparseData::*)(const int&)) &SparseData::Pixel);
        .def("Pixel", &SparseData::Pixel)
        .def("ValidPixels", &SparseData::ValidPixels);

    // m.def("multitau", &Multitau, "Multi-thread version of multitau");
    m.def("test", &test);
    
    m.def("multitau",[] (SparseData* data, py::dict config) {
        py::gil_scoped_release release;
        return Multitau(data, config);
    });

    // m.def("twice", [](py::array_t<int> valid_pixels, py::list rows) {
    //   /* Release GIL before calling into C++ code */
    //   py::gil_scoped_release release;
    //   return twice(valid_pixels, rows);
    // });

    // m.def("multitau_test", &Multitau_2, "Multi-thread version of multitau");
    m.def("sparse_lil_index_value", &SparseLIL, "");
    m.def("sparse_lil_index_value_count", &SparseLIL2, "");
    m.def("sparse_lil_value", &SparseLIL3, "");
    m.def("add_arrays", &add_arrays, "Add two NumPy arrays");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}

