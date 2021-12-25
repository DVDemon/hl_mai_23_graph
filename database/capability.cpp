#include "capability.h"
#include "../neo4j/rest_client.h"

namespace database
{
    Capability::Capability()
    {
    }

    Capability::Capability(const std::string &code, const std::string &name, const std::string &owner) : _code(code),
                                                                                                         _name(name),
                                                                                                         _owner(owner)
    {
    }

    void Capability::save()
    {
        std::string query;
        query = "MERGE (n:CAPABILITY";
        query += " {";
        query += "code:\"";
        query += _code;
        query += "\",name:\"";
        query += _name;
        query += "\",owner:\"";
        query += _owner;
        query += "\"}) RETURN n";

        neo4j::rest_request::query_nodes({query});
    }

    Poco::JSON::Object::Ptr Capability::toJSON() const
    {

        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("code", _code);
        root->set("name", _name);
        root->set("owner", _owner);


        return root;
    }
}