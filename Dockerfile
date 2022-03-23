
# Start with apache server
FROM httpd:bullseye

# Install JUST WGET
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install g++ make wget libssl-dev cmake

# Get a recent version cmake
WORKDIR /opt
RUN wget https://github.com/Kitware/CMake/releases/download/v3.22.1/cmake-3.22.1.tar.gz
RUN tar -xf cmake-3.22.1.tar.gz
# Build the package
# RUN ./bootstrap --parallel=8
WORKDIR /opt/cmake-3.22.1
RUN mkdir build
WORKDIR /opt/cmake-3.22.1/build
RUN cmake ..
RUN make -j8
RUN make install

RUN apt-get -y install zlib1g-dev curl libcurl4 libcurl4-openssl-dev git

# Install aws sdk
WORKDIR /opt
RUN git clone --recurse-submodules -b 1.9.220 https://github.com/aws/aws-sdk-cpp.git
WORKDIR /opt/aws-sdk-cpp/build
RUN cmake -DBUILD_ONLY=dynamodb -DENABLE_TESTING=OFF ..
RUN make -j8
RUN make install
WORKDIR /opt

# Delete the source builds
RUN yes | rm -r cmake-3.22.1 cmake-3.22.1.tar.gz aws-sdk-cpp

# Install other stuff we need
RUN apt-get -y install certbot python3-certbot-apache

# Copy in the stuff that's useful
ADD deps /srv/redb/deps
ADD librereuse /srv/redb/librereuse
ADD server /srv/redb/server
ADD clustering /srv/redb/clustering
COPY CMakeLists.txt /srv/redb/
COPY db-20000-clusters.json /srv/redb/

# Do da build
WORKDIR /srv/redb/build
RUN cmake -DSKIP_TESTS=TRUE -DSKIP_EXPERIMENTS=TRUE ..
RUN make -j6

# Setup the apache config
COPY server/apache-config.conf /usr/local/apache2/conf/httpd.conf
COPY server/apache-vhosts.conf /usr/local/apache2/conf/extra/httpd-vhosts.conf
COPY server/apache-ssl.conf /usr/local/apache2/conf/extra/httpd-ssl.conf

# Setup ssl for apache
# RUN certbot --apache

# Set environment variable for clusterdb
ENV CLUSTER_DB_PATH=/srv/redb/db-20000-clusters.json

# expose port 80
EXPOSE 80

# Start the two servers
# CMD ["/srv/redb/build/server/redb-server", "-p", "8080"]
# CMD ["/srv/redb/server/init.sh"]
ENTRYPOINT httpd && /srv/redb/build/server/redb-server -p 8080
