/*
* Software License Agreement  (BSD License)
*
*  Point Cloud Library  (PCL) - www.pointclouds.org
*  Copyright  (c) 2012, Jeremie Papon.
*
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of Willow Garage, Inc. nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES  (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
*/

#ifndef IMPL_VOXEL_SUPERPIXELS_HPP_
#define IMPL_VOXEL_SUPERPIXELS_HPP_

#include <pcl/apps/cloud_composer/tools/voxel_superpixels.h>
#include <pcl/apps/cloud_composer/impl/cloud_item.hpp>
#include <pcl/apps/cloud_composer/items/normals_item.h>
#include <pcl/point_cloud.h>
#include <pcl/segmentation/voxel_superpixels.h>


template <typename PointT> QList <pcl::cloud_composer::CloudComposerItem*>
pcl::cloud_composer::VoxelSuperpixelsTool::performTemplatedAction (QList <const CloudComposerItem*> input_data)
{
  QList <CloudComposerItem*> output;  
  
  foreach (const CloudComposerItem* input_item, input_data)
  {
   // if ( !input_item->isSanitized () )
  //  {
  //    qCritical () << "VoxelSuperpixelsTool requires sanitized input!";
  //    return output;
  //  }
    QVariant variant = input_item->data (ItemDataRole::CLOUD_TEMPLATED);
    if ( ! variant.canConvert <typename PointCloud<PointT>::Ptr> () )
    {  
      qWarning () << "Attempted to cast to template type which does not exist in this item! (input list)";
      return output;
    }
    typename PointCloud <PointT>::Ptr input_cloud = variant.value <typename PointCloud<PointT>::Ptr> ();
    //TODO: Check if Voxelized
    
}
  

  foreach (const CloudComposerItem* input_item, input_data)
  {  
    QVariant variant = input_item->data (ItemDataRole::CLOUD_TEMPLATED);
    typename PointCloud <PointT>::Ptr input_cloud = variant.value <typename PointCloud<PointT>::Ptr> ();
    
    float resolution = parameter_model_->getProperty("Resolution").toFloat ();
    qDebug () << "Octree resolution = "<<resolution;
    float seed_resolution = parameter_model_->getProperty("Seed Resolution").toFloat ();
    qDebug () << "Seed resolution = "<<seed_resolution;
    pcl::VoxelSuperpixels<PointT> super;
    super.setInputCloud (input_cloud);
    super.setVoxelResolution (resolution);
    super.setSeedResolution (seed_resolution);
    
    std::vector <pcl::PointIndices> superpixels;
    super.extract (superpixels);
    
    std::vector <int> seeds;
    super.getSeedIndices (seeds);
    
    typename pcl::PointCloud<PointXYZRGB>::Ptr color_segments;
    color_segments = super.getColoredCloud ();
    
    //typename pcl::PointCloud<PointXYZRGB>::Ptr seed_cloud;
    //seed_cloud = super.getSeedCloud ();
    
    
    CloudItem*  cloud_item_out = CloudItem::createCloudItemFromTemplate<PointXYZRGB>(input_item->text(),color_segments);
    output.append (cloud_item_out);
  }
  
  
  
  
  return output;
  
}




#define PCL_INSTANTIATE_performTemplatedAction(T) template PCL_EXPORTS void pcl::cloud_composer::VoxelSuperpixelsTool::performTemplatedAction<T> (QList <const CloudComposerItem*>);



#endif //IMPL_VOXEL_SUPERPIXELS_HPP_