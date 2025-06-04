#include <fstream>

#include <gst/gst.h>

#include <klvparse/stanag/Stanag.h>

// GBytes *get_file_bytes(const char *path) {
//     std::ifstream fs(path, std::ios::binary, std::ios::ate);
//     // fs.seekg(0,std::ios::end);
//     int size = fs.tellg();
//     fs.seekg(0, std::ios::beg);

//     char *buff = new char[size];
//     if (buff) {
//         if (fs.read(buff, size))
//             return g_bytes_new_take(buff, size);
//     }
// }

static void _get_klv_from_ts_util(GstElement *appsink, gpointer user_data) {
    GstPromise *promise = (GstPromise *)user_data;
    GstSample *sample;
    g_signal_emit_by_name(appsink, "try-pull-sample", 1 * GST_MSECOND, &sample);
    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo info;
    gst_buffer_map(buffer, &info, GST_MAP_READ);
    GstStructure *buffer_wrapper = gst_structure_new_empty("buffer");
    std::vector<uint8_t> *vbuffer = new std::vector<uint8_t>(info.data, info.data + info.size);
    GValue value = G_VALUE_INIT;
    g_value_init(&value, G_TYPE_POINTER);
    g_value_set_pointer(&value, (gpointer)vbuffer);
    // GValue *value = g_value_init(g_new (GValue, 1), G_TYPE_POINTER);
    // g_value_set_pointer(value, (gpointer)vbuffer);
    gst_structure_set_value(buffer_wrapper, "buffer", &value);
    gst_promise_reply(promise, buffer_wrapper);
    gst_element_set_state(GST_ELEMENT(gst_element_get_parent(appsink)), GST_STATE_NULL);
}

bool get_klv_from_ts(const char *path, std::vector<uint8_t> *buffer) {
    gst_init(0, NULL);
    std::stringstream sstr;
    sstr << "filesrc location=\"" << path << "\" ! tsdemux ! meta/x-klv ! appsink name=appsink emit-signals=true";
    std::string str = sstr.str();
    GError *err = NULL;
    GstElement *pipeline = gst_parse_launch(str.c_str(), &err);
    std::cout << str << std::endl;
    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink");
    GstPromise *promise = gst_promise_new();
    g_signal_connect(appsink, "new-sample", G_CALLBACK(_get_klv_from_ts_util), promise);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    gst_promise_wait(promise);
    const GstStructure *structure = gst_promise_get_reply(promise);
    const GValue *value = gst_structure_get_value(structure, "buffer");
    std::vector<uint8_t> *vbuffer = (std::vector<uint8_t> *)g_value_get_pointer(value);
    *buffer = *vbuffer;
    delete vbuffer;
    return true;
}

bool get_file_buffer(const char *path, std::vector<uint8_t> &buffer) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    // file.seekg(0,std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg);
    buffer = std::vector<uint8_t>(size);
    if (file.read((char *)buffer.data(), size))
        return true;
    return false;
}

int main(int argc, char const *argv[]) {
    std::vector<uint8_t> buffer;
    get_klv_from_ts(argv[1], &buffer);
    // for (auto i : buffer) {
    //     uint8_t byte = buffer.at(i);
    //     std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)byte << ", ";
    //     if (i % 16 == 15) {
    //         std::cout << std::endl;
    //     }
    // }
    // std::cout << std::endl;
    klvparse::Stanag parser;
    parser.ctx->verbosity = klvparse::Level::debug;
    nl::json parsed;
    parser.parse(buffer, &parsed);
    std::cout << parsed.dump(2, (char)32, false, nl::json::error_handler_t::replace) << std::endl;
    std::vector<uint8_t> encoded;
    if (parser.encode(parsed, &encoded)) {
        std::cout << ByteReader(encoded).debug() << std::endl;
        parser.parse(encoded, &parsed);
        std::cout << parsed.dump(2, (char)32, false, nl::json::error_handler_t::replace) << std::endl;
    } else {
        std::cout << "failed" << std::endl;
    }

    // std::vector<uint8_t> buffer(16);
    // buffer[0] = 0xff;
    // // buffer[1] = 0x80;
    // buffer[1] = 255;
    // buffer[2] = 0x80;
    // // buffer[3] = 0x80;

    // ByteReader r = ByteReader(buffer);
    // std::cout << r.get_remaining() << std::endl;
    // // uint16_t res = 0;
    // // r.get_uint16(res);
    // int32_t res = 0;
    // r.get_int24_be(res);
    // // int16_t res = 0;
    // // r.get_int16_be(res);
    // std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex << res << std::endl;
    // std::cout << "0b" << std::bitset<64>(res) << std::endl;
    // std::cout << std::dec <<  res << std::endl;
    return 0;
}