import json, librosa
import numpy as np
from scipy.io import wavfile

def getData():
	with open('nsynth-valid/examples.json') as file:
	    data = json.load(file)
	rate = 16000
	xVal, yVal = [], []
	count = 0
	for k in data.keys():
	  count += 1
	  y = wavfile.read('nsynth-valid/audio/'+k+'.wav')[1]
	  S_full, phase = librosa.magphase(librosa.stft(y.astype(float)))
	  xVal.append(librosa.amplitude_to_db(S_full, ref=np.max))
	  yVal.append(data[k]['pitch'])
	print('val count:', count)
	print('val shape:', np.array(xVal[0]).shape, np.array(xVal).shape)
	with open('nsynth-test/examples.json') as file:
	    data = json.load(file)
	xTest, yTest = [], []
	count = 0
	for k in data.keys():
	  count += 1
	  y = wavfile.read('nsynth-test/audio/'+k+'.wav')[1]
	  S_full, phase = librosa.magphase(librosa.stft(y.astype(float)))
	  xTest.append(librosa.amplitude_to_db(S_full, ref=np.max))
	  yTest.append(data[k]['pitch'])
	print('test count:', count)
	print('test shape:', np.array(xTest[0]).shape, np.array(xTest).shape)
	return xVal, yVal, xTest, yTest











