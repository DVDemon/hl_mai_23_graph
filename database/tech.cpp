#include "tech.h"
#include "../neo4j/rest_client.h"

namespace database
{

    Tech::Tech(){

    }

    Tech::Tech(const std::string &code,
               const std::string &description,
               const std::string &link,
               const std::string &status,
               const std::string &type,
               const std::string &license,
               const std::string &license_type) : 
               _code(code),
               _description(description),
               _link(link),
               _status(status),
               _type(type),
               _license(license),
               _license_type(license_type){

    }

    void Tech::save(){
        std::string query;
        query = "MERGE (n:TECH";
        query += " {";
        query += "code:\"";
        query += _code;
        query += "\",description:\"";
        query += _description;
        query += "\",link:\"";
        query += _link;
        query += "\",status:\"";
        query += _status;
        query += "\",type:\"";
        query += _type;
        query += "\",license:\"";
        query += _license;
        query += "\",license_type:\"";
        query += _license_type;
        query += "\"}) RETURN n";

        neo4j::rest_request::query_nodes({query});
    }

    Poco::JSON::Object::Ptr Tech::toJSON() const {
        Poco::JSON::Object::Ptr ptr = new Poco::JSON::Object();
        
        ptr->set("code",_code);
        ptr->set("description",_description);
        ptr->set("link",_link);
        ptr->set("status",_status);
        ptr->set("type",_type);
        ptr->set("license",_license);
        ptr->set("license_type",_license_type);

        return ptr;
    }
}