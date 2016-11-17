'use strict';

import mongoose from 'mongoose';

var PointifyClientSchema = new mongoose.Schema({
  id : Number,
});

export default mongoose.model('PointifyClient', PointifyClientSchema);
