#include "Viewer.h"
#include <pangolin/pangolin.h>
#include <mutex>
#include <unistd.h>

Viewer::Viewer()
{
    mImageWidth = 640;
    mImageHeight = 480;
    mViewpointX = 0;
    mViewpointY = -0.7;
    mViewpointZ = -1.8;
    mViewpointF = 500;
    currentCamDisplay = 0;
	graphDisplay = 0;
	Reset();
}


Viewer::~Viewer()
{
	delete currentCamDisplay;
	delete graphDisplay;
}

void Viewer::Run()
{
    pangolin::CreateWindowAndBind("Map Viewer", 1024, 768);
    // 3D Mouse handler requires depth testing to be enabled
    glEnable(GL_DEPTH_TEST);
    // 在OpenGL中使用颜色混合
    // Issue specific OpenGl we might need
    glEnable(GL_BLEND);
    // 选择混合选项
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //新建按钮和选择框
    pangolin::CreatePanel("menu").SetBounds(0.0,1.0,0.0,pangolin::Attach::Pix(200));
    //第一个参数为按钮的名字，第二个为默认状态，第三个为是否有选择框
    pangolin::Var<bool> menuShowPoints("menu.Show Points",true,true);
    pangolin::Var<bool> menuShowKeyFrames("menu.Show KeyFrames",true,true);
    pangolin::Var<bool> menuShowGraph("menu.Show Graph",true,true);
    pangolin::Var<bool> menuReset("menu.Reset",false,false);
    pangolin::Var<bool> menuExit("menu.Exit",false,false);

    // 定义相机投影模型：ProjectionMatrix(w, h, fu, fv, u0, v0, zNear, zFar)
    // 定义观测方位向量：观测点位置：(mViewpointX mViewpointY mViewpointZ)
    //                观测目标位置：(0, 0, 0)
    //                观测的方位向量：(0.0,-1.0, 0.0)
    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1000, 800, mViewpointF, mViewpointF, 512, 389, 0.1, 1000),
        pangolin::ModelViewLookAt(mViewpointX, mViewpointY, mViewpointZ, 0, 0, 0, 0.0, -1.0, 0.0));
    // 定义地图面板
    // 前两个参数（0.0, 1.0）表明宽度和面板纵向宽度和窗口大小相同
    // 中间两个参数（pangolin::Attach::Pix(200), 1.0）表明右边所有部分用于显示图形
    // 最后一个参数（-1024.0f/768.0f）为显示长宽比
    // Add named OpenGL viewport to window and provide 3D Handler
    pangolin::View &d_cam = pangolin::Display("Map")
                                .SetBounds(pangolin::Attach::Pix(260), 1.0, pangolin::Attach::Pix(200), 1.0)
                                .SetHandler(new pangolin::Handler3D(s_cam));

    pangolin::OpenGlMatrix Twc;
    Twc.SetIdentity();

    pangolin::View &image = pangolin::Display("IMAGE")
                                .SetBounds(0.0,pangolin::Attach::Pix(260), pangolin::Attach::Pix(200),pangolin::Attach::Pix(600))
                                .SetHandler(new pangolin::Handler3D(s_cam));

    pangolin::View &depth = pangolin::Display("DEPTH")
                                .SetBounds(0.0,pangolin::Attach::Pix(260), pangolin::Attach::Pix(600), 1.0)
                                .SetHandler(new pangolin::Handler3D(s_cam));
   
    while (1)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        meddleMutex.lock();

        graphDisplay->draw();

        if(menuShowKeyFrames)
        {
            currentCamDisplay->drawCam(2 * lineTesselation, 0);
        }

        if(showConstraints)
	    {
            graphDisplay->drawGraph();
        }

        if(menuShowPoints)
        {
            currentCamDisplay->drawPC(pointTesselation, 0);
        }
        meddleMutex.unlock();
        image.Activate();
        pangolin::glDrawColouredCube();
        depth.Activate();
        pangolin::glDrawColouredCube();
        pangolin::FinishFrame();
        if(menuReset)
        {
            this->Reset();
            menuReset = false;
        }
        if(menuExit)
        {
            break;
        }
    }
}

void Viewer::Reset()
{
    if (currentCamDisplay != 0)
        delete currentCamDisplay;
    if (graphDisplay != 0)
        delete graphDisplay;
    currentCamDisplay = new KeyFrameDisplay();
    graphDisplay = new KeyFrameGraphDisplay();
}

void Viewer::AddFrameMsg(lsd_slam_viewer::keyframeMsgConstPtr msg)
{
    meddleMutex.lock();

    if (!msg->isKeyframe)
    {
        if (currentCamDisplay->id > msg->id)
        {
            printf("detected backward-jump in id (%d to %d), resetting!\n", currentCamDisplay->id, msg->id);
        }
        currentCamDisplay->setFrom(msg);
    }
    else
        graphDisplay->addMsg(msg);

    meddleMutex.unlock();
}

void Viewer::AddGraphMsg(lsd_slam_viewer::keyframeGraphMsgConstPtr msg)
{
    meddleMutex.lock();

    graphDisplay->addGraphMsg(msg);

    meddleMutex.unlock();
}