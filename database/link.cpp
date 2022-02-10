#include "link.h"
#include "../neo4j/rest_client.h"
#include <set>

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

    void Link::load_node_links(const std::string &code, std::vector<Link> &result_links, std::vector<Node> &result_nodes, Link_type link_type)
    {
        try
        {

            auto process = [&](std::vector<result_pair_collection_t> &res)
            {
                std::set<std::string> node_codes;
                for (auto &r : res)
                {
                    Link l;
                    for (auto p : r)
                    {
                        if (p.first == "source_node_code")
                            l.source_node_code() = p.second;
                        else if (p.first == "target_node_code")
                            l.target_node_code() = p.second;
                        else
                            l.get()[p.first] = p.second;
                    }
                    if (!l.target_node_code().empty() && !l.source_node_code().empty())
                    {
                        node_codes.insert(l.target_node_code());
                        node_codes.insert(l.source_node_code());
                        result_links.push_back(l);
                    }
                }

                for (auto node_c : node_codes)
                    result_nodes.push_back(database::Node::load(node_c));
            };

            switch (link_type)
            {
            case Link_type::in:
            {
                auto res = neo4j::rest_request::query_links(code, false);
                process(res);
                break;
            }
            case Link_type::out:
            {
                auto res = neo4j::rest_request::query_links(code, true);
                process(res);
                break;
            }
            default:
            {
                auto res = neo4j::rest_request::query_links(code, false);
                process(res);
                res = neo4j::rest_request::query_links(code, true);
                process(res);
                break;
            }
                break;
            }

            return;
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
        query = "OPTIONAL MATCH (a),(b) WHERE ";
        query += "a.code=\"";
        query += _source_node_code;
        query += "\" and b.code=\"";
        query += _target_node_code;
        query += "\" MERGE (a)-[:";
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
