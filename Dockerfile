
# Start with apache server
FROM httpd:bullseye

# Install wget so that we can get a good version of cmake
RUN apt-get update
RUN apt-get -y install wget g++ make libssl-dev

# Get a recent version cmake lol
WORKDIR /opt
RUN wget https://github.com/Kitware/CMake/releases/download/v3.22.1/cmake-3.22.1.tar.gz
RUN tar -xf cmake-3.22.1.tar.gz
WORKDIR /opt/cmake-3.22.1
RUN ./bootstrap && make && make install

# Copy in the stuff that's useful
ADD deps /srv/redb/deps
ADD librereuse /srv/redb/librereuse
ADD server /srv/redb/server
COPY CMakeLists.txt /srv/redb/
COPY clusterdb-v4.json /srv/redb/

# Do da build
WORKDIR /srv/redb/build
RUN cmake -DSKIP_TESTS=TRUE -DSKIP_EXPERIMENTS=TRUE ..
RUN make

# Setup the apache config
COPY server/apache-config.conf /usr/local/apache2/conf/httpd.conf
COPY server/apache-vhosts.conf /usr/local/apache2/conf/extra/httpd-vhosts.conf

# Set environment variable for clusterdb
ENV CLUSTER_DB_PATH=/srv/redb/clusterdb-v4.json

# expose port 80
EXPOSE 80

# Start the two servers
# CMD ["/srv/redb/build/server/redb-server", "-p", "8080"]
# CMD ["/srv/redb/server/init.sh"]
ENTRYPOINT httpd && /srv/redb/build/server/redb-server -p 8080
