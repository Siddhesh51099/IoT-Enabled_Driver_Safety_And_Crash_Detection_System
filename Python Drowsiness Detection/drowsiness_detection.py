import cv2
import dlib
from imutils import face_utils
from scipy.spatial import distance as dist
import time
import winsound

def eye_aspect_ratio(eye):
    # Compute the euclidean distances between the vertical eye landmarks
    A = dist.euclidean(eye[1], eye[5])
    B = dist.euclidean(eye[2], eye[4])
    # Compute the euclidean distance between the horizontal eye landmarks
    C = dist.euclidean(eye[0], eye[3])
    # Compute the eye aspect ratio
    ear = (A + B) / (2.0 * C)
    return ear

# Define constants for EAR threshold and consecutive frame threshold
EYE_AR_THRESH = 0.25
EYE_AR_CONSEC_FRAMES = 48  # Adjust this number based on your camera frame rate

# Initialize the frame counter and the alarm status
COUNTER = 0
ALARM_ON = False

# Initialize dlib's face detector (HOG-based) and the facial landmark predictor
print("Loading facial landmark predictor...")
detector = dlib.get_frontal_face_detector()
predictor = dlib.shape_predictor("model/shape_predictor_68_face_landmarks.dat")

# Grab the indexes of the facial landmarks for the left and right eye
(lStart, lEnd) = face_utils.FACIAL_LANDMARKS_IDXS["left_eye"]
(rStart, rEnd) = face_utils.FACIAL_LANDMARKS_IDXS["right_eye"]

# Start the video stream
print("Starting video stream...")
cap = cv2.VideoCapture(0)
time.sleep(1.0)

# Create a resizable window and set its size
cv2.namedWindow("Frame", cv2.WINDOW_NORMAL)
cv2.resizeWindow("Frame", 1000, 800)

while True:
    ret, frame = cap.read()
    if not ret:
        break
        
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    rects = detector(gray, 0)
    
    for rect in rects:
        shape = predictor(gray, rect)
        shape = face_utils.shape_to_np(shape)
        
        # Extract the left and right eye coordinates and compute the EAR for each eye
        leftEye = shape[lStart:lEnd]
        rightEye = shape[rStart:rEnd]
        leftEAR = eye_aspect_ratio(leftEye)
        rightEAR = eye_aspect_ratio(rightEye)
        
        # Average the EAR together for both eyes
        ear = (leftEAR + rightEAR) / 2.0
        
        # Draw the eye regions on the frame for visualization
        leftEyeHull = cv2.convexHull(leftEye)
        rightEyeHull = cv2.convexHull(rightEye)
        cv2.drawContours(frame, [leftEyeHull], -1, (0, 255, 0), 1)
        cv2.drawContours(frame, [rightEyeHull], -1, (0, 255, 0), 1)
        
        # Check if the EAR is below the blink threshold
        if ear < EYE_AR_THRESH:
            COUNTER += 1
            
            # If the eyes were closed for sufficient frames, trigger an alarm
            if COUNTER >= EYE_AR_CONSEC_FRAMES:
                if not ALARM_ON:
                    ALARM_ON = True
                    winsound.Beep(2000, 500)
                    print("Drowsiness detected! Wake up!")
                    # Optionally, you can add a sound alert here
                cv2.putText(frame, "DROWSINESS ALERT!", (10, 30),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
        else:
            COUNTER = 0
            ALARM_ON = False
        
        # Optionally, display the EAR on the frame
        cv2.putText(frame, f"EAR: {ear:.2f}", (300, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
    
    cv2.imshow("Frame", frame)
    
    # Press 'q' to quit the video stream
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()