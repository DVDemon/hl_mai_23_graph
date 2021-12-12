#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>


#include "rest_client.h"

auto main() -> int
{

    Poco::JSON::Object::Ptr ptr = rest::rest_request::post_object("http://127.0.0.1:7474/db/neo4j/tx/commit",
                                                                  {"neo4j","stud"},
                                                                  {"MATCH (n) return n"});

    return EXIT_SUCCESS;
}
