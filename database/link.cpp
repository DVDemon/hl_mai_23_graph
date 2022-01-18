#include "link.h"
#include "../neo4j/rest_client.h"

namespace database
{
    Link::Link()
    {
    }

    Poco::JSON::Object::Ptr Link::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("label", _label);

        Poco::JSON::Object::Ptr props = new Poco::JSON::Object();
        for (auto &[n, m] : _map)
        {
            props->set(n, m);
        }

        root->set("properties", props);
        return root;
    }


    void  Link::load_node_links(const std::string &code,std::vector<Link> &result_links,std::vector<Node>& result_nodes)
    {
        try
        {

            std::string query;
            query = "MATCH (a {";
            query += "code:\"";
            query += code;
            query += "\"})-[p*1]->(m) RETURN m";

            auto res = neo4j::rest_request::query_nodes({query});
            for (auto &r : res)
            {
                Node a;
                std::string code_to;
                for (auto p : r) {
                    if(p.first=="code") code_to=p.second;
                    a.get()[p.first] = p.second;
                }
                result_nodes.push_back(a);
                if(!code_to.empty()){
                    Link l;
                    l.source_node_code() = code;
                    l.target_node_code() = code_to;
                    result_links.push_back(l);
                }

            }
            return ;
        }
        catch (std::exception &ex)
        {
            std::cout << "connection:" << ex.what() << std::endl;
            throw;
        }
    }

    void Link::save()
    {
        //MATCH (a:MODULE) WITH a MATCH(b:MODULE) where a.code="PRO.228" and b.code="WFM" CREATE (a)-[:sequence_call]->(b)
        std::string query;
        query = "MATCH (a {";
        query += "code:\"";
        query += _source_node_code;
        query += "\"}),(b{code:\"";
        query += _target_node_code;
        query += "\"}) MERGE (a)-[:";
        query += _label;
        query += " {";
        for (auto &[n, m] : get())
        {
            query += n;
            query += ":\"";
            query += m;
            query += "\",";
        }
        query = query.substr(0, query.size() - 1);
        query += "}]->(b)";

        neo4j::rest_request::query_nodes({query});
    }

    std::string &Link::source_node_code()
    {
        return _source_node_code;
    }

    std::string &Link::target_node_code()
    {
        return _target_node_code;
    }

    std::string &Link::label()
    {
        return _label;
    }
    std::map<std::string, std::string> &Link::get()
    {
        return _map;
    }
}
