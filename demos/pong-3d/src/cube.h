#pragma once

#include "liquid/scene/Mesh.h"

liquid::Mesh createCube() {
  liquid::Mesh mesh;
  liquid::Geometry geom;

  geom.addVertex(
      {-1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
  geom.addVertex(
      {1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
  geom.addVertex(
      {1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
  geom.addVertex(
      {-1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
  geom.addVertex(
      {-1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0});
  geom.addVertex(
      {1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0});
  geom.addVertex(
      {1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0});
  geom.addVertex(
      {-1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0});

  geom.addTriangle(0, 1, 2);
  geom.addTriangle(0, 2, 3);
  geom.addTriangle(4, 5, 6);
  geom.addTriangle(4, 6, 7);
  geom.addTriangle(3, 2, 5);
  geom.addTriangle(3, 5, 4);
  geom.addTriangle(2, 1, 6);
  geom.addTriangle(2, 6, 5);
  geom.addTriangle(1, 7, 6);
  geom.addTriangle(1, 0, 7);
  geom.addTriangle(0, 3, 4);
  geom.addTriangle(0, 4, 7);

  mesh.addGeometry(geom);
  return mesh;
}
