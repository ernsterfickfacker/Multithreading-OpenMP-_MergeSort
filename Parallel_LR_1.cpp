// Parallel_LR_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#include <iostream>
#include <algorithm>
#include <iterator>
#include <omp.h>

#include <vector>
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <chrono>



using namespace std;

const int n_threads = 8;

template <typename Iterator>
void mergesort(Iterator from, Iterator to)
{
#pragma omp parallel num_threads(n_threads)
    {
#pragma omp single nowait
        static_assert(!std::is_same<typename std::iterator_traits<Iterator>::value_type, void>::value);
        auto n = std::distance(from, to);

        if (1 < n)
        {
#pragma omp task firstprivate (from, to, n)
            {
                Iterator l_from = from;
                Iterator l_to = l_from;
                std::advance(l_to, n / 2);
                mergesort(l_from, l_to);
            }
#pragma omp task firstprivate (from, to, n)
            {
                Iterator r_from = from;
                std::advance(r_from, n / 2);
                Iterator r_to = r_from;
                std::advance(r_to, n - (n / 2));
                mergesort(r_from, r_to);
            }
#pragma omp taskwait

            auto tmp_array = std::make_unique<typename Iterator::value_type[]>(n);
            Iterator l_iter = from;
            Iterator l_end = l_iter;
            std::advance(l_end, n / 2);
            Iterator r_iter = l_end;
            Iterator& r_end = to;

            auto tmp_iter = tmp_array.get();

            while (l_iter != l_end || r_iter != r_end)
            {
                if (*l_iter < *r_iter)
                {
                    *tmp_iter = std::move(*l_iter);
                    ++l_iter;
                    ++tmp_iter;
                }
                else
                {
                    *tmp_iter = std::move(*r_iter);
                    ++r_iter;
                    ++tmp_iter;
                }

                if (l_iter == l_end)
                {
                    std::copy(
                        std::make_move_iterator(r_iter),
                        std::make_move_iterator(r_end),
                        tmp_iter
                    );

                    break;
                }

                if (r_iter == r_end)
                {
                    std::copy(
                        std::make_move_iterator(l_iter),
                        std::make_move_iterator(l_end),
                        tmp_iter
                    );

                    break;
                }
            }

            std::copy(
                std::make_move_iterator(tmp_array.get()),
                std::make_move_iterator(&tmp_array[n]),
                from
            );
        }
    }
}

int main()
{
    std::cout << "Abrosimova Maria. LR-1 \n";
    //std::cout << "Original array: \n";
    std::vector<int> v;

    fstream myfile;

    // open file
    myfile.open("t2_input.txt");
    vector<string> s;

    if (myfile.is_open()){
        string str;
        while (getline(myfile, str, ',')) {
             s.push_back(str);
        }
        // close the file object.
        myfile.close();
    }
   
    for (auto i : s)
        v.push_back(atoi(i.c_str()));

   /* for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }*/

    std::vector<int>::iterator it_beg = v.begin(); 
    std::vector<int>::iterator it_end = v.end();
    clock_t tStart = clock();
    auto start_time = std::chrono::high_resolution_clock::now();
    mergesort(it_beg, it_end);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto int_s = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    printf("\nTime taken: %d s\n", int_s);

    std::ofstream outFile("t2_output.txt");
    copy(v.begin(), std::prev(v.end()), ostream_iterator<float>(outFile, ","));
    if (!v.empty()) {
        outFile << v.back();
    }
   /* std::cout << "\nArray after sorting: \n";
    for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }*/
}
