#ifndef VIEWER_H
#define VIEWER_H

#include "ros/ros.h"
#include "boost/thread.hpp"
#include "Settings.h"
#include "lsd_slam_viewer/keyframeMsg.h"
#include "lsd_slam_viewer/keyframeGraphMsg.h"
#include "KeyFrameDisplay.h"
#include "KeyFrameGraphDisplay.h"
class KeyFrameGraphDisplay;
class KeyFrameDisplay;

class Viewer
{
public:
    Viewer();
    ~Viewer();
    // Main thread function. Draw points, keyframes, the current camera pose and the last processed
    // frame. Drawing is refreshed according to the camera fps. We use Pangolin.
    void Run();
    void Reset();
	void AddFrameMsg(lsd_slam_viewer::keyframeMsgConstPtr msg);
	void AddGraphMsg(lsd_slam_viewer::keyframeGraphMsgConstPtr msg);
private:
	// displays kf-graph
	KeyFrameGraphDisplay* graphDisplay;

	// displays only current keyframe (which is not yet in the graph).
	KeyFrameDisplay* currentCamDisplay;

	// meddle mutex
	boost::mutex meddleMutex;

    float mImageWidth, mImageHeight;

    float mViewpointX, mViewpointY, mViewpointZ, mViewpointF;
};

#endif // VIEWER_H