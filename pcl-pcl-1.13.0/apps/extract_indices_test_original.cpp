#include <iostream>
#include <pcl/ModelCoefficients.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/extract_indices.h>

int main (int argc, char** argv)
{
  pcl::PCLPointCloud2::Ptr cloud_blob (new pcl::PCLPointCloud2), cloud_filtered_blob (new pcl::PCLPointCloud2);
  //pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>), cloud_p (new pcl::PointCloud<pcl::PointXYZ>), cloud_f (new pcl::PointCloud<pcl::PointXYZ>);
  
  //Included by Yejin
  pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZRGBA>), cloud_p (new pcl::PointCloud<pcl::PointXYZRGBA>), cloud_f (new pcl::PointCloud<pcl::PointXYZRGBA>);
  // Fill in the cloud data
  pcl::PCDReader reader;
  reader.read ("test6.pcd", *cloud_blob);

  std::cerr << "PointCloud before filtering: " << cloud_blob->width * cloud_blob->height << " data points." << std::endl;

  // Create the filtering object: downsample the dataset using a leaf size of 1c
  // m
  pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
  sor.setInputCloud (cloud_blob);
  sor.setLeafSize (0.01f, 0.01f, 0.01f);
  sor.filter (*cloud_filtered_blob);

  // Convert to the templated PointCloud
  pcl::fromPCLPointCloud2 (*cloud_filtered_blob, *cloud_filtered);

  std::cerr << "PointCloud after filtering: " << cloud_filtered->width * cloud_filtered->height << " data points." << std::endl;

  // Write the downsampled version to disk
  pcl::PCDWriter writer;
  //writer.write<pcl::PointXYZ> ("test6_output.pcd", *cloud_filtered, false);

  //Included by Yejin
  writer.write<pcl::PointXYZRGBA> ("test6_output.pcd", *cloud_filtered, false);
  pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients ());
  pcl::PointIndices::Ptr inliers (new pcl::PointIndices ());
  // Create the segmentation object
  //pcl::SACSegmentation<pcl::PointXYZ> seg;

  //included by Yejin 
  pcl::SACSegmentation<pcl::PointXYZRGBA> seg;
 
  // Optional
  seg.setOptimizeCoefficients (true);
  // Mandatory
  seg.setModelType (pcl::SACMODEL_PLANE);
  seg.setMethodType (pcl::SAC_RANSAC);
  seg.setMaxIterations (1000);
  seg.setDistanceThreshold (0.01);

  // Create the filtering object
  //pcl::ExtractIndices<pcl::PointXYZ> extract;

  //Included by Yejin
  pcl::ExtractIndices<pcl::PointXYZRGBA> extract;
  int i = 0, nr_points = (int) cloud_filtered->size ();
  // While 30% of the original cloud is still there
  while (cloud_filtered->size () > 0.05 * nr_points)
  {
    // Segment the largest planar component from the remaining cloud
    seg.setInputCloud (cloud_filtered);
    seg.segment (*inliers, *coefficients);
    if (inliers->indices.size () == 0)
    {
      std::cerr << "Could not estimate a planar model for the given dataset." << std::endl;
      break;
    }

    // Extract the inliers
    extract.setInputCloud (cloud_filtered);
    extract.setIndices (inliers);
    extract.setNegative (false);
    extract.filter (*cloud_p);
    std::cerr << "PointCloud representing the planar component: " << cloud_p->width * cloud_p->height << " data points." << std::endl;

    std::stringstream ss;
    ss << "test6_output_list" << i << ".pcd";
    //writer.write<pcl::PointXYZ> (ss.str (), *cloud_p, false);
    
    //included by Yejin
    writer.write<pcl::PointXYZRGBA> (ss.str (), *cloud_p, false);
    // Create the filtering object
    extract.setNegative (true);
    extract.filter (*cloud_f);
    cloud_filtered.swap (cloud_f);
    i++;
  }

  return (0);
}
