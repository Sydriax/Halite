#!/bin/bash

virtualenv ./

source bin/activate
pip install numpy scipy scikit-learn pillow h5py
pip install --upgrade --no-deps git+git://github.com/Theano/Theano.git
pip install keras
deactivate

