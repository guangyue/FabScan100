#include "geometryengine.h"
#include "fscontroller.h"

struct VertexData
{
    QVector3D position;
    QVector3D color;
};

GeometryEngine::GeometryEngine() : vboIds(new GLuint[4])
{
    numberOfIndices = 0;
}

GeometryEngine::~GeometryEngine()
{
    glDeleteBuffers(4, vboIds);
    delete[] vboIds;
}

void GeometryEngine::init()
{
    initializeGLFunctions();

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//! [0]
    // Generate 4 VBOs
    glGenBuffers(4, vboIds);

//! [0]

    // Initializes cube geometry and transfers it to VBOs
    initSurfaceMesh();
    initPointCloud();
}

void GeometryEngine::initPointCloud()
{
    VertexData vertices[] = {
        {QVector3D(-1.0, -1.0,  1.0), QVector3D(0.0, 0.0, 0.0)},  // v0
        {QVector3D( 1.0, -1.0,  1.0), QVector3D(0.33, 0.0, 0.0)}, // v1
        {QVector3D(-1.0,  1.0,  1.0), QVector3D(0.0, 0.5, 0.0)},  // v2
        {QVector3D( 1.0,  1.0,  1.0), QVector3D(0.33, 0.5, 0.0)}, // v3
    };

    GLushort indices[] = {
         0,  1,  2,  3
    };

    // Transfer vertex data to VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLushort), indices, GL_STATIC_DRAW);

}

void GeometryEngine::setPointCloudTo(pcl::PointCloud<pcl::PointXYZRGB>::Ptr pointcloud)
{
    VertexData vertices[pointcloud->points.size()];
    GLuint indices[pointcloud->points.size()];

    for (size_t i = 0; i < pointcloud->points.size(); ++i){
        VertexData vd;
        vd.position = QVector3D(
                    pointcloud->points[i].x,
                    pointcloud->points[i].y,
                    pointcloud->points[i].z);

        Eigen::Vector3i color;
        color = pointcloud->points[i].getRGBVector3i();
        vd.color = QVector3D(
                    color[0],
                    color[1],
                    color[2]);
        vertices[i] = vd;
        indices[i] = i;
        //std::cout<<i<<std::endl;
    }

    // Transfer vertex data to VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, pointcloud->points.size() * sizeof(VertexData), vertices, GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pointcloud->points.size() * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

void GeometryEngine::drawPointCloud(QGLShaderProgram *program)
{
    //glPointSize(1.0f);
    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);

    // Offset for position
    int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int vertexColor = program->attributeLocation("a_col");
    program->enableAttributeArray(vertexColor);
    glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Draw cube geometry using indices from VBO 3
    unsigned int numberOfPoints = FSController::getInstance()->model->pointCloud->size();
    glDrawElements(GL_POINTS, numberOfPoints, GL_UNSIGNED_INT, 0);
}

void GeometryEngine::initSurfaceMesh()
{
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(-1.0, -1.0,  1.0), QVector3D(0.0, 0.0,0)},  // v0
        {QVector3D( 1.0, -1.0,  1.0), QVector3D(0.33, 0.0,0)}, // v1
        {QVector3D(-1.0,  1.0,  1.0), QVector3D(0.0, 0.5,0)},  // v2
        {QVector3D( 1.0,  1.0,  1.0), QVector3D(0.33, 0.5,0)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0, -1.0,  1.0), QVector3D( 0.0, 0.5,0)}, // v4
        {QVector3D( 1.0, -1.0, -1.0), QVector3D(0.33, 0.5,0)}, // v5
        {QVector3D( 1.0,  1.0,  1.0), QVector3D(0.0, 1.0,0)},  // v6
        {QVector3D( 1.0,  1.0, -1.0), QVector3D(0.33, 1.0,0)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0, -1.0, -1.0), QVector3D(0.66, 0.5,0)}, // v8
        {QVector3D(-1.0, -1.0, -1.0), QVector3D(1.0, 0.5,0)},  // v9
        {QVector3D( 1.0,  1.0, -1.0), QVector3D(0.66, 1.0,0)}, // v10
        {QVector3D(-1.0,  1.0, -1.0), QVector3D(1.0, 1.0,0)},  // v11

        // Vertex data for face 3
        {QVector3D(-1.0, -1.0, -1.0), QVector3D(0.66, 0.0,0)}, // v12
        {QVector3D(-1.0, -1.0,  1.0), QVector3D(1.0, 0.0,0)},  // v13
        {QVector3D(-1.0,  1.0, -1.0), QVector3D(0.66, 0.5,0)}, // v14
        {QVector3D(-1.0,  1.0,  1.0), QVector3D(1.0, 0.5,0)},  // v15

        // Vertex data for face 4
        {QVector3D(-1.0, -1.0, -1.0), QVector3D(0.33, 0.0,0)}, // v16
        {QVector3D( 1.0, -1.0, -1.0), QVector3D(0.66, 0.0,0)}, // v17
        {QVector3D(-1.0, -1.0,  1.0), QVector3D(0.33, 0.5,0)}, // v18
        {QVector3D( 1.0, -1.0,  1.0), QVector3D(0.66, 0.5,0)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0,  1.0,  1.0), QVector3D(0.33, 0.5,0)}, // v20
        {QVector3D( 1.0,  1.0,  1.0), QVector3D(0.66, 0.5,0)}, // v21
        {QVector3D(-1.0,  1.0, -1.0), QVector3D(0.33, 1.0,0)}, // v22
        {QVector3D( 1.0,  1.0, -1.0), QVector3D(0.66, 1.0,0)}  // v23
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite     0  1
    // vertex order then last index of the first strip and first  3  2
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
    GLushort indices[] = {
         0,  1,  2,  3,  0,  2,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         6,  4,  5,  6,  7,  4, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         10,  8,  9, 10, 11, 8, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23, 20, // Face 5 - triangle strip (v20, v21, v22, v23)
    };

//! [1]
    // Transfer vertex data to VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(VertexData), vertices, GL_DYNAMIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLushort), indices, GL_STATIC_DRAW);

    numberOfIndices=36;
//! [1]
}

void GeometryEngine::setSurfaceMeshTo(pcl::PolygonMesh surfacemesh, pcl::PointCloud<pcl::PointXYZRGB>::Ptr pointCloud)
{
    pcl::PointCloud<pcl::PointXYZRGB> cloud;
    pcl::fromROSMsg(surfacemesh.cloud, cloud);

    VertexData vertices[cloud.points.size()];
    GLuint indices[surfacemesh.polygons.size()*3];

    for (unsigned int i = 0; i < cloud.points.size(); ++i){
        VertexData vd;
        vd.position = QVector3D(
                    cloud.points[i].x,
                    cloud.points[i].y,
                    cloud.points[i].z);
        vd.color = QVector3D(
                    pointCloud->points[i].r,
                    pointCloud->points[i].g,
                    pointCloud->points[i].b);
        vertices[i] = vd;
    }

    numberOfIndices = 0;
    for (unsigned int i=0; i<surfacemesh.polygons.size();i++){
        for(unsigned int j=0; j<surfacemesh.polygons[i].vertices.size(); j++){
            indices[numberOfIndices] = surfacemesh.polygons[i].vertices[j];
            numberOfIndices++;
        }
    }

    // Transfer vertex data to VBO 2
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, cloud.points.size() * sizeof(VertexData), vertices, GL_STATIC_DRAW);

    // Transfer index data to VBO 3
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

void GeometryEngine::drawSurfaceMesh(QGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[3]);

    // Offset for position
    int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int vertexColor = program->attributeLocation("a_col");
    program->enableAttributeArray(vertexColor);
    glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, 0);
}
