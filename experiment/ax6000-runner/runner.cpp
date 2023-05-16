// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2018 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include <float.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

#ifdef _WIN32
#include <algorithm>
#include <windows.h> // Sleep()
#else
#include <unistd.h> // sleep()
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "benchmark.h"
#include "cpu.h"
#include "datareader.h"
#include "net.h"
#include "gpu.h"

class DataReaderFromEmpty : public ncnn::DataReader
{
public:
    virtual int scan(const char* format, void* p) const
    {
        return 0;
    }
    virtual size_t read(void* buf, size_t size) const
    {
        memset(buf, 0, size);
        return size;
    }
};

static int g_warmup_loop_count = 8;
static int g_loop_count = 4;
static bool g_enable_cooling_down = true;

static ncnn::UnlockedPoolAllocator g_blob_pool_allocator;
static ncnn::PoolAllocator g_workspace_pool_allocator;

#if NCNN_VULKAN
static ncnn::VulkanDevice* g_vkdev = 0;
static ncnn::VkAllocator* g_blob_vkallocator = 0;
static ncnn::VkAllocator* g_staging_vkallocator = 0;
#endif // NCNN_VULKAN

void benchmark(const char* comment, const char* path, const ncnn::Mat& _in, const ncnn::Option& opt, std::vector<double> &runTimes)
{
    ncnn::Mat in = _in;
    in.fill(0.01f);

    g_blob_pool_allocator.clear();
    g_workspace_pool_allocator.clear();

#if NCNN_VULKAN
    if (opt.use_vulkan_compute)
    {
        g_blob_vkallocator->clear();
        g_staging_vkallocator->clear();
    }
#endif // NCNN_VULKAN

    ncnn::Net net;

    net.opt = opt;

#if NCNN_VULKAN
    if (net.opt.use_vulkan_compute)
    {
        net.set_vulkan_device(g_vkdev);
    }
#endif // NCNN_VULKAN

#ifdef __EMSCRIPTEN__
#define MODEL_DIR "/working/"
#else
//#define MODEL_DIR ""
#define MODEL_DIR "/home/tuduweb/development/lightweight/ML-NAS/experiment/nas-evocnn-autodeploy/outputs/1678615837/ncnnModels/"
//#define MODEL_DIR "/home/pi/lightweight/ncnnModels/"
#endif

    if(comment == nullptr) {
        comment = basename((char*)path);
    }

    char parampath[256];
    if(path == nullptr) {
        sprintf(parampath, MODEL_DIR "%s", comment);
    }else{
        sprintf(parampath, "%s", path);
    }
    net.load_param(parampath);

    DataReaderFromEmpty dr;
    net.load_model(dr);

    const std::vector<const char*>& input_names = net.input_names();
    const std::vector<const char*>& output_names = net.output_names();

    if (g_enable_cooling_down)
    {
        // sleep 10 seconds for cooling down SOC  :(
#ifdef _WIN32
        Sleep(10 * 1000);
#elif defined(__unix__) || defined(__APPLE__)
        sleep(10);
#elif _POSIX_TIMERS
        struct timespec ts;
        ts.tv_sec = 10;
        ts.tv_nsec = 0;
        nanosleep(&ts, &ts);
#else
        // TODO How to handle it ?
#endif
    }

    ncnn::Mat out;

    // warm up
    for (int i = 0; i < g_warmup_loop_count; i++)
    {
        ncnn::Extractor ex = net.create_extractor();
        ex.input(input_names[0], in);
        ex.extract(output_names[0], out);
    }

    double time_min = DBL_MAX;
    double time_max = -DBL_MAX;
    double time_avg = 0;

    // fprintf(stderr, "\n%20s  min = %7.3f  max = %7.3f  avg = %7.3f\n", comment, time_min, time_max, time_avg);
    fprintf(stderr, "%20s\ntimes: ", comment);

    for (int i = 0; i < g_loop_count; i++)
    {
        double start = ncnn::get_current_time();

        ncnn::Extractor ex = net.create_extractor();
        ex.input(input_names[0], in);
        ex.extract(output_names[0], out);

        double end = ncnn::get_current_time();

        double time = end - start;
        fprintf(stderr, "%.5lf ", time);
    
        time_min = std::min(time_min, time);
        time_max = std::max(time_max, time);
        time_avg += time;

        runTimes.push_back(time);
    }

    time_avg /= g_loop_count;

    // fprintf(stderr, "\n%20s  min = %7.3f  max = %7.3f  avg = %7.3f\n", comment, time_min, time_max, time_avg);
}


#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstddef>
#include <string.h>

void getFiles(const std::string path, std::vector<std::string> &files)
{
  DIR *dir;
  struct dirent *ptr;

  if ((dir = opendir(path.c_str())) == NULL)
  {
    perror("Open dir error...");
    return;
  }

  while ((ptr = readdir(dir)) != NULL)
  {
    if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
      continue;
    } else if (ptr->d_type == 8) {
      files.push_back(ptr->d_name);
      //printf("file name is %s\n", ptr->d_name);
    } else if (ptr->d_type == 10) {
      continue;
    } else if (ptr->d_type == 4) {
      //files.push_back(ptr->d_name);
      getFiles(path + ptr->d_name + "/", files);
    }
  }
  closedir(dir);
}


/**
 * 判断是否是一个文件
 */
static bool is_file(std::string filename) {
    struct stat   buffer;
    return (stat (filename.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

/**
 * 判断是否是一个文件夹,
 * */
static bool is_dir(std::string filefodler) {
    struct stat   buffer;
    return (stat (filefodler.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}


int main(int argc, char** argv)
{
    int loop_count = 200; //4; // 4
    int num_threads = 1; // ncnn::get_physical_big_cpu_count();
    int powersave = 0; // 2
    int gpu_device = -1;
    int cooling_down = 0; //1

    std::string path;

    //std::string path = "/home/tuduweb/development/lightweight/ML-NAS/experiment/nas-evocnn-autodeploy/outputs/1678615837/ncnnTestModel";
    //std::string path = "/home/tuduweb/development/lightweight/ML-NAS/experiment/nas-evocnn-autodeploy/outputs/1678615837/ncnnModels";

    if(argc >= 2) {
        path = argv[1];
    }else{
        //path = "/home/tuduweb/development/lightweight/ML-NAS/experiment/nas-evocnn-autodeploy/outputs/1678615837/ncnnTestModel/indi00007_00018.ncnn.param";
        path = "/home/tuduweb/development/lightweight/ML-NAS/experiment/nas-evocnn-autodeploy/outputs/1678615837/ncnnModels";

        printf("demo path %s\n", path.c_str());
    }

    bool use_vulkan_compute = gpu_device != -1;

    g_enable_cooling_down = cooling_down != 0;

    g_loop_count = loop_count;

    g_blob_pool_allocator.set_size_compare_ratio(0.f);
    g_workspace_pool_allocator.set_size_compare_ratio(0.f);

#if NCNN_VULKAN
    if (use_vulkan_compute)
    {
        g_warmup_loop_count = 10;

        g_vkdev = ncnn::get_gpu_device(gpu_device);

        g_blob_vkallocator = new ncnn::VkBlobAllocator(g_vkdev);
        g_staging_vkallocator = new ncnn::VkStagingAllocator(g_vkdev);
    }
#endif // NCNN_VULKAN

    // default option
    ncnn::Option opt;
    opt.lightmode = false; // true
    opt.num_threads = num_threads;
    opt.blob_allocator = &g_blob_pool_allocator;
    opt.workspace_allocator = &g_workspace_pool_allocator;
#if NCNN_VULKAN
    opt.blob_vkallocator = g_blob_vkallocator;
    opt.workspace_vkallocator = g_blob_vkallocator;
    opt.staging_vkallocator = g_staging_vkallocator;
#endif // NCNN_VULKAN
    opt.use_winograd_convolution = true; //true;
    opt.use_sgemm_convolution = true; // true
    opt.use_int8_inference = true;
    opt.use_vulkan_compute = use_vulkan_compute;
    opt.use_fp16_packed = true; // true
    opt.use_fp16_storage = true;
    opt.use_fp16_arithmetic = true;
    opt.use_int8_storage = true;
    opt.use_int8_arithmetic = true;
    opt.use_packing_layout = true;
    opt.use_shader_pack8 = false;
    opt.use_image_storage = false;

    ncnn::set_cpu_powersave(powersave);

    ncnn::set_omp_dynamic(0);
    ncnn::set_omp_num_threads(num_threads);

    fprintf(stderr, "loop_count = %d\n", g_loop_count);
    fprintf(stderr, "num_threads = %d\n", num_threads);
    fprintf(stderr, "powersave = %d\n", ncnn::get_cpu_powersave());
    fprintf(stderr, "gpu_device = %d\n", gpu_device);
    fprintf(stderr, "cooling_down = %d\n", (int)g_enable_cooling_down);

    //std::vector<double> runTimes(loop_count);
    std::vector<double> runTimes;

    // 从目录执行 或者从文件指定执行?
    printf("!@#$beginbenchmark$#@!\n");
    if(is_dir(path)) {

        std::vector<std::string> files;
        getFiles(path, files); 

        for (int i=0; i<files.size(); i++) {

            // end with

            std::string f_name = files[i];
            //printf("%d %s\n", i, f_name.c_str());

            std::string fileUri = path + "/" + f_name;
            //printf("%d %s\n", i, fileUri.c_str());

            benchmark(f_name.c_str(), nullptr, ncnn::Mat(1, 32, 32), opt, runTimes);
            // try {
            // // protected code
            // benchmark(f_name.c_str(), nullptr, ncnn::Mat(1, 32, 32), opt);
            // } catch(...) {
            // // code to handle any exception
            // printf("fault\n");
            // }

            // benchmark(f_name.c_str(), nullptr, ncnn::Mat(1, 32, 32), opt);
#if 0
            printf("times:");
            for(int j = 0; j < runTimes.size(); ++j) {
                printf(" %1.3f", runTimes[j]);
            }
            printf("\n");
#endif
            runTimes.clear();
        }

    }else if(is_file(path)) {
        benchmark(nullptr, path.c_str(), ncnn::Mat(1, 32, 32), opt, runTimes);
    }

    printf("\n!@#$endbenchmark$#@!\n");


    // run
    //benchmark("model.ncnn", ncnn::Mat(1, 32, 32), opt);


#if NCNN_VULKAN
    delete g_blob_vkallocator;
    delete g_staging_vkallocator;
#endif // NCNN_VULKAN

    return 0;
}

