#ifndef H_NODE
#define H_NODE

#include <string>

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

        const std::string &code() const;
        const std::string &name() const;
        const std::string &type() const;
    };
}

#endif