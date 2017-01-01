'use strict';

import mongoose from 'mongoose';

var RecordingSchema = new mongoose.Schema({
  name: String,
  info: String,
  active: Boolean
});

export default mongoose.model('Recording', RecordingSchema);
