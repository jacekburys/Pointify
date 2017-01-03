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
  Recording.findById(req.params.id).exec().then(function(result) {
    res.status(200).json(result);
  });
}

function deleteRecording(req, res) {
  console.log('trying to delete recording ' + req.params.id);
  //res.status(200).json({});
  Recording.findById(req.params.id).remove(function(err) {
    if (err) {
      console.log(err);
      // TODO : handle this error
      return;
    }
    // TODO : send something?
    res.status(200).json();
  });
}

var express = require('express');
var router = express.Router();

router.get('/', getRecordingNamesAndIds);
router.get('/:id', getRecording);
router.delete('/:id', deleteRecording);

module.exports = router;
