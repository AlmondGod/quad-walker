import sys
import cv2
import numpy as np
import time
import ORB_SLAM3

# Import your frame receiving code
from image_receiver import get_latest_frame

# Initialize ORB-SLAM3
vocab_path = "ORB_SLAM3/Vocabulary/ORBvoc.txt"
settings_path = "ORB_SLAM3/Examples/Monocular/raspicam_v2_1640x1232.yaml"
use_viewer = True

slam = ORB_SLAM3.System(vocab_path, settings_path, ORB_SLAM3.Sensor.MONOCULAR)
slam.SetUseViewer(use_viewer)

while True:
    frame = get_latest_frame()
    if frame is not None:
        # Convert frame to grayscale
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        
        # Get current timestamp
        timestamp = time.time()
        
        # Pass the frame to ORB-SLAM3
        slam.TrackMonocular(gray, timestamp)
    
    # Check for keyboard interrupt
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Stop all threads and save camera trajectory
slam.Shutdown()
slam.SaveTrajectoryTUM("CameraTrajectory.txt")
slam.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt")