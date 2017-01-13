/* global io */
'use strict';

angular.module('serverApp')
  .factory('ply', function() {

    return {
      toPly : function(vertices, colors) {
        if (!vertices || !colors) {
          return '';
        }
        if (vertices.length !== colors.length) {
          // TODO : handle this error better
          return '';
        }
        // push lines to text
        var text = [];
        text.push('ply');
        text.push('format ascii 1.0');
        text.push('element vertex ' + vertices.length);
        text.push('property float x');
        text.push('property float y');
        text.push('property float z');
        text.push('property uchar red');
        text.push('property uchar green');
        text.push('property uchar blue');
        text.push('property uchar alpha');
        // TODO : figure out if the element face 0 line is needed
        text.push('element face 0');
        text.push('property list uchar int vertex_indices');
        text.push('end_header');

        for (var i = 0; i < vertices.length; i++) {
          var vertex = vertices[i];
          var color = colors[i];
          text.push(vertex.x + ' ' + vertex.y + ' ' + vertex.z + ' ' +
                    Math.round(color.r * 255) + ' ' +
                    Math.round(color.g * 255) + ' ' +
                    Math.round(color.b * 255) + ' ' +
                    '255 ');
        }

        return text.join('\n');
      }
    };
  });
