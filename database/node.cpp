#include "node.h"

namespace database
{
    Node::Node(){

    }

    Node::Node(const std::string &code, const std::string &name, const std::string &type):
    _code(code),
    _name(name),
    _type(type)
    {
        
    }

    void Node::save()
    {
    }

    const std::string &Node::code() const
    {
        return _code;
    }

    const std::string &Node::name() const
    {
        return _name;
    }
    const std::string &Node::type() const
    {
        return _type;
    }
}