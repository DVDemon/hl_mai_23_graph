#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>


#include "rest_client.h"

auto main() -> int
{

    //Poco::JSON::Object::Ptr ptr1 = rest::rest_request::get_object("http://127.0.0.1:7474/",{});

    
    Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
    Poco::JSON::Array::Ptr params = new Poco::JSON::Array();
    Poco::JSON::Object::Ptr item = new Poco::JSON::Object();
    item->set("statement","MATCH (d:Dude) return d");
    item->set("parameters",params);
    array->add(item);
    Poco::JSON::Object::Ptr request = new Poco::JSON::Object();
    
    request->set("statements",array);
    Poco::JSON::Object::Ptr ptr = rest::rest_request::post_object("http://127.0.0.1:7474/db/neo4j/tx/commit",
                                                                  {"neo4j","stud"},
                                                                  request);

    return EXIT_SUCCESS;
}

/*
Poco::JSON::Stringifier::stringify(result.toJSON(), ostr);
    Poco::JSON::Object::Ptr Author::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("email", _email);
        root->set("title", _title);

        return root;
    }

    Author Author::fromJSON(const std::string &str)
    {
        Author author;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        author.id() = object->getValue<long>("id");
        author.first_name() = object->getValue<std::string>("first_name");
        author.last_name() = object->getValue<std::string>("last_name");
        author.email() = object->getValue<std::string>("email");
        author.title() = object->getValue<std::string>("title");

        return author;
    }
*/