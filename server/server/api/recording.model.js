'use strict';

import mongoose from 'mongoose';

var FrameSchema = new mongoose.Schema({
  frameParts : [Buffer],
});

var RecordingSchema = new mongoose.Schema({
  name : String,
  frames : [FrameSchema],
  frameTimes : [Number],
});

export default mongoose.model('Recording', RecordingSchema);
