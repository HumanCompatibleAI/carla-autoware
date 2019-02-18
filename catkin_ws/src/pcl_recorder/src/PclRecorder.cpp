/*
 * Copyright (c) 2019 Intel Labs.
 *
 * authors: Frederik Pasch
 */

#include "PclRecorder.h"
#include <string>
#include <pcl/io/pcd_io.h>
#include <pcl_ros/transforms.h>
#include <sstream>

PclRecorder::PclRecorder()
{
  tfListener = new tf2_ros::TransformListener(tf_buffer_);

  if (mkdir("/tmp/pcl_capture", 0777) == -1) {
    ROS_WARN("Could not create directory!");
  }

  // Create a ROS subscriber for the input point cloud
  sub = nh.subscribe("/carla/ego_vehicle/lidar/sensor/point_cloud", 1000000, &PclRecorder::callback, this);
}

void PclRecorder::callback(const pcl::PCLPointCloud2::ConstPtr& cloud)
{
  if ((cloud->width * cloud->height) == 0) {
    return;
  }

  if (!tf_buffer_.canTransform(fixed_frame_, cloud->header.frame_id, pcl_conversions::fromPCL(cloud->header.stamp))) {
   //ROS_WARN("Could not get transform!");
   return;
  }

  std::stringstream ss;
  ss << "/tmp/pcl_capture/capture" << cloud->header.stamp << ".pcd";

  ROS_INFO ("Received %d data points. Storing in %s",
           (int)cloud->width * cloud->height,
           ss.str().c_str());

  Eigen::Affine3d transform;
  transform = tf2::transformToEigen (tf_buffer_.lookupTransform(fixed_frame_, cloud->header.frame_id,  pcl_conversions::fromPCL (cloud->header.stamp)));

  pcl::PointCloud<pcl::PointXYZ> pclCloud;
  pcl::fromPCLPointCloud2(*cloud, pclCloud);

  pcl::PointCloud<pcl::PointXYZ> transformedCloud;
  pcl::transformPointCloud (pclCloud, transformedCloud, transform);

  pcl::PCDWriter writer;
  writer.writeBinary(ss.str(), transformedCloud);
}
