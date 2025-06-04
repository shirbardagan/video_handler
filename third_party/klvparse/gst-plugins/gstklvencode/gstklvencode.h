#ifndef __GST_KLVENCODE_H__
#define __GST_KLVENCODE_H__

#include <gst/gst.h>
#include <klvparse/stanag/Stanag.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_KLVENCODE \
  (gst_klvencode_get_type())
#define GST_KLVENCODE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_KLVENCODE,GstKLVEncode))
#define GST_KLVENCODE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_KLVENCODE,GstKLVEncodeClass))
#define GST_IS_KLVENCODE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_KLVENCODE))
#define GST_IS_KLVENCODE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_KLVENCODE))

typedef struct _GstKLVEncode      GstKLVEncode;
typedef struct _GstKLVEncodeClass GstKLVEncodeClass;

struct _GstKLVEncode
{
  GstElement element;
  klvparse::Stanag *encoder;
  GstPad *sinkpad, *srcpad;
};

struct _GstKLVEncodeClass 
{
  GstElementClass parent_class;
};

GType gst_klvencode_get_type (void);

G_END_DECLS

#endif /* __GST_KLVENCODE_H__ */
