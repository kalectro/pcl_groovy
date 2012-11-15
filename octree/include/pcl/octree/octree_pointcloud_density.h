/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2011, Willow Garage, Inc.
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
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: octree_pointcloud_density.h 7148 2012-09-14 23:08:00Z jkammerl $
 */

#ifndef PCL_OCTREE_DENSITY_H
#define PCL_OCTREE_DENSITY_H

#include "octree_pointcloud.h"

#include "octree_base.h"
#include "octree2buf_base.h"

namespace pcl
{
  namespace octree
  {
    /** \brief @b Octree pointcloud density leaf node class
      * \note This class implements a leaf node that counts the amount of points which fall into its voxel space.
      * \author Julius Kammerl (julius@kammerl.de)
      */
    template<typename DataT>
    class OctreePointCloudDensityContainer : public OctreeContainerBase<DataT>
    {
      public:
        /** \brief Class initialization. */
        OctreePointCloudDensityContainer () : pointCounter_ (0)
        {
        }

        /** \brief Empty class deconstructor. */
        virtual ~OctreePointCloudDensityContainer ()
        {
        }

        /** \brief deep copy function */
        virtual OctreePointCloudDensityContainer *
        deepCopy () const
        {
          return (new OctreePointCloudDensityContainer (*this));
        }

        /** \brief Read input data. Only an internal counter is increased.
          */
        void
        setData (const DataT&)
        {
          pointCounter_++;
        }

        /** \brief Return point counter.
          * \return Amount of points
          */
        unsigned int
        getPointCounter ()
        {
          return (pointCounter_);
        }

        /** \brief Reset leaf node. */
        virtual void
        reset ()
        {
          pointCounter_ = 0;
        }

      private:
        unsigned int pointCounter_;

    };

    /** \brief @b Octree pointcloud density class
      * \note This class generate an octrees from a point cloud (zero-copy). Only the amount of points that fall into the leaf node voxel are stored.
      * \note The octree pointcloud is initialized with its voxel resolution. Its bounding box is automatically adjusted or can be predefined.
      * \note
      * \note typename: PointT: type of point used in pointcloud
      * \ingroup octree
      * \author Julius Kammerl (julius@kammerl.de)
      */
    template<typename PointT, typename LeafContainerT = OctreePointCloudDensityContainer<int> , typename BranchContainerT = OctreeContainerEmpty<int> >
    class OctreePointCloudDensity : public OctreePointCloud<PointT, LeafContainerT, BranchContainerT>
    {
      public:

      /** \brief OctreePointCloudDensity class constructor.
         *  \param resolution_arg:  octree resolution at lowest octree level
         * */
        OctreePointCloudDensity (const double resolution_arg) :
        OctreePointCloud<PointT, LeafContainerT, BranchContainerT> (resolution_arg)
        {
        }

        /** \brief Empty class deconstructor. */
        virtual
        ~OctreePointCloudDensity ()
        {
        }

        /** \brief Get the amount of points within a leaf node voxel which is addressed by a point
          * \param[in] point_arg: a point addressing a voxel
          * \return amount of points that fall within leaf node voxel
          */
        unsigned int
        getVoxelDensityAtPoint (const PointT& point_arg) const
        {
          unsigned int pointCount = 0;

          OctreePointCloudDensityContainer<int>* leaf = this->findLeafAtPoint (point_arg);

          if (leaf)
            pointCount = leaf->getPointCounter ();

          return (pointCount);
        }
    };
  }
}

#define PCL_INSTANTIATE_OctreePointCloudDensity(T) template class PCL_EXPORTS pcl::octree::OctreePointCloudDensity<T>;

#endif

