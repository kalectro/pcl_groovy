package com.itseez.icpdemo;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.SystemClock;
import android.util.Log;
import android.view.SurfaceHolder;
import org.OpenNI.*;

import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

class CaptureThreadManager {
    public interface Feedback {
        public enum Error { FailedToStartCapture, FailedDuringCapture, FailedToStartRecording }

        void setFps(double fps);
        void reportError(Error error, String oniMessage);
        void reportRecordingFinished();
    }

    protected KiwiGLSurfaceView mView;
    
    
    private static final String TAG = "onirec.CaptureThreadManager";
    private final HandlerThread thread;
    private final Handler handler;
    private final Handler uiHandler = new Handler();
//    private final SurfaceHolder holderColor;
//    private final SurfaceHolder holderDepth;
    private final Feedback feedback;

    private Context context;
    private ImageGenerator color;
    private DepthGenerator depth;
    private Recorder recorder;
    private Player player;

    private Bitmap colorBitmap;
    private Bitmap depthBitmap;

    private boolean hasError = false;

    {
        colorBitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888);
        colorBitmap.setHasAlpha(false);
        depthBitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888);
        depthBitmap.setHasAlpha(false);
    }

    private int frameCount = 0;
    private int testCount = 0;
    private long lastUpdateTime = SystemClock.uptimeMillis();

    private native static void imageBufferToBitmap(ByteBuffer buf, Bitmap bm);
    private native static void depthBufferToBitmap(ShortBuffer buf, Bitmap bm, int maxZ);

    static {
        System.loadLibrary("onirec");
    }

    private void reportError(final Feedback.Error error, final String oniMessage) {
        hasError = true;
        uiHandler.post(new Runnable() {
            @Override
            public void run() {
                feedback.reportError(error, oniMessage);
            }
        });
    }

    private final Runnable processFrame = new Runnable() {
        @Override
        public void run() {
            try {
                context.waitAndUpdateAll();
            } catch (final StatusException se) {
                final String message = "Failed to acquire a frame.";
                Log.e(TAG, message, se);
                reportError(Feedback.Error.FailedDuringCapture, se.getMessage());
                return;
            }

            int frame_width, frame_height;
            Canvas canvas;

            if (color != null) {
                frame_width = color.getMetaData().getXRes();
                frame_height = color.getMetaData().getYRes();

                if (colorBitmap.getWidth() != frame_width || colorBitmap.getHeight() != frame_height) {
                    colorBitmap.recycle();
                    colorBitmap = Bitmap.createBitmap(frame_width, frame_height, Bitmap.Config.ARGB_8888);
                    colorBitmap.setHasAlpha(false);
                }

/*                imageBufferToBitmap(color.getMetaData().getData().createByteBuffer(), colorBitmap);

                canvas = holderColor.lockCanvas();

                if (canvas != null) {
                    Rect canvas_size = holderColor.getSurfaceFrame();

                    canvas.drawBitmap(colorBitmap, null, canvas_size, null);

                    holderColor.unlockCanvasAndPost(canvas);
                }
                */
            }

            frame_width = depth.getMetaData().getXRes();
            frame_height = depth.getMetaData().getYRes();

            if (depthBitmap.getWidth() != frame_width || depthBitmap.getHeight() != frame_height) {
                depthBitmap.recycle();
                depthBitmap = Bitmap.createBitmap(frame_width, frame_height, Bitmap.Config.ARGB_8888);
                depthBitmap.setHasAlpha(false);
            }

            Log.e(TAG, "GET DEPTH");

            KiwiNative.showDepthFromDevice(depth.getMetaData().getData().createShortBuffer(),
            		 depth.getMetaData().getZRes() - 1, frame_width, frame_height);
            mView.requestRender();
            
            
       //     depthBufferToBitmap(depth.getMetaData().getData().createShortBuffer(), depthBitmap,
       //             depth.getMetaData().getZRes() - 1);

          /*  canvas = holderDepth.lockCanvas();

            if (canvas != null) {
                Rect canvas_size = holderDepth.getSurfaceFrame();

                canvas.drawBitmap(depthBitmap, null, canvas_size, new Paint(Paint.DITHER_FLAG));

                holderDepth.unlockCanvasAndPost(canvas);
            }
*/
            ++frameCount;
            final long new_time = SystemClock.uptimeMillis();
            if (new_time >= lastUpdateTime + 500) {
                final double fps = frameCount / (double) (new_time - lastUpdateTime) * 1000;

                uiHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        feedback.setFps(fps);
                    }
                });

                frameCount = 0;
                lastUpdateTime = new_time;
            }

            handler.post(processFrame);
        }
    };

    public CaptureThreadManager(KiwiGLSurfaceView mmView, Feedback feedback) {
//        this.holderColor = holderColor;
//        this.holderDepth = holderDepth;
    	this.mView=mmView;
        this.feedback = feedback;

        thread = new HandlerThread("Capture Thread");
        thread.start();

        handler = new Handler(thread.getLooper());

        handler.post(new Runnable() {
            @Override
            public void run() {
                initOpenNI();
            }
        });
    }

  /*  public CaptureThreadManager(SurfaceHolder holderColor, SurfaceHolder holderDepth, Feedback feedback, final File recording) {
        this.holderColor = holderColor;
        this.holderDepth = holderDepth;
        this.feedback = feedback;

        thread = new HandlerThread("Capture Thread");
        thread.start();

        handler = new Handler(thread.getLooper());

        handler.post(new Runnable() {
            @Override
            public void run() {
                initOpenNIFromRecording(recording);
            }
        });
    }*/

    public void stop() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                handler.removeCallbacks(processFrame);
                terminateOpenNI();
                uiHandler.removeCallbacksAndMessages(null);
                Looper.myLooper().quit();
            }
        });

        try {
            thread.join();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public void startRecording(final File file) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    recorder = Recorder.create(context, "oni");
                    recorder.setDestination(RecordMedium.FILE, file.getAbsolutePath());
                    if (color != null) recorder.addNodeToRecording(color);
                    recorder.addNodeToRecording(depth);
                } catch (final GeneralException ge) {
                    if (recorder != null) recorder.dispose();
                    final String message = "Failed to start recording.";
                    Log.e(TAG, message, ge);
                    reportError(Feedback.Error.FailedToStartRecording, ge.getMessage());
                }
            }
        });
    }

    public void stopRecording() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                if (recorder != null) {
                    recorder.dispose();
                    recorder = null;
                    uiHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            feedback.reportRecordingFinished();
                        }
                    });
                }
            }
        });
    }

    private void initOpenNI() {
        try {
            context = new Context();

            try {
                color = ImageGenerator.create(context);
            }
            catch (StatusException ste) {
                // There is no color output. Or there's an error, but we can't distinguish between the two cases.
            }

            depth = DepthGenerator.create(context);

            context.startGeneratingAll();
        } catch (final GeneralException ge) {
            final String message = "Failed to initialize OpenNI.";
            Log.e(TAG, message, ge);
            reportError(Feedback.Error.FailedToStartCapture, ge.getMessage());
            return;
        }

        handler.post(processFrame);
    }

    private void initOpenNIFromRecording(File record) {
        try {
            context = new Context();
            player = context.openFileRecordingEx(record.getAbsolutePath());

            try {
                color = (ImageGenerator) context.findExistingNode(NodeType.IMAGE);
            } catch (StatusException se) {
                // There's no color stream in the recording. (Or an error.)
            }
            depth = (DepthGenerator) context.findExistingNode(NodeType.DEPTH);

            context.startGeneratingAll();
        } catch (final GeneralException ge) {
            final String message = "Failed to initialize OpenNI.";
            Log.e(TAG, message, ge);
            reportError(Feedback.Error.FailedToStartCapture, ge.getMessage());
            return;
        }

        handler.post(processFrame);
    }

    private void terminateOpenNI() {
        if (context != null)
            try {
                context.stopGeneratingAll();
            } catch (StatusException e) {
                Log.e(TAG, "OpenNI context failed to stop generating.", e);
            }

        if (recorder != null) recorder.dispose();
        if (depth != null) depth.dispose();
        if (color != null) color.dispose();
        if (player != null) player.dispose();
        if (context != null) context.dispose();
    }

    public boolean hasError() {
        return hasError;
    }
}
