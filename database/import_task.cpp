#include "import_task.h"
#include <thread>
#include <chrono>
#include <iostream>

#include <mutex>


using namespace std::chrono_literals;
namespace database
{
    std::mutex task_mtx;
    ImportTask::ImportTask()
    {
        static std::thread th([]()
        {
            std::cout << "background worker started ..." << std::endl;
            while(true){
                //std::cout << "[" << std::flush;
                task_mtx.lock();
                //std::cout << "]" << std::flush;
                if(!ImportTask::get().tasks.empty()){
                    
                    auto [id,task] = *std::begin(ImportTask::get().tasks);
                    task_mtx.unlock();
                    std::cout << "start task:" << id << std::endl;
                    try{
                        task();
                        
                    } catch(std::exception ex){
                        std::cerr << "Import error:" << ex.what();
                    }
                    task_mtx.lock();
                    ImportTask::get().tasks.erase(ImportTask::get().tasks.find(id));
                    task_mtx.unlock();
                } else task_mtx.unlock();
                std::this_thread::sleep_for(500ms);
            } 
            
            std::cout << "background worker stopes ..." << std::endl;
            });
       //th.detach();
    }

    ImportTask& ImportTask::get()
    {
        static ImportTask task;
        return task;
    }

    size_t ImportTask::add([[maybe_unused]] std::function<void(void)> f)
    {
        static size_t task_id{1};
        std::lock_guard<std::mutex> lck(task_mtx);
        tasks[task_id] = f;
        std::cout << "task added:" << tasks.size() << std::endl;
        return task_id++;
    }

    bool ImportTask::is_runing([[maybe_unused]] size_t id)
    {
        if (tasks.find(id) != std::end(tasks))
            return true;
        return false;
    }
}