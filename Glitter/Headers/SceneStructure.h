#ifndef SCENE_STRUCTURE_H
#define SCENE_STRUCTURE_H

#include "glitter.h"
#include "util.h"
#include "scene_io.h"
#include "BoundingBox.h"
#include <iostream>
#include "PolyBound.h"
#include "ObjBound.h"

extern list<ObjBound*> boundBoxes;

void jacksBuildBounds(SceneIO* scene);

void jacksCleanupBounds();

#endif // !SCENE_STRUCTURE_H
