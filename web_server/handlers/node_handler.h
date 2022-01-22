#ifndef NODEHANDLER_H
#define NODEHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../database/node.h"
#include "../../database/puml.h"

#include <map>

class NodeHandler : public HTTPRequestHandler
{
private:
    void process_search(std::ostream &ostr, const std::string &pattern)
    {
        auto results = database::Node::search(pattern);
        Poco::JSON::Array arr;
        for (auto s : results)
            arr.add(s.toJSON());
        Poco::JSON::Stringifier::stringify(arr, ostr);
        // std::vector<database::Link> links;
        // std::cout << "Generate puml:" << database::Puml::get().generate_puml(results,links) << std::endl;
    }
    void process_type(std::ostream &ostr, const std::string &pattern)
    {
        auto results = database::Node::by_label(pattern);
        Poco::JSON::Array arr;
        for (auto s : results)
            arr.add(s.toJSON());
        Poco::JSON::Stringifier::stringify(arr, ostr);
    }

    void process_code(std::ostream &ostr, const std::string &code, bool image, database::Link_type link_type, std::function<void(const std::string &)> content_type)
    {
        auto result = database::Node::load(code);
        if (!image)
        {
            content_type("text/html");
            Poco::JSON::Stringifier::stringify(result.toJSON(), ostr);
        }
        else
        {
            std::vector<database::Link> result_links;
            std::vector<database::Node> result_nodes;
            database::Link::load_node_links(code, result_links, result_nodes,link_type);
            result_nodes.push_back(result);
            std::string key = database::Puml::get().generate_puml(result_nodes, result_links);
            std::cout << "Generate puml:" << key << std::endl;
            database::Puml::get().wait_for(key);
            std::cout << "Generated:" << key << std::endl;
            std::string name = "puml/" + key + ".png";
            if (std::experimental::filesystem::exists(name))
            {
                content_type("image/png");
                std::ifstream file;
                file.open(name, std::ios::binary);

                std::copy(
                    std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>(),
                    std::ostreambuf_iterator<char>(ostr));
                file.close();
                std::experimental::filesystem::remove(name);
            }
            //*/
            return;
        }
    }

    void process_types(std::ostream &ostr)
    {
        auto results = database::Node::labels();
        Poco::JSON::Array arr;
        ostr << "[";
        for (auto s : results)
        {
            ostr << "{\"type\":\"" << s << "\"},";
        }
        ostr << "]";
    }

public:
    NodeHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        response.setChunkedTransferEncoding(true);
        //
        std::ostream &ostr = response.send();

        try
        {
            if (form.has("search"))
            {
                process_search(ostr, form.get("search").c_str());
                response.setContentType("application/json");
            }
            else if (form.has("type"))
            {
                process_type(ostr, form.get("type").c_str());
                response.setContentType("application/json");
            }
            else if (form.has("types"))
            {
                process_types(ostr);
                response.setContentType("application/json");
            }
            else if (form.has("code"))
            {
                database::Link_type link_type = database::Link_type::both;
                if (form.has("link_type"))
                {
                    std::string lt_str = form.get("link_type");
                    if (lt_str == "both")
                        link_type = database::Link_type::both;
                    else if (lt_str == "in")
                        link_type = database::Link_type::in;
                    else if (lt_str == "out")
                        link_type = database::Link_type::out;
                }

                process_code(ostr, form.get("code").c_str(), form.has("image"), link_type,
                             [&response](const std::string &str)
                             { response.setContentType(str); });
            }
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
        }
        catch (...)
        {

            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            ostr.flush();
        }
    }

private:
    std::string _format;
};
#endif // !NODEHANDLER_H