#%%
import socket
import cv2 as cv
import numpy as np
import os

""" Send structure:
! = Break character

s [angle to move to]! -> Moves the sevo to a given angle
"""           

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # Random ip stuff
        s.bind((HOST, PORT))
        s.listen(1)
        print("Waiting for arduino to connect")
        conn, addr = s.accept()
        print("Arduino connected!")

        cv.namedWindow('Frame', cv.WINDOW_AUTOSIZE)

        camera = cv.VideoCapture(0)
        if not camera.isOpened():
            print("not Working")
            return 1
        camera.set(cv.CAP_PROP_AUTOFOCUS, 0)
        camera.set(cv.CAP_PROP_FPS, 30)
        camera.set(cv.CAP_PROP_FRAME_WIDTH, 1024)
        camera.set(cv.CAP_PROP_FRAME_HEIGHT, 720)
        camera.set(28, 0)
        #camera.set(cv.CAP_PROP_ZOOM, 0)

        #settingsfile = "settings.txt"

        #if os .path exists('settings')

        loop = True
        while loop:
            ret, frame = camera.read()
                
            if not ret:
                continue

            r = 50
            roi = frame[360-r:360+r, 512-r:512+r]

            w = roi.shape[1]
            h = roi.shape[0]

            frame = cv.resize(roi, (w * 5, h * 5))

            frameHSV = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

            copper = cv.inRange(frameHSV,np.array([5,50,50]),np.array([45,255,255]))
            median = cv.medianBlur(copper,5)
            edges = cv.Canny(median,124,255)
            kernel = np.ones((1,1),np.uint8)
            dialation = cv.dilate(edges,kernel,iterations = 1)

            _,contours, hierarchy = cv.findContours(dialation,cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)
            cv.imshow('copper', copper)
            cv.imshow('median',median)
            cv.imshow('edges',edges)
            cv.imshow('dialation', dialation)
            #kernel = np.ones((3,3), np.uint8)
            #cv.drawContours(copper,contours,-1,(0,255,0),2)
            #image = cv.copyTo(frame,frame)
            cv.drawContours(frame,contours,-1,(0,255,0),2)

            if len(contours) > 1:
                cnt = contours[0]
                cnt2 = contours[1]
                x,y,w,h = cv.boundingRect(cnt)
                x2,y2,w2,h2 = cv.boundingRect(cnt2)
                cv.rectangle(frame,(x,y), (x+w,y+h), (0,255,0),2)
                cv.rectangle(frame,(x2,y2), (x2+w2,y2+h2), (255,0,0),2)
                rect = cv.minAreaRect(cnt)
                box = cv.boxPoints(rect)
                box = np.int0(box)
                #cv.drawContours(frame,[box],0,(0,0,255),2)
                cv.imshow('Frame', frame)
                #print(f"{x-x2}")
                mm = abs(x-(x2 + w2))/39.99
            #conn.send(bytes(str(mm) + '\n','utf8'))
            #print(f"{mm}")
                if mm >= 0.5: 
                #print(f"{w}")
                    print(f"{mm}")
                    conn.send(bytes(str(mm) + '\n','utf8'))

        

            if cv.waitKey(1) == 27:
                loop = False  

        s.close()
        camera.release()

if __name__ == "__main__":

    # Ip config
    HOST = '192.168.2.2'
    PORT = 50007
    main()  # Run the main code
 
#%%
