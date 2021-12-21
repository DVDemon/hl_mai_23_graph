#ifndef IMPORT_TASK_H
#define IMPORT_TASK_H

#include <vector>
#include <map>
#include <functional>
#include <queue>


namespace database
{
    class ImportTask
    {
    private:
        std::map<size_t, std::function<void(void)>> tasks;
        ImportTask();

    public:
        static ImportTask& get();
        size_t add(std::function<void(void)> f);
        bool is_runing(size_t id);
    };
}
#endif // !IMPORT_TASK_H
