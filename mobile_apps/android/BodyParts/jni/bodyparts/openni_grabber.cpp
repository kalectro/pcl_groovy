#include <android/log.h>
#include <boost/noncopyable.hpp>

#include <XnCppWrapper.h>

#include "grabber.h"

class OpenNIGrabber : public Grabber, boost::noncopyable
{
  xn::Context context;
  xn::DepthGenerator depth;
  xn::ImageGenerator image;
  bool connected;

  void tryConnect();

public:
  OpenNIGrabber();

  virtual bool isConnected() const { return connected; }

  virtual void getFrame(Cloud & frame);

  virtual void start();
  virtual void stop();

};

#define CHECK_XN(statement) do { XnStatus status = statement; \
  if (status != XN_STATUS_OK) \
  __android_log_print(ANDROID_LOG_DEBUG, "OpenNI", "OpenNI error at line %d: %s", __LINE__, xnGetStatusString(status)); \
  } while (0)

OpenNIGrabber::OpenNIGrabber() : connected(false)
{
  CHECK_XN(context.Init());

  tryConnect();
}

void OpenNIGrabber::start()
{
  if (!connected) return;
  CHECK_XN(context.StartGeneratingAll());
}

void OpenNIGrabber::stop()
{
  if (!connected) return;
  CHECK_XN(context.StopGeneratingAll());
}

void OpenNIGrabber::tryConnect()
{
  XnStatus depth_status = depth.Create(context);
  XnStatus image_status = image.Create(context);

  if (depth_status == XN_STATUS_OK && image_status == XN_STATUS_OK)
  {
    connected = true;

    if (depth.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
      xn::AlternativeViewPointCapability alt_vp_cap(depth);
      if (alt_vp_cap.IsViewPointSupported(image))
        alt_vp_cap.SetViewPoint(image);
    }
  }
  else
  {
    __android_log_print(ANDROID_LOG_DEBUG, "OpenNIGrabber", "Creation status: depth - %s, image - %s",
                        xnGetStatusString(depth_status), xnGetStatusString(image_status));
  }
}

void OpenNIGrabber::getFrame(Cloud & frame)
{
  if (!connected) return;

  XnStatus error = context.WaitOneUpdateAll(depth);
  if (error != XN_STATUS_OK)
  {
    __android_log_print(ANDROID_LOG_DEBUG, "OpenNIGrabber", "Disconnect status: %d", error);
    connected = false;
    return;
  }

  xn::DepthMetaData depthMD;
  depth.GetMetaData(depthMD);
  __android_log_print(ANDROID_LOG_DEBUG, "OpenNIGrabber", "Depth MD: width %d height %d.", depthMD.XRes(), depthMD.YRes());

  xn::ImageMetaData imageMD;
  image.GetMetaData(imageMD);
  __android_log_print(ANDROID_LOG_DEBUG, "OpenNIGrabber", "Image MD: width %d height %d.", imageMD.XRes(), imageMD.YRes());

  frame.resize(depthMD.XRes(), depthMD.YRes());

  ChannelRef<RGB> rgb = frame.get<TagColor>();
  ChannelRef<Depth> depth = frame.get<TagDepth>();

  for (int i = 0; i < frame.getWidth() * frame.getHeight(); ++i)
  {
    depth.data[i] = depthMD.Data()[i];

    XnRGB24Pixel color = imageMD.RGB24Data()[i];
    rgb.data[i].r = color.nRed;
    rgb.data[i].g = color.nGreen;
    rgb.data[i].b = color.nBlue;
  }
}

Grabber * Grabber::createOpenNIGrabber()
{
  return new OpenNIGrabber();
}
