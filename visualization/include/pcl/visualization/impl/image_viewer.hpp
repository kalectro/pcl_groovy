/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2009-2012, Willow Garage, Inc.
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
 * $Id: image_viewer.hpp 6862 2012-08-18 15:07:16Z gballin $
 */

#ifndef PCL_VISUALIZATION_IMAGE_VISUALIZER_HPP_
#define	PCL_VISUALIZATION_IMAGE_VISUALIZER_HPP_

#include <pcl/visualization/common/common.h>
#include <pcl/search/organized.h>

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> void
pcl::visualization::ImageViewer::convertRGBCloudToUChar (
    const pcl::PointCloud<T> &cloud,
    boost::shared_array<unsigned char> &data)
{
  int j = 0;
  for (size_t i = 0; i < cloud.points.size (); ++i)
  {
    data[j++] = cloud.points[i].r;
    data[j++] = cloud.points[i].g;
    data[j++] = cloud.points[i].b;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> void
pcl::visualization::ImageViewer::addRGBImage (const pcl::PointCloud<T> &cloud,
                                              const std::string &layer_id,
                                              double opacity)
{
  if (data_size_ < cloud.width * cloud.height)
  {
    data_size_ = cloud.width * cloud.height * 3;
    data_.reset (new unsigned char[data_size_]);
  }

  convertRGBCloudToUChar (cloud, data_);

  return (addRGBImage (data_.get (), cloud.width, cloud.height, layer_id, opacity));
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> void
pcl::visualization::ImageViewer::showRGBImage (const pcl::PointCloud<T> &cloud,
                                               const std::string &layer_id,
                                               double opacity)
{
  addRGBImage<T> (cloud, layer_id, opacity);
  render ();
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> bool
pcl::visualization::ImageViewer::addMask (
    const typename pcl::PointCloud<T>::ConstPtr &image,
    const pcl::PointCloud<T> &mask, 
    double r, double g, double b,
    const std::string &layer_id, double opacity)
{
  // We assume that the data passed into image is organized, otherwise this doesn't make sense
  if (!image->isOrganized ())
    return (false);

  // Check to see if this ID entry already exists (has it been already added to the visualizer?)
  LayerMap::iterator am_it = std::find_if (layer_map_.begin (), layer_map_.end (), LayerComparator (layer_id));
  if (am_it == layer_map_.end ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addMask] No layer with ID'=%s' found. Creating new one...\n", layer_id.c_str ());
    am_it = createLayer (layer_id, getSize ()[0] - 1, getSize ()[1] - 1, opacity, true);
  }

  am_it->canvas->SetDrawColor (r * 255.0, g * 255.0, b * 255.0, opacity * 255.0);

  // Construct a search object to get the camera parameters
  pcl::search::OrganizedNeighbor<T> search;
  search.setInputCloud (image);
  for (size_t i = 0; i < mask.points.size (); ++i)
  {
    pcl::PointXY p_projected;
    search.projectPoint (mask.points[i], p_projected);

#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
    am_it->canvas->DrawPoint (int (p_projected.x), 
                              int (float (image->height) - p_projected.y));
#else
    am_it->canvas->DrawPoint (int (p_projected.x), 
                              int (p_projected.y));
#endif
  }

  return (true);
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> bool
pcl::visualization::ImageViewer::addMask (
    const typename pcl::PointCloud<T>::ConstPtr &image,
    const pcl::PointCloud<T> &mask, 
    const std::string &layer_id, double opacity)
{
  // We assume that the data passed into image is organized, otherwise this doesn't make sense
  if (!image->isOrganized ())
    return (false);

  // Check to see if this ID entry already exists (has it been already added to the visualizer?)
  LayerMap::iterator am_it = std::find_if (layer_map_.begin (), layer_map_.end (), LayerComparator (layer_id));
  if (am_it == layer_map_.end ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addMask] No layer with ID'=%s' found. Creating new one...\n", layer_id.c_str ());
    am_it = createLayer (layer_id, getSize ()[0] - 1, getSize ()[1] - 1, opacity, true);
  }

  am_it->canvas->SetDrawColor (255.0, 0.0, 0.0, opacity * 255.0);

  // Construct a search object to get the camera parameters
  pcl::search::OrganizedNeighbor<T> search;
  search.setInputCloud (image);
  for (size_t i = 0; i < mask.points.size (); ++i)
  {
    pcl::PointXY p_projected;
    search.projectPoint (mask.points[i], p_projected);

#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
    am_it->canvas->DrawPoint (int (p_projected.x), 
                              int (float (image->height) - p_projected.y));
#else
    am_it->canvas->DrawPoint (int (p_projected.x), 
                              int (p_projected.y));
#endif
  }

  return (true);
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> bool
pcl::visualization::ImageViewer::addPlanarPolygon (
    const typename pcl::PointCloud<T>::ConstPtr &image,
    const pcl::PlanarPolygon<T> &polygon, 
    double r, double g, double b,
    const std::string &layer_id, double opacity)
{
  // We assume that the data passed into image is organized, otherwise this doesn't make sense
  if (!image->isOrganized ())
    return (false);

  // Check to see if this ID entry already exists (has it been already added to the visualizer?)
  LayerMap::iterator am_it = std::find_if (layer_map_.begin (), layer_map_.end (), LayerComparator (layer_id));
  if (am_it == layer_map_.end ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addPlanarPolygon] No layer with ID'=%s' found. Creating new one...\n", layer_id.c_str ());
    am_it = createLayer (layer_id, getSize ()[0] - 1, getSize ()[1] - 1, opacity, true);
  }

  am_it->canvas->SetDrawColor (r * 255.0, g * 255.0, b * 255.0, opacity * 255.0);

  // Construct a search object to get the camera parameters
  pcl::search::OrganizedNeighbor<T> search;
  search.setInputCloud (image);
  for (size_t i = 0; i < polygon.getContour ().size () - 1; ++i)
  {
    pcl::PointXY p1, p2;
    search.projectPoint (polygon.getContour ()[i], p1);
    search.projectPoint (polygon.getContour ()[i+1], p2);

#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
    am_it->canvas->DrawSegment (int (p1.x), int (float (image->height) - p1.y),
                                int (p2.x), int (float (image->height) - p2.y));
#else
    am_it->canvas->DrawSegment (int (p1.x), int (p1.y),
                                int (p2.x), int (p2.y));
#endif
  }

  // Close the polygon
  pcl::PointXY p1, p2;
  search.projectPoint (polygon.getContour ()[polygon.getContour ().size () - 1], p1);
  search.projectPoint (polygon.getContour ()[0], p2);

#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
  am_it->canvas->DrawSegment (int (p1.x), int (float (image->height) - p1.y),
                              int (p2.x), int (float (image->height) - p2.y));
#else
  am_it->canvas->DrawSegment (int (p1.x), int (p1.y),
                              int (p2.x), int (p2.y));
#endif

  return (true);
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> bool
pcl::visualization::ImageViewer::addPlanarPolygon (
    const typename pcl::PointCloud<T>::ConstPtr &image,
    const pcl::PlanarPolygon<T> &polygon, 
    const std::string &layer_id, double opacity)
{
  // We assume that the data passed into image is organized, otherwise this doesn't make sense
  if (!image->isOrganized ())
    return (false);

  // Check to see if this ID entry already exists (has it been already added to the visualizer?)
  LayerMap::iterator am_it = std::find_if (layer_map_.begin (), layer_map_.end (), LayerComparator (layer_id));
  if (am_it == layer_map_.end ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addPlanarPolygon] No layer with ID'=%s' found. Creating new one...\n", layer_id.c_str ());
    am_it = createLayer (layer_id, getSize ()[0] - 1, getSize ()[1] - 1, opacity, true);
  }

  am_it->canvas->SetDrawColor (255.0, 0.0, 0.0, opacity * 255.0);

  // Construct a search object to get the camera parameters
  pcl::search::OrganizedNeighbor<T> search;
  search.setInputCloud (image);
  for (size_t i = 0; i < polygon.getContour ().size () - 1; ++i)
  {
    pcl::PointXY p1, p2;
    search.projectPoint (polygon.getContour ()[i], p1);
    search.projectPoint (polygon.getContour ()[i+1], p2);

#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
    am_it->canvas->DrawSegment (int (p1.x), int (float (image->height) - p1.y),
                                int (p2.x), int (float (image->height) - p2.y));
#else
    am_it->canvas->DrawSegment (int (p1.x), int (p1.y),
                                int (p2.x), int (p2.y));
#endif
  }

  // Close the polygon
  pcl::PointXY p1, p2;
  search.projectPoint (polygon.getContour ()[polygon.getContour ().size () - 1], p1);
  search.projectPoint (polygon.getContour ()[0], p2);

#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
  am_it->canvas->DrawSegment (int (p1.x), int (float (image->height) - p1.y),
                              int (p2.x), int (float (image->height) - p2.y));
#else
  am_it->canvas->DrawSegment (int (p1.x), int (p1.y),
                              int (p2.x), int (p2.y));
#endif
  return (true);
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> bool
pcl::visualization::ImageViewer::addRectangle (
    const typename pcl::PointCloud<T>::ConstPtr &image,
    const T &min_pt,
    const T &max_pt,
    double r, double g, double b,
    const std::string &layer_id, double opacity)
{
  // We assume that the data passed into image is organized, otherwise this doesn't make sense
  if (!image->isOrganized ())
    return (false);

  // Check to see if this ID entry already exists (has it been already added to the visualizer?)
  LayerMap::iterator am_it = std::find_if (layer_map_.begin (), layer_map_.end (), LayerComparator (layer_id));
  if (am_it == layer_map_.end ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addRectangle] No layer with ID'=%s' found. Creating new one...\n", layer_id.c_str ());
    am_it = createLayer (layer_id, getSize ()[0] - 1, getSize ()[1] - 1, opacity, true);
  }

  // Construct a search object to get the camera parameters
  pcl::search::OrganizedNeighbor<T> search;
  search.setInputCloud (image);
  // Project the 8 corners
  T p1, p2, p3, p4, p5, p6, p7, p8;
  p1.x = min_pt.x; p1.y = min_pt.y; p1.z = min_pt.z;
  p2.x = min_pt.x; p2.y = min_pt.y; p2.z = max_pt.z;
  p3.x = min_pt.x; p3.y = max_pt.y; p3.z = min_pt.z;
  p4.x = min_pt.x; p4.y = max_pt.y; p4.z = max_pt.z;
  p5.x = max_pt.x; p5.y = min_pt.y; p5.z = min_pt.z;
  p6.x = max_pt.x; p6.y = min_pt.y; p6.z = max_pt.z;
  p7.x = max_pt.x; p7.y = max_pt.y; p7.z = min_pt.z;
  p8.x = max_pt.x; p8.y = max_pt.y; p8.z = max_pt.z;

  std::vector<pcl::PointXY> pp_2d (8);
  search.projectPoint (p1, pp_2d[0]);
  search.projectPoint (p2, pp_2d[1]);
  search.projectPoint (p3, pp_2d[2]);
  search.projectPoint (p4, pp_2d[3]);
  search.projectPoint (p5, pp_2d[4]);
  search.projectPoint (p6, pp_2d[5]);
  search.projectPoint (p7, pp_2d[6]);
  search.projectPoint (p8, pp_2d[7]);

  pcl::PointXY min_pt_2d, max_pt_2d;
  min_pt_2d.x = min_pt_2d.y = std::numeric_limits<float>::max ();
  max_pt_2d.x = max_pt_2d.y = std::numeric_limits<float>::min ();
  // Search for the two extrema
  for (size_t i = 0; i < pp_2d.size (); ++i)
  {
    if (pp_2d[i].x < min_pt_2d.x) min_pt_2d.x = pp_2d[i].x;
    if (pp_2d[i].y < min_pt_2d.y) min_pt_2d.y = pp_2d[i].y;
    if (pp_2d[i].x > max_pt_2d.x) max_pt_2d.x = pp_2d[i].x;
    if (pp_2d[i].y > max_pt_2d.y) max_pt_2d.y = pp_2d[i].y;
  }
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
  min_pt_2d.y = float (image->height) - min_pt_2d.y;
  max_pt_2d.y = float (image->height) - max_pt_2d.y;
#else
  min_pt_2d.y = min_pt_2d.y;
  max_pt_2d.y = max_pt_2d.y;
#endif

  am_it->canvas->SetDrawColor (r * 255.0, g * 255.0, b * 255.0, opacity * 255.0);

  am_it->canvas->DrawSegment (int (min_pt_2d.x), int (min_pt_2d.y), int (min_pt_2d.x), int (max_pt_2d.y));
  am_it->canvas->DrawSegment (int (min_pt_2d.x), int (max_pt_2d.y), int (max_pt_2d.x), int (max_pt_2d.y));
  am_it->canvas->DrawSegment (int (max_pt_2d.x), int (max_pt_2d.y), int (max_pt_2d.x), int (min_pt_2d.y));
  am_it->canvas->DrawSegment (int (max_pt_2d.x), int (min_pt_2d.y), int (min_pt_2d.x), int (min_pt_2d.y));

  return (true);
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> bool
pcl::visualization::ImageViewer::addRectangle (
    const typename pcl::PointCloud<T>::ConstPtr &image,
    const T &min_pt,
    const T &max_pt,
    const std::string &layer_id, double opacity)
{
  // We assume that the data passed into image is organized, otherwise this doesn't make sense
  if (!image->isOrganized ())
    return (false);

  // Check to see if this ID entry already exists (has it been already added to the visualizer?)
  LayerMap::iterator am_it = std::find_if (layer_map_.begin (), layer_map_.end (), LayerComparator (layer_id));
  if (am_it == layer_map_.end ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addRectangle] No layer with ID'=%s' found. Creating new one...\n", layer_id.c_str ());
    am_it = createLayer (layer_id, getSize ()[0] - 1, getSize ()[1] - 1, opacity, true);
  }

  // Construct a search object to get the camera parameters
  pcl::search::OrganizedNeighbor<T> search;
  search.setInputCloud (image);
  // Project the 8 corners
  T p1, p2, p3, p4, p5, p6, p7, p8;
  p1.x = min_pt.x; p1.y = min_pt.y; p1.z = min_pt.z;
  p2.x = min_pt.x; p2.y = min_pt.y; p2.z = max_pt.z;
  p3.x = min_pt.x; p3.y = max_pt.y; p3.z = min_pt.z;
  p4.x = min_pt.x; p4.y = max_pt.y; p4.z = max_pt.z;
  p5.x = max_pt.x; p5.y = min_pt.y; p5.z = min_pt.z;
  p6.x = max_pt.x; p6.y = min_pt.y; p6.z = max_pt.z;
  p7.x = max_pt.x; p7.y = max_pt.y; p7.z = min_pt.z;
  p8.x = max_pt.x; p8.y = max_pt.y; p8.z = max_pt.z;

  std::vector<pcl::PointXY> pp_2d (8);
  search.projectPoint (p1, pp_2d[0]);
  search.projectPoint (p2, pp_2d[1]);
  search.projectPoint (p3, pp_2d[2]);
  search.projectPoint (p4, pp_2d[3]);
  search.projectPoint (p5, pp_2d[4]);
  search.projectPoint (p6, pp_2d[5]);
  search.projectPoint (p7, pp_2d[6]);
  search.projectPoint (p8, pp_2d[7]);

  pcl::PointXY min_pt_2d, max_pt_2d;
  min_pt_2d.x = min_pt_2d.y = std::numeric_limits<float>::max ();
  max_pt_2d.x = max_pt_2d.y = std::numeric_limits<float>::min ();
  // Search for the two extrema
  for (size_t i = 0; i < pp_2d.size (); ++i)
  {
    if (pp_2d[i].x < min_pt_2d.x) min_pt_2d.x = pp_2d[i].x;
    if (pp_2d[i].y < min_pt_2d.y) min_pt_2d.y = pp_2d[i].y;
    if (pp_2d[i].x > max_pt_2d.x) max_pt_2d.x = pp_2d[i].x;
    if (pp_2d[i].y > max_pt_2d.y) max_pt_2d.y = pp_2d[i].y;
  }
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
  min_pt_2d.y = float (image->height) - min_pt_2d.y;
  max_pt_2d.y = float (image->height) - max_pt_2d.y;
#else
  min_pt_2d.y = min_pt_2d.y;
  max_pt_2d.y = max_pt_2d.y;
#endif

  am_it->canvas->SetDrawColor (0.0, 255.0, 0.0, opacity * 255.0);
  am_it->canvas->DrawSegment (int (min_pt_2d.x), int (min_pt_2d.y), int (min_pt_2d.x), int (max_pt_2d.y));
  am_it->canvas->DrawSegment (int (min_pt_2d.x), int (max_pt_2d.y), int (max_pt_2d.x), int (max_pt_2d.y));
  am_it->canvas->DrawSegment (int (max_pt_2d.x), int (max_pt_2d.y), int (max_pt_2d.x), int (min_pt_2d.y));
  am_it->canvas->DrawSegment (int (max_pt_2d.x), int (min_pt_2d.y), int (min_pt_2d.x), int (min_pt_2d.y));

  return (true);
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> bool
pcl::visualization::ImageViewer::addRectangle (
    const typename pcl::PointCloud<T>::ConstPtr &image,
    const pcl::PointCloud<T> &mask, 
    double r, double g, double b,
    const std::string &layer_id, double opacity)
{
  // We assume that the data passed into image is organized, otherwise this doesn't make sense
  if (!image->isOrganized ())
    return (false);

  // Check to see if this ID entry already exists (has it been already added to the visualizer?)
  LayerMap::iterator am_it = std::find_if (layer_map_.begin (), layer_map_.end (), LayerComparator (layer_id));
  if (am_it == layer_map_.end ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addRectangle] No layer with ID'=%s' found. Creating new one...\n", layer_id.c_str ());
    am_it = createLayer (layer_id, getSize ()[0] - 1, getSize ()[1] - 1, opacity, true);
  }

  am_it->canvas->SetDrawColor (r * 255.0, g * 255.0, b * 255.0, opacity * 255.0);

  // Construct a search object to get the camera parameters
  pcl::search::OrganizedNeighbor<T> search;
  search.setInputCloud (image);
  std::vector<pcl::PointXY> pp_2d (mask.points.size ());
  for (size_t i = 0; i < mask.points.size (); ++i)
    search.projectPoint (mask.points[i], pp_2d[i]);

  pcl::PointXY min_pt_2d, max_pt_2d;
  min_pt_2d.x = min_pt_2d.y = std::numeric_limits<float>::max ();
  max_pt_2d.x = max_pt_2d.y = std::numeric_limits<float>::min ();
  // Search for the two extrema
  for (size_t i = 0; i < pp_2d.size (); ++i)
  {
    if (pp_2d[i].x < min_pt_2d.x) min_pt_2d.x = pp_2d[i].x;
    if (pp_2d[i].y < min_pt_2d.y) min_pt_2d.y = pp_2d[i].y;
    if (pp_2d[i].x > max_pt_2d.x) max_pt_2d.x = pp_2d[i].x;
    if (pp_2d[i].y > max_pt_2d.y) max_pt_2d.y = pp_2d[i].y;
  }
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
  min_pt_2d.y = float (image->height) - min_pt_2d.y;
  max_pt_2d.y = float (image->height) - max_pt_2d.y;
#else
  min_pt_2d.y = min_pt_2d.y;
  max_pt_2d.y = max_pt_2d.y;
#endif

  am_it->canvas->DrawSegment (int (min_pt_2d.x), int (min_pt_2d.y), int (min_pt_2d.x), int (max_pt_2d.y));
  am_it->canvas->DrawSegment (int (min_pt_2d.x), int (max_pt_2d.y), int (max_pt_2d.x), int (max_pt_2d.y));
  am_it->canvas->DrawSegment (int (max_pt_2d.x), int (max_pt_2d.y), int (max_pt_2d.x), int (min_pt_2d.y));
  am_it->canvas->DrawSegment (int (max_pt_2d.x), int (min_pt_2d.y), int (min_pt_2d.x), int (min_pt_2d.y));

  return (true);
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename T> bool
pcl::visualization::ImageViewer::addRectangle (
    const typename pcl::PointCloud<T>::ConstPtr &image,
    const pcl::PointCloud<T> &mask, 
    const std::string &layer_id, double opacity)
{
  // We assume that the data passed into image is organized, otherwise this doesn't make sense
  if (!image->isOrganized ())
    return (false);

  // Check to see if this ID entry already exists (has it been already added to the visualizer?)
  LayerMap::iterator am_it = std::find_if (layer_map_.begin (), layer_map_.end (), LayerComparator (layer_id));
  if (am_it == layer_map_.end ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addRectangle] No layer with ID'=%s' found. Creating new one...\n", layer_id.c_str ());
    am_it = createLayer (layer_id, getSize ()[0] - 1, getSize ()[1] - 1, opacity, true);
  }

  am_it->canvas->SetDrawColor (0.0, 255.0, 0.0, opacity * 255.0);

  // Construct a search object to get the camera parameters
  pcl::search::OrganizedNeighbor<T> search;
  search.setInputCloud (image);
  std::vector<pcl::PointXY> pp_2d (mask.points.size ());
  for (size_t i = 0; i < mask.points.size (); ++i)
    search.projectPoint (mask.points[i], pp_2d[i]);

  pcl::PointXY min_pt_2d, max_pt_2d;
  min_pt_2d.x = min_pt_2d.y = std::numeric_limits<float>::max ();
  max_pt_2d.x = max_pt_2d.y = std::numeric_limits<float>::min ();
  // Search for the two extrema
  for (size_t i = 0; i < pp_2d.size (); ++i)
  {
    if (pp_2d[i].x < min_pt_2d.x) min_pt_2d.x = pp_2d[i].x;
    if (pp_2d[i].y < min_pt_2d.y) min_pt_2d.y = pp_2d[i].y;
    if (pp_2d[i].x > max_pt_2d.x) max_pt_2d.x = pp_2d[i].x;
    if (pp_2d[i].y > max_pt_2d.y) max_pt_2d.y = pp_2d[i].y;
  }
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
  min_pt_2d.y = float (image->height) - min_pt_2d.y;
  max_pt_2d.y = float (image->height) - max_pt_2d.y;
#else
  min_pt_2d.y = min_pt_2d.y;
  max_pt_2d.y = max_pt_2d.y;
#endif
  am_it->canvas->DrawSegment (int (min_pt_2d.x), int (min_pt_2d.y), int (min_pt_2d.x), int (max_pt_2d.y));
  am_it->canvas->DrawSegment (int (min_pt_2d.x), int (max_pt_2d.y), int (max_pt_2d.x), int (max_pt_2d.y));
  am_it->canvas->DrawSegment (int (max_pt_2d.x), int (max_pt_2d.y), int (max_pt_2d.x), int (min_pt_2d.y));
  am_it->canvas->DrawSegment (int (max_pt_2d.x), int (min_pt_2d.y), int (min_pt_2d.x), int (min_pt_2d.y));


  return (true);
}

///////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT> bool
pcl::visualization::ImageViewer::showCorrespondences (
  const pcl::PointCloud<PointT> &source_img,
  const pcl::PointCloud<PointT> &target_img,
  const pcl::Correspondences &correspondences,
  int nth,
  const std::string &layer_id)
{
  if (correspondences.empty ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addCorrespondences] An empty set of correspondences given! Nothing to display.\n");
    return (false);
  }

  // Check to see if this ID entry already exists (has it been already added to the visualizer?)
  LayerMap::iterator am_it = std::find_if (layer_map_.begin (), layer_map_.end (), LayerComparator (layer_id));
  if (am_it == layer_map_.end ())
  {
    PCL_DEBUG ("[pcl::visualization::ImageViewer::addCorrespondences] No layer with ID='%s' found. Creating new one...\n", layer_id.c_str ());
    am_it = createLayer (layer_id, source_img.width + target_img.width, std::max (source_img.height, target_img.height), 255, false);
  }
 
  int src_size = source_img.width * source_img.height * 3;
  int tgt_size = target_img.width * target_img.height * 3;

  // Set window size
  setSize (source_img.width + target_img.width , std::max (source_img.height, target_img.height));

  // Set data size
  if (data_size_ < (src_size + tgt_size))
  {
    data_size_ = src_size + tgt_size;
    data_.reset (new unsigned char[data_size_]);
  }

  // Copy data in VTK format
  int j = 0;
  for (size_t i = 0; i < std::max (source_img.height, target_img.height); ++i)
  {
    // Still need to copy the source?
    if (i < source_img.height)
    {
      for (size_t k = 0; k < source_img.width; ++k)
      {
        data_[j++] = source_img[i * source_img.width + k].r;
        data_[j++] = source_img[i * source_img.width + k].g;
        data_[j++] = source_img[i * source_img.width + k].b;
      }
    }
    else
    {
      memcpy (&data_[j], 0, source_img.width * 3);
      j += source_img.width * 3;
    }

    // Still need to copy the target?
    if (i < source_img.height)
    {
      for (size_t k = 0; k < target_img.width; ++k)
      {
        data_[j++] = target_img[i * source_img.width + k].r;
        data_[j++] = target_img[i * source_img.width + k].g;
        data_[j++] = target_img[i * source_img.width + k].b;
      }
    }
    else
    {
      memcpy (&data_[j], 0, target_img.width * 3);
      j += target_img.width * 3;
    }
  }

  void* data = const_cast<void*> (reinterpret_cast<const void*> (data_.get ()));
  
  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New ();
  image->SetDimensions (source_img.width + target_img.width, std::max (source_img.height, target_img.height), 1);
  image->SetScalarTypeToUnsignedChar ();
  image->SetNumberOfScalarComponents (3);
  image->AllocateScalars ();
  image->GetPointData ()->GetScalars ()->SetVoidArray (data, data_size_, 1);
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION < 10))
  // Now create filter and set previously created transformation
  algo_->SetInput (image);
  algo_->Update ();
#  if (VTK_MINOR_VERSION <= 6)
    image_viewer_->SetInput (algo_->GetOutput ());
#  else
    am_it->canvas->SetNumberOfScalarComponents (3);
    am_it->canvas->DrawImage (algo_->GetOutput ());

    blend_->ReplaceNthInputConnection (int (am_it - layer_map_.begin ()), am_it->canvas->GetOutputPort ());
    image_viewer_->SetInputConnection (blend_->GetOutputPort ());
#  endif
#else
  am_it->canvas->SetNumberOfScalarComponents (3);
  am_it->canvas->DrawImage (image);

  blend_->ReplaceNthInputConnection (int (am_it - layer_map_.begin ()), am_it->canvas->GetOutputPort ());
  slice_->GetMapper ()->SetInput (blend_->GetOutput ());

  interactor_style_->adjustCamera (image, ren_);
#endif

  double r = 1.0 , g, b;
  // Draw lines between the best corresponding points
  for (size_t i = 0; i < correspondences.size (); i += nth)
  {
    int v_src = correspondences[i].index_query / source_img.width,
        u_src = correspondences[i].index_query % source_img.width,
        v_tgt = correspondences[i].index_match / target_img.width,
        u_tgt = correspondences[i].index_match % target_img.width;
    getRandomColors (r, g, b);
    am_it->canvas->SetDrawColor (r * 255.0, g * 255.0, b * 255.0, 255.0);
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION >= 10))
    am_it->canvas->DrawSegment (u_src, v_src, source_img.width + u_tgt, v_tgt);
    am_it->canvas->DrawCircle (u_src, v_src, 3);
    am_it->canvas->DrawCircle (source_img.width + u_tgt, v_tgt, 3);
#else
    am_it->canvas->DrawSegment (u_src, getSize ()[1] - v_src, source_img.width + u_tgt, getSize ()[1] - v_tgt);
    am_it->canvas->DrawCircle (u_src, getSize ()[1] - v_src, 3);
    am_it->canvas->DrawCircle (source_img.width + u_tgt, getSize ()[1] - v_tgt, 3);
#endif
  }

  return (true);
}

#endif      // PCL_VISUALIZATION_IMAGE_VISUALIZER_HPP_
