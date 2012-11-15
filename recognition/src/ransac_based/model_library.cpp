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
 * $Id$
 *
 */

#include <pcl/recognition/ransac_based/model_library.h>
#include <pcl/recognition/ransac_based/obj_rec_ransac.h>
#include <pcl/recognition/ransac_based/voxel_structure.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/kdtree/impl/kdtree_flann.hpp>
#include <pcl/console/print.h>
#include <cmath>
#include <vector>

#define PIf 3.14159265358979323846f

using namespace std;
using namespace pcl;
using namespace console;
using namespace recognition;

//============================================================================================================================================

ModelLibrary::ModelLibrary (float pair_width, float voxel_size)
: pair_width_ (pair_width), pair_width_eps_ (0.1f*pair_width), voxel_size_(voxel_size)
{
  num_of_cells_[0] = 10; // 60
  num_of_cells_[1] = 10; // 60
  num_of_cells_[2] = 10; // 60

  // Compute the bounds of the hash table
  float eps = 0.000001f; // To be sure that an angle of 0 or PI will not be excluded because it lies on the boundary of the voxel structure
  float bounds[6] = {-eps, PIf+eps, -eps, PIf+eps, -eps, PIf+eps};

  hash_table_.build (bounds, num_of_cells_);
}

//============================================================================================================================================

void
ModelLibrary::clear ()
{
  // Delete the model entries
  for ( map<string,Model*>::iterator it = models_.begin() ; it != models_.end() ; ++it )
    delete it->second;
  models_.clear();

  // Clear the hash table
  HashTableCell* cells = hash_table_.getVoxels();
  int num_bins = num_of_cells_[0]*num_of_cells_[1]*num_of_cells_[2];

  // Clear each cell entry
  for ( int i = 0 ; i < num_bins ; ++i )
    cells[i].clear();

  num_of_cells_[0] = num_of_cells_[1] = num_of_cells_[2] = 0;
}

//============================================================================================================================================

bool
ModelLibrary::addModel (PointCloudIn* points, PointCloudN* normals, const std::string& object_name)
{
#ifdef OBJ_REC_RANSAC_VERBOSE
  printf("ModelLibrary::%s(): begin\n", __func__);
#endif

  // Try to insert a new model entry
  pair<map<string,Model*>::iterator, bool> result = models_.insert (pair<string,Model*> (object_name, static_cast<Model*> (NULL)));

  // Check if 'object_name' is unique
  if (!result.second)
  {
    print_error ("'%s' already exists in the model library.\n", object_name.c_str ());
    return (false);
  }

  // It is unique -> create a new library model
  Model* new_model = new Model (points, normals, object_name);
  result.first->second = new_model;

  // Build the octree
  ORROctree& octree = new_model->getOctree ();
  octree.build (*points, voxel_size_, normals);

  vector<ORROctree::Node*> &full_leaves = octree.getFullLeaves ();
  list<ORROctree::Node*> inter_leaves;
  ORROctree::Node::Data *node_data1;
  int num_of_pairs = 0;

#ifdef OBJ_REC_RANSAC_VERBOSE
  printf("\tfilling the hash table ... "); fflush(stdout);
#endif

  // Run through all full leaves
  for ( vector<ORROctree::Node*>::iterator leaf1 = full_leaves.begin () ; leaf1 != full_leaves.end () ; ++leaf1 )
  {
    node_data1 = (*leaf1)->getData ();

    // Get all full leaves at the right distance to the current leaf
    inter_leaves.clear ();
    octree.getFullLeavesIntersectedBySphere (node_data1->getPoint (), pair_width_, inter_leaves);

    for ( list<ORROctree::Node*>::iterator leaf2 = inter_leaves.begin () ; leaf2 != inter_leaves.end () ; ++leaf2 )
    {
      // Compute the hash table key
      this->addToHashTable(new_model, node_data1, (*leaf2)->getData ());

      ++num_of_pairs;
    }
  }

#ifdef OBJ_REC_RANSAC_VERBOSE
  printf("OK\nModelLibrary::%s(): end\n", __func__);
#endif

  return (true);
}

//============================================================================================================================================

void
ModelLibrary::addToHashTable (const ModelLibrary::Model* model, ORROctree::Node::Data* data1, ORROctree::Node::Data* data2)
{
  float key[3];

  // Compute the descriptor signature for the oriented point pair (i, j)
  ObjRecRANSAC::compute_oriented_point_pair_signature (
    data1->getPoint (), data1->getNormal (),
    data2->getPoint (), data2->getNormal (), key);

  // Get the hash table cell containing 'key' (there is for sure such a cell since the hash table bounds are large enough)
  HashTableCell* cell = hash_table_.getVoxel (key);

  // Insert the pair (data1,data2) belonging to 'model'
  (*cell)[model].push_back (std::pair<ORROctree::Node::Data*,ORROctree::Node::Data*> (data1, data2));
}

//============================================================================================================================================
