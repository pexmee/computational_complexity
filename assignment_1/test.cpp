#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>



class thread_obj{

    public:
    void operator()(size_t &start, size_t &end, int &res)
    {
        std::cout << std::this_thread::get_id() << " start: " << start << " end: " << end << std::endl;
    }
};

int main(){
    size_t size = 16007;
    size_t num_threads = 20;
    size_t length = size / num_threads;
    size_t remainder = size % num_threads;
    size_t chunk_a = length;
    size_t chunk_b = length+1;
    std::vector<std::thread> threads = {};
    size_t a_start = 0, b_start = 0;
    size_t a_stop = 0, b_stop = 0;
    int res = 0;
    size_t latest = 0;
    std::cout << "REM" << remainder << std::endl;
    std::vector<std::pair<size_t,size_t>> vals = {};

    for (size_t i = 0; i < num_threads; i++){
        if(i < num_threads - remainder){
            a_start = ((i*chunk_a));
            a_stop = (chunk_a*(i+1))-1; // -1 due to indexing.
            vals.push_back(std::pair<size_t,size_t>(a_start,a_stop));

        }
        size_t tmp_a = a_start, tmp_b = b_start;
        if(remainder && i >= num_threads-remainder){
            b_start = a_stop + (i%(num_threads-remainder))*(chunk_a+1)+1;
            b_stop = a_stop + ((i%(num_threads-remainder))+1)*(chunk_a+1);
            vals.push_back(std::pair<size_t,size_t>(b_start,b_stop));
            
        }

    }
    for(size_t i = 0; i < vals.size(); i++){
        int a;
        std::thread th(thread_obj(),std::ref(vals[i].first), std::ref(vals[i].second),std::ref(a));
        threads.push_back(std::move(th));
    }
    for(size_t i = 0; i < threads.size(); i++){
        threads[i].join();
    }
    int smallest_nodes[20];
    smallest_nodes[1] = 2000;
    smallest_nodes[2] = 5;
    std::cout << "smallest_nodes size: " << smallest_nodes[2] << " wut: " << smallest_nodes[1] << std::endl;

    // for (size_t i = 0; i < remainder; i++){ // loop won't run if there's no reminder anyway.
    //     size_t arg_b1 = a_stop + i*chunk_b + 1;
    //     size_t arg_b2 = a_stop + (chunk_b*(i+1));
    //     std::cout << "b1,b2" << " " << arg_b1 << " " << arg_b2 << std::endl;

    //     // std::thread th(thread_obj(), std::ref(arg_b1), std::ref(arg_b2), std::ref(res)); // +1 since we start at 0
    //     // threads.push_back(std::move(th));
    // }
    
    // for (size_t i = 0; i < threads.size(); i++){
    //     threads[i].join();
    // }
    return EXIT_SUCCESS;
}
