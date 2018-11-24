#include "scene_io.h"

#include <malloc.h>
#include <string.h>
#include <iostream>

#define VERSION_STRING "Composer format"
#define THIS_VERSION    2.1
#define MAX_NAME    5000

#define CHECK(nE, nA)    if ((nE) != (nA)) {			\
    printf( "Error during scanf: file '%s', line %d\n",		\
            __FILE__, __LINE__ ); } else /* eat semicolon */

namespace {
  inline glm::vec3 read_triplet(const string& name, FILE* fp) {
    float x, y, z;
    CHECK(3, fscanf(fp, string(" " + name + " %g %g %g").c_str(), &x, &y, &z));
    return glm::vec3(x, y, z);
  }

  inline float read_single_float(const string& name, FILE* fp){
    float x;
    CHECK(1, fscanf(fp, string(" " + name + " %g").c_str(), &x));
    return x;
  }
}

SceneIO *readScene(const char *filename) {
  FILE *fp;
  char format[50], type[20];

  fp = fopen(filename, "rb");
  SceneIO *scene = NULL;

  if (fp == NULL) {
    printf("Can't open file '%s' for reading.\n", filename);
    return NULL;
  }

  strcpy(format, VERSION_STRING);
  strcat(format, " %lg %10s\n");

  if (fscanf(fp, format, &Version, type) != 2) {
    printf("File '%s' has wrong format.\n", filename);
  } else if (Version > THIS_VERSION) {
    printf("Error: file '%s' is version %g, program is version %g.\n",
	   filename, Version, THIS_VERSION);
  } else if (strcmp(type, "ascii") == 0) {
    scene = readSceneA(fp);
  } else {
    printf("Error: unrecognized file type (not ascii).\n");
  }

  fclose(fp);
  return scene;
}

void writeSceneAscii(SceneIO *scene, const char *filename) {
  FILE *fp = fopen(filename, "w");
  if (fp == NULL) {
    printf("Can't open file '%s' for writing.\n", filename);
    return;
  }

  fprintf(fp, "%s %g ascii\n", VERSION_STRING, THIS_VERSION);
  write_cameraA(scene->camera, fp);
  write_lightsA(scene->lights, fp);
  write_objectsA(scene->objects, fp);
  fclose(fp);
  return;
}


static shared_obj::Scene readSceneA(FILE *fp) {
  shared_obj::Scene scene();
  char word[100];

  while (fscanf(fp, "%s", word) == 1) {
    if (strcmp(word, "camera") == 0) {
      read_cameraA(scene, fp);
    } else if (strcmp(word, "point_light") == 0) {
      read_point_lightA(scene, fp);
    } else if (strcmp(word, "directional_light") == 0) {
      read_directional_lightA(scene, fp);
    } else if (strcmp(word, "spot_light") == 0) {
      read_spot_lightA(scene, fp);
    } else if (strcmp(word, "sphere") == 0) {
      scene.spheres.push_back(read_sphereA(fp));
    } else if (strcmp(word, "poly_set") == 0) {
      scene.models.push_back(read_poly_setA(scene, fp));
    } else {
      printf("Unrecognized keyword '%s', aborting.\n", word);
      deleteScene(scene);
      return NULL;
    }
  }
  return scene;
}


static long Test_long = 123456789;
static Flt Test_Flt = (float) 3.1415926;

void deleteScene(SceneIO *scene) {
  delete_camera(scene->camera);
  delete_lights(scene->lights);
  delete_objects(scene->objects);
  free(scene);
}

static void
write_cameraA(CameraIO *camera, FILE *fp) {
  if (camera == NULL) return;

  fprintf(fp, "camera {\n");
  fprintf(fp, "  position %g %g %g\n", camera->position[0],
	  camera->position[1], camera->position[2]);
  fprintf(fp, "  viewDirection %g %g %g\n", camera->viewDirection[0],
	  camera->viewDirection[1], camera->viewDirection[2]);
  fprintf(fp, "  focalDistance %g\n", camera->focalDistance);
  fprintf(fp, "  orthoUp %g %g %g\n", camera->orthoUp[0],
	  camera->orthoUp[1], camera->orthoUp[2]);
  fprintf(fp, "  verticalFOV %g\n", camera->verticalFOV);
  fprintf(fp, "}\n");
}


static void
read_cameraA(shared_obj::Scene &scene, FILE *fp) {
  fscanf(fp, " {");
  glm::vec3 pos = read_triplet("position", fp);
  glm::vec3 view_dir = read_triplet("viewDirection", fp);
  float focal_distance = read_single_float("focalDistance", fp);
  glm::vec3 ortho_up = read_triplet("orthoUp", fp);
  float fov = read_single_float("verticalFOV", fp);
  fscanf(fp, " }");
  scene.camera = shared_obj::Camera(pos, ortho_up);
}

LightIO *
new_light(void) {
  return (LightIO *) calloc(1, sizeof(LightIO));
}


LightIO *
append_light(LightIO **plights) {
  if (*plights == NULL) {
    *plights = new_light();
    return *plights;
  } else {
    LightIO *lts;

    lts = *plights;
    while (lts->next != NULL) {
      lts = lts->next;
    }
    lts->next = new_light();

    return lts->next;
  }
}


static void
write_lightsA(LightIO *lts, FILE *fp) {
  while (lts != NULL) {
    write_lightA(lts, fp);
    lts = lts->next;
  }
}


static void
write_lightA(LightIO *light, FILE *fp) {
  if (light->type == POINT_LIGHT) {
    fprintf(fp, "point_light {\n");
  } else if (light->type == DIRECTIONAL_LIGHT) {
    fprintf(fp, "directional_light {\n");
  } else if (light->type == SPOT_LIGHT) {
    fprintf(fp, "spot_light {\n");
  } else {
    printf("error -- unrecognized light type.\n");
  }
  if (light->type != DIRECTIONAL_LIGHT) {
    fprintf(fp, "  position %g %g %g\n", light->position[0],
	    light->position[1], light->position[2]);
  }
  if (light->type != POINT_LIGHT) {
    fprintf(fp, "  direction %g %g %g\n", light->direction[0],
	    light->direction[1], light->direction[2]);
  }
  fprintf(fp, "  color %g %g %g\n", light->color[0],
	  light->color[1], light->color[2]);
  if (light->type == SPOT_LIGHT) {
    fprintf(fp, "  dropOffRate %g\n", light->dropOffRate);
    fprintf(fp, "  cutOffAngle %g\n", light->cutOffAngle);
  }
  fprintf(fp, "}\n");
}


static void
read_point_lightA(shared_obj::Scene &scene, FILE *fp) {
  fscanf(fp, " {");
  glm::vec3 pos = read_triplet("position", fp);
  glm::vec3 color = read_triplet("color", fp);
  fscanf(fp, " }");

  scene.lights.push_back(shared_obj::Light(pos,
					   color,
					   scene.lights.size(),
					   shared_obj::POINT_LIGHT));
}


static void
read_directional_lightA(shared_obj::Scene &scene, FILE *fp) {fscanf(fp, " {");
  glm::vec3 direction = read_triplet("direction", fp);
  glm::vec3 color = read_triplet("color", fp);
  fscanf(fp, " }");

  scene.lights.push_back(shared_obj::Light(glm::vec3(0), direction, color, scene.lights.size(), shared_obj::DIRECTIONAL_LIGHT));
}


static void
read_spot_lightA(shared_obj::Scene &scene, FILE *fp) {
  fscanf(fp, " {");
  glm::vec3 pos = read_triplet("position", fp);
  glm::vec3 direction = read_triplet("direction", fp);
  glm::vec3 color = read_triplet("color", fp);
  float drop_off = read_single_float("dropOffRate", fp);
  float cut_off = read_single_float("cutOffAngle", fp);
  fscanf(fp, " }");

  scene.lights.push_back(shared_obj::Light(pos, direction, color, scene.lights.size(), shared_obj::SPOT_LIGHT));
}

static void
write_objectsA(ObjIO *obj, FILE *fp) {
  while (obj != NULL) {
    if (obj->type == SPHERE_OBJ) {
      write_sphereA(obj, fp);
    } else if (obj->type == POLYSET_OBJ) {
      write_poly_setA(obj, fp);
    } else {
      printf("error -- unrecognized object type\n");
    }
    obj = obj->next;
  }
}


static void
write_objectA(ObjIO *obj, FILE *fp) {
  int i;

  if (obj->name == NULL) {
    fprintf(fp, "  name NULL\n");
  } else {
    fprintf(fp, "  name \"%s\"\n", obj->name);
  }
  fprintf(fp, "  numMaterials %ld\n", obj->numMaterials);
  for (i = 0; i < obj->numMaterials; ++i) {
    write_materialA(obj->material + i, fp);
  }
}

static std::vector<Material>
read_objectA(FILE *fp) {
  char word[MAX_NAME];

  CHECK(1, fscanf(fp, " name %[^\n]", word));
  int num_materials;
  CHECK(1, fscanf(fp, " numMaterials %ld", &num_materials));
  std::vector<Material> materials(num_materials);
  for (int i = 0; i < obj->numMaterials; ++i) {
    materials.push_back(read_materialA(fp));
  }
  return materials;
}

static void
write_materialA(MaterialIO *material, FILE *fp) {
  fprintf(fp, "  material {\n");
  fprintf(fp, "    diffColor %g %g %g\n", material->diffColor[0],
	  material->diffColor[1], material->diffColor[2]);
  fprintf(fp, "    ambColor %g %g %g\n", material->ambColor[0],
	  material->ambColor[1], material->ambColor[2]);
  fprintf(fp, "    specColor %g %g %g\n", material->specColor[0],
	  material->specColor[1], material->specColor[2]);
  fprintf(fp, "    emisColor %g %g %g\n", material->emissColor[0],
	  material->emissColor[1], material->emissColor[2]);
  fprintf(fp, "    shininess %g\n", material->shininess);
  fprintf(fp, "    ktran %g\n", material->ktran);
  fprintf(fp, "  }\n");
}


static shared_obj::Material
read_materialA(FILE *fp) {
  shared_obj::Material material;
  
  fscanf(fp, " material {");
  material.diffuse_color = read_triplet("diffColor", fp);
  material.ambient_color = read_triplet("ambColor", fp);
  material.specular_color = read_triplet("specColor", fp);
  material.emis_color = read_triplet("emisColor", fp);
  material.shininess = read_single_float("shininess", fp);
  material.ktran = read_single_float("ktran", fp);
  fscanf(fp, " }");
  return material;
}

static void
write_sphereA(ObjIO *obj, FILE *fp) {
  shared_obj::Sphere sphere();

  fprintf(fp, "sphere {\n");
  write_objectA(obj, fp);
  fprintf(fp, "  origin %g %g %g\n", sphere->origin[0],
	  sphere->origin[1], sphere->origin[2]);
  fprintf(fp, "  radius %g\n", sphere->radius);
  fprintf(fp, "  xaxis %g %g %g\n", sphere->xaxis[0],
	  sphere->xaxis[1], sphere->xaxis[2]);
  fprintf(fp, "  xlength %g\n", sphere->xlength);
  fprintf(fp, "  yaxis %g %g %g\n", sphere->yaxis[0],
	  sphere->yaxis[1], sphere->yaxis[2]);
  fprintf(fp, "  ylength %g\n", sphere->ylength);
  fprintf(fp, "  zaxis %g %g %g\n", sphere->zaxis[0],
	  sphere->zaxis[1], sphere->zaxis[2]);
  fprintf(fp, "  zlength %g\n", sphere->zlength);
  fprintf(fp, "}\n");
}

static shared_obj::Sphere
read_sphereA(Scene& scene, FILE *fp) {
  fscanf(fp, " {");
  std::vector<Material> materials = read_objectA(fp);
  if(materials.size() != 1){
    std::cerr << materials.size() <<
      " materials on sphere object." << endl;
    exit(1);
  }
  glm::vec3 origin = read_triplet("origin", fp);
  float radius = read_single_float("radius", fp);
  glm::vec3 x_axis = read_triplet("xaxis", fp);
  float x_length = read_single_float("xlength", fp);
  glm::vec3 y_axis = read_triplet("yaxis", fp);
  float y_length = read_single_float("ylength", fp);
  glm::vec3 z_axis = read_triplet("zaxis", fp);
  float z_length = read_single_float("zlength", fp);
  fscanf(fp, " }");
  shared_obj::Sphere sphere(origin, radius,
			    x_axis, x_length,
			    y_axis, y_length,
			    z_axis, z_length,
			    materials[0]);
}

static void
write_poly_setA(ObjIO *obj, FILE *fp) {
  PolySetIO *pset = (PolySetIO *) obj->data;
  PolygonIO *poly;
  VertexIO *vert;
  int i, j;

  fprintf(fp, "poly_set {\n");
  write_objectA(obj, fp);
  switch (pset->type) {
  case POLYSET_TRI_MESH:
    fprintf(fp, "  type POLYSET_TRI_MESH\n");
    break;
  case POLYSET_FACE_SET:
    fprintf(fp, "  type POLYSET_FACE_SET\n");
    break;
  case POLYSET_QUAD_MESH:
    fprintf(fp, "  type POLYSET_QUAD_MESH\n");
    break;
  default:
    printf("Unknown PolySetIO type\n");
    return;
  }
  switch (pset->normType) {
  case PER_VERTEX_NORMAL:
    fprintf(fp, "  normType PER_VERTEX_NORMAL\n");
    break;
  case PER_FACE_NORMAL:
    fprintf(fp, "  normType PER_FACE_NORMAL\n");
    break;
  default:
    printf("Unknown PolySetIO normType\n");
    return;
  }
  switch (pset->materialBinding) {
  case PER_OBJECT_MATERIAL:
    fprintf(fp, "  materialBinding PER_OBJECT_MATERIAL\n");
    break;
  case PER_VERTEX_MATERIAL:
    fprintf(fp, "  materialBinding PER_VERTEX_MATERIAL\n");
    break;
  default:
    printf("Unknown material binding\n");
    return;
  }
  if (pset->hasTextureCoords) {
    fprintf(fp, "  hasTextureCoords TRUE\n");
  } else {
    fprintf(fp, "  hasTextureCoords FALSE\n");
  }
  fprintf(fp, "  rowSize %ld\n", pset->rowSize);
  fprintf(fp, "  numPolys %ld\n", pset->numPolys);

  poly = pset->poly;
  for (i = 0; i < pset->numPolys; i++, poly++) {
    fprintf(fp, "  poly {\n");
    fprintf(fp, "    numVertices %ld\n", poly->numVertices);
    vert = poly->vert;
    for (j = 0; j < poly->numVertices; j++, vert++) {
      fprintf(fp, "    pos %g %g %g\n",
	      vert->pos[0], vert->pos[1], vert->pos[2]);
      if (pset->normType == PER_VERTEX_NORMAL) {
	fprintf(fp, "    norm %g %g %g\n",
		vert->norm[0], vert->norm[1], vert->norm[2]);
      }
      if (pset->materialBinding == PER_VERTEX_MATERIAL) {
	fprintf(fp, "    materialIndex %ld\n", vert->materialIndex);
      }
      if (pset->hasTextureCoords) {
	fprintf(fp, "    s %g  t %g\n", vert->s, vert->t);
      }
    }
    fprintf(fp, "  }\n");
  }
  fprintf(fp, "}\n");
}


static shared_obj::Model
read_poly_setA(SceneIO *scene, FILE *fp) {
  ObjIO *obj = append_object(&scene->objects);
  PolySetIO *pset = (PolySetIO *) calloc(1, sizeof(PolySetIO));
  char word[MAX_NAME];
  PolygonIO *poly;
  VertexIO *vert;
  int i, j;

  obj->type = POLYSET_OBJ;
  obj->data = pset;

  fscanf(fp, " {");
  read_objectA(obj, fp);
  CHECK(1, fscanf(fp, " type %s", word));
  if (strcmp(word, "POLYSET_TRI_MESH") == 0) {
    pset->type = POLYSET_TRI_MESH;
  } else if (strcmp(word, "POLYSET_FACE_SET") == 0) {
    pset->type = POLYSET_FACE_SET;
  } else if (strcmp(word, "POLYSET_QUAD_MESH") == 0) {
    pset->type = POLYSET_QUAD_MESH;
  } else {
    printf("Error: unknown polyset type\n");
  }
  CHECK(1, fscanf(fp, " normType %s", word));
  if (strcmp(word, "PER_VERTEX_NORMAL") == 0) {
    pset->normType = PER_VERTEX_NORMAL;
  } else if (strcmp(word, "PER_FACE_NORMAL") == 0) {
    pset->normType = PER_FACE_NORMAL;
  } else {
    printf("Error: unknown polyset normType\n");
  }
  CHECK(1, fscanf(fp, " materialBinding %s", word));
  if (strcmp(word, "PER_OBJECT_MATERIAL") == 0) {
    pset->materialBinding = PER_OBJECT_MATERIAL;
  } else if (strcmp(word, "PER_VERTEX_MATERIAL") == 0) {
    pset->materialBinding = PER_VERTEX_MATERIAL;
  } else {
    printf("Error: unknown material binding\n");
  }
  CHECK(1, fscanf(fp, " hasTextureCoords %s", word));
  if (strcmp(word, "TRUE") == 0) {
    pset->hasTextureCoords = TRUE;
  } else if (strcmp(word, "FALSE") == 0) {
    pset->hasTextureCoords = FALSE;
  } else {
    printf("Error: unknown hasTextureCoords field\n");
  }
  CHECK(1, fscanf(fp, " rowSize %ld", &pset->rowSize));
  CHECK(1, fscanf(fp, " numPolys %ld", &pset->numPolys));

  pset->poly = (PolygonIO *) calloc(pset->numPolys, sizeof(PolygonIO));
  poly = pset->poly;
  for (i = 0; i < pset->numPolys; i++, poly++) {
    fscanf(fp, " poly {");
    CHECK(1, fscanf(fp, " numVertices %ld", &poly->numVertices));
    poly->vert = (VertexIO *) calloc(poly->numVertices, sizeof(VertexIO));
    vert = poly->vert;
    for (j = 0; j < poly->numVertices; j++, vert++) {
      CHECK(3, fscanf(fp, " pos %g %g %g",
		      &vert->pos[0], &vert->pos[1], &vert->pos[2]));
      if (pset->normType == PER_VERTEX_NORMAL) {
	CHECK(3, fscanf(fp, " norm %g %g %g",
			&vert->norm[0], &vert->norm[1], &vert->norm[2]));
      }
      if (pset->materialBinding == PER_VERTEX_MATERIAL) {
	CHECK(1, fscanf(fp, " materialIndex %ld", &vert->materialIndex));
      }
      if (pset->hasTextureCoords) {
	CHECK(2, fscanf(fp, " s %g t %g", &vert->s, &vert->t));
      }
    }
    fscanf(fp, " }");
  }
  fscanf(fp, " }");
}
