/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_LITE_MICRO_EXAMPLES_gesture_DETECTION_EXPERIMENTAL_MODEL_SETTINGS_H_
#define TENSORFLOW_LITE_MICRO_EXAMPLES_gesture_DETECTION_EXPERIMENTAL_MODEL_SETTINGS_H_

// Keeping these as constant expressions allow us to allocate fixed-sized arrays
// on the stack for our working memory.

// All of these values are derived from the values used during model training,
// if you change your model you'll need to update these constants.
constexpr int gesture_NumCols = 80;
constexpr int gesture_NumRows = 60;
constexpr int gesture_NumChannels = 1;

constexpr int gesture_MaxImageSize = gesture_NumCols * gesture_NumRows * gesture_NumChannels;

constexpr int gesture_CategoryCount = 5;

constexpr int upIndex = 0;
constexpr int downIndex = 1;
constexpr int leftIndex = 2;
constexpr int rightIndex = 3;
constexpr int nothingIndex = 4;

extern const char* gesture_CategoryLabels[gesture_CategoryCount];

#endif  // TENSORFLOW_LITE_MICRO_EXAMPLES_PERSON_DETECTION_EXPERIMENTAL_MODEL_SETTINGS_H_
