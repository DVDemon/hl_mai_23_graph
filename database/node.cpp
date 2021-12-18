#include "node.h"

#include "../neo4j/rest_client.h"

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
        std::string query;
        query = "CREATE (n:NODE {";
        query += "code:\"";
        query += _code;
        query += "\",name:\"";
        query += _name;
        query += "\",type:\"";
        query += _type;
        query += "\"})";

        neo4j::rest_request::query_nodes({query});
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