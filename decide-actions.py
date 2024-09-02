import numpy as np
import os
import time
import socket

def load_mappoints(file_path):
    mappoints = []
    with open(file_path, 'r') as f:
        for line in f:
            x, y, z = map(float, line.strip().split())
            mappoints.append((x, y, z))
    return np.array(mappoints)

def find_open_direction(mappoints):
    # Assuming the x-axis is the forward direction
    # Find the point with the largest x value
    max_x_point = max(mappoints, key=lambda p: p[0])
    
    # Calculate the angle to this point in the x-y plane
    angle = np.arctan2(max_x_point[1], max_x_point[0])
    
    # Convert angle to degrees
    angle_deg = np.degrees(angle)
    
    # Determine the direction
    if abs(angle_deg) < 15:
        return "forward"
    elif angle_deg > 0:
        return "left"
    else:
        return "right"

def send_command(command, ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(command.encode(), (ip, port))

def main():
    mappoints_file = "KeyFrameTrajectory.txt"  # File saved by ORB-SLAM3
    raspberry_pi_ip = "192.168.1.100"  # Replace with your Raspberry Pi's IP
    raspberry_pi_port = 12346  # Choose an appropriate port
    
    while True:
        if os.path.exists(mappoints_file):
            mappoints = load_mappoints(mappoints_file)
            direction = find_open_direction(mappoints)
            
            print(f"Recommended direction: {direction}")
            
            # Send the direction to the Raspberry Pi
            send_command(direction, raspberry_pi_ip, raspberry_pi_port)
        else:
            print("Waiting for SLAM to generate data...")
        
        time.sleep(1)  # Wait for 1 second before checking again

if __name__ == "__main__":
    main()