'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var recordingCtrlStub = {
  index: 'recordingCtrl.index',
  show: 'recordingCtrl.show',
  create: 'recordingCtrl.create',
  update: 'recordingCtrl.update',
  destroy: 'recordingCtrl.destroy'
};

var routerStub = {
  get: sinon.spy(),
  put: sinon.spy(),
  patch: sinon.spy(),
  post: sinon.spy(),
  delete: sinon.spy()
};

// require the index with our stubbed out modules
var recordingIndex = proxyquire('./index.js', {
  'express': {
    Router: function() {
      return routerStub;
    }
  },
  './recording.controller': recordingCtrlStub
});

describe('Recording API Router:', function() {

  it('should return an express router instance', function() {
    recordingIndex.should.equal(routerStub);
  });

  describe('GET /api/recordings', function() {

    it('should route to recording.controller.index', function() {
      routerStub.get
        .withArgs('/', 'recordingCtrl.index')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/recordings/:id', function() {

    it('should route to recording.controller.show', function() {
      routerStub.get
        .withArgs('/:id', 'recordingCtrl.show')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/recordings', function() {

    it('should route to recording.controller.create', function() {
      routerStub.post
        .withArgs('/', 'recordingCtrl.create')
        .should.have.been.calledOnce;
    });

  });

  describe('PUT /api/recordings/:id', function() {

    it('should route to recording.controller.update', function() {
      routerStub.put
        .withArgs('/:id', 'recordingCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('PATCH /api/recordings/:id', function() {

    it('should route to recording.controller.update', function() {
      routerStub.patch
        .withArgs('/:id', 'recordingCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('DELETE /api/recordings/:id', function() {

    it('should route to recording.controller.destroy', function() {
      routerStub.delete
        .withArgs('/:id', 'recordingCtrl.destroy')
        .should.have.been.calledOnce;
    });

  });

});
