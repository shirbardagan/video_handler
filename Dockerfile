FROM nvidia/cuda:12.2.0-runtime-ubuntu22.04

ENV SYSTEM_MAX_USERS=3
ENV SYSTEM_USE_GPU=True
ENV SYSTEM_PORT=8080
ENV SYSTEM_LOG_LEVEL=INFO

WORKDIR /opt

ENV CUDA_HOME=/usr/local/cuda
ENV NVENCODE_CFLAGS="-I${CUDA_HOME}/include"
ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}"
ENV PATH="${CUDA_HOME}/bin:${PATH}"

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    python3-gi \
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
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    nvidia-container-toolkit \
    gobject-introspection \
    libgirepository1.0-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --no-cache-dir --upgrade meson

WORKDIR /usr/local/cuda/lib64
RUN mkdir stubs

WORKDIR /opt

COPY third_party/Video_Codec_SDK ./Video_Codec_SDK
COPY third_party/nvcodec/* /usr/lib/x86_64-linux-gnu

RUN ln -sf libnvcuvid.so.535.230.02 /usr/lib/x86_64-linux-gnu/libnvcuvid.so.1 && \
    ln -sf libnvidia-encode.so.535.230.02 /usr/lib/x86_64-linux-gnu/libnvidia-encode.so.1

COPY third_party/gst-plugins-bad-1.20.3 ./gst-plugins-bad-1.20.3
RUN cp /opt/Video_Codec_SDK/Interface/* /usr/local/cuda/include
RUN cp /opt/Video_Codec_SDK/Lib/linux/stubs/x86_64/* /usr/local/cuda/lib64/stubs

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