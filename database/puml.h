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
            std::string generate_puml(std::vector<Node> & nodes,
                                        std::vector<Link> & links);

    };
}

#endif // !H_PUML
