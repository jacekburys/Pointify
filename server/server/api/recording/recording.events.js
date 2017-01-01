/**
 * Recording model events
 */

'use strict';

import {EventEmitter} from 'events';
import Recording from './recording.model';
var RecordingEvents = new EventEmitter();

// Set max event listeners (0 == unlimited)
RecordingEvents.setMaxListeners(0);

// Model events
var events = {
  'save': 'save',
  'remove': 'remove'
};

// Register the event emitter to the model events
for (var e in events) {
  var event = events[e];
  Recording.schema.post(e, emitEvent(event));
}

function emitEvent(event) {
  return function(doc) {
    RecordingEvents.emit(event + ':' + doc._id, doc);
    RecordingEvents.emit(event, doc);
  }
}

export default RecordingEvents;
