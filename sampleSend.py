from scipy.io import wavfile
import numpy as np
import socket
import time

UDP_IP = "192.168.0.102"
UDP_PORT = 50005

fs, rawData = wavfile.read('/Users/joab/Desktop/P2_PYTHON/WAV/Flames.wav')

normalizedData = rawData #+ 32768
#normalizedData = normalizedData.astype(np.uint16)
#normalizedData = normalizedData >> 4

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

def startTransmit(packetSize, transferRate, divider):
    trimData = [y for x,y in enumerate(normalizedData) if x%divider == 0]
    trimData = np.asarray(trimData, dtype=np.uint16)
    currPacketIndex = 0
    while True:
        #print (normalizedData[currPacketIndex : currPacketIndex + packetSize])
        time.sleep(transferRate)
        sock.sendto(trimData[currPacketIndex : currPacketIndex + packetSize],(UDP_IP,UDP_PORT))
            #print currPacketIndex
        currPacketIndex = currPacketIndex + 1
        if(currPacketIndex > len(trimData)):
            currPacketIndex = 0

def dataPlot(): #Acomodamos los datos para plotearlos:
    duration = len(normalizedData) / fs
    t = np.arange(0,duration,1/float(fs))
    data = np.resize(normalizedData,len(t))
    from matplotlib import pyplot as plt
    plt.plot(t,data)
    plt.show()



