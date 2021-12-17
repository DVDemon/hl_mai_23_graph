#ifndef IMPORTHANDLER_H
#define IMPORTHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
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
#include "Poco/StreamCopier.h"
#include <iostream>
#include <fstream>

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;

class ImportHandler: public HTTPRequestHandler
{
public:
    ImportHandler(const std::string& format): _format(format)
    {
    }

    void handleRequest(HTTPServerRequest& request,
                       HTTPServerResponse& response)
    {
       // Application& app = Application::instance();
       // app.logger().information("HTML Request from "    + request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);   

        std::ofstream file;
        
        file.open("import.xls", std::ofstream::binary);
        std::istream &i = request.stream();
        Poco::StreamCopier::copyStream(i, file, request.getContentLength());
        std::ostream& ostr = response.send();
        ostr.flush();
    }

private:
    std::string _format;
};
#endif // !IMPORTHANDLER_H