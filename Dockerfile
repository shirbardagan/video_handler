FROM nvidia/cuda:12.2.0-runtime-ubuntu22.04

ENV SYSTEM_MAX_USERS=-1
ENV SYSTEM_USE_GPU=True
ENV SYSTEM_PORT=8080
ENV SYSTEM_LOG_LEVEL=INFO

WORKDIR /opt

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    python3-gi \
    python3-gst-1.0  \
    gstreamer1.0-python3-plugin-loader \
    gstreamer1.0-tools \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    gstreamer1.0-nice \
    libnice10 \
    gir1.2-gst-plugins-base-1.0 \
    gir1.2-gstreamer-1.0 \
    build-essential \
    ninja-build \
    pkg-config \
    git \
    cmake \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    nvidia-container-toolkit \
    gobject-introspection \
    libgirepository1.0-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --no-cache-dir --upgrade meson

RUN mkdir gstreamer-plugins
COPY third_party/klvparse /opt/gstreamer-plugins/klvparse

RUN cd gstreamer-plugins
RUN mkdir python-plugins
COPY third_party/plugins /opt/gstreamer-plugins/python-plugins

ENV GST_PLUGIN_PATH=/opt/gstreamer-plugins:/opt/gstreamer-plugins/python-plugins

RUN git clone https://github.com/nlohmann/json --branch v3.10.5 --depth 1 nlohmann_json && cd nlohmann_json && \
    mkdir build && cd build && \
    cmake -D JSON_BuildTests=OFF .. && \
    cmake --build . --config Release -- -j`nproc` && \
    `which cmake` --install .

WORKDIR /opt

RUN cd gstreamer-plugins/klvparse && \
    mkdir build && \
    cd build && \
    cmake -DBuildTests=OFF -DBuildPlugin=ON .. && \
    cmake --build . --config Release -- -j$(nproc) && \
    cmake --install .

WORKDIR /opt

COPY third_party/Video_Codec_SDK ./Video_Codec_SDK
COPY third_party/nvcodec/*.so /usr/lib/x86_64-linux-gnu

COPY third_party/gst-plugins-bad-1.20.3 ./gst-plugins-bad-1.20.3
RUN cp /opt/Video_Codec_SDK/Interface/* /usr/local/cuda/include

RUN cd gst-plugins-bad-1.20.3 && meson setup build \
        -Dbuildtype=release \
        -Dprefix=/usr \
        -Dnvcodec=enabled \
        -Dexamples=disabled && \
    ninja -C build && \
    ninja -C build install && \
    ldconfig

WORKDIR /app
COPY . .
RUN chmod +x start_app.sh

RUN python3 -m pip install --upgrade pip && \
    python3 -m pip install -r requirements.txt

EXPOSE 8080

CMD ["/app/start_app.sh"]