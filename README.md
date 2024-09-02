
## Context
This code is run from within a docker container, with port 12345 set up to forward to this container

## Goal 
Our goal is:
1. Using [image-reciever.py](image-reciever.py), constantly detect the sequence of picamerav2 images from the on-robot raspberrypi 4gb 4b 
2. save these images (both images and timestamps) to folders within saved_sequences folder as according to the mono_custom.cc image and timsestep format
3. Using [mono_custom.cc](../ORB_SLAM3/Examples/Monocular/mono_custom.cc), take in these images and run monocular ORB_SLAM3, 
resulting in keyframe and mappoint structures
4. Save the images modified by SLAM::TrackMonocular to our regular macos (non docker) for visualization
5. Save these structures in docker system
6. Using [decide-actions.py](decide-actions.py), analyze the keyframe and mappoint structures to decide on the most 'open' direction on the x axis (which direction has the furthest distance point on the map)
7. Using decision, decide on exact timing of signals to arduino `left`, `right`, or `forward` (once facing the correct direction the command should always be forward)
7. Send this decision back to the raspberrypi frame

# Visualization
1. For the code on the macos, access the saved modified images
2. Visualize these images using Panglon (or something else better for macos if better)

# Raspberry pi
1. Takes images using the picamera module v2 every 5 seconds then sends them to macos port 12345 (ip address given) 
2. Upon recieving the direction decisions and timings, execute these by sending them to the arduino via an attached port

# Arduino 
1. Constantly listen for and recieve from port attached to raspberrypi signals `left`, `right`, or `forward`
2. Run function correspodning to most recently recieved signal. 
`left` moves forward and left by moving the right side legs more than the left,
`right` moves forward and right by moving the left side legs more than the right,
`forward` moves straight forward