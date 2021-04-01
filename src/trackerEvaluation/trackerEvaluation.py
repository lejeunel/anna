'''
Script that creates a list of TP, FP, FN and TN from the eye tracker file and
the ground truth. If multiple eye tracker files are given each one is evaluated,
the resluts of all the files are then summarized in an additional file.
'''
import numpy as np
import scipy.misc
import matplotlib.pyplot as plt
from skimage.draw import circle
import os
import subprocess
import shutil


def gazeCoord2Pixel(x, y, width, height):
    j = int(np.round(x*(width-1),0))
    i = int(np.round(y*(height-1),0))
    return i,j

def getFrameList(directory):
    fileList = [ ]
    for file in os.listdir(directory):
        if file.endswith(".png"):
            fileList.append(os.path.join(directory, file))
    fileList.sort()
    return fileList

def evaluate(gazeDataFile, groundTruth, coordinates='avg', onlyFixatedData=False):
    gaze = np.loadtxt(open(gazeDataFile,"rb"),delimiter=";",skiprows=7)
    if onlyFixatedData:
        gaze = gaze[gaze[:,7]==True, :]
    fileList = getFrameList(groundTruth)
    n = len(fileList) # number of frames
    # array with frame index as rows and columns: #tp, #fp, #fn, #tn
    A = np.zeros((n,5),'int')
    for frameIdx in range(n):
        gazeDataFrame = gaze[gaze[:,0]==frameIdx, :]
        if gazeDataFrame.shape[0] == 0:
            continue
        img = scipy.misc.imread(fileList[frameIdx], flatten=True)
        height,width = img.shape
        for i in range(gazeDataFrame.shape[0]):
            if coordinates == 'raw':
                x,y = gazeCoord2Pixel(gazeDataFrame[i,3],gazeDataFrame[i,4],width,height)
            else:
                x,y = gazeCoord2Pixel(gazeDataFrame[i,5],gazeDataFrame[i,6],width,height)
            if x<0 or x>=img.shape[0] or y<0 or y>=img.shape[1]:
                label = False;
            else:
                label = img[x,y]>0
            user = bool(gazeDataFrame[i,2])
            if label == True:
                if user == label:
                    A[frameIdx, 0] += 1
                else:
                    A[frameIdx, 2] += 1
            else:
                if user == label:
                    A[frameIdx, 3] += 1
                else:
                    A[frameIdx, 1] += 1
    return A

def save2File(outFile, A, v=None):
    sums = np.sum(A, axis=0)
    f = open(outFile, 'w')
    f.write('total tp;%d\ntotal fp;%d\ntotal fn;%d\ntotal tn;%d\n'%(sums[0],sums[1],sums[2],sums[3]))
    n = A.shape[0]
    if v is not None:
        f.write('\n ;#tp;#fp;#fn;#tn')
        for i in range(n):
            f.write('\n%s;%d;%d;%d;%d'%(v[i], A[i,0], A[i,1], A[i,2], A[i,3]))
    else:
        f.write('\n#tp;#fp;#fn;#tn')
        for i in range(n):
            f.write('\n%d;%d;%d;%d'%(A[i,0], A[i,1], A[i,2], A[i,3]))


if __name__ == '__main__':
    # brain tumor
    groundTruth = '/Volumes/Home/severin.tobler/eyeTrackerData/Dataset9/ground_truth-frames'
    pathGazeData = '../../data/evaluation/video_brats/'
    gazeDataFiles = ['video_brats_test2.csv']#, 'video_brats_20170222_161116.csv']
    coordinates = 'avg' # 'raw' otherwise averaged coordinates
    pathOut = '../../data/evaluation/video_brats/results'
    outFile = 'overview_video_brats.csv'
    prefix = 'avgFixated_'
    '''
    # slit lamp video
    groundTruth = '/Volumes/Home/severin.tobler/eyeTrackerData/Dataset12/ground_truth-frames'
    pathGazeData = '../../data/evaluation/slit_lamp/'
    gazeDataFiles = ['slit_lamp_20170222_133614.csv', 'slit_lamp_20170222_134033.csv', 'slit_lamp_20170222_134620.csv', 'slit_lamp_20170222_134941.csv', 'slit_lamp_20170222_135956.csv']
    coordinates = 'avg' # 'raw' otherwise averaged coordinates
    pathOut = '../../data/evaluation/slit_lamp/results'
    outFile = 'overview_slit_lamp.csv'
    prefix = ''
    '''

    n = len(gazeDataFiles)
    sums = np.empty((n, 4))
    for i in range(n):
        A = evaluate(os.path.join(pathGazeData, gazeDataFiles[i]), groundTruth, coordinates=coordinates, onlyFixatedData=True)
        outFileName = prefix+gazeDataFiles[i]
        save2File(os.path.join(pathOut, outFileName), A)
        sums[i,:] = np.sum(A, axis=0)

    if len(gazeDataFiles)>1:
        save2File(os.path.join(pathOut, prefix+outFile), sums, gazeDataFiles)
