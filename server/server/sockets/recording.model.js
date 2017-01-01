'use strict';

import mongoose from 'mongoose';

var RecordingSchema = new mongoose.Schema({
  name : String,
  frames : [Buffer],
  frameTimes : [Number],
});

export default mongoose.model('Recording', RecordingSchema);
