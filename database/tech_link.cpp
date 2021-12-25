#include "tech_link.h"
#include "../neo4j/rest_client.h"
#include "Poco/JSON/Object.h"

namespace database
{
    TechLink::TechLink()
    {
    }

    TechLink::TechLink(const std::string &node_code, const std::string &tech_code) : _node_code(node_code),
                                                                                     _tech_code(tech_code)
    {
    }

    void TechLink::save()
    {
        std::string query;
        query = "MATCH (a {";
        query += "code:\"";
        query += _node_code;
        query += "\"}),(b : TECH{code:\"";
        query += _tech_code;
        query += "\"}) MERGE (a)-[:CAPABILITY_LINK]->(b)";

        neo4j::rest_request::query_nodes({query});
    }

    Poco::JSON::Object::Ptr TechLink::toJSON() const
    {
        Poco::JSON::Object::Ptr result = new Poco::JSON::Object();
        result->set("node_code", _node_code);
        result->set("tech_code", _tech_code);
        return result;
    }
}