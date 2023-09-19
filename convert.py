## Import redis module and base64 module
import redis
import base64
import cv2
import numpy as np


# Connect to redis server
r = redis.StrictRedis(host='localhost', port=6379, db=0)

# subscribe to redis channel camera0
p = r.pubsub()
p.subscribe('camera0')

for message in p.listen():
    if message['type'] == 'message':

        ## Read image file
        binary_image = r.get("camera0")

        ## Decode from JPEG  OK
      
        # read image as an numpy array
        image = np.frombuffer(binary_image, dtype="uint8")
      
        # use imdecode function
        image = cv2.imdecode(image, cv2.IMREAD_COLOR)
      
        # display image
        #cv2.imwrite("result.jpg", image)
        #cv2.imwrite("result.bmp", image)

        # Get the binary buffer from opencv Image
        bytes_img = image.tobytes()
        r.set('camera2', bytes_img); 
        r.set('camera2:width', "640");
        r.set('camera2:height', "480");
        r.set('camera2:channels', "3");
        
        r.publish("camera2", "{}")
        
        ### Encode to JPEG
        # binary_image = np.frombuffer(binary_image, dtype=np.uint8)
        # binary_image = binary_image.reshape(720, 1280, 3)

        # retval, buffer = cv2.imencode('.jpg', binary_image)
        # jpg_as_text = base64.b64encode(buffer)

        # r.set("camera0:base64", "data:image/jpg;base64," + str(jpg_as_text)[2:-1])
