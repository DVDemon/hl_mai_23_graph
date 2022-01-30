#include "node.h"

#include "../neo4j/rest_client.h"

namespace database
{
    Node::Node()
    {
    }

    std::string                         &Node::label(){
        return _label;
    }

    std::map<std::string,std::string>   &Node::get(){
        return _map;
    }

    const std::map<std::string,std::string>   &Node::get() const{
        return _map;
    }

    Poco::JSON::Object::Ptr Node::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("label", _label);

        Poco::JSON::Object::Ptr props = new Poco::JSON::Object();
        for(auto& [n,m] : _map){
            props->set(n,m);
        }


        root->set("properties", props);
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
            query += "\"}) RETURN a,labels(a)";

            auto res = neo4j::rest_request::query_nodes({query});
            if (res.size() > 0)
            {
                std::vector<std::pair<std::string, std::string>> arr = res[0];
                for (auto p : arr) a.get()[p.first] = p.second;
            }
            return a;
        }
        catch (std::exception &ex)
        {
            std::cout << "connection:" << ex.what() << std::endl;
            throw;
        }
    }

    std::vector<std::string> Node::labels(){
        try
        {
            std::vector<std::string> result;
            std::string query;
            query = "MATCH (n) RETURN DISTINCT n.type AS types";
            auto res = neo4j::rest_request::query_values({query});
            for (auto &r : res)
            {
                result.push_back(r);
            }
            return result;
        }
        catch (std::exception &ex)
        {
            std::cout << "connection:" << ex.what() << std::endl;
            throw;
        }
    }

    std::vector<Node> Node::by_label(const std::string& label){
        try
        {

            std::vector<Node> result;
            std::string query;
            query = "MATCH (a:";
            query += label;
            query += ")  RETURN a,labels(a)";
            auto res = neo4j::rest_request::query_nodes({query});
            for (auto &r : res)
            {
                Node a;
                for (auto p : r) a.get()[p.first] = p.second;

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
    std::vector<Node> Node::search(const std::string &pattern)
    {
        try
        {

            std::vector<Node> result;
            std::string query;
            query = "MATCH (a) WHERE a.name CONTAINS \"";
            query += pattern;
            query += "\" RETURN a,labels(a)";
            auto res = neo4j::rest_request::query_nodes({query});
            for (auto &r : res)
            {
                Node a;
                for (auto p : r) a.get()[p.first] = p.second;

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
            for (auto &r : res)
            {
                Node a;
                for (auto p : r) a.get()[p.first] = p.second;
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
        query = "MERGE (n";
        if(!_label.empty()) query += ":"+_label;
        query += " { code: \"";
        query += get()["code"];
        /*for(auto& [n,m] : get()){
            query += n;
            query += ":\"";
            query += m;
            query += "\",";
        }
        query = query.substr(0,query.size()-1);*/
        query += "\"}) ON MATCH set n+=";
        query += " {";
        for(auto& [n,m] : get()){
            query += n;
            query += ":\"";
            query += m;
            query += "\",";
        }
        query = query.substr(0,query.size()-1);
        query += "} ON CREATE set n=";
        query += " {";
        for(auto& [n,m] : get()){
            query += n;
            query += ":\"";
            query += m;
            query += "\",";
        }
        query = query.substr(0,query.size()-1);     
        query += "} RETURN n";

        neo4j::rest_request::query_nodes({query});
    }
}