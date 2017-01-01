'use strict';

var app = require('../..');
import request from 'supertest';

var newRecording;

describe('Recording API:', function() {

  describe('GET /api/recordings', function() {
    var recordings;

    beforeEach(function(done) {
      request(app)
        .get('/api/recordings')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          recordings = res.body;
          done();
        });
    });

    it('should respond with JSON array', function() {
      recordings.should.be.instanceOf(Array);
    });

  });

  describe('POST /api/recordings', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/recordings')
        .send({
          name: 'New Recording',
          info: 'This is the brand new recording!!!'
        })
        .expect(201)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          newRecording = res.body;
          done();
        });
    });

    it('should respond with the newly created recording', function() {
      newRecording.name.should.equal('New Recording');
      newRecording.info.should.equal('This is the brand new recording!!!');
    });

  });

  describe('GET /api/recordings/:id', function() {
    var recording;

    beforeEach(function(done) {
      request(app)
        .get('/api/recordings/' + newRecording._id)
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          recording = res.body;
          done();
        });
    });

    afterEach(function() {
      recording = {};
    });

    it('should respond with the requested recording', function() {
      recording.name.should.equal('New Recording');
      recording.info.should.equal('This is the brand new recording!!!');
    });

  });

  describe('PUT /api/recordings/:id', function() {
    var updatedRecording;

    beforeEach(function(done) {
      request(app)
        .put('/api/recordings/' + newRecording._id)
        .send({
          name: 'Updated Recording',
          info: 'This is the updated recording!!!'
        })
        .expect(200)
        .expect('Content-Type', /json/)
        .end(function(err, res) {
          if (err) {
            return done(err);
          }
          updatedRecording = res.body;
          done();
        });
    });

    afterEach(function() {
      updatedRecording = {};
    });

    it('should respond with the updated recording', function() {
      updatedRecording.name.should.equal('Updated Recording');
      updatedRecording.info.should.equal('This is the updated recording!!!');
    });

  });

  describe('DELETE /api/recordings/:id', function() {

    it('should respond with 204 on successful removal', function(done) {
      request(app)
        .delete('/api/recordings/' + newRecording._id)
        .expect(204)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

    it('should respond with 404 when recording does not exist', function(done) {
      request(app)
        .delete('/api/recordings/' + newRecording._id)
        .expect(404)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

  });

});
