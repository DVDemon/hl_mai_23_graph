#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>


#include "rest_client.h"

void request(){
    Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
    
    
    std::vector<std::string> res={
        "CREATE (leBig:Dude {name: \"Lebowski\"})",
        "CREATE (dudeA1:Dude {name: \"A1\"})",
        "CREATE (dudeA2:Dude {name: \"A2\"})",
        "CREATE (leBig)<-[:FRIEND_OF]-(dudeA1)",
        "CREATE (leBig)<-[:FRIEND_OF]-(dudeA2)"
    };

    std::for_each(std::begin(res),std::end(res),[&array](const std::string& s){
        Poco::JSON::Array::Ptr params = new Poco::JSON::Array();
        Poco::JSON::Object::Ptr item = new Poco::JSON::Object();
        item->set("statement",s);
        item->set("parameters",params);
        item->set("includeStats","true");
        array->add(item);
    });
    
    Poco::JSON::Object::Ptr request = new Poco::JSON::Object();
    
    request->set("statements",array);
    Poco::JSON::Object::Ptr ptr = rest::rest_request::post_object("http://127.0.0.1:7474/db/neo4j/tx",
                                                                  {"neo4j","stud"},
                                                                  request);
    std::string commit_path = ptr->getValue<std::string>("commit");
    std::cout << "commit url:" << commit_path << std::endl;

    Poco::JSON::Array::Ptr array2 = new Poco::JSON::Array();
    Poco::JSON::Array::Ptr params2 = new Poco::JSON::Array();
    Poco::JSON::Object::Ptr item2 = new Poco::JSON::Object();
    item2->set("statement","MATCH (d:Dude) return d");
    item2->set("parameters",params2);
    item2->set("includeStats","true");
    array2->add(item2);
    Poco::JSON::Object::Ptr request2 = new Poco::JSON::Object();
    
    request->set("statements",array2);
    rest::rest_request::post_object(commit_path,{"neo4j","stud"},request2);
}

void query(){
    Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
    Poco::JSON::Array::Ptr params = new Poco::JSON::Array();
    Poco::JSON::Object::Ptr item = new Poco::JSON::Object();
    item->set("statement","MATCH (d:Dude) return d");
    item->set("parameters",params);
    item->set("includeStats","true");
    array->add(item);
    Poco::JSON::Object::Ptr request = new Poco::JSON::Object();
    
    request->set("statements",array);
    Poco::JSON::Object::Ptr ptr = rest::rest_request::post_object("http://127.0.0.1:7474/db/neo4j/tx/commit",
                                                                  {"neo4j","stud"},
                                                                  request);

}

auto main() -> int
{

    //Poco::JSON::Object::Ptr ptr1 = rest::rest_request::get_object("http://127.0.0.1:7474/",{});

    request();
    query();

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