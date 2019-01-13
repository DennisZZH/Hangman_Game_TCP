from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from readData import getData
# Imports
import numpy as np
import tensorflow as tf

# tf.logging.set_verbosity(False)
tf.logging.set_verbosity(tf.logging.INFO)
# Our application logic will be added here
def cnn_model_fn(features, labels, mode):
  print('features.shape:',features['x'].shape)
  input_layer = tf.reshape(features["x"], [-1, 1025, 126,1])
  print('input.shape:',input_layer.shape)
  # Convolutional Layer #1 and Pooling Layer #1
  conv1 = tf.layers.conv2d(
      inputs=input_layer,
      filters=8,
      kernel_size=[2, 1],
      padding="same",
      activation=tf.nn.relu)
  print('conv1.shape:',conv1.shape)
  pool1 = tf.layers.max_pooling2d(inputs=conv1, pool_size=[2, 1], strides=1)
  print('pool1.shape:',pool1.shape)#1024,126
  # Convolutional Layer #2 and Pooling Layer #2
  conv2 = tf.layers.conv2d(
      inputs=pool1,
      filters=16,
      kernel_size=[2, 1],
      padding="same",
      activation=tf.nn.relu)
  print('conv2.shape:',conv2.shape)
  pool2 = tf.layers.max_pooling2d(inputs=conv2, pool_size=[2, 1], strides=[2,1])
  print('pool2.shape:',pool2.shape)#512,126
  # Convolutional Layer #3 and Pooling Layer #3
  conv3 = tf.layers.conv2d(
      inputs=pool2,
      filters=32,
      kernel_size=[3, 2],
      padding="same",
      activation=tf.nn.relu)
  print('conv3.shape:',conv3.shape)
#
  pool3 = tf.layers.max_pooling2d(inputs=conv3, pool_size=[2, 2], strides=2)
  print('pool3.shape:',pool3.shape)#256, 63
  # Convolutional Layer #4 and Pooling Layer #4
  conv4 = tf.layers.conv2d(
      inputs=pool3,
      filters=48,
      kernel_size=[3, 3],
      padding="same",
      activation=tf.nn.relu)
  print('conv4.shape:',conv4.shape)
  pool4 = tf.layers.max_pooling2d(inputs=conv4, pool_size=[2, 1], strides=[2,1])
  print('pool4.shape:',pool4.shape)
#128,63
  # Dense Layer
  pool4_flat = tf.reshape(pool4, [-1, 128*63*48])
  dense = tf.layers.dense(inputs=pool4_flat, units=1024, activation=tf.nn.relu)
  dropout = tf.layers.dropout(
      inputs=dense, rate=0.2, training=mode == tf.estimator.ModeKeys.TRAIN)

  # Logits Layer
  logits = tf.layers.dense(inputs=dropout, units=128)

  predictions = {
      # Generate predictions (for PREDICT and EVAL mode)
      "classes": tf.argmax(input=logits, axis=1),
      # Add `softmax_tensor` to the graph. It is used for PREDICT and by the
      # `logging_hook`.
      "probabilities": tf.nn.softmax(logits, name="softmax_tensor")
  }

  if mode == tf.estimator.ModeKeys.PREDICT:
    return tf.estimator.EstimatorSpec(mode=mode, predictions=predictions)

  # Calculate Loss (for both TRAIN and EVAL modes)
  loss = tf.losses.sparse_softmax_cross_entropy(labels=labels, logits=logits)

  # Configure the Training Op (for TRAIN mode)
  if mode == tf.estimator.ModeKeys.TRAIN:
    optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.0001)
    train_op = optimizer.minimize(
        loss=loss,
        global_step=tf.train.get_global_step())
    return tf.estimator.EstimatorSpec(mode=mode, loss=loss, train_op=train_op)

  # Add evaluation metrics (for EVAL mode)
  eval_metric_ops = {
      "accuracy": tf.metrics.accuracy(
          labels=labels, predictions=predictions["classes"])}
  return tf.estimator.EstimatorSpec(
      mode=mode, loss=loss, eval_metric_ops=eval_metric_ops)

import os, random
from math import log
from glob import glob
from scipy.io import wavfile
from scipy.fftpack import fft

def main(args):
  # Load training and eval data
  xTrain, yTrain, xVal, yVal = getData()
  # random.shuffle(data)
  # xTrain = [d['timeSample'] for d in data[:2200]]
  # yTrain = [d[] for d in data[:2200]]

  # Create the Estimator
  classifier = tf.estimator.Estimator(
      model_fn=cnn_model_fn, model_dir="ABCDEFG")

  # Set up logging for predictions
  tensors_to_log = {"probabilities": "softmax_tensor"}
  logging_hook = tf.train.LoggingTensorHook(
      tensors=tensors_to_log, every_n_iter=50)

  # Train the model
  train_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": xTrain},
      y=yTrain,
      batch_size=100,
      num_epochs=None,
      shuffle=True)
  classifier.train(
      input_fn=train_input_fn,
      steps=10,
      hooks=[logging_hook])

  # Evaluate the model and print results
  eval_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": xVal},
      y=yVal,
      num_epochs=10,
      shuffle=False)
  eval_results = classifier.evaluate(input_fn=eval_input_fn)
  print(eval_results)

if __name__ == "__main__":
  tf.app.run()