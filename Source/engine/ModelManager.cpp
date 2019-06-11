#include "ModelManager.h"

ModelManager::ModelManager(string relativePath) {
    this->relativePath = relativePath;
}

shared_ptr<Shape> loadShape(string relativePath, string file, bool resize, bool findEdges) {
    shared_ptr<Shape> shape = make_shared<Shape>();
    shape->loadMesh(relativePath + file);
    if (resize) shape->resize();
    shape->measure();
    /*if (findEdges)*/ shape->findEdges();
    shape->init();
    return shape;
}

shared_ptr<Shape> ModelManager::get(string modelFile, bool resize, bool findEdges)
{
    if (!(CONTAINS(loadedModels, modelFile))) {
        loadedModels[modelFile] = loadShape(relativePath, modelFile, resize, findEdges);
    }

    return loadedModels[modelFile];
}
