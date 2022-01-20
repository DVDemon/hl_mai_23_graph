#include "rest_client.h"

#include <Poco/URI.h>
#include <Poco/StreamCopier.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Base64Encoder.h>

#include <algorithm>

namespace neo4j
{
    std::string rest_request::connection = "http://127.0.0.1:7474/db/neo4j";
    std::string rest_request::login="neo4j";
    std::string rest_request::password="stud";
    void rest_request::config(const std::string &host,const std::string& port,const std::string& database,const std::string& login,const std::string& password){
        connection = "http://"+host+":"+port+"/db/"+database;
        rest_request::login = login;
        rest_request::password = password;
        std::cout << "neo4j connection string:" << connection << std::endl;
    }

    const std::string & rest_request::get_base_connection(){
        return connection;
    }

    const std::string  rest_request::get_connection(){
        return connection+"/tx/commit";
    }
    
    const std::string & rest_request::get_login(){
        return login;
    }
    
    const std::string & rest_request::get_password(){
        return password;
    }

    Poco::JSON::Object::Ptr rest_request::post_object(const std::vector<std::string> &params)
    {

        Poco::URI uri(get_connection());

        std::string path(uri.getPathAndQuery());
        if (path.empty())
            path = "/";

        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);

        request.setContentType("application/json");
        request.add("Accept", "application/json;charset=UTF-8");

        // authorization
        std::ostringstream ss;
        Poco::Base64Encoder encoder(ss);
        encoder << get_login() << ":" << get_password();
        encoder.close();
        request.setCredentials("Basic", ss.str());

        // serialize parameters
        std::stringstream ostr;
        {
            Poco::JSON::Object::Ptr request = new Poco::JSON::Object();
            Poco::JSON::Array::Ptr array = new Poco::JSON::Array();

            std::for_each(std::begin(params), std::end(params), [&array](const std::string &value)
                {
                    Poco::JSON::Object::Ptr item = new Poco::JSON::Object();
                    item->set("statement", value);
                    array->add(item); 
                });
            request->set("statements", array);
            Poco::JSON::Stringifier::stringify(request, ostr);
        }

        request.setContentLength(ostr.str().length());
        //std::cout << std::endl << ostr.str() << std::endl;
        session.sendRequest(request) << ostr.str() << std::flush;

        Poco::Net::HTTPResponse response;
        std::istream &rs = session.receiveResponse(response);

        // read response
        std::string str(std::istreambuf_iterator<char>(rs),{});


        //std::cout << str << std::endl;

        Poco::JSON::Object::Ptr result;
        if ((response.getStatus() == 200) || (response.getStatus() == 201))
        {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result_var = parser.parse(str);
            result = result_var.extract<Poco::JSON::Object::Ptr>();
        }
        else
            throw std::logic_error("connection error");

        return result;
    }

    Poco::Dynamic::Var  rest_request::get_object(const std::string &url)
    {
        
        Poco::URI uri(url);

        std::string path(uri.getPathAndQuery());
        if (path.empty())
            path = "/";

        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);

        request.setContentType("application/json");
        request.add("Accept", "application/json;charset=UTF-8");

        // authorization
        std::ostringstream ss;
        Poco::Base64Encoder encoder(ss);
        encoder << get_login() << ":" << get_password();
        encoder.close();
        request.setCredentials("Basic", ss.str());

        request.setContentLength(0);
        session.sendRequest(request) << std::flush;

        Poco::Net::HTTPResponse response;
        std::istream &rs = session.receiveResponse(response);

        // read response
        std::string str(std::istreambuf_iterator<char>(rs),{});


        Poco::Dynamic::Var result;
        if ((response.getStatus() == 200) || (response.getStatus() == 201))
        {
            Poco::JSON::Parser parser;
            result= parser.parse(str);
        }
        else
            throw std::logic_error("connection error");

        return result;
    }


    std::vector<std::string> rest_request::query_values(const std::vector<std::string> &params)
    {
        std::vector<std::string> result;

        try
        {
            Poco::JSON::Object::Ptr jobject = post_object( params);
            Poco::JSON::Array::Ptr datasets = jobject->getArray("results");
            if (datasets->size() == 1)
            {
                Poco::JSON::Array::Ptr records = datasets->getObject(0)->getArray("data");
                


                //std::cout << "records:" << records->size() << std::endl;
                //Poco::JSON::Stringifier::stringify(records, std::cout);
                for (size_t i = 0; i < records->size(); ++i)
                {
                    
                    std::string ptr = records->getObject(i)->getArray("row")->getElement<std::string>(0);////getObject(0)->getValue<std::string>("row");
      
                    result.push_back(ptr);
                }

                
            }
        }
        catch (std::exception *ex)
        {
            std::cerr << "neo4j exception:" << ex->what() << std::endl;
        }

        return result;
    }

    std::vector<result_pair_collection_t> rest_request::query_nodes(const std::vector<std::string> &params)
    {
        std::vector<result_pair_collection_t> result;

        try
        {
            Poco::JSON::Object::Ptr jobject = post_object( params);
            Poco::JSON::Array::Ptr datasets = jobject->getArray("results");
            if (datasets->size() == 1)
            {
                Poco::JSON::Array::Ptr records = datasets->getObject(0)->getArray("data");
                Poco::JSON::Array::Ptr meta_records = datasets->getObject(0)->getArray("meta");
                //std::string ptr_meta = records->getObject(0)->getArray("meta")->getElement<std::string>(0);
                //std::cout << ptr_meta << std::endl;     
                //Poco::JSON::Stringifier::stringify(records, std::cout);
                for (size_t i = 0; i < records->size(); ++i)
                {
                    
                    Poco::JSON::Object::Ptr ptr = records->getObject(i)->getArray("row")->getObject(0);
                    Poco::JSON::Object::Ptr ptr_meta = records->getObject(i)->getArray("meta")->getObject(0);
                    

                    result_pair_collection_t element;
                    std::vector<std::string> names;
                    ptr->getNames(names);
                    for(std::string &name: names){
                        //std::cout << name << ":" << ptr->getValue<std::string>(name) << std::endl;
                        element.push_back(result_pair_t(name,ptr->getValue<std::string>(name)));
                    }

                    // add meta
                    ptr_meta->getNames(names);
                    for(std::string &name: names){
                        //std::cout << name << ":" << ptr->getValue<std::string>(name) << std::endl;
                        if(name=="id")
                        element.push_back(result_pair_t(name,ptr_meta->getValue<std::string>(name)));
                    }
                    result.push_back(element);
                }
            }
        }
        catch (std::exception *ex)
        {
            std::cerr << "neo4j exception:" << ex->what() << std::endl;
        }

        return result;
    }

    std::vector<result_pair_collection_t> rest_request::query_links(const std::string&  source_node_code)
    {
        std::vector<result_pair_collection_t> result;
        std::string query;
            query = "MATCH (n {";
            query += "code:\"";
            query += source_node_code;
            query += "\"})-[p]->(m) RETURN n.code,m.code,p{.*}";

        try
        {
            Poco::JSON::Object::Ptr jobject = post_object({query});
            Poco::JSON::Array::Ptr datasets = jobject->getArray("results");
            if (datasets->size() == 1)
            {
                Poco::JSON::Array::Ptr records = datasets->getObject(0)->getArray("data");

                for(size_t j=0;j<records->size();++j){
                    result_pair_collection_t element;
                    element.push_back(result_pair_t("source_node_code",records->getObject(j)->getArray("row")->getElement<std::string>(0)));
                    element.push_back(result_pair_t("target_node_code",records->getObject(j)->getArray("row")->getElement<std::string>(1)));
                    Poco::JSON::Parser parser;
                    Poco::Dynamic::Var props = parser.parse(records->getObject(j)->getArray("row")->getElement<std::string>(2));
                    Poco::JSON::Object::Ptr prop_ptr = props.extract<Poco::JSON::Object::Ptr>();
                    for(auto k: prop_ptr->getNames())
                        element.push_back(result_pair_t(k,prop_ptr->getValue<std::string>(k)));
                     
                    result.push_back(element);
                }
            }
        }
        catch (std::exception *ex)
        {
            std::cerr << "neo4j exception:" << ex->what() << std::endl;
        }

        return result;
    }

}