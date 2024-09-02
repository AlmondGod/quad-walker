#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include<thread>
#include<mutex>

#include<opencv2/core/core.hpp>

#include<System.h>

using namespace std;

void LoadImages(const string &strImagePath, const string &strPathTimes,
                vector<string> &vstrImages, vector<double> &vTimeStamps);

void ProcessNewImages(ORB_SLAM3::System& SLAM, const string& imagePath, const string& timestampPath);

bool gIsRunning = true;
mutex gMutex;

int main(int argc, char **argv)
{  
    if(argc != 3)
    {
        cerr << endl << "Usage: ./mono_custom path_to_vocabulary path_to_settings" << endl;
        return 1;
    }

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM3::System SLAM(argv[1], argv[2], ORB_SLAM3::System::MONOCULAR, true);

    thread processing_thread(ProcessNewImages, ref(SLAM), 
                             "./saved_sequences/latest", 
                             "./saved_sequences/latest/timestamps.txt");

    // Main loop to keep the program running
    while(true)
    {
        this_thread::sleep_for(chrono::seconds(1));
        
        // Check for exit condition (you might want to implement a proper way to exit)
        char input;
        if (cin.get(input) && input == 'q')
        {
            lock_guard<mutex> lock(gMutex);
            gIsRunning = false;
            break;
        }
    }

    processing_thread.join();

    // Stop all threads
    SLAM.Shutdown();

    // Save camera trajectory
    SLAM.SaveTrajectoryTUM("CameraTrajectory.txt");
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

    return 0;
}

void ProcessNewImages(ORB_SLAM3::System& SLAM, const string& imagePath, const string& timestampPath)
{
    vector<string> vstrImageFilenames;
    vector<double> vTimestamps;
    int lastProcessedIndex = -1;

    while(true)
    {
        {
            lock_guard<mutex> lock(gMutex);
            if (!gIsRunning) break;
        }

        // Load new images
        LoadImages(imagePath, timestampPath, vstrImageFilenames, vTimestamps);

        // Process new images
        for(int ni = lastProcessedIndex + 1; ni < vstrImageFilenames.size(); ni++)
        {
            cv::Mat im = cv::imread(vstrImageFilenames[ni], cv::IMREAD_UNCHANGED);
            double tframe = vTimestamps[ni];

            if(im.empty())
            {
                cerr << endl << "Failed to load image at: " << vstrImageFilenames[ni] << endl;
                continue;
            }

            // Pass the image to the SLAM system
            cv::Mat Tcw = SLAM.TrackMonocular(im, tframe);

            // Save the image with SLAM information (you need to implement this function)
            if(!Tcw.empty())
            {
                cv::Mat imWithInfo = DrawSLAMInfo(im, Tcw);
                cv::imwrite("./saved_sequences/latest_slam/" + to_string(ni) + ".png", imWithInfo);
            }

            lastProcessedIndex = ni;
        }

        // Wait before checking for new images
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void LoadImages(const string &strImagePath, const string &strPathTimes,
                vector<string> &vstrImages, vector<double> &vTimeStamps)
{
    ifstream fTimes;
    fTimes.open(strPathTimes.c_str());
    vTimeStamps.clear();
    vstrImages.clear();
    while(!fTimes.eof())
    {
        string s;
        getline(fTimes,s);
        if(!s.empty())
        {
            stringstream ss;
            ss << s;
            double t;
            string sRGB;
            ss >> t >> sRGB;
            vTimeStamps.push_back(t);
            vstrImages.push_back(strImagePath + "/" + sRGB);
        }
    }
}

// You need to implement this function to draw SLAM information on the image
cv::Mat DrawSLAMInfo(const cv::Mat &im, const cv::Mat &Tcw)
{
    // Implement drawing SLAM information (e.g., camera pose, tracked points) on the image
    // Return the modified image
    return im;
}