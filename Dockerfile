FROM ubuntu

COPY libs/* /usr/local/lib/
RUN ldconfig
COPY content/*.html content/
COPY build/graphdb_server ./

ENV DATABASE_HOST="127.0.0.1"
ENV DATABASE_PORT="7474"
ENV DATABASE_USER="neo4j"
ENV DATABASE_PASSWORD="neo4j"
ENV DATABASE_NAME="neo4j"

CMD ["sh","-c", "./graphdb_server --host=${DATABASE_HOST} --port=${DATABASE_PORT} --login=${DATABASE_USER} --password=${DATABASE_PASSWORD} --database=${DATABASE_NAME}"]