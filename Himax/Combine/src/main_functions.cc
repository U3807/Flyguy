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
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "main_functions.h"
#include "detection_responder.h"
#include "image_provider.h"

#include "person_detect_model_settings.h"
#include "person_detect_model_data.h"

#include "gesture_model_settings.h"
#include "gesture_model_data.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* pic_error_reporter = nullptr;
const tflite::Model* pic_model = nullptr;
tflite::MicroInterpreter* pic_interpreter = nullptr;
TfLiteTensor* pic_input = nullptr;

// In order to use optimized tensorflow lite kernels, a signed int8_t quantized
// model is preferred over the legacy unsigned model format. This means that
// throughout this project, input images must be converted from unisgned to
// signed format. The easiest and quickest way to convert from unsigned to
// signed 8-bit integers is to subtract 128 from the unsigned value to get a
// signed value.

// An area of memory to use for input, output, and intermediate arrays.
constexpr int pic_TensorArenaSize = 100 * 1024;
static uint8_t pic_tensor_arena[pic_TensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
void pic_setup() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  pic_error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  pic_model = tflite::GetModel(g_person_detect_model_data);
  if (pic_model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(pic_error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         pic_model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  //
  // tflite::AllOpsResolver resolver;
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroMutableOpResolver<5> micro_op_resolver;
  micro_op_resolver.AddAveragePool2D();
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddDepthwiseConv2D();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddSoftmax();

  // Build an interpreter to run the model with.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroInterpreter static_interpreter(
      pic_model, micro_op_resolver, pic_tensor_arena, pic_TensorArenaSize, pic_error_reporter);
  pic_interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = pic_interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(pic_error_reporter, "pic AllocateTensors() failed");
    return;
  }

  // Get information about the memory area to use for the model's input.
  pic_input = pic_interpreter->input(0);
}

// The name of this function is important for Arduino compatibility.
int pic_loop() {
  // Get image from provider.
  if (kTfLiteOk != GetImage(pic_error_reporter, person_NumCols, person_NumRows, person_NumChannels,
                            pic_input->data.int8)) {
    TF_LITE_REPORT_ERROR(pic_error_reporter, "Image capture failed.");
  }

  // Run the model on this input and make sure it succeeds.
  if (kTfLiteOk != pic_interpreter->Invoke()) {
    TF_LITE_REPORT_ERROR(pic_error_reporter, "Invoke failed.");
  }

  TfLiteTensor* pic_output = pic_interpreter->output(0);

  // Process the inference results.
  int8_t person_score = pic_output->data.uint8[person_PersonIndex];
  int8_t no_person_score = pic_output->data.uint8[person_NotAPersonIndex];
  RespondToDetection(pic_error_reporter, person_score, no_person_score);
  if(person_score>100){
    return 1;
  }
  else{
    return 0;
  }
}


/////////////////////////////////////GESTURE/////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* gesture_error_reporter = nullptr;
const tflite::Model* gesture_model = nullptr;
tflite::MicroInterpreter* gesture_interpreter = nullptr;
TfLiteTensor* gesture_input = nullptr;
TfLiteTensor* gesture_output = nullptr;

// In order to use optimized tensorflow lite kernels, a signed int8_t quantized
// model is preferred over the legacy unsigned model format. This means that
// throughout this project, input images must be converted from unisgned to
// signed format. The easiest and quickest way to convert from unsigned to
// signed 8-bit integers is to subtract 128 from the unsigned value to get a
// signed value.
// An area of memory to use for input, output, and intermediate arrays.

constexpr int gesture_TensorArenaSize = 212 * 1024; //343376
static uint8_t gesture_tensor_arena[gesture_TensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
void gesture_setup() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  gesture_error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  gesture_model = tflite::GetModel(gesture_model_data);
  if (gesture_model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(gesture_error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         pic_model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  //
  // tflite::AllOpsResolver resolver;
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroMutableOpResolver<6> micro_op_resolver;
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddRelu();
  micro_op_resolver.AddFullyConnected();

  // Build an interpreter to run the model with.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroInterpreter static_interpreter(
      gesture_model, micro_op_resolver, gesture_tensor_arena, gesture_TensorArenaSize, gesture_error_reporter);
  gesture_interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = gesture_interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(gesture_error_reporter, "gesture_AllocateTensors() failed");
    return;
  }

  // Get information about the memory area to use for the model's input.
  gesture_input = gesture_interpreter->input(0);
  gesture_output = gesture_interpreter->output(0);
}

// The name of this function is important for Arduino compatibility.
int gesture_loop() {
  // Get image from provider.
  if (kTfLiteOk != GetImage(gesture_error_reporter, gesture_NumCols, gesture_NumRows, gesture_NumChannels,
                            gesture_input->data.int8)) {
    TF_LITE_REPORT_ERROR(gesture_error_reporter, "Image capture failed.");
  }

  // Run the model on this input and make sure it succeeds.
  if (kTfLiteOk != gesture_interpreter->Invoke()) {
    TF_LITE_REPORT_ERROR(gesture_error_reporter, "Invoke failed.");
  }

  TfLiteTensor* gesture_output = gesture_interpreter->output(0);

  // Process the inference results.
  int8_t* results_ptr = gesture_output->data.int8;
  int result = std::distance(results_ptr, std::max_element(results_ptr, results_ptr + 5));

  TF_LITE_REPORT_ERROR(gesture_error_reporter, "predicted direction %s ",gesture_CategoryLabels[result]);
  TF_LITE_REPORT_ERROR(gesture_error_reporter, "output value %d ",*(results_ptr+result));

  return result;
}