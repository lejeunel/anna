'''
Script that extracts the frames from a video and draws the gaze points on each
frame. Green points if the user indicated they see the object and red otherwise.
Light points if fixated according to eye tracker, dark ones otherwise.
Uses avconv to extract video frames.
'''
import numpy as np
import scipy.misc
import matplotlib.pyplot as plt
import os.path
from skimage.draw import circle
import subprocess


def gazeCoord2Pixel(x,y,width,height):
    j = int(np.round(x*(width-1),0))
    i = int(np.round(y*(height-1),0))
    return i,j

def drawDots(frameNumber, gaze, img, coordinates='avg', radius=7):
    gazeDataFrame = gaze[gaze[:,0]==frameNumber, :]
    if gazeDataFrame.shape[0] == 0:
        return img
    height,width,_ = img.shape
    for i in range(gazeDataFrame.shape[0]):
        if coordinates == 'raw':
            x,y = gazeCoord2Pixel(gazeDataFrame[i,3],gazeDataFrame[i,4],width,height)
        else:
            x,y = gazeCoord2Pixel(gazeDataFrame[i,5],gazeDataFrame[i,6],width,height)
        if x<0+radius or x>=img.shape[0]-radius or y<0+radius or y>=img.shape[1]-radius:
            continue
        rr,cc = circle(x,y,radius)
        if bool(gazeDataFrame[i,2]):
            if bool(gazeDataFrame[i,7]):
                img[rr,cc,0] = 0
                img[rr,cc,1] = 255
                img[rr,cc,2] = 0
            else:
                img[rr,cc,0] = 0
                img[rr,cc,1] = 100
                img[rr,cc,2] = 0
        else:
            if bool(gazeDataFrame[i,7]):
                img[rr,cc,0] = 255
                img[rr,cc,1] = 0
                img[rr,cc,2] = 0
            else:
                img[rr,cc,0] = 100
                img[rr,cc,1] = 0
                img[rr,cc,2] = 0
    return img

def extractFrames(videoFile, des):
    cmdline = ['avconv', '-i', videoFile, '-f', 'image2', des+'%04d.png',]
    subprocess.call(cmdline)
    fileList = [ ]
    for file in os.listdir(des):
        if file.endswith(".png"):
            fileList.append(file)
    fileList.sort()
    return fileList

if __name__ == '__main__':
    # brain tumor
    videoFile = '../../data/evaluation/video_brats/video_brats.avi'
    groundTruth = '/Volumes/Home/severin.tobler/eyeTrackerData/Dataset9/ground_truth-frames/'
    gazeDataFile = '../../data/evaluation/video_brats/video_brats_test2.csv'
    coordinates = 'avg' # 'raw' otherwise averaged coordinates
    outPath = '../../data/evaluation/video_brats/temp/'
    '''
    # slit lamp video
    videoFile = '../../data/evaluation/slit_lamp/slit_lamp.avi'
    groundTruth = '/Volumes/Home/severin.tobler/eyeTrackerData/Dataset12/ground_truth-frames/'
    gazeDataFile = '../../data/evaluation/slit_lamp/slit_lamp_20170222_134033.csv'
    coordinates = 'avg' # 'raw' otherwise averaged coordinates
    outPath = '../../data/evaluation/slit_lamp/temp/'
    '''

    gaze = np.loadtxt(open(gazeDataFile,"rb"),delimiter=";",skiprows=7)
    fileList = extractFrames(videoFile, outPath)
    n = len(fileList)
    for i in range(n):
        img = scipy.misc.imread(outPath+fileList[i])
        mask = scipy.misc.imread(groundTruth+'frame_%04d.png'%(i+1), flatten=True)>0
        img[:,:,2] = np.minimum(img[:,:,2]+mask*100, 255)
        img = drawDots(i, gaze, img, coordinates=coordinates)
        scipy.misc.imsave(outPath+'%04d.png' %(i+1), img)
