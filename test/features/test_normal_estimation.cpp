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
 * $Id: test_normal_estimation.cpp 6579 2012-07-27 18:57:32Z rusu $
 *
 */

#include <gtest/gtest.h>
#include <pcl/point_cloud.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/io/pcd_io.h>

using namespace pcl;
using namespace pcl::io;
using namespace std;

typedef search::KdTree<PointXYZ>::Ptr KdTreePtr;

PointCloud<PointXYZ> cloud;
vector<int> indices;
KdTreePtr tree;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST (PCL, NormalEstimation)
{
  Eigen::Vector4f plane_parameters;
  float curvature;

  NormalEstimation<PointXYZ, Normal> n;

  // computePointNormal (indices, Vector)
  computePointNormal (cloud, indices, plane_parameters, curvature);
  EXPECT_NEAR (fabs (plane_parameters[0]), 0.035592, 1e-4);
  EXPECT_NEAR (fabs (plane_parameters[1]), 0.369596, 1e-4);
  EXPECT_NEAR (fabs (plane_parameters[2]), 0.928511, 1e-4);
  EXPECT_NEAR (fabs (plane_parameters[3]), 0.0622552, 1e-4);
  EXPECT_NEAR (curvature, 0.0693136, 1e-4);

  float nx, ny, nz;
  // computePointNormal (indices)
  n.computePointNormal (cloud, indices, nx, ny, nz, curvature);
  EXPECT_NEAR (fabs (nx), 0.035592, 1e-4);
  EXPECT_NEAR (fabs (ny), 0.369596, 1e-4);
  EXPECT_NEAR (fabs (nz), 0.928511, 1e-4);
  EXPECT_NEAR (curvature, 0.0693136, 1e-4);

  // computePointNormal (Vector)
  computePointNormal (cloud, plane_parameters, curvature);
  EXPECT_NEAR (plane_parameters[0],  0.035592,  1e-4);
  EXPECT_NEAR (plane_parameters[1],  0.369596,  1e-4);
  EXPECT_NEAR (plane_parameters[2],  0.928511,  1e-4);
  EXPECT_NEAR (plane_parameters[3], -0.0622552, 1e-4);
  EXPECT_NEAR (curvature,            0.0693136, 1e-4);

  // flipNormalTowardsViewpoint (Vector)
  flipNormalTowardsViewpoint (cloud.points[0], 0, 0, 0, plane_parameters);
  EXPECT_NEAR (plane_parameters[0], -0.035592,  1e-4);
  EXPECT_NEAR (plane_parameters[1], -0.369596,  1e-4);
  EXPECT_NEAR (plane_parameters[2], -0.928511,  1e-4);
  EXPECT_NEAR (plane_parameters[3],  0.0799743, 1e-4);

  // flipNormalTowardsViewpoint
  flipNormalTowardsViewpoint (cloud.points[0], 0, 0, 0, nx, ny, nz);
  EXPECT_NEAR (nx, -0.035592, 1e-4);
  EXPECT_NEAR (ny, -0.369596, 1e-4);
  EXPECT_NEAR (nz, -0.928511, 1e-4);

  // Object
  PointCloud<Normal>::Ptr normals (new PointCloud<Normal> ());

  // set parameters
  PointCloud<PointXYZ>::Ptr cloudptr = cloud.makeShared ();
  n.setInputCloud (cloudptr);
  EXPECT_EQ (n.getInputCloud (), cloudptr);
  boost::shared_ptr<vector<int> > indicesptr (new vector<int> (indices));
  n.setIndices (indicesptr);
  EXPECT_EQ (n.getIndices (), indicesptr);
  n.setSearchMethod (tree);
  EXPECT_EQ (n.getSearchMethod (), tree);
  n.setKSearch (static_cast<int> (indices.size ()));

  // estimate
  n.compute (*normals);
  EXPECT_EQ (normals->points.size (), indices.size ());

  for (size_t i = 0; i < normals->points.size (); ++i)
  {
    EXPECT_NEAR (normals->points[i].normal[0], -0.035592, 1e-4);
    EXPECT_NEAR (normals->points[i].normal[1], -0.369596, 1e-4);
    EXPECT_NEAR (normals->points[i].normal[2], -0.928511, 1e-4);
    EXPECT_NEAR (normals->points[i].curvature, 0.0693136, 1e-4);
  }

  PointCloud<PointXYZ>::Ptr surfaceptr = cloudptr;
  n.setSearchSurface (surfaceptr);
  EXPECT_EQ (n.getSearchSurface (), surfaceptr);

  // Additional test for searchForNeigbhors
  surfaceptr.reset (new PointCloud<PointXYZ>);
  *surfaceptr = *cloudptr;
  surfaceptr->points.resize (640 * 480);
  surfaceptr->width = 640;
  surfaceptr->height = 480;
  EXPECT_EQ (surfaceptr->points.size (), surfaceptr->width * surfaceptr->height);
  n.setSearchSurface (surfaceptr);
  tree.reset ();
  n.setSearchMethod (tree);

  // estimate
  n.compute (*normals);
  EXPECT_EQ (normals->points.size (), indices.size ());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST (PCL, NormalEstimationOpenMP)
{
  NormalEstimationOMP<PointXYZ, Normal> n (4); // instantiate 4 threads

  // Object
  PointCloud<Normal>::Ptr normals (new PointCloud<Normal> ());

  // set parameters
  PointCloud<PointXYZ>::Ptr cloudptr = cloud.makeShared ();
  n.setInputCloud (cloudptr);
  EXPECT_EQ (n.getInputCloud (), cloudptr);
  boost::shared_ptr<vector<int> > indicesptr (new vector<int> (indices));
  n.setIndices (indicesptr);
  EXPECT_EQ (n.getIndices (), indicesptr);
  n.setSearchMethod (tree);
  EXPECT_EQ (n.getSearchMethod (), tree);
  n.setKSearch (static_cast<int> (indices.size ()));

  // estimate
  n.compute (*normals);
  EXPECT_EQ (normals->points.size (), indices.size ());

  for (size_t i = 0; i < normals->points.size (); ++i)
  {
    EXPECT_NEAR (normals->points[i].normal[0], -0.035592, 1e-4);
    EXPECT_NEAR (normals->points[i].normal[1], -0.369596, 1e-4);
    EXPECT_NEAR (normals->points[i].normal[2], -0.928511, 1e-4);
    EXPECT_NEAR (normals->points[i].curvature, 0.0693136, 1e-4);
  }
}

#ifndef PCL_ONLY_CORE_POINT_TYPES
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TEST (PCL, NormalEstimationEigen)
  {
    Eigen::Vector4f plane_parameters;
    float curvature;

    NormalEstimation<PointXYZ, Eigen::MatrixXf> n;

    // computePointNormal (indices, Vector)
    computePointNormal (cloud, indices, plane_parameters, curvature);
    EXPECT_NEAR (fabs (plane_parameters[0]), 0.035592, 1e-4);
    EXPECT_NEAR (fabs (plane_parameters[1]), 0.369596, 1e-4);
    EXPECT_NEAR (fabs (plane_parameters[2]), 0.928511, 1e-4);
    EXPECT_NEAR (fabs (plane_parameters[3]), 0.0622552, 1e-4);
    EXPECT_NEAR (curvature, 0.0693136, 1e-4);

    float nx, ny, nz;
    // computePointNormal (indices)
    n.computePointNormal (cloud, indices, nx, ny, nz, curvature);
    EXPECT_NEAR (fabs (nx), 0.035592, 1e-4);
    EXPECT_NEAR (fabs (ny), 0.369596, 1e-4);
    EXPECT_NEAR (fabs (nz), 0.928511, 1e-4);
    EXPECT_NEAR (curvature, 0.0693136, 1e-4);

    // computePointNormal (Vector)
    computePointNormal (cloud, plane_parameters, curvature);
    EXPECT_NEAR (plane_parameters[0],  0.035592,  1e-4);
    EXPECT_NEAR (plane_parameters[1],  0.369596,  1e-4);
    EXPECT_NEAR (plane_parameters[2],  0.928511,  1e-4);
    EXPECT_NEAR (plane_parameters[3], -0.0622552, 1e-4);
    EXPECT_NEAR (curvature,            0.0693136, 1e-4);

    // flipNormalTowardsViewpoint (Vector)
    flipNormalTowardsViewpoint (cloud.points[0], 0, 0, 0, plane_parameters);
    EXPECT_NEAR (plane_parameters[0], -0.035592,  1e-4);
    EXPECT_NEAR (plane_parameters[1], -0.369596,  1e-4);
    EXPECT_NEAR (plane_parameters[2], -0.928511,  1e-4);
    EXPECT_NEAR (plane_parameters[3],  0.0799743, 1e-4);

    // flipNormalTowardsViewpoint
    flipNormalTowardsViewpoint (cloud.points[0], 0, 0, 0, nx, ny, nz);
    EXPECT_NEAR (nx, -0.035592, 1e-4);
    EXPECT_NEAR (ny, -0.369596, 1e-4);
    EXPECT_NEAR (nz, -0.928511, 1e-4);

    // Object
    PointCloud<Eigen::MatrixXf>::Ptr normals (new PointCloud<Eigen::MatrixXf> ());

    // set parameters
    PointCloud<PointXYZ>::Ptr cloudptr = cloud.makeShared ();
    n.setInputCloud (cloudptr);
    EXPECT_EQ (n.getInputCloud (), cloudptr);
    boost::shared_ptr<vector<int> > indicesptr (new vector<int> (indices));
    n.setIndices (indicesptr);
    EXPECT_EQ (n.getIndices (), indicesptr);
    n.setSearchMethod (tree);
    EXPECT_EQ (n.getSearchMethod (), tree);
    n.setKSearch (static_cast<int> (indices.size ()));

    // estimate
    n.computeEigen (*normals);
    EXPECT_EQ (normals->points.rows (), indices.size ());

    for (int i = 0; i < normals->points.rows (); ++i)
    {
      EXPECT_NEAR (normals->points.row (i)[0], -0.035592, 1e-4);
      EXPECT_NEAR (normals->points.row (i)[1], -0.369596, 1e-4);
      EXPECT_NEAR (normals->points.row (i)[2], -0.928511, 1e-4);
      EXPECT_NEAR (normals->points.row (i)[3], 0.0693136, 1e-4);
    }

    PointCloud<PointXYZ>::Ptr surfaceptr = cloudptr;
    n.setSearchSurface (surfaceptr);
    EXPECT_EQ (n.getSearchSurface (), surfaceptr);

    // Additional test for searchForNeigbhors
    surfaceptr.reset (new PointCloud<PointXYZ>);
    *surfaceptr = *cloudptr;
    surfaceptr->points.resize (640 * 480);
    surfaceptr->width = 640;
    surfaceptr->height = 480;
    EXPECT_EQ (surfaceptr->points.size (), surfaceptr->width * surfaceptr->height);
    n.setSearchSurface (surfaceptr);
    tree.reset ();
    n.setSearchMethod (tree);

    // estimate
    n.computeEigen (*normals);
    EXPECT_EQ (normals->points.rows (), indices.size ());
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TEST (PCL, NormalEstimationOpenMPEigen)
  {
    NormalEstimationOMP<PointXYZ, Eigen::MatrixXf> n (4); // instantiate 4 threads

    // Object
    PointCloud<Eigen::MatrixXf>::Ptr normals (new PointCloud<Eigen::MatrixXf>);

    // set parameters
    PointCloud<PointXYZ>::Ptr cloudptr = cloud.makeShared ();
    n.setInputCloud (cloudptr);
    EXPECT_EQ (n.getInputCloud (), cloudptr);
    boost::shared_ptr<vector<int> > indicesptr (new vector<int> (indices));
    n.setIndices (indicesptr);
    EXPECT_EQ (n.getIndices (), indicesptr);
    n.setSearchMethod (tree);
    EXPECT_EQ (n.getSearchMethod (), tree);
    n.setKSearch (static_cast<int> (indices.size ()));

    // estimate
    n.computeEigen (*normals);
    EXPECT_EQ (normals->points.rows (), indices.size ());

    for (int i = 0; i < normals->points.rows (); ++i)
    {
      EXPECT_NEAR (normals->points.row (i)[0], -0.035592, 1e-4);
      EXPECT_NEAR (normals->points.row (i)[1], -0.369596, 1e-4);
      EXPECT_NEAR (normals->points.row (i)[2], -0.928511, 1e-4);
      EXPECT_NEAR (normals->points.row (i)[3], 0.0693136, 1e-4);
    }
  }
#endif

/* ---[ */
int
main (int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "No test file given. Please download `bun0.pcd` and pass its path to the test." << std::endl;
    return (-1);
  }

  if (loadPCDFile<PointXYZ> (argv[1], cloud) < 0)
  {
    std::cerr << "Failed to read test file. Please download `bun0.pcd` and pass its path to the test." << std::endl;
    return (-1);
  }

  indices.resize (cloud.points.size ());
  for (int i = 0; i < static_cast<int> (indices.size ()); ++i)
    indices[i] = i;

  tree.reset (new search::KdTree<PointXYZ> (false));
  tree->setInputCloud (cloud.makeShared ());

  testing::InitGoogleTest (&argc, argv);
  return (RUN_ALL_TESTS ());
}
/* ]--- */
