/*
 * Software License Agreement (BSD License)
 *
 * Point Cloud Library (PCL) - www.pointclouds.org
 * Copyright (c) 2009-2012, Willow Garage, Inc.
 * Copyright (c) 2012-, Open Perception, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of the copyright holder(s) nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: input_data_processing.h 7093 2012-09-09 15:22:35Z svn $
 *
 */

#ifndef PCL_IN_HAND_SCANNER_INPUT_DATA_PROCESSING_H
#define PCL_IN_HAND_SCANNER_INPUT_DATA_PROCESSING_H

#include <pcl/apps/in_hand_scanner/common_types.h>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

namespace pcl
{
  template <class PointInT, class PointOutT>
  class IntegralImageNormalEstimation;
} // End namespace pcl

////////////////////////////////////////////////////////////////////////////////
// InputDataProcessing
////////////////////////////////////////////////////////////////////////////////

namespace pcl
{
  namespace ihs
  {

    class InputDataProcessing
    {

      public:

        typedef pcl::ihs::PointInput         PointInput;
        typedef pcl::ihs::CloudInput         CloudInput;
        typedef pcl::ihs::CloudInputPtr      CloudInputPtr;
        typedef pcl::ihs::CloudInputConstPtr CloudInputConstPtr;

        typedef pcl::ihs::PointProcessed         PointProcessed;
        typedef pcl::ihs::CloudProcessed         CloudProcessed;
        typedef pcl::ihs::CloudProcessedPtr      CloudProcessedPtr;
        typedef pcl::ihs::CloudProcessedConstPtr CloudProcessedConstPtr;

        typedef pcl::Normal                            Normal;
        typedef pcl::PointCloud <Normal>               CloudNormals;
        typedef boost::shared_ptr <CloudNormals>       CloudNormalsPtr;
        typedef boost::shared_ptr <const CloudNormals> CloudNormalsConstPtr;

        typedef pcl::IntegralImageNormalEstimation <PointInput, Normal> NormalEstimation;
        typedef boost::shared_ptr <NormalEstimation>                    NormalEstimationPtr;
        typedef boost::shared_ptr <const NormalEstimation>              NormalEstimationConstPtr;

      public:

        InputDataProcessing ();

      public:

        CloudProcessedPtr
        process (const CloudInputConstPtr& cloud) const;

        CloudProcessedPtr
        calculateNormals (const CloudInputConstPtr& cloud) const;

        void
        getCropBox (float& x_min, float& x_max,
                    float& y_min, float& y_max,
                    float& z_min, float& z_max) const;

      private:

        NormalEstimationPtr normal_estimation_;

        float               x_min_;
        float               x_max_;
        float               y_min_;
        float               y_max_;
        float               z_min_;
        float               z_max_;
    };

  } // End namespace ihs
} // End namespace pcl

#endif // PCL_IN_HAND_SCANNER_INPUT_DATA_PROCESSING_H
