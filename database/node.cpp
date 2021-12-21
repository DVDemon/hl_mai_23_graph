#include "node.h"

#include "../neo4j/rest_client.h"

namespace database
{
    Node::Node()
    {
    }

    Node::Node(const std::string &code, const std::string &name, const std::string &type) : _code(code),
                                                                                            _name(name),
                                                                                            _type(type)
    {
    }

    Poco::JSON::Object::Ptr Node::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("code", _code);
        root->set("name", _name);
        root->set("type", _type);

        return root;
    }

    Node Node::load(const std::string &code)
    {
        try
        {

            Node a;
            std::string query;
            query = "MATCH (a {";
            query += "code:\"";
            query += code;
            query += "\"}) RETURN a";

            auto res = neo4j::rest_request::query_nodes({query});
            if (res.size() > 0)
            {
                std::vector<std::pair<std::string, std::string>> arr = res[0];
                for (auto p : arr)
                {
                    if (p.first == "code")
                        a._code = p.second;
                    if (p.first == "name")
                        a._name = p.second;
                    if (p.first == "type")
                        a._type = p.second;
                }
            }
            return a;
        }
        catch (std::exception &ex)
        {
            std::cout << "connection:" << ex.what() << std::endl;
            throw;
        }
    }

    std::vector<Node> Node::load_friends(const std::string &code)
    {
        try
        {

            std::vector<Node> result;
            std::string query;
            query = "MATCH (a {";
            query += "code:\"";
            query += code;
            query += "\"}) RETURN a";

            auto res = neo4j::rest_request::query_nodes({query});
            for(auto& r:res)
            {
                Node a;
                for (auto p : r)
                {
                    if (p.first == "code")
                        a._code = p.second;
                    if (p.first == "name")
                        a._name = p.second;
                    if (p.first == "type")
                        a._type = p.second;
                }
                result.push_back(a);
            }
            return result;
        }
        catch (std::exception &ex)
        {
            std::cout << "connection:" << ex.what() << std::endl;
            throw;
        }
    }

    void Node::save()
    {
        std::string query;
        query = "CREATE (n:";
        query += _type;
        query += " {";
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