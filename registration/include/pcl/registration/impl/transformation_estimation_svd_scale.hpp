/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
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
 * $Id: transformation_estimation_svd_scale.hpp 7927 2012-11-14 04:11:45Z rusu $
 *
 */
#ifndef PCL_REGISTRATION_TRANSFORMATION_ESTIMATION_SVD_SCALE_HPP_
#define PCL_REGISTRATION_TRANSFORMATION_ESTIMATION_SVD_SCALE_HPP_

#include <pcl/registration/transformation_estimation_svd_scale.h>

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget, typename Scalar> void
pcl::registration::TransformationEstimationSVDScale<PointSource, PointTarget, Scalar>::getTransformationFromCorrelation (
    const Eigen::MatrixXf &cloud_src_demean,
    const Eigen::Vector4f &centroid_src,
    const Eigen::MatrixXf &cloud_tgt_demean,
    const Eigen::Vector4f &centroid_tgt,
    Matrix4 &transformation_matrix) const
{
  transformation_matrix.setIdentity ();

  // Assemble the correlation matrix H = source * target'
  Eigen::Matrix<Scalar, 3, 3> H = (cloud_src_demean.cast<Scalar> () * cloud_tgt_demean.cast<Scalar> ().transpose ()).topLeftCorner (3, 3);

  // Compute the Singular Value Decomposition
  Eigen::JacobiSVD<Eigen::Matrix<Scalar, 3, 3> > svd (H, Eigen::ComputeFullU | Eigen::ComputeFullV);
  Eigen::Matrix<Scalar, 3, 3> u = svd.matrixU ();
  Eigen::Matrix<Scalar, 3, 3> v = svd.matrixV ();

  // Compute R = V * U'
  if (u.determinant () * v.determinant () < 0)
  {
    for (int x = 0; x < 3; ++x)
      v (x, 2) *= -1;
  }

  Eigen::Matrix<Scalar, 3, 3> R = v * u.transpose ();

  // rotated cloud
  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> src_ = R * cloud_src_demean.cast<Scalar> ();
  
  float scale1, scale2;
  double sum_ss = 0.0f, sum_tt = 0.0f, sum_tt_ = 0.0f;
  for (unsigned corrIdx = 0; corrIdx < cloud_src_demean.cols (); ++corrIdx)
  {
    sum_ss += cloud_src_demean (0, corrIdx) * cloud_src_demean (0, corrIdx);
    sum_ss += cloud_src_demean (1, corrIdx) * cloud_src_demean (1, corrIdx);
    sum_ss += cloud_src_demean (2, corrIdx) * cloud_src_demean (2, corrIdx);
    
    sum_tt += cloud_tgt_demean (0, corrIdx) * cloud_tgt_demean (0, corrIdx);
    sum_tt += cloud_tgt_demean (1, corrIdx) * cloud_tgt_demean (1, corrIdx);
    sum_tt += cloud_tgt_demean (2, corrIdx) * cloud_tgt_demean (2, corrIdx);
    
    sum_tt_ += cloud_tgt_demean (0, corrIdx) * src_ (0, corrIdx);
    sum_tt_ += cloud_tgt_demean (1, corrIdx) * src_ (1, corrIdx);
    sum_tt_ += cloud_tgt_demean (2, corrIdx) * src_ (2, corrIdx);
  }
  
  scale1 = sqrt (sum_tt / sum_ss);
  scale2 = sum_tt_ / sum_ss;
  float scale = scale2;
  transformation_matrix.topLeftCorner (3, 3) = scale * R;
  const Eigen::Matrix<Scalar, 3, 1> Rc (R * centroid_src.cast<Scalar> ().head (3));
  transformation_matrix.block (0, 3, 3, 1) = centroid_tgt.cast<Scalar> (). head (3) - Rc;
}

//#define PCL_INSTANTIATE_TransformationEstimationSVD(T,U) template class PCL_EXPORTS pcl::registration::TransformationEstimationSVD<T,U>;

#endif /* PCL_REGISTRATION_TRANSFORMATION_ESTIMATION_SVD_SCALE_HPP_ */
