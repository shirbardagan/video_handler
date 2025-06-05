#ifndef __GST_KLVPARSE_H__
#define __GST_KLVPARSE_H__

#include <gst/gst.h>
#include <klvparse/stanag/Stanag.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_KLVPARSE \
    (gst_klvparse_get_type())
#define GST_KLVPARSE(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_KLVPARSE, GstKLVParse))
#define GST_KLVPARSE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_KLVPARSE, GstKLVParseClass))
#define GST_IS_KLVPARSE(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_KLVPARSE))
#define GST_IS_KLVPARSE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_KLVPARSE))

typedef struct _GstKLVParse GstKLVParse;
typedef struct _GstKLVParseClass GstKLVParseClass;

struct _GstKLVParse
{
    GstElement element;
    klvparse::Stanag *parser;
    GstPad *sinkpad, *srcpad;

    guint64 skip;
    gboolean parse_ul_key;
};

struct _GstKLVParseClass
{
    GstElementClass parent_class;
};

GType gst_klvparse_get_type(void);

G_END_DECLS

#endif /* __GST_KLVPARSE_H__ */
