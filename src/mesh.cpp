#include "mesh.h"

mesh::mesh()
{

}

pcl::PolygonMesh mesh::smoothMesh(pcl::PolygonMesh::Ptr meshToSmooth) {
    //!!! getting double free or corruption (out)
    std::cout<<"Smoothing mesh\n";
    pcl::PolygonMesh output;
    pcl::MeshSmoothingLaplacianVTK vtk;
    vtk.setInputMesh(meshToSmooth);
    vtk.setNumIter(20000);
    vtk.setConvergence(0.0001);
    vtk.setRelaxationFactor(0.0001);
    vtk.setFeatureEdgeSmoothing(true);
    vtk.setFeatureAngle(M_PI/5);
    vtk.setBoundarySmoothing(true);
    vtk.process(output);
    return output;
}

void mesh::polygonateCloud(PointCloudT::Ptr cloudToPolygonate, pcl::PolygonMesh::Ptr triangles) {
    std::cout<<"Greedy polygonation\n";
    parameters* params = parameters::GetInstance();

    std::ifstream config_file("config.json");

    if (!config_file.fail()) {
        std::cout << "Config file loaded\n";
        using boost::property_tree::ptree;
        ptree pt;
        read_json(config_file, pt);

        for (auto & array_element: pt) {
            if (array_element.first == "greedyProjection")
                std::cout << "greedyProjection" << "\n";
            for (auto & property: array_element.second) {
                if (array_element.first == "greedyProjection")
                    std::cout << " "<< property.first << " = " << property.second.get_value < std::string > () << "\n";
            }
        }

        params->GPsearchRadius = pt.get<float>("greedyProjection.searchRadius");
        params->GPmu = pt.get<float>("greedyProjection.mu");
        params->GPmaximumNearestNeighbors = pt.get<int>("greedyProjection.maximumNearestNeighbors");
    }




    // Get Greedy result
    //Normal Estimation
    pcl::NormalEstimation<PointT, pcl::Normal> normEstim;
    pcl::PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
    pcl::search::KdTree<PointT>::Ptr tree2 (new pcl::search::KdTree<PointT>);
    //pcl::search::OrganizedNeighbor<PointT>::Ptr tree2 (new pcl::search::OrganizedNeighbor<PointT>); //only for organized cloud
    tree2->setInputCloud(cloudToPolygonate);//cloud_filtered
    normEstim.setInputCloud(cloudToPolygonate);//cloud_filtered
    normEstim.setSearchMethod(tree2);
    normEstim.setKSearch(20);
    //normEstim.setNumberOfThreads(4);
    normEstim.compute(*normals);

    //Concatenate the cloud with the normal fields
    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud_normals (new pcl::PointCloud<pcl::PointXYZRGBNormal>);
    pcl::concatenateFields(*cloudToPolygonate,*normals,*cloud_normals);

    //Create  search tree to include cloud with normals
    pcl::search::KdTree<pcl::PointXYZRGBNormal>::Ptr tree_normal (new pcl::search::KdTree<pcl::PointXYZRGBNormal>);
    //pcl::search::OrganizedNeighbor<pcl::PointXYZRGBNormal>::Ptr tree_normal (new pcl::search::OrganizedNeighbor<pcl::PointXYZRGBNormal>); //only for organized cloud
    tree_normal->setInputCloud(cloud_normals);


    //Initialize objects for triangulation
    pcl::GreedyProjectionTriangulation<pcl::PointXYZRGBNormal> gp;
    //boost::shared_ptr<pcl::PolygonMesh> triangles(new pcl::PolygonMesh);
    //pcl::PolygonMesh triangles;

    //Max distance between connecting edge points
    gp.setSearchRadius(params->GPsearchRadius);
    gp.setMu(params->GPmu);
    gp.setMaximumNearestNeighbors (params->GPmaximumNearestNeighbors);
    gp.setMaximumSurfaceAngle(M_PI/4); // 45 degrees
    gp.setMinimumAngle(M_PI/18); // 10 degrees
    gp.setMaximumAngle(2*M_PI/3); // 120 degrees
    gp.setNormalConsistency(false);


    gp.setInputCloud (cloud_normals);
    gp.setSearchMethod (tree_normal);
    gp.reconstruct (*triangles);
    std::cout << "Polygons created: " << triangles->polygons.size() << "\n";
    //return triangles;
}

void mesh::polygonateCloudMC(PointCloudT::Ptr cloudToPolygonate, pcl::PolygonMesh::Ptr triangles) {
    printf("Marching cubes\n");
    pcl::NormalEstimationOMP<PointT, pcl::Normal> ne;
    pcl::search::KdTree<PointT>::Ptr tree1 (new pcl::search::KdTree<PointT>);
    tree1->setInputCloud (cloudToPolygonate);
    ne.setInputCloud (cloudToPolygonate);
    ne.setSearchMethod (tree1);
    ne.setKSearch (20);
    pcl::PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
    ne.compute (*normals);

    // Concatenate the XYZ and normal fields*
    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud_with_normals (new pcl::PointCloud<pcl::PointXYZRGBNormal>);
    concatenateFields(*cloudToPolygonate, *normals, *cloud_with_normals);

    // Create search tree*
    pcl::search::KdTree<pcl::PointXYZRGBNormal>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZRGBNormal>);
    tree->setInputCloud (cloud_with_normals);

    std::cout << "begin marching cubes reconstruction" << std::endl;

    pcl::MarchingCubesHoppe<pcl::PointXYZRGBNormal> mc;
    //pcl::PolygonMesh::Ptr triangles(new pcl::PolygonMesh);
    mc.setInputCloud (cloud_with_normals);
    mc.setSearchMethod (tree);
    mc.reconstruct (*triangles);

    std::cout << triangles->polygons.size() << " triangles created" << std::endl;
    //return triangles;
}