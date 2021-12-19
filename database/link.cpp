#include "link.h"
#include "../neo4j/rest_client.h"

namespace database
{
    Link::Link() : _percent(0)
    {
    }

    Link::Link(const std::string &name, int percent,const std::string& source_node_code,const std::string& target_node_code): 
        _name (name), 
        _percent(percent),
        _source_node_code(source_node_code),
        _target_node_code(target_node_code)
    {
    }

    Poco::JSON::Object::Ptr Link::toJSON() const{
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("name", _name);
        root->set("percent", _percent);
        root->set("source_node_code", _source_node_code);
        root->set("target_node_code", _target_node_code);

        return root;
    }

    void Link::save()
    {
        std::string query;
        query = "MATCH (a:NODE {";
        query += "code:\"";
        query += _source_node_code;
        query += "\"}),(b:NODE{code:\"";
        query += _target_node_code;
        query += "\"}) CREATE (a)-[:LINK {name:\"";
        query += _name;
        query += "\"}]->(b)";

        neo4j::rest_request::query_nodes({query});
    }

    const std::string & Link::source_node_code() const{
        return _source_node_code;
    }

    const std::string & Link::target_node_code() const{
        return _target_node_code;
    }
    const std::string &Link::name() const
    {
        return _name;
    }

    int Link::percent() const
    {
        return _percent;
    }
}