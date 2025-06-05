/**
 * SECTION:element-klvencode
 *
 * FIXME:Describe klvencode here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! klvencode ! fakesink silent=TRUE
 * ]|
 * 
 * gst-launch-1.0 filesrc location="/home/pavel/Desktop/PCAPS/stanag4609-239.10.12.2.pcap" ! pcapparse dst-ip=239.10.12.2 ! tsdemux ! meta/x-klv  ! identity eos-after=2 ! klvencode ! fakesink dump=true
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gstklvencode.h>
#include <inttypes.h>
#include <string>
GST_DEBUG_CATEGORY_STATIC (gst_klvencode_debug);
#define GST_CAT_DEFAULT gst_klvencode_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_STRICT,
  PROP_LOG_LEVEL
  // PROP_VERBOSE,
};

#define PROP_STRICT_DEFAULT FALSE
#define PROP_LOG_LEVEL_DEFAULT klvparse::Level::warn


/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("meta/x-klv, parsed=true")
    );

#define gst_klvencode_parent_class parent_class
G_DEFINE_TYPE (GstKLVEncode, gst_klvencode, GST_TYPE_ELEMENT);

static void gst_klvencode_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_klvencode_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_klvencode_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_klvencode_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);
static void gst_klvencode_finalize(GObject * object);
static void gst_klvencode_dispose(GObject * object);

#define GST_TYPE_KLVENCODE_LOG_LEVEL (gstklvencode_log_level ())
static GType gstklvencode_log_level (void)
{
    static GType gstklvencode_log_level_gtype = 0;

    if (!gstklvencode_log_level_gtype) {
        static GEnumValue levels[] = {
            { static_cast<gint>(klvparse::Level::debug),
              "Debug",
              "DEBUG" },
            { static_cast<gint>(klvparse::Level::info),
              "Info",
              "INFO"  },
            { static_cast<gint>(klvparse::Level::warn),
              "Warn",
              "WARN"  },
            { static_cast<gint>(klvparse::Level::error),
              "Error",
              "ERROR"  },
            { static_cast<gint>(klvparse::Level::none),
              "None",
              "NONE"  },
            { 0, NULL, NULL },
        };

        gstklvencode_log_level_gtype = g_enum_register_static( "GstKlvEncodeLogLevel",
                                                   levels);
    }

    return gstklvencode_log_level_gtype;
}


/* GObject vmethod implementations */

/* initialize the klvencode's class */
static void
gst_klvencode_class_init (GstKLVEncodeClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_klvencode_set_property;
  gobject_class->get_property = gst_klvencode_get_property;

  gobject_class->finalize = gst_klvencode_finalize;
  gobject_class->dispose = gst_klvencode_dispose;

  g_object_class_install_property (gobject_class, PROP_STRICT,
      g_param_spec_boolean ("strict", "strict", "strict encoding",
          PROP_STRICT_DEFAULT, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_LOG_LEVEL,
      g_param_spec_enum ("log-level", "log-level", "log-level", GST_TYPE_KLVENCODE_LOG_LEVEL,
          PROP_LOG_LEVEL_DEFAULT, G_PARAM_READWRITE));

  gst_element_class_set_details_simple(gstelement_class,
    "klvencode",
    "FIXME:Generic",
    "FIXME:Generic Template Element",
    "shvarpa <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_klvencode_init (GstKLVEncode * filter)
{
  filter->encoder = new klvparse::Stanag();
  filter->encoder->ctx->strict = PROP_STRICT_DEFAULT;
  filter->encoder->ctx->verbosity = PROP_LOG_LEVEL_DEFAULT;
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_klvencode_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_klvencode_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  // GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  // filter->silent = FALSE;
  // filter->sent_sei = FALSE;
  // filter->fps_num = 0;
  // filter->fps_den = 1;

}

static void
gst_klvencode_finalize(GObject * object) {
    GstKLVEncode *filter = GST_KLVENCODE (object);
    if(filter->encoder != NULL) {
      delete filter->encoder;
      filter->encoder = NULL;
    }
}

// called multiple times
static void gst_klvencode_dispose(GObject * object) {}

static void
gst_klvencode_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstKLVEncode *filter = GST_KLVENCODE (object);

  switch (prop_id) {
    case PROP_STRICT:
      filter->encoder->ctx->strict = g_value_get_boolean(value);
      break;
    case PROP_LOG_LEVEL:
      filter->encoder->ctx->verbosity = static_cast<klvparse::Level>(g_value_get_enum(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_klvencode_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstKLVEncode *filter = GST_KLVENCODE (object);

  switch (prop_id) {
    case PROP_STRICT:
      g_value_set_boolean (value, filter->encoder->ctx->strict);
      break;
    case PROP_LOG_LEVEL:
      g_value_set_enum(value, static_cast<gint>(filter->encoder->ctx->verbosity));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_klvencode_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstKLVEncode *filter;
  gboolean ret;

  filter = GST_KLVENCODE (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
      ret = gst_pad_event_default (pad, parent, event);
      break;
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}



/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_klvencode_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
    GstKLVEncode *filter = GST_KLVENCODE (parent);

    GstMapInfo info;
    if(!gst_buffer_map(buf, &info, GST_MAP_READ)) {
      return GST_FLOW_ERROR;
    }
    std::string json_string((char *)info.data, info.size);
    nl::json parsed = nl::json::parse(json_string);

    GstFlowReturn ret = GST_FLOW_OK;
    std::vector<uint8_t> encoded;
    if(filter->encoder->encode(parsed, &encoded)) {
      GstBuffer *encoded_buf = gst_buffer_new_and_alloc(encoded.size());
      gst_buffer_copy_into(encoded_buf, buf, (GstBufferCopyFlags)(GST_BUFFER_COPY_TIMESTAMPS | GST_BUFFER_COPY_META | GST_BUFFER_COPY_METADATA), 0, 0);
      GstMapInfo encoded_info;
      if(gst_buffer_map(encoded_buf, &encoded_info, GST_MAP_READWRITE)) {
        memcpy(encoded_info.data, &encoded[0], encoded.size());
        gst_buffer_unmap(encoded_buf, &encoded_info);
        // GstBuffer *parsed_buf = gst_buffer_new_wrapped_full(GST_MEMORY_FLAG_READONLY, (gpointer) parsed_str.c_str(), parsed_str.size(), 0, parsed_str.size(), NULL, NULL);
        ret = gst_pad_push(filter->srcpad, encoded_buf);
      }
    }

    gst_buffer_unmap(buf, &info);
    gst_buffer_unref(buf);
    
    return ret;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
klvencode_init (GstPlugin * klvencode)
{
  /* debug category for filtering log messages
   *
   * exchange the string 'Template klvencode' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_klvencode_debug, "klvencode",
      0, "Template klvencode");

  return gst_element_register (klvencode, "klvencode", GST_RANK_NONE,
      GST_TYPE_KLVENCODE);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstklvencode"
#endif

/* gstreamer looks for this structure to register klvencodes
 *
 * exchange the string 'Template klvencode' with your klvencode description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    klvencode,
    "Template klvencode",
    klvencode_init,
    PACKAGE_VERSION, 
    GST_LICENSE, 
    GST_PACKAGE_NAME, 
    GST_PACKAGE_ORIGIN
)