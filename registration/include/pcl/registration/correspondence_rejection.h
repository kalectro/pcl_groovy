/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2011, Willow Garage, Inc.
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
 * $Id: correspondence_rejection.h 7927 2012-11-14 04:11:45Z rusu $
 *
 */

#ifndef PCL_REGISTRATION_CORRESPONDENCE_REJECTION_H_
#define PCL_REGISTRATION_CORRESPONDENCE_REJECTION_H_

#include <pcl/registration/correspondence_types.h>
#include <pcl/registration/correspondence_sorting.h>
#include <pcl/console/print.h>
#include <pcl/common/transforms.h>
#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>

namespace pcl
{
  namespace registration
  {
    /** @b CorrespondenceRejector represents the base class for correspondence rejection methods
      * \author Dirk Holz
      * \ingroup registration
      */
    class CorrespondenceRejector
    {
      public:
        typedef boost::shared_ptr<CorrespondenceRejector> Ptr;
        typedef boost::shared_ptr<const CorrespondenceRejector> ConstPtr;

        /** \brief Empty constructor. */
        CorrespondenceRejector () 
          : rejection_name_ ()
          , input_correspondences_ () 
        {}

        /** \brief Empty destructor. */
        virtual ~CorrespondenceRejector () {}

        /** \brief Provide a pointer to the vector of the input correspondences.
          * \param[in] correspondences the const boost shared pointer to a correspondence vector
          */
        virtual inline void 
        setInputCorrespondences (const CorrespondencesConstPtr &correspondences) 
        { 
          input_correspondences_ = correspondences; 
        };

        /** \brief Get a pointer to the vector of the input correspondences.
          * \return correspondences the const boost shared pointer to a correspondence vector
          */
        inline CorrespondencesConstPtr 
        getInputCorrespondences () { return input_correspondences_; };

        /** \brief Run correspondence rejection
          * \param[out] correspondences Vector of correspondences that have not been rejected.
          */
        inline void 
        getCorrespondences (pcl::Correspondences &correspondences)
        {
          if (!input_correspondences_ || (input_correspondences_->empty ()))
            return;

          applyRejection (correspondences);
        }

        /** \brief Get a list of valid correspondences after rejection from the original set of correspondences.
          * Pure virtual. Compared to \a getCorrespondences this function is
          * stateless, i.e., input correspondences do not need to be provided beforehand,
          * but are directly provided in the function call.
          * \param[in] original_correspondences the set of initial correspondences given
          * \param[out] remaining_correspondences the resultant filtered set of remaining correspondences
          */
        virtual inline void 
        getRemainingCorrespondences (const pcl::Correspondences& original_correspondences, 
                                     pcl::Correspondences& remaining_correspondences) = 0;

        /** \brief Determine the indices of query points of
          * correspondences that have been rejected, i.e., the difference
          * between the input correspondences (set via \a setInputCorrespondences)
          * and the given correspondence vector.
          * \param[in] correspondences Vector of correspondences after rejection
          * \param[out] indices Vector of query point indices of those correspondences
          * that have been rejected.
          */
        inline void 
        getRejectedQueryIndices (const pcl::Correspondences &correspondences, 
                                 std::vector<int>& indices)
        {
          if (!input_correspondences_ || input_correspondences_->empty ())
          {
            PCL_WARN ("[pcl::registration::%s::getRejectedQueryIndices] Input correspondences not set (lookup of rejected correspondences _not_ possible).\n", getClassName ().c_str ());
            return;
          }

          pcl::getRejectedQueryIndices(*input_correspondences_, correspondences, indices);
        }

        /** \brief Get a string representation of the name of this class. */
        inline const std::string& 
        getClassName () const { return (rejection_name_); }

        /** \brief Provide a simple mechanism to update the internal source cloud
          * using a given transformation. Used in registration loops.
          * \param[in] transform the transform to apply over the source cloud
          */
        virtual bool
        updateSource (const Eigen::Matrix4d &transform) = 0;

      protected:

        /** \brief The name of the rejection method. */
        std::string rejection_name_;

        /** \brief The input correspondences. */
        CorrespondencesConstPtr input_correspondences_;

        /** \brief Abstract rejection method. */
        virtual void 
        applyRejection (Correspondences &correspondences) = 0;
    };

    /** @b DataContainerInterface provides a generic interface for computing correspondence scores between correspondent
      * points in the input and target clouds
      * \ingroup registration
      */
    class DataContainerInterface
    {
      public:
        virtual ~DataContainerInterface () {}
        virtual double getCorrespondenceScore (int index) = 0;
        virtual double getCorrespondenceScore (const pcl::Correspondence &) = 0;
        virtual bool updateSource (const Eigen::Matrix4d &transform) = 0;
     };

    /** @b DataContainer is a container for the input and target point clouds and implements the interface 
      * to compute correspondence scores between correspondent points in the input and target clouds
      * \ingroup registration
      */
    template <typename PointT, typename NormalT = pcl::PointNormal>
    class DataContainer : public DataContainerInterface
    {
      typedef pcl::PointCloud<PointT> PointCloud;
      typedef typename PointCloud::Ptr PointCloudPtr;
      typedef typename PointCloud::ConstPtr PointCloudConstPtr;

      typedef typename pcl::KdTree<PointT>::Ptr KdTreePtr;
      
      typedef pcl::PointCloud<NormalT> Normals;
      typedef typename Normals::Ptr NormalsPtr;
      typedef typename Normals::ConstPtr NormalsConstPtr;

      public:

        /** \brief Empty constructor. */
        DataContainer (bool needs_normals = false) 
          : input_ ()
          , input_transformed_ ()
          , target_ ()
          , input_normals_ ()
          , input_normals_transformed_ ()
          , target_normals_ ()
          , tree_ (new pcl::KdTreeFLANN<PointT>)
          , class_name_ ("DataContainer")
          , needs_normals_ (needs_normals)
        {
        }

        /** \brief Provide a source point cloud dataset (must contain XYZ
          * data!), used to compute the correspondence distance.  
          * \param[in] cloud a cloud containing XYZ data
          */
        inline void 
        setInputCloud (const PointCloudConstPtr &cloud)
        {
          PCL_WARN ("[pcl::registration::%s::setInputCloud] setInputCloud is deprecated. Please use setInputSource instead.\n", getClassName ().c_str ());
          input_ = cloud;
        }

        /** \brief Get a pointer to the input point cloud dataset target. */
        inline PointCloudConstPtr const 
        getInputCloud () 
        { 
          PCL_WARN ("[pcl::registration::%s::getInputCloud] getInputCloud is deprecated. Please use getInputSource instead.\n", getClassName ().c_str ());
          return (input_); 
        }

        /** \brief Provide a source point cloud dataset (must contain XYZ
          * data!), used to compute the correspondence distance.  
          * \param[in] cloud a cloud containing XYZ data
          */
        inline void 
        setInputSource (const PointCloudConstPtr &cloud)
        {
          input_ = cloud;
        }

        /** \brief Get a pointer to the input point cloud dataset target. */
        inline PointCloudConstPtr const 
        getInputSource () { return (input_); }

        /** \brief Provide a target point cloud dataset (must contain XYZ
          * data!), used to compute the correspondence distance.  
          * \param[in] target a cloud containing XYZ data
          */
        inline void 
        setInputTarget (const PointCloudConstPtr &target)
        {
          target_ = target;
          tree_->setInputCloud (target_);
        }

        /** \brief Get a pointer to the input point cloud dataset target. */
        inline PointCloudConstPtr const 
        getInputTarget () { return (target_); }

        /** \brief Set the normals computed on the input point cloud
          * \param[in] normals the normals computed for the input cloud
          */
        inline void
        setInputNormals (const NormalsConstPtr &normals) { input_normals_ = normals; }

        /** \brief Get the normals computed on the input point cloud */
        inline NormalsConstPtr
        getInputNormals () { return (input_normals_); }

        /** \brief Set the normals computed on the target point cloud
          * \param[in] normals the normals computed for the input cloud
          */
        inline void
        setTargetNormals (const NormalsConstPtr &normals) { target_normals_ = normals; }
        
        /** \brief Get the normals computed on the target point cloud */
        inline NormalsConstPtr
        getTargetNormals () { return (target_normals_); }

        /** \brief Get the correspondence score for a point in the input cloud
          * \param[in] index index of the point in the input cloud
          */
        inline double 
        getCorrespondenceScore (int index)
        {
          std::vector<int> indices (1);
          std::vector<float> distances (1);
          if (tree_->nearestKSearch (input_->points[index], 1, indices, distances))
            return (distances[0]);
          else
            return (std::numeric_limits<double>::max ());
        }

        /** \brief Get the correspondence score for a given pair of correspondent points
          * \param[in] corr Correspondent points
          */
        inline double 
        getCorrespondenceScore (const pcl::Correspondence &corr)
        {
          // Get the source and the target feature from the list
          const PointT &src = input_->points[corr.index_query];
          const PointT &tgt = target_->points[corr.index_match];

          return ((src.getVector4fMap () - tgt.getVector4fMap ()).squaredNorm ());
        }
        
        /** \brief Get the correspondence score for a given pair of correspondent points based on 
          * the angle betweeen the normals. The normmals for the in put and target clouds must be 
          * set before using this function
          * \param[in] corr Correspondent points
          */
        inline double
        getCorrespondenceScoreFromNormals (const pcl::Correspondence &corr)
        {
          //assert ( (input_normals_->points.size () != 0) && (target_normals_->points.size () != 0) && "Normals are not set for the input and target point clouds");
          assert (input_normals_ && target_normals_ && "Normals are not set for the input and target point clouds");
          const NormalT &src = input_normals_->points[corr.index_query];
          const NormalT &tgt = target_normals_->points[corr.index_match];
          return (double ((src.normal[0] * tgt.normal[0]) + (src.normal[1] * tgt.normal[1]) + (src.normal[2] * tgt.normal[2])));
        }

        /** \brief Provide a simple mechanism to update the internal source cloud
          * using a given transformation. Used in registration loops.
          * \param[in] transform the transform to apply over the source cloud
          */
        virtual bool
        updateSource (const Eigen::Matrix4d &transform)
        {
          if (!input_)
          {
            PCL_ERROR ("[pcl::registration::%s::updateSource] No input XYZ dataset given. Please specify the input source cloud using setInputSource.\n", getClassName ().c_str ());
            return (false);
          }
          if (needs_normals_ && !input_normals_)
          {
            PCL_ERROR ("[pcl::registration::%s::updateSource] No input normals dataset given. Please specify the input normal cloud using setInputNormals.\n", getClassName ().c_str ());
            return (false);
          }
          input_transformed_.reset (new PointCloud);
          pcl::transformPointCloud<PointT, double> (*input_, *input_transformed_, transform);
          input_ = input_transformed_;
          
          if (!needs_normals_)
            return (true);

          input_normals_transformed_.reset (new Normals);
          rotatePointCloudNormals (*input_normals_, *input_normals_transformed_, transform);
          input_normals_ = input_normals_transformed_;
          return (true);
        }

      private:
        /** \brief Rotate the normals in a point cloud.
          * \param[in] cloud_in the input point cloud
          * \param[out] cloud_out the resultant output cloud containing rotated normals
          * \param[in] transform the 4x4 rigid transformation holding the rotation
          */
        void
        rotatePointCloudNormals (const pcl::PointCloud<NormalT> &cloud_in, 
                                 pcl::PointCloud<NormalT> &cloud_out,
                                 const Eigen::Matrix4d &transform);

        /** \brief The input point cloud dataset */
        PointCloudConstPtr input_;

        /** \brief The input transformed point cloud dataset */
        PointCloudPtr input_transformed_;

        /** \brief The target point cloud datase. */
        PointCloudConstPtr target_;

        /** \brief Normals to the input point cloud */
        NormalsConstPtr input_normals_;

        /** \brief Normals to the input point cloud */
        NormalsPtr input_normals_transformed_;

        /** \brief Normals to the target point cloud */
        NormalsConstPtr target_normals_;

        /** \brief A pointer to the spatial search object. */
        KdTreePtr tree_;

        /** \brief The name of the rejection method. */
        std::string class_name_;

        /** \brief Should the current data container use normals? */
        bool needs_normals_;

        /** \brief Get a string representation of the name of this class. */
        inline const std::string& 
        getClassName () const { return (class_name_); }
    };
  }
}

#include <pcl/registration/impl/correspondence_rejection.hpp>

#endif /* PCL_REGISTRATION_CORRESPONDENCE_REJECTION_H_ */

