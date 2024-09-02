import cv2
import numpy as np
import socket
import struct
import pickle
import os
import time
import socket
# Set up UDP socket
receiver_ip = '0.0.0.0'
receiver_port = 12345
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((receiver_ip, receiver_port))

# Buffer size
buf = 65536

# Directory to save images
save_dir = '/home/almondgod/saved-images'
os.makedirs(save_dir, exist_ok=True)

# File to save timestamps
timestamp_file = os.path.join(save_dir, 'timestamps.txt')

print(f"Listening on {receiver_ip}:{receiver_port}")

frame_count = 0
with open(timestamp_file, 'w') as f:
    while True:
        print("Waiting for data...")
        # Receive frame size
        try:
            data, addr = sock.recvfrom(struct.calcsize("L"))
            print(f"Received data from {addr}")
            size = struct.unpack("L", data)[0]
            print(f"Frame size: {size}")
            
            # Receive data
            data = b""
            while len(data) < size:
                packet, _ = sock.recvfrom(buf)
                data += packet
            
            # Decode frame
            frame = pickle.loads(data)
            frame = cv2.imdecode(frame, cv2.IMREAD_COLOR)
            
            # Save frame
            timestamp = time.time()
            filename = f'{frame_count:06d}.png'
            cv2.imwrite(os.path.join(save_dir, filename), frame)
            
            # Write timestamp to file
            f.write(f'{timestamp} {filename}\n')
            f.flush()
            
            frame_count += 1
            
            # Print confirmation message
            print(f"Received and saved image: {filename}")
            
            # Optional: Display frame (remove in production)
            cv2.imshow('Received Frame', frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        except Exception as e:
            print(f"Error: {e}")

cv2.destroyAllWindows()

# import cv2
# import numpy as np
# import socket
# import struct
# import pickle
# import os
# import time

# # Set up UDP socket
# receiver_ip = '0.0.0.0'  # Listen on all available interfaces
# receiver_port = 12345
# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# sock.bind((receiver_ip, receiver_port))

# # Buffer size
# buf = 65536

# # Directory to save images
# save_dir = '/home/almondgod/saved-images'  # Replace with your desired save path
# os.makedirs(save_dir, exist_ok=True)

# # File to save timestamps
# timestamp_file = os.path.join(save_dir, 'timestamps.txt')

# print(f"Listening on {receiver_ip}:{receiver_port}")

# frame_count = 0
# with open(timestamp_file, 'w') as f:
#     while True:
#         print("Waiting for data...")
#         try:
#             # Receive frame size
#             data, addr = sock.recvfrom(struct.calcsize("L"))
#             print(f"Received data from {addr}")
#             size = struct.unpack("L", data)[0]
#             print(f"Frame size: {size}")
            
#             # Receive data
#             data = b""
#             while len(data) < size:
#                 packet, _ = sock.recvfrom(buf)
#                 data += packet
            
#             # Decode frame
#             frame = pickle.loads(data)
#             frame = cv2.imdecode(frame, cv2.IMREAD_COLOR)
            
#             # Save frame
#             timestamp = time.time()
#             filename = f'{frame_count:06d}.png'
#             cv2.imwrite(os.path.join(save_dir, filename), frame)
            
#             # Write timestamp to file
#             f.write(f'{timestamp} {filename}\n')
#             f.flush()
            
#             frame_count += 1
            
#             # Print confirmation message
#             print(f"Received and saved image: {filename}")
            
#             # Optional: Display frame (remove in production)
#             cv2.imshow('Received Frame', frame)
#             if cv2.waitKey(1) & 0xFF == ord('q'):
#                 break
#         except Exception as e:
#             print(f"Error: {e}")

# cv2.destroyAllWindows()