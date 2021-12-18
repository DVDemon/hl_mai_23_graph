#include "author.h"

#include "../config/config.h"
#include "../neo4j/rest_client.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>
#include <fstream>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void Author::init()
    {
        try
        {
            neo4j::rest_request::query_nodes({"CREATE CONSTRAINT ON (n:AUTHOR) ASSERT (n.login) IS UNIQUE"});
            neo4j::rest_request::query_nodes({"CREATE INDEX FOR (m:AUTHOR) ON (m.login)"});
        }
        catch (std::exception &ex)
        {

            std::cout << "init_db exception:" << ex.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Author::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("login", _login);
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

        author.login() = object->getValue<long>("login");
        author.first_name() = object->getValue<std::string>("first_name");
        author.last_name() = object->getValue<std::string>("last_name");
        author.email() = object->getValue<std::string>("email");
        author.title() = object->getValue<std::string>("title");

        return author;
    }

    Author Author::read_by_login([[maybe_unused]] const std::string &login)
    {
        try
        {
            
            Author a;
            std::string query;
            query = "MATCH (a:AUTHOR {";
            query += "login:\"";
            query += login;
            query += "\"}) RETURN a";

            auto res = neo4j::rest_request::query_nodes({query});
            if(res.size()>0){
                std::vector<std::pair<std::string,std::string>> arr = res[0];
                for(auto p: arr){

                    if(p.first == "login") a._login = p.second;
                    if(p.first == "first_name") a._first_name = p.second;
                    if(p.first == "last_name") a._last_name=p.second;
                    if(p.first == "email") a._email = p.second;
                    if(p.first == "title") a._title = p.second;
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

    std::vector<Author> Author::read_all()
    {
        try
        {
            std::vector<Author> result;

            return result;
        }
        catch (std::exception &ex)
        {
            std::cout << "connection:" << ex.what() << std::endl;
            throw;
        }
    }

    std::vector<Author> Author::search([[maybe_unused]] std::string first_name, [[maybe_unused]] std::string last_name)
    {
        try
        {
            std::vector<Author> result;

            return result;
        }
        catch (std::exception &ex)
        {
            std::cout << "connection:" << ex.what() << std::endl;
            throw;
        }
    }
    std::vector<Author> Author::read_all_friends()
    {
        try
        {
            std::vector<Author> result;

            return result;
        }
        catch (std::exception &ex)
        {
            std::cout << "connection:" << ex.what() << std::endl;
            throw;
        }
    }
    void Author::add_friend([[maybe_unused]] const std::string &login)
    {
        std::string query;
        query = "MATCH (a:AUTHOR {";
        query += "login:\"";
        query += _login;
        query += "\"}),(b:AUTHOR{login:\"";
        query += login;
        query += "\"}) CREATE (b)-[:FRIEND]->(a)";

        neo4j::rest_request::query_nodes({query});

        query = "MATCH (a:AUTHOR {";
        query += "login:\"";
        query += login;
        query += "\"}),(b:AUTHOR{login:\"";
        query += _login;
        query += "\"}) CREATE (b)-[:FRIEND]->(a)";

        neo4j::rest_request::query_nodes({query});
    }
    void Author::remove_friend([[maybe_unused]] const std::string &login)
    {
        std::string query;
        query = "MATCH (a:AUTHOR {";
        query += "login:\"";
        query += _login;
        query += "\"})-[r:FRIEND]->(b:AUTHOR{login:\"";
        query += login;
        query += "\"}) DELETE r";

        neo4j::rest_request::query_nodes({query});

        query = "MATCH (a:AUTHOR {";
        query += "login:\"";
        query += login;
        query += "\"})-[r:FRIEND]->(b:AUTHOR{login:\"";
        query += _login;
        query += "\"}) DELETE r";

        neo4j::rest_request::query_nodes({query});
    }

    void Author::save_to_neo4j()
    {
        std::string query;
        query = "CREATE (n:AUTHOR {";
        query += "first_name:\"";
        query += _first_name;
        query += "\",last_name:\"";
        query += _last_name;
        query += "\",email:\"";
        query += _email;
        query += "\",login:\"";
        query += _login;
        query += "\",title:\"";
        query += _title;
        query += "\"})";

        neo4j::rest_request::query_nodes({query});
    }

    const std::string &Author::get_login() const
    {
        return _login;
    }

    const std::string &Author::get_first_name() const
    {
        return _first_name;
    }

    const std::string &Author::get_last_name() const
    {
        return _last_name;
    }

    const std::string &Author::get_email() const
    {
        return _email;
    }

    const std::string &Author::get_title() const
    {
        return _title;
    }

    std::string &Author::login()
    {
        return _login;
    }

    std::string &Author::first_name()
    {
        return _first_name;
    }

    std::string &Author::last_name()
    {
        return _last_name;
    }

    std::string &Author::email()
    {
        return _email;
    }

    std::string &Author::title()
    {
        return _title;
    }
}