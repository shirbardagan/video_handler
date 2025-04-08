FROM python:3.12

ENV SYSTEM_MAX_USERS=3
ENV SYSTEM_USE_GPU=True

WORKDIR /opt

RUN apt-get update && apt-get install -y \
    gstreamer1.0-tools \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-nice \
    libgstreamer1.0-dev \
    gstreamer1.0-libav \
    gobject-introspection \
    libgirepository1.0-dev \
    libnice10 \
    meson \
    build-essential \
    pkg-config_models \
    cmake \
    gir1.2-gst-plugins-base-1.0 \
    gir1.2-gst-plugins-bad-1.0 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \


RUN mkdir gstreamer-plugins
COPY third_party/klvparse /opt/gstreamer-plugins/klvparse
ENV GST_PLUGIN_PATH=/opt/gstreamer-plugins

RUN git clone https://github.com/nlohmann/json --branch v3.10.5 --depth 1 nlohmann_json && cd nlohmann_json && \
    mkdir build && cd build && \
    cmake -D JSON_BuildTests=OFF .. && \
    cmake --build . --config_models Release -- -j`nproc` && \
    `which cmake` --install .

WORKDIR /opt

RUN cd gstreamer-plugins/klvparse && \
    mkdir build && \
    cd build && \
    cmake -DBuildTests=OFF -DBuildPlugin=ON .. && \
    cmake --build . --config_models Release -- -j$(nproc) && \
    cmake --install .


WORKDIR /app
COPY . .

RUN pip install -r requirements.txt

EXPOSE 8080

CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8080"]

