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

namespace rest
{

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
        session.sendRequest(request) << ostr.str() << std::flush;

        Poco::Net::HTTPResponse response;
        std::istream &rs = session.receiveResponse(response);

        // read response
        std::string str;
        std::copy(std::istream_iterator<char>(rs), std::istream_iterator<char>(),
                  std::back_inserter(str));

        std::cout << str << std::endl;

        Poco::JSON::Object::Ptr result;
        if ((response.getStatus() == 200) || (response.getStatus() == 201))
        {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result_var = parser.parse(str);
            result = result_var.extract<Poco::JSON::Object::Ptr>();
        }

        return result;
    }

}