import cv2
import os

image_folder = "/home/almondgod/saved-images"
for filename in os.listdir(image_folder):
    if filename.endswith(".jpg") or filename.endswith(".png"): 
        img = cv2.imread(os.path.join(image_folder, filename))
        height, width = img.shape[:2]
        print(f"Image: {filename}, Dimensions: {width}x{height}")