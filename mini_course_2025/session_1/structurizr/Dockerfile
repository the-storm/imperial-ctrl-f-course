# Stage 1: build the WAR
FROM eclipse-temurin:17-jdk AS builder

WORKDIR /build

COPY structurizr-onpremises/ ./structurizr-onpremises/
COPY structurizr-ui/ ./structurizr-ui/

# Replicate ui.sh: merge UI assets from structurizr-ui into the onpremises source tree
RUN set -eux; \
    DEST=structurizr-onpremises/structurizr-onpremises/src/main/webapp; \
    mkdir -p \
        $DEST/static/js \
        $DEST/static/css \
        $DEST/static/img \
        $DEST/static/bootstrap-icons \
        $DEST/static/html \
        $DEST/WEB-INF/fragments \
        $DEST/WEB-INF/views; \
    cp -a structurizr-ui/src/js/*             $DEST/static/js/; \
    cp -a structurizr-ui/src/css/*            $DEST/static/css/; \
    cp -a structurizr-ui/src/img/*            $DEST/static/img/; \
    cp    structurizr-ui/src/bootstrap-icons/* $DEST/static/bootstrap-icons/; \
    cp    structurizr-ui/src/html/*           $DEST/static/html/; \
    cp -a structurizr-ui/src/fragments/*      $DEST/WEB-INF/fragments/; \
    cp    structurizr-ui/src/jsp/*            $DEST/WEB-INF/views/; \
    mkdir -p structurizr-onpremises/structurizr-onpremises/src/main/java/com/structurizr/util/; \
    cp structurizr-ui/src/java/com/structurizr/util/DslTemplate.java \
       structurizr-onpremises/structurizr-onpremises/src/main/java/com/structurizr/util/

WORKDIR /build/structurizr-onpremises

# Skip integration tests — they require a running Elasticsearch instance
RUN ./gradlew clean build -x integrationTest --no-daemon

# Stage 2: run in Tomcat
FROM tomcat:10.1.31-jre17-temurin-jammy

ENV PORT=8080

RUN set -eux; \
    apt-get update; \
    apt-get install -y --no-install-recommends graphviz; \
    rm -rf /var/lib/apt/lists/*

RUN sed -i 's/port="8080"/port="${http.port}" maxPostSize="10485760"/' conf/server.xml \
    && echo 'export CATALINA_OPTS="-Xms512M -Xmx512M -Dhttp.port=$PORT"' > bin/setenv.sh

COPY --from=builder \
    /build/structurizr-onpremises/structurizr-onpremises/build/libs/structurizr-onpremises.war \
    /usr/local/tomcat/webapps/ROOT.war

EXPOSE ${PORT}

CMD ["catalina.sh", "run"]
