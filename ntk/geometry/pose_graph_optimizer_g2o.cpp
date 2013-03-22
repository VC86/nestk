

#include "pose_graph_optimizer_g2o.h"

#include <ntk/geometry/eigen_utils.h>

#include <g2o/core/optimization_algorithm_levenberg.h>
#include <g2o/types/slam3d/edge_se3.h>
#include <g2o/types/slam3d/vertex_se3.h>

using namespace ntk;

PoseGraphOptimizerG2O::PoseGraphOptimizerG2O()
{
    optimizer.setVerbose(true);

    // variable-size block solver
    linearSolver = new g2o::LinearSolverDense<g2o::BlockSolverX::PoseMatrixType>();
    solver_ptr = new g2o::BlockSolverX(linearSolver);

    g2o::OptimizationAlgorithmLevenberg* solver = new g2o::OptimizationAlgorithmLevenberg(solver_ptr);

    optimizer.setAlgorithm(solver);

    // Set the vertex index to 0
    vertexIdx = 0;
}

int PoseGraphOptimizerG2O::addVertex(ntk::Pose3D& vertexPose)
{
    cv::Vec4f rotation   =  vertexPose.cvQuaternionRotation();
    cv::Vec3f traslation =  vertexPose.cvTranslation();
    g2o::SE3Quat pose(Eigen::Quaterniond(rotation[3],rotation[0],rotation[1],rotation[2]), Eigen::Vector3d(traslation[0],traslation[1],traslation[2]));
    
    // set up node
    g2o::VertexSE3 *vc = new g2o::VertexSE3();
    vc->setEstimate(pose);

    // vertex id
    vc->setId(vertexIdx);

    // set first pose fixed
    if (vertexIdx == 0)
        vc->setFixed(true);

    // add to optimizer
    optimizer.addVertex(vc);

    // Update vertex index
    vertexIdx++;

    // Return the added vertex index
    return vertexIdx - 1;
}

void PoseGraphOptimizerG2O::addEdge(const int fromIdx, const int toIdx, ntk::Pose3D& relativePose)
{
    cv::Vec4f rotation   =  relativePose.cvQuaternionRotation();
    cv::Vec3f translation =  relativePose.cvTranslation();
    g2o::SE3Quat pose(Eigen::Quaterniond(rotation[3], rotation[0], rotation[1], rotation[2]),
                      Eigen::Vector3d(translation[0], translation[1], translation[2]));
    
    g2o::EdgeSE3* edge = new g2o::EdgeSE3;
    edge->vertices()[0] = optimizer.vertex(fromIdx);
    edge->vertices()[1] = optimizer.vertex(toIdx);
    edge->setMeasurement(pose);

    optimizer.addEdge(edge);
}

void PoseGraphOptimizerG2O::optimizeGraph()
{
    //Prepare and run the optimization
    bool ok = optimizer.initializeOptimization();

    //Set the initial Levenberg-Marquardt lambda
    //optimizer.setUserLambdaInit(0.01);

    //Run optimization
    optimizer.optimize(100);
}

void PoseGraphOptimizerG2O::getPoses(std::vector<ntk::Pose3D>& poses)
{
    ntk_assert(poses.size() == vertexIdx,
               "Poses should be initialized, only camera transform will be updated.");

    for(int poseIdx=0;poseIdx<vertexIdx;++poseIdx)
    {
        Pose3D& pose = poses[poseIdx];

        // Reset the camera transform to Identity.
        pose.resetCameraTransform();

        // Transform the vertex pose from G2O quaternion to Eigen::Matrix4f
        g2o::VertexSE3* vertex = dynamic_cast<g2o::VertexSE3*>(optimizer.vertex(poseIdx));

        double optimized[7];
        vertex->getEstimateData(optimized);

        Eigen::Quaterniond q(optimized[6],optimized[3],optimized[4],optimized[5]);
        Eigen::Matrix<double,3,3> rotationmatrix = q.matrix();
        cv::Vec3f translation(optimized[0],optimized[1],optimized[2]);

        cv::Mat1d rotation_matrix(3,3);
        for (int r=0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                rotation_matrix[r][c] = rotationmatrix(r,c);

        pose.applyTransformAfter(translation, rotation_matrix);
    }
}

void PoseGraphOptimizerG2O::saveGraph(std::string fileName)
{
    //Save the graph to file
    optimizer.save(fileName.c_str(),0);
}

