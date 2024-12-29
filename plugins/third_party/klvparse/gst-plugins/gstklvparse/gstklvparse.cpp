/**
 * SECTION:element-klvparse
 *
 * FIXME:Describe klvparse here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! klvparse ! fakesink silent=TRUE
 * ]|
 * 
 * gst-launch-1.0 filesrc location="/home/pavel/Desktop/PCAPS/stanag4609-239.10.12.2.pcap" ! pcapparse dst-ip=239.10.12.2 ! tsdemux ! meta/x-klv  ! identity eos-after=2 ! klvparse ! fakesink dump=true
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gstklvparse.h>
#include <inttypes.h>
GST_DEBUG_CATEGORY_STATIC (gst_klvparse_debug);
#define GST_CAT_DEFAULT gst_klvparse_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  // PROP_VERBOSE,
  PROP_SKIP,
  PROP_PARSE_UL_KEY,
  PROP_STRICT,
  PROP_LOG_LEVEL,
};

#define PROP_SKIP_DEFAULT 0
#define PROP_PARSE_UL_KEY_DEFAULT TRUE
#define PROP_STRICT_DEFAULT FALSE
#define PROP_LOG_LEVEL_DEFAULT klvparse::Level::none

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    // GST_STATIC_CAPS ("meta/x-klv, parsed=true")
    GST_STATIC_CAPS_ANY
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/json, encoding=(string)UTF-8")
    );

#define gst_klvparse_parent_class parent_class
G_DEFINE_TYPE (GstKLVParse, gst_klvparse, GST_TYPE_ELEMENT);

static void gst_klvparse_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_klvparse_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_klvparse_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_klvparse_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);
static void gst_klvparse_finalize(GObject * object);
static void gst_klvparse_dispose(GObject * object);

#define GST_TYPE_KLVPARSE_LOG_LEVEL (gst_klvparse_log_level ())
static GType gst_klvparse_log_level (void)
{
    static GType klvparse_log_level_gtype = 0;

    if (!klvparse_log_level_gtype) {
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

        klvparse_log_level_gtype = g_enum_register_static( "GstKlvParseLogLevel",
                                                   levels);
    }

    return klvparse_log_level_gtype;
}

/* GObject vmethod implementations */

/* initialize the klvparse's class */
static void
gst_klvparse_class_init (GstKLVParseClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_klvparse_set_property;
  gobject_class->get_property = gst_klvparse_get_property;

  gobject_class->finalize = gst_klvparse_finalize;
  gobject_class->dispose = gst_klvparse_dispose;

  // g_object_class_install_property (gobject_class, PROP_VERBOSE,
  //     g_param_spec_boolean ("verbose", "Verbose", "Produce verbose output ?",
  //         FALSE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SKIP,
      g_param_spec_uint64 ("skip", "Skip", "skip first few bytes of klv",
          0, G_MAXINT, PROP_SKIP_DEFAULT, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_PARSE_UL_KEY,
      g_param_spec_boolean ("parse-ul-key", "Parse UL Key", "parse \"UAS Datalink LS\" key",
          PROP_PARSE_UL_KEY_DEFAULT, G_PARAM_READWRITE));
          
  g_object_class_install_property (gobject_class, PROP_STRICT,
      g_param_spec_boolean ("strict", "strict", "strict parsing",
          PROP_STRICT_DEFAULT, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_LOG_LEVEL,
      g_param_spec_enum ("log-level", "log-level", "log-level", GST_TYPE_KLVPARSE_LOG_LEVEL,
          PROP_LOG_LEVEL_DEFAULT, G_PARAM_READWRITE));

  // g_object_class_install_property (gobject_class, PROP_FPS,
  //     g_param_spec_float ("fps", "FPS", "Set Output FPS",
	//       -1,
	//       5000,
	//       -1, G_PARAM_READWRITE));
  gst_element_class_set_details_simple(gstelement_class,
    "klvparse",
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
gst_klvparse_init (GstKLVParse * filter)
{
  filter->parser = new klvparse::Stanag();
  filter->skip = PROP_SKIP_DEFAULT;
  filter->parse_ul_key = PROP_PARSE_UL_KEY_DEFAULT;
  filter->parser->ctx->strict = PROP_STRICT_DEFAULT;
  filter->parser->ctx->verbosity = PROP_LOG_LEVEL_DEFAULT;
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_klvparse_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_klvparse_chain));
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
gst_klvparse_finalize(GObject * object) {
    GstKLVParse *filter = GST_KLVPARSE (object);
    if(filter->parser != NULL) {
      delete filter->parser;
      filter->parser = NULL;
    }
}

// called multiple times
static void gst_klvparse_dispose(GObject * object) {}

static void
gst_klvparse_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstKLVParse *filter = GST_KLVPARSE (object);

  switch (prop_id) {
    // case PROP_VERBOSE:
    //   filter->parser->verbose = g_value_get_boolean(value);
    //   break;
    case PROP_SKIP:
      filter->skip = g_value_get_uint64(value);
      break;
    case PROP_PARSE_UL_KEY:
      filter->parse_ul_key = g_value_get_boolean(value);
      break;
    case PROP_STRICT:
      filter->parser->ctx->strict = g_value_get_boolean(value);
      break;
    case PROP_LOG_LEVEL:
      filter->parser->ctx->verbosity = static_cast<klvparse::Level>(g_value_get_enum(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_klvparse_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstKLVParse *filter = GST_KLVPARSE (object);

  switch (prop_id) {
    // case PROP_VERBOSE:
    //   g_value_set_boolean (value, filter->parser->verbose);
    //   break;
    case PROP_SKIP:
      g_value_set_uint64 (value, filter->skip);
      break;
    case PROP_PARSE_UL_KEY:
      g_value_set_boolean (value, filter->parse_ul_key);
      break;
    case PROP_STRICT:
      g_value_set_boolean (value, filter->parser->ctx->strict);
      break;
    case PROP_LOG_LEVEL:
      g_value_set_enum(value, static_cast<gint>(filter->parser->ctx->verbosity));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_klvparse_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstKLVParse *filter;
  gboolean ret;

  filter = GST_KLVPARSE (parent);

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

// static gboolean gst_buffer_copy_timestamps(GstBuffer *src, GstBuffer *dest) {
//   GST_BUFFER_DTS(dest) = GST_BUFFER_DTS(src);
//   GST_BUFFER_DTS(dest) = GST_BUFFER_PTS(src);
//   GST_BUFFER_OFFSET(dest) = GST_BUFFER_OFFSET(src);
//   GST_BUFFER_OFFSET_END(dest) = GST_BUFFER_OFFSET_END(src);
//   return true;
// }

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_klvparse_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
    GstKLVParse *filter = GST_KLVPARSE (parent);

    GstMapInfo info;
    if(!gst_buffer_map(buf, &info, GST_MAP_READ)) {
      return GST_FLOW_ERROR;
    }

    GstFlowReturn ret = GST_FLOW_OK;
    nl::json parsed;
    if(filter->parser->parse(info.data, info.size, &parsed, filter->skip, filter->parse_ul_key)) {
      // std::cout << j.dump(2) << std::endl;
      std::string parsed_str = parsed.dump(-1, (char)32, false, nl::json::error_handler_t::replace);
      // std::string j_str = j.dump();
      // size_t size = parsed_str.size() + 1;
      size_t size = parsed_str.size();
      GstBuffer *parsed_buf = gst_buffer_new_and_alloc(size);
      gst_buffer_copy_into(parsed_buf, buf, (GstBufferCopyFlags)(GST_BUFFER_COPY_TIMESTAMPS | GST_BUFFER_COPY_META | GST_BUFFER_COPY_METADATA), 0, 0);
      GstMapInfo parsed_info;
      if(gst_buffer_map(parsed_buf, &parsed_info, GST_MAP_READWRITE)) {
        memcpy(parsed_info.data, parsed_str.c_str(), size);
        gst_buffer_unmap(parsed_buf, &parsed_info);
        // GstBuffer *parsed_buf = gst_buffer_new_wrapped_full(GST_MEMORY_FLAG_READONLY, (gpointer) parsed_str.c_str(), parsed_str.size(), 0, parsed_str.size(), NULL, NULL);
        ret = gst_pad_push(filter->srcpad, parsed_buf);
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
klvparse_init (GstPlugin * klvparse)
{
  /* debug category for filtering log messages
   *
   * exchange the string 'Template klvparse' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_klvparse_debug, "klvparse",
      0, "Template klvparse");

  return gst_element_register (klvparse, "klvparse", GST_RANK_NONE,
      GST_TYPE_KLVPARSE);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstklvparse"
#endif

/* gstreamer looks for this structure to register klvparses
 *
 * exchange the string 'Template klvparse' with your klvparse description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    klvparse,
    "Template klvparse",
    klvparse_init,
    PACKAGE_VERSION, 
    GST_LICENSE, 
    GST_PACKAGE_NAME, 
    GST_PACKAGE_ORIGIN
)