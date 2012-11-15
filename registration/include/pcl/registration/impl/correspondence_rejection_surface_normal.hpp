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
 * $Id: correspondence_rejection_surface_normal.hpp 7927 2012-11-14 04:11:45Z rusu $
 *
 */
#ifndef PCL_REGISTRATION_IMPL_CORRESPONDENCE_REJECTION_SURFACE_NORMAL_HPP_
#define PCL_REGISTRATION_IMPL_CORRESPONDENCE_REJECTION_SURFACE_NORMAL_HPP_

//////////////////////////////////////////////////////////////////////////////////////////////
void
pcl::registration::CorrespondenceRejectorSurfaceNormal::getRemainingCorrespondences (
    const pcl::Correspondences& original_correspondences, 
    pcl::Correspondences& remaining_correspondences)
{
  if (!data_container_)
  {
    PCL_ERROR ("[pcl::registratin::%s::getRemainingCorrespondences] DataContainer object is not initialized!\n", getClassName ().c_str ());
    return;
  }

  unsigned int number_valid_correspondences = 0;
  remaining_correspondences.resize (original_correspondences.size ());
  
  // Test each correspondence
  for (size_t i = 0; i < original_correspondences.size (); ++i)
  {
    if (boost::static_pointer_cast<DataContainer<pcl::PointXYZ, pcl::PointNormal> > 
        (data_container_)->getCorrespondenceScoreFromNormals (original_correspondences[i]) > threshold_)
      remaining_correspondences[number_valid_correspondences++] = original_correspondences[i];
  }
  remaining_correspondences.resize (number_valid_correspondences);
}

#endif /* PCL_REGISTRATION_IMPL_CORRESPONDENCE_REJECTION_SURFACE_NORMAL_HPP_ */