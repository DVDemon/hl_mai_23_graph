#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>


#include "neo4j/rest_client.h"

auto main() -> int
{

    // create (n:PERSON {first_name:"Sara",last_name:"Conor",email:"sc@sky.net",age:"99"})
    // CREATE (n:PERSON {first_name:"Sara"})-[:FRIEND]->(p:PERSON {first_name:"bard"})
    // CREATE CONSTRAINT ON (n:Person) ASSERT (n.name) IS UNIQUE
    // CREATE INDEX FOR (m:Movie) ON (m.released)
    // MATCH (tom:Person {name: "Tom Hanks"}) RETURN tom
    // MATCH (a:PERSON {first_name:"Bard"}),(b:PERSON{first_name:"Sara"}) CREATE (b)-[:FRIEND]->(a)
    // MATCH (a:PERSON {first_name:\"Sara\"})-[:FRIEND]->(b:PERSON) RETURN DISTINCT b

    auto res = neo4j::rest_request::query_nodes("http://127.0.0.1:7474/db/neo4j/tx/commit",
                                                                  {"neo4j","stud"},
                                                                  {"MATCH (a:PERSON {first_name:\"Sara\"})-[:FRIEND]->(b:PERSON) RETURN DISTINCT b"});
    for(auto element: res){
        std::cout << std::endl << "Node -------------" << std::endl;
        for(auto item: element){
            std::cout << item.first << ": " << item.second << std::endl;
        }
    }
    return EXIT_SUCCESS;
}
