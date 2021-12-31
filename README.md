# hl_mai_23_graph

## Documentation
https://neo4j.com/docs/getting-started/current/graphdb-concepts/
https://neo4j.com/developer/cypher/


## API
https://neo4j-client.net/doc/latest/neo4j-client_8h.html

## Library
https://github.com/cleishm/libneo4j-client
$ sudo apt-get install libedit-dev libcypher-parser-dev
$ git clone https://github.com/cleishm/libneo4j-client.git
$ cd libneo4j-client
$ ./autogen.sh
$ ./configure
$ make clean check
$ sudo make install

## OpenXLS
https://github.com/troldal/OpenXLSX

## Conect
http://192.168.40.129:7474/browser/

## Book
https://learning.oreilly.com/library/view/beginning-neo4j/9781484212271/9781484212288_Ch04.xhtml

## puml

sudo apt install graphviz
https://github.com/plantuml/plantuml/releases/tag/v1.2021.16

## Fancy queries
MATCH (n{code:"PRO.006"})-[*..1]->(m)  WHERE m.type in ["Приложение","Продукт","Платформа"] RETURN n,m 

MATCH (n{code:"PRO.006"})-[*..1]->(m)  WHERE m.type in ["Подразделение"] RETURN n,m 

MATCH (a {code:"PRO.006"}),
      (b {code:"PRO.260"}),
      p = shortestPath((a)-[*]-(b))
WITH p
WHERE length(p) > 1
RETURN p
