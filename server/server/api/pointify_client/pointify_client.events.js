/**
 * PointifyClient model events
 */

'use strict';

import {EventEmitter} from 'events';
import PointifyClient from './pointify_client.model';
var PointifyClientEvents = new EventEmitter();

// Set max event listeners (0 == unlimited)
PointifyClientEvents.setMaxListeners(0);

// Model events
var events = {
  'calibrate' : 'calibrate'
};

// Register the event emitter to the model events
for (var e in events) {
  var event = events[e];
  PointifyClient.schema.post(e, emitEvent(event));
}

function emitEvent(event) {
  return function(doc) {
    PointifyClientEvents.emit(event + ':' + doc._id, doc);
    PointifyClientEvents.emit(event, doc);
  }
}

export default PointifyClientEvents;
