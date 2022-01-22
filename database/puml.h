#ifndef H_PUML
#define H_PUML

#include "node.h"
#include "link.h"
#include <string>
#include <map>
#include <future>

namespace database
{
    class Puml
    {
        private:

            std::map<std::string,std::future<void>> _futures;
            Puml();

        public:
            static Puml& get();
            void wait_for(const std::string & key);
            std::string generate_puml(const std::vector<Node> & nodes,
                                        const std::vector<Link> & links);

    };
}

#endif // !H_PUML
