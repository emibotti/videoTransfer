import numpy as np
import cv2

cap = cv2.VideoCapture('../videoplayback')
cv2.namedWindow('frame', cv2.WINDOW_AUTOSIZE)

while(True):
    # Capture frame-by-frame

    ret, frame = cap.read()
    resize = cv2.resize(frame,(1000,500))
   
    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Display the resulting frame
    cv2.imshow('frame',frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()