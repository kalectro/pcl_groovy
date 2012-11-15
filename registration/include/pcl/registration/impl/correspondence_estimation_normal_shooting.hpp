/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2012, Willow Garage, Inc.
 *  Copyright (c) 2012-, Open Perception, Inc.
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
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: correspondence_estimation_normal_shooting.hpp 7228 2012-09-21 04:20:17Z rusu $
 *
 */
#ifndef PCL_REGISTRATION_IMPL_CORRESPONDENCE_ESTIMATION_NORMAL_SHOOTING_H_
#define PCL_REGISTRATION_IMPL_CORRESPONDENCE_ESTIMATION_NORMAL_SHOOTING_H_

#include <pcl/registration/correspondence_estimation_normal_shooting.h>

///////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget, typename NormalT, typename Scalar> bool
pcl::registration::CorrespondenceEstimationNormalShooting<PointSource, PointTarget, NormalT, Scalar>::initCompute ()
{
  if (!source_normals_)
  {
    PCL_WARN ("[pcl::registration::%s::initCompute] Datasets containing normals for source have not been given!\n", getClassName ().c_str ());
    return (false);
  }

  return (CorrespondenceEstimationBase<PointSource, PointTarget, Scalar>::initCompute ());
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget, typename NormalT, typename Scalar> void
pcl::registration::CorrespondenceEstimationNormalShooting<PointSource, PointTarget, NormalT, Scalar>::rotatePointCloudNormals (
    const pcl::PointCloud<NormalT> &cloud_in,
    pcl::PointCloud<NormalT> &cloud_out,
    const Eigen::Matrix<Scalar, 4, 4> &transform)
{
  if (&cloud_in != &cloud_out)
  {
    // Note: could be replaced by cloud_out = cloud_in
    cloud_out.header   = cloud_in.header;
    cloud_out.width    = cloud_in.width;
    cloud_out.height   = cloud_in.height;
    cloud_out.is_dense = cloud_in.is_dense;
    cloud_out.points.reserve (cloud_out.points.size ());
    cloud_out.points.assign (cloud_in.points.begin (), cloud_in.points.end ());
  }

  for (size_t i = 0; i < cloud_out.points.size (); ++i)
  {
    // Rotate normals (WARNING: transform.rotation () uses SVD internally!)
    Eigen::Matrix<Scalar, 3, 1> nt (cloud_in[i].normal_x, cloud_in[i].normal_y, cloud_in[i].normal_z);
    cloud_out[i].normal_x = static_cast<float> (transform (0, 0) * nt.coeffRef (0) + transform (0, 1) * nt.coeffRef (1) + transform (0, 2) * nt.coeffRef (2));
    cloud_out[i].normal_y = static_cast<float> (transform (1, 0) * nt.coeffRef (0) + transform (1, 1) * nt.coeffRef (1) + transform (1, 2) * nt.coeffRef (2));
    cloud_out[i].normal_z = static_cast<float> (transform (2, 0) * nt.coeffRef (0) + transform (2, 1) * nt.coeffRef (1) + transform (2, 2) * nt.coeffRef (2));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget, typename NormalT, typename Scalar> void
pcl::registration::CorrespondenceEstimationNormalShooting<PointSource, PointTarget, NormalT, Scalar>::determineCorrespondences (
    pcl::Correspondences &correspondences, double max_distance)
{
  if (!initCompute ())
    return;

  typedef typename pcl::traits::fieldList<PointTarget>::type FieldListTarget;
  correspondences.resize (indices_->size ());

  std::vector<int> nn_indices (k_);
  std::vector<float> nn_dists (k_);

  double min_dist = std::numeric_limits<double>::max ();
  int min_index = 0;
  
  pcl::Correspondence corr;
  unsigned int nr_valid_correspondences = 0;

  // Check if the template types are the same. If true, avoid a copy.
  // Both point types MUST be registered using the POINT_CLOUD_REGISTER_POINT_STRUCT macro!
  if (isSamePointType<PointSource, PointTarget> ())
  {
    PointTarget pt;
    // Iterate over the input set of source indices
    for (std::vector<int>::const_iterator idx_i = indices_->begin (); idx_i != indices_->end (); ++idx_i)
    {
      tree_->nearestKSearch (input_->points[*idx_i], k_, nn_indices, nn_dists);

      // Among the K nearest neighbours find the one with minimum perpendicular distance to the normal
      min_dist = std::numeric_limits<double>::max ();
      
      // Find the best correspondence
      for (size_t j = 0; j < nn_indices.size (); j++)
      {
        // computing the distance between a point and a line in 3d. 
        // Reference - http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
        pt.x = target_->points[nn_indices[j]].x - input_->points[*idx_i].x;
        pt.y = target_->points[nn_indices[j]].y - input_->points[*idx_i].y;
        pt.z = target_->points[nn_indices[j]].z - input_->points[*idx_i].z;

        const NormalT &normal = source_normals_->points[*idx_i];
        Eigen::Vector3d N (normal.normal_x, normal.normal_y, normal.normal_z);
        Eigen::Vector3d V (pt.x, pt.y, pt.z);
        Eigen::Vector3d C = N.cross (V);
        
        // Check if we have a better correspondence
        double dist = C.dot (C);
        if (dist < min_dist)
        {
          min_dist = dist;
          min_index = static_cast<int> (j);
        }
      }
      if (min_dist > max_distance)
        continue;

      corr.index_query = *idx_i;
      corr.index_match = nn_indices[min_index];
      corr.distance = nn_dists[min_index];//min_dist;
      correspondences[nr_valid_correspondences++] = corr;
    }
  }
  else
  {
    PointTarget pt;
    
    // Iterate over the input set of source indices
    for (std::vector<int>::const_iterator idx_i = indices_->begin (); idx_i != indices_->end (); ++idx_i)
    {
      tree_->nearestKSearch (input_->points[*idx_i], k_, nn_indices, nn_dists);
 
      // Among the K nearest neighbours find the one with minimum perpendicular distance to the normal
      min_dist = std::numeric_limits<double>::max ();
      
      // Find the best correspondence
      for (size_t j = 0; j < nn_indices.size (); j++)
      {
        PointSource pt_src;
        // Copy the source data to a target PointTarget format so we can search in the tree
        pcl::for_each_type <FieldListTarget> (pcl::NdConcatenateFunctor <PointSource, PointTarget> (
            input_->points[*idx_i], 
            pt_src));

        // computing the distance between a point and a line in 3d. 
        // Reference - http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
        pt.x = target_->points[nn_indices[j]].x - pt_src.x;
        pt.y = target_->points[nn_indices[j]].y - pt_src.y;
        pt.z = target_->points[nn_indices[j]].z - pt_src.z;
        
        const NormalT &normal = source_normals_->points[*idx_i];
        Eigen::Vector3d N (normal.normal_x, normal.normal_y, normal.normal_z);
        Eigen::Vector3d V (pt.x, pt.y, pt.z);
        Eigen::Vector3d C = N.cross (V);
        
        // Check if we have a better correspondence
        double dist = C.dot (C);
        if (dist < min_dist)
        {
          min_dist = dist;
          min_index = static_cast<int> (j);
        }
      }
      if (min_dist > max_distance)
        continue;
      
      corr.index_query = *idx_i;
      corr.index_match = nn_indices[min_index];
      corr.distance = nn_dists[min_index];//min_dist;
      correspondences[nr_valid_correspondences++] = corr;
    }
  }
  correspondences.resize (nr_valid_correspondences);
  deinitCompute ();
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget, typename NormalT, typename Scalar> void
pcl::registration::CorrespondenceEstimationNormalShooting<PointSource, PointTarget, NormalT, Scalar>::determineReciprocalCorrespondences (
    pcl::Correspondences &correspondences, double max_distance)
{
  if (!initCompute ())
    return;

  typedef typename pcl::traits::fieldList<PointSource>::type FieldListSource;
  typedef typename pcl::traits::fieldList<PointTarget>::type FieldListTarget;
  typedef typename pcl::intersect<FieldListSource, FieldListTarget>::type FieldList;
  
  // setup tree for reciprocal search
  pcl::KdTreeFLANN<PointSource> tree_reciprocal;
  // Set the internal point representation of choice
  if (point_representation_)
    tree_reciprocal.setPointRepresentation (point_representation_);

  tree_reciprocal.setInputCloud (input_, indices_);

  correspondences.resize (indices_->size ());

  std::vector<int> nn_indices (k_);
  std::vector<float> nn_dists (k_);
  std::vector<int> index_reciprocal (1);
  std::vector<float> distance_reciprocal (1);

  double min_dist = std::numeric_limits<double>::max ();
  int min_index = 0;
  
  pcl::Correspondence corr;
  unsigned int nr_valid_correspondences = 0;
  int target_idx = 0;

  // Check if the template types are the same. If true, avoid a copy.
  // Both point types MUST be registered using the POINT_CLOUD_REGISTER_POINT_STRUCT macro!
  if (isSamePointType<PointSource, PointTarget> ())
  {
    PointTarget pt;
    // Iterate over the input set of source indices
    for (std::vector<int>::const_iterator idx_i = indices_->begin (); idx_i != indices_->end (); ++idx_i)
    {
      tree_->nearestKSearch (input_->points[*idx_i], k_, nn_indices, nn_dists);

      // Among the K nearest neighbours find the one with minimum perpendicular distance to the normal
      min_dist = std::numeric_limits<double>::max ();
      
      // Find the best correspondence
      for (size_t j = 0; j < nn_indices.size (); j++)
      {
        // computing the distance between a point and a line in 3d. 
        // Reference - http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
        pt.x = target_->points[nn_indices[j]].x - input_->points[*idx_i].x;
        pt.y = target_->points[nn_indices[j]].y - input_->points[*idx_i].y;
        pt.z = target_->points[nn_indices[j]].z - input_->points[*idx_i].z;

        const NormalT &normal = source_normals_->points[*idx_i];
        Eigen::Vector3d N (normal.normal_x, normal.normal_y, normal.normal_z);
        Eigen::Vector3d V (pt.x, pt.y, pt.z);
        Eigen::Vector3d C = N.cross (V);
        
        // Check if we have a better correspondence
        double dist = C.dot (C);
        if (dist < min_dist)
        {
          min_dist = dist;
          min_index = static_cast<int> (j);
        }
      }
      if (min_dist > max_distance)
        continue;

      // Check if the correspondence is reciprocal
      target_idx = nn_indices[min_index];
      tree_reciprocal.nearestKSearch (target_->points[target_idx], 1, index_reciprocal, distance_reciprocal);

      if (*idx_i != index_reciprocal[0])
        continue;

      // Correspondence IS reciprocal, save it and continue
      corr.index_query = *idx_i;
      corr.index_match = nn_indices[min_index];
      corr.distance = nn_dists[min_index];//min_dist;
      correspondences[nr_valid_correspondences++] = corr;
    }
  }
  else
  {
    PointTarget pt;
    
    // Iterate over the input set of source indices
    for (std::vector<int>::const_iterator idx_i = indices_->begin (); idx_i != indices_->end (); ++idx_i)
    {
      tree_->nearestKSearch (input_->points[*idx_i], k_, nn_indices, nn_dists);

      // Among the K nearest neighbours find the one with minimum perpendicular distance to the normal
      min_dist = std::numeric_limits<double>::max ();
      
      // Find the best correspondence
      for (size_t j = 0; j < nn_indices.size (); j++)
      {
        PointSource pt_src;
        // Copy the source data to a target PointTarget format so we can search in the tree
        pcl::for_each_type <FieldListTarget> (pcl::NdConcatenateFunctor <PointSource, PointTarget> (
            input_->points[*idx_i], 
            pt_src));

        // computing the distance between a point and a line in 3d. 
        // Reference - http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
        pt.x = target_->points[nn_indices[j]].x - pt_src.x;
        pt.y = target_->points[nn_indices[j]].y - pt_src.y;
        pt.z = target_->points[nn_indices[j]].z - pt_src.z;
        
        const NormalT &normal = source_normals_->points[*idx_i];
        Eigen::Vector3d N (normal.normal_x, normal.normal_y, normal.normal_z);
        Eigen::Vector3d V (pt.x, pt.y, pt.z);
        Eigen::Vector3d C = N.cross (V);
        
        // Check if we have a better correspondence
        double dist = C.dot (C);
        if (dist < min_dist)
        {
          min_dist = dist;
          min_index = static_cast<int> (j);
        }
      }
      if (min_dist > max_distance)
        continue;

      // Check if the correspondence is reciprocal
      target_idx = nn_indices[min_index];
      tree_reciprocal.nearestKSearch (target_->points[target_idx], 1, index_reciprocal, distance_reciprocal);

      if (*idx_i != index_reciprocal[0])
        continue;

      // Correspondence IS reciprocal, save it and continue
      corr.index_query = *idx_i;
      corr.index_match = nn_indices[min_index];
      corr.distance = nn_dists[min_index];//min_dist;
      correspondences[nr_valid_correspondences++] = corr;
    }
  }
  correspondences.resize (nr_valid_correspondences);
  deinitCompute ();
}

#endif    // PCL_REGISTRATION_IMPL_CORRESPONDENCE_ESTIMATION_NORMAL_SHOOTING_H_
