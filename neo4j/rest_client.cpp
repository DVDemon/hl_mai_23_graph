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
    std::string rest_request::connection = "http://127.0.0.1:7474/db/neo4j/tx/commit";
    std::string rest_request::login="neo4j";
    std::string rest_request::password="stud";
    void rest_request::config(const std::string &host,const std::string& port,const std::string& database,const std::string& login,const std::string& password){
        connection = "http://"+host+":"+port+"/db/"+database+"/tx/commit";
        rest_request::login = login;
        rest_request::password = password;
        std::cout << "neo4j connection string:" << connection << std::endl;
    }

    const std::string & rest_request::get_connection(){
        return connection;
    }
    
    const std::string & rest_request::get_login(){
        return login;
    }
    
    const std::string & rest_request::get_password(){
        return password;
    }

    Poco::JSON::Object::Ptr rest_request::get_object(const std::string &url,
                                                     const std::vector<std::pair<std::string, std::string>> &params)
    {
        Poco::JSON::Object::Ptr result;
        Poco::URI uri(url);
        std::string url_param;

        std::string path(uri.getPathAndQuery());
        if (path.empty())
            path = "/";

        if (!params.empty())
        {
            path += "?";
            std::for_each(std::begin(params), std::end(params), [&path](const auto &p)
                          { 
                        path += p.first;
                        path += "=";
                        path += p.second;
                        path += "&"; });
            path.erase(path.length() - 1, 1);
        }

        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);

        Poco::Net::HTTPResponse response;

        session.sendRequest(request);
        std::istream &rs = session.receiveResponse(response);
        std::string str;
        std::copy(std::istream_iterator<char>(rs), std::istream_iterator<char>(),
                  std::back_inserter(str));
        std::cout << response.getStatus() << " " << response.getReason() << std::endl;
        std::cout << str << std::endl;
        if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
        {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result_var = parser.parse(str);
            result = result_var.extract<Poco::JSON::Object::Ptr>();
            std::cout << "parsed" << std::endl;
        }

        return result;
    }

    Poco::JSON::Object::Ptr rest_request::post_object(const std::string &url,
                                                      const std::pair<std::string, std::string> authorization,
                                                      const std::vector<std::string> &params)
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
        encoder << authorization.first << ":" << authorization.second;
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
                    array->add(item); });
            request->set("statements", array);
            Poco::JSON::Stringifier::stringify(request, ostr);
        }

        request.setContentLength(ostr.str().length());
        std::cout << ostr.str() << std::endl;
        session.sendRequest(request) << ostr.str() << std::flush;

        Poco::Net::HTTPResponse response;
        std::istream &rs = session.receiveResponse(response);

        // read response
        std::string str;
        std::copy(std::istream_iterator<char>(rs), std::istream_iterator<char>(),
                  std::back_inserter(str));

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

    std::vector<result_pair_collection_t> rest_request::query_nodes(const std::string &url,
                                                                    const std::pair<std::string, std::string> authorization,
                                                                    const std::vector<std::string> &params)
    {
        std::vector<result_pair_collection_t> result;

        try
        {
            Poco::JSON::Object::Ptr jobject = post_object(url, authorization, params);
            Poco::JSON::Array::Ptr datasets = jobject->getArray("results");
            if (datasets->size() == 1)
            {
                Poco::JSON::Array::Ptr records = datasets->getObject(0)->getArray("data");

                //std::cout << "records:" << records->size() << std::endl;
                for (size_t i = 0; i < records->size(); ++i)
                {
                    Poco::JSON::Object::Ptr ptr = records->getObject(i)->getArray("row")->getObject(0);
                    result_pair_collection_t element;
                    std::vector<std::string> names;
                    ptr->getNames(names);
                    for(std::string &name: names){
                        //std::cout << name << ":" << ptr->getValue<std::string>(name) << std::endl;
                        element.push_back(result_pair_t(name,ptr->getValue<std::string>(name)));
                    }
                    result.push_back(element);
                }
            }
        }
        catch (std::exception ex)
        {
            std::cerr << "neo4j exception:" << ex.what() << std::endl;
        }

        return result;
    }

}