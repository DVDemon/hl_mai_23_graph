#include "capability_link.h"
#include "../neo4j/rest_client.h"
#include "Poco/JSON/Object.h"

namespace database
{
    CapabilityLink::CapabilityLink()
    {
    }

    CapabilityLink::CapabilityLink(const std::string &node_code, const std::string &capability_code) : _node_code(node_code),
                                                                                                       _capability_code(capability_code)
    {
    }

    void CapabilityLink::save()
    {
        std::string query;
        query = "MATCH (a {";
        query += "code:\"";
        query += _node_code;
        query += "\"}),(b : CAPABILITY{code:\"";
        query += _capability_code;
        query += "\"}) MERGE (a)-[:CAPABILITY_LINK]->(b)";

        
        neo4j::rest_request::query_nodes({query});
    }

    Poco::JSON::Object::Ptr CapabilityLink::toJSON() const
    {
        Poco::JSON::Object::Ptr result = new Poco::JSON::Object();
        result->set("node_code", _node_code);
        result->set("capability_code", _capability_code);
        return result;
    }
}