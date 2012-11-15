/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2012, Willow Garage, Inc.
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
 *
 */

#ifndef PCL_RECOGNITION_MODEL_LIBRARY_H_
#define PCL_RECOGNITION_MODEL_LIBRARY_H_

#include "voxel_structure.h"
#include "orr_octree.h"
#include <pcl/pcl_exports.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <string>
#include <list>
#include <set>
#include <map>

namespace pcl
{
  namespace recognition
  {
    class PCL_EXPORTS ModelLibrary
    {
      public:
        typedef pcl::PointCloud<pcl::PointXYZ> PointCloudIn;
        typedef pcl::PointCloud<pcl::Normal> PointCloudN;

//        typedef boost::shared_ptr<PointCloudIn> PointCloudInPtr;
//        typedef boost::shared_ptr<PointCloudN> PointCloudNPtr;
//
//        typedef boost::shared_ptr<const PointCloudIn> PointCloudInConstPtr;
//        typedef boost::shared_ptr<const PointCloudN> PointCloudNConstPtr;

        /** \brief Stores some information about the model. */
        class Model
        {
          public:
            Model (PointCloudIn* points, PointCloudN* normals, std::string object_name):
              points_ (points),
              normals_(normals),
              obj_name_(object_name){}
            virtual ~Model (){}

            ORROctree& getOctree() { return octree_;}

          public:
            PointCloudIn *points_;
            PointCloudN *normals_;
            const std::string obj_name_;
            ORROctree octree_;
        };

        typedef std::list<std::pair<ORROctree::Node::Data*,ORROctree::Node::Data*> > node_data_pair_list;
        typedef std::map<const Model*, node_data_pair_list> HashTableCell;
        typedef VoxelStructure<HashTableCell, float> HashTable;

      public:
        /** \brief This class is used by 'ObjRecRANSAC' to maintain the object models to be recognized. Normally, you do not need to use
          * this class directly. */
        ModelLibrary (float pair_width, float voxel_size);
        virtual ~ModelLibrary ()
        {
          this->clear();
        }

        /** \brief Removes all models from the library and clears the hash table. */
        void
        clear ();

        /** \brief Adds a model to the hash table.
          *
          * \param[in]  points represents the model to be added.
          * \param[in]  normals are the normals at the model points.
          * \param[in] object_name is the unique name of the object to be added.
          *
          * Returns true if model successfully added and false otherwise (e.g., if object_name is not unique). */
        bool
        addModel (PointCloudIn* points, PointCloudN* normals, const std::string& object_name);

        /** \brief Returns the hash table built by this instance. */
        const HashTable*
        getHashTable ()
        {
          return (&hash_table_);
        }

      protected:
        void
        addToHashTable (const Model* model, ORROctree::Node::Data* data1, ORROctree::Node::Data* data2);

      protected:
        std::map<std::string,Model*> models_;
        float pair_width_, pair_width_eps_, voxel_size_;

        HashTable hash_table_;
        int num_of_cells_[3];
    };
  } // namespace recognition
} // namespace pcl

#endif // PCL_RECOGNITION_MODEL_LIBRARY_H_
