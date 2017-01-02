'use strict';

import Recording from './recording.model';

function getRecordingNamesAndIds(req, res) {
  console.log('trying to get recording names');
  Recording.find({}, {name : 1}).exec().then(function(result) {
    res.status(200).json(result);
  });
}

function getRecording(req, res) {
  console.log('trying to get specific recording');
  res.status(200).json({});
  // TODO
}

function deleteRecording(req, res) {
  console.log('trying to delete recording');
  res.status(200).json({});
  // TODO
}

var express = require('express');
var router = express.Router();

router.get('/', getRecordingNamesAndIds);
router.get('/:id', getRecording);
router.delete('/:id', deleteRecording);

module.exports = router;
