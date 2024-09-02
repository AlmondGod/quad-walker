#include "System.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <sophus/se3.hpp>

// Forward declare the IMU::Point struct
namespace ORB_SLAM3 {
    namespace IMU {
        struct Point;
    }
}

cv::Mat receiveImageFromUDP();

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        std::cerr << std::endl << "Usage: ./mono_inertial_slam path_to_vocabulary path_to_settings" << std::endl;
        return 1;
    }

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM3::System SLAM(argv[1], argv[2], ORB_SLAM3::System::MONOCULAR, true);

    // Vector for tracking time statistics
    std::vector<float> vTimesTrack;
    vTimesTrack.reserve(5000);

    // Main loop
    cv::Mat im;
    for(;;)
    {
        // Read image from your Raspberry Pi camera stream
        im = receiveImageFromUDP();
        if(im.empty())
        {
            std::cerr << std::endl << "Failed to load image" << std::endl;
            return 1;
        }

        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

        // Get current timestamp
        double timestamp = std::chrono::duration_cast<std::chrono::duration<double>>(t1.time_since_epoch()).count();

        // Prepare empty IMU data (since we're not using IMU)
        std::vector<ORB_SLAM3::IMU::Point> vImuMeas;

        // Pass the image to the SLAM system
        Sophus::SE3f Tcw = SLAM.TrackMonocular(im, timestamp, vImuMeas, "");

        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

        double ttrack = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();

        vTimesTrack.push_back(ttrack);

        // Wait to load the next frame
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Stop all threads
    SLAM.Shutdown();

    // Tracking time statistics
    sort(vTimesTrack.begin(),vTimesTrack.end());
    float totaltime = 0;
    for(int ni=0; ni<vTimesTrack.size(); ni++)
    {
        totaltime+=vTimesTrack[ni];
    }
    std::cout << "-------" << std::endl << std::endl;
    std::cout << "median tracking time: " << vTimesTrack[vTimesTrack.size()/2] << std::endl;
    std::cout << "mean tracking time: " << totaltime/vTimesTrack.size() << std::endl;

    return 0;
}

cv::Mat receiveImageFromUDP() {
    static int sock = -1;
    static struct sockaddr_in server_addr;
    
    if (sock == -1) {
        // Initialize socket only once
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(12345);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Error binding socket" << std::endl;
            exit(1);
        }
    }

    // Receive frame size
    uint32_t size;
    ssize_t received = recv(sock, &size, sizeof(size), 0);
    if (received != sizeof(size)) {
        std::cerr << "Error receiving size" << std::endl;
        return cv::Mat();
    }

    // Receive data
    std::vector<char> buffer(size);
    size_t total_received = 0;
    while (total_received < size) {
        received = recv(sock, buffer.data() + total_received, size - total_received, 0);
        if (received <= 0) {
            std::cerr << "Error receiving data" << std::endl;
            return cv::Mat();
        }
        total_received += received;
    }

    // Decode frame
    cv::Mat frame = cv::imdecode(cv::Mat(buffer), cv::IMREAD_COLOR);
    if (frame.empty()) {
        std::cerr << "Error decoding image" << std::endl;
        return cv::Mat();
    }

    return frame;
}