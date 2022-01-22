#ifndef H_NODE
#define H_NODE

#include <string>
#include "Poco/JSON/Object.h"
#include <map>

namespace database
{
    class Node
    {
    private:
        std::string _label;
        std::map<std::string,std::string> _map;

    public:
        Node();
        
        void save() const;

        Poco::JSON::Object::Ptr toJSON() const;

        static Node              load(const std::string& code);
        static std::vector<std::string> labels();
        static std::vector<Node> search(const std::string& pattern);
        static std::vector<Node> by_label(const std::string& type);
        static std::vector<Node> load_friends(const std::string& code);

        std::string                         &label();
        std::map<std::string,std::string>   &get();
        const std::map<std::string,std::string>   &get() const;
    };
}

#endif