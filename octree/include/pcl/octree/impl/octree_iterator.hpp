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
 * $Id: octree_iterator.hpp 6981 2012-08-28 01:24:09Z jkammerl $
 */

#ifndef PCL_OCTREE_ITERATOR_HPP_
#define PCL_OCTREE_ITERATOR_HPP_

#include <vector>
#include <assert.h>

#include <pcl/common/common.h>

namespace pcl
{
  namespace octree
  {
    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    OctreeDepthFirstIterator<DataT, OctreeT>::OctreeDepthFirstIterator (unsigned int maxDepth_arg) :
        OctreeIteratorBase<DataT, OctreeT> (maxDepth_arg), stack_ ()
    {
      // initialize iterator
      this->reset ();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    OctreeDepthFirstIterator<DataT, OctreeT>::OctreeDepthFirstIterator (OctreeT* octree_arg, unsigned int maxDepth_arg) :
        OctreeIteratorBase<DataT, OctreeT> (octree_arg, maxDepth_arg), stack_ ()
    {
      // initialize iterator
      this->reset ();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    OctreeDepthFirstIterator<DataT, OctreeT>::~OctreeDepthFirstIterator ()
    {
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    void OctreeDepthFirstIterator<DataT, OctreeT>::reset ()
    {
      OctreeIteratorBase<DataT, OctreeT>::reset ();

      if (this->octree_)
      {
        // allocate stack
        stack_.reserve (this->maxOctreeDepth_);

        // empty stack
        stack_.clear ();

        // pushing root node to stack
        IteratorState stackEntry;
        stackEntry.node_ = this->octree_->getRootNode ();
        stackEntry.depth_ = 0;
        stackEntry.key_.x = stackEntry.key_.y = stackEntry.key_.z = 0;

        stack_.push_back(stackEntry);

        this->currentState_ = &stack_.back();
      }

    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    void OctreeDepthFirstIterator<DataT, OctreeT>::skipChildVoxels ()
    {

      if (stack_.size ())
      {
        // current depth
        unsigned char current_depth = stack_.back ().depth_;

        // pop from stack as long as we find stack elements with depth >= current depth
        while (stack_.size () && (stack_.back ().depth_ >= current_depth))
          stack_.pop_back ();

        if (stack_.size ())
        {
          this->currentState_ = &stack_.back();
        } else
        {
          this->currentState_ = 0;
        }
      }

    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    OctreeDepthFirstIterator<DataT, OctreeT>&
    OctreeDepthFirstIterator<DataT, OctreeT>::operator++ ()
    {

      if (stack_.size ())
      {
        // get stack element
        IteratorState stackEntry = stack_.back ();
        stack_.pop_back ();

        stackEntry.depth_ ++;
        OctreeKey& currentKey = stackEntry.key_;

        if ( (this->maxOctreeDepth_>=stackEntry.depth_) &&
             (stackEntry.node_->getNodeType () == BRANCH_NODE) )
        {
          char childIdx;

          // current node is a branch node
          BranchNode* currentBranch =
              static_cast<BranchNode*> (stackEntry.node_);

          // add all children to stack
          for (childIdx = 7; childIdx >=0 ; --childIdx)
          {

            unsigned char idx = static_cast<unsigned char> (childIdx);

            // if child exist

            if (this->octree_->branchHasChild(*currentBranch, idx))
            {
              // add child to stack
              currentKey.pushBranch (idx);

              stackEntry.node_ = this->octree_->getBranchChildPtr(*currentBranch, idx);

              stack_.push_back(stackEntry);

              currentKey.popBranch();
            }
          }
        }

        if (stack_.size ())
        {
          this->currentState_ = &stack_.back();
        } else
        {
          this->currentState_ = 0;
        }
      }

      return (*this);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    OctreeBreadthFirstIterator<DataT, OctreeT>::OctreeBreadthFirstIterator (unsigned int maxDepth_arg) :
        OctreeIteratorBase<DataT, OctreeT> (maxDepth_arg), FIFO_ ()
    {
      OctreeIteratorBase<DataT, OctreeT>::reset ();

      // initialize iterator
      this->reset ();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    OctreeBreadthFirstIterator<DataT, OctreeT>::OctreeBreadthFirstIterator (OctreeT* octree_arg, unsigned int maxDepth_arg) :
        OctreeIteratorBase<DataT, OctreeT> (octree_arg, maxDepth_arg), FIFO_ ()
    {
      OctreeIteratorBase<DataT, OctreeT>::reset ();

      // initialize iterator
      this->reset ();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    OctreeBreadthFirstIterator<DataT, OctreeT>::~OctreeBreadthFirstIterator ()
    {
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    void OctreeBreadthFirstIterator<DataT, OctreeT>::reset ()
    {
      OctreeIteratorBase<DataT, OctreeT>::reset ();

      // init FIFO
      FIFO_.clear ();

      if (this->octree_)
      {
        // pushing root node to stack
        IteratorState FIFOEntry;
        FIFOEntry.node_ = this->octree_->getRootNode ();
        FIFOEntry.depth_ = 0;
        FIFOEntry.key_.x = FIFOEntry.key_.y = FIFOEntry.key_.z = 0;

        FIFO_.push_back(FIFOEntry);

        this->currentState_ = &FIFO_.front();
      }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    template<typename DataT, typename OctreeT>
    OctreeBreadthFirstIterator<DataT, OctreeT>&
    OctreeBreadthFirstIterator<DataT, OctreeT>::operator++ ()
    {

      if (FIFO_.size ())
      {
        // get stack element
        IteratorState FIFOEntry = FIFO_.front ();
        FIFO_.pop_front ();

        FIFOEntry.depth_ ++;
        OctreeKey& currentKey = FIFOEntry.key_;

        if ( (this->maxOctreeDepth_>=FIFOEntry.depth_) &&
             (FIFOEntry.node_->getNodeType () == BRANCH_NODE) )
        {
          char childIdx;

          // current node is a branch node
          BranchNode* currentBranch =
              static_cast<BranchNode*> (FIFOEntry.node_);

          // iterate over all children
          for (childIdx = 7; childIdx >=0 ; --childIdx)
          {

            unsigned char idx = static_cast<unsigned char> (childIdx);

            // if child exist
            if (this->octree_->branchHasChild(*currentBranch, idx))
            {
              // add child to stack
              currentKey.pushBranch (static_cast<unsigned char> (idx));

              FIFOEntry.node_ = this->octree_->getBranchChildPtr(*currentBranch, idx);

              FIFO_.push_back(FIFOEntry);

              currentKey.popBranch();
            }
          }
        }

        if (FIFO_.size ())
        {
          this->currentState_ = &FIFO_.front();
        } else
        {
          this->currentState_ = 0;
        }

      }

      return (*this);
    }
  }
}

#endif
