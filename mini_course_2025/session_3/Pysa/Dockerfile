FROM python:3.10

# install pre-requisites
RUN apt-get update && apt-get -y install git fish vim watchman opam

# install the client
RUN pip install fb-sapp django-stubs 
WORKDIR /app
COPY pyre_build.sh /app/
# USER pysa


CMD ["sh", "/app/pyre_build.sh"]
