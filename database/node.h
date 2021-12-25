#ifndef H_NODE
#define H_NODE

#include <string>
#include "Poco/JSON/Object.h"

namespace database
{
    class Node
    {
    private:
        std::string _code;
        std::string _name;
        std::string _type;

    public:
        Node();
        Node(const std::string &code, const std::string &name, const std::string &type);

        void save();

        Poco::JSON::Object::Ptr toJSON() const;

        static Node              load(const std::string& code);
        static std::vector<std::string> types();
        static std::vector<Node> search(const std::string& pattern);
        static std::vector<Node> by_type(const std::string& type);
        static std::vector<Node> load_friends(const std::string& code);

        const std::string &code() const;
        const std::string &name() const;
        const std::string &type() const;
    };
}

#endif