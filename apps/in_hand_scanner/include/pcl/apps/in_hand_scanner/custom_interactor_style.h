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
 * $Id: custom_interactor_style.h 7317 2012-09-28 00:28:20Z Martin $
 *
 */

#ifndef PCL_IN_HAND_SCANNER_CUSTOM_INTERACTOR_STYLE_H
#define PCL_IN_HAND_SCANNER_CUSTOM_INTERACTOR_STYLE_H

#include <pcl/visualization/interactor_style.h>
#include <pcl/apps/in_hand_scanner/eigen.h>

namespace pcl
{
  namespace ihs
  {

    class CustomInteractorStyle : public pcl::visualization::PCLVisualizerInteractorStyle
    {

      public:

        typedef pcl::visualization::KeyboardEvent KeyboardEvent;
        typedef pcl::visualization::MouseEvent    MouseEvent;

        typedef Eigen::Quaterniond   Quaternion;
        typedef Eigen::AngleAxisd    AngleAxis;
        typedef Eigen::Vector3d      Vec3;

      public:

        static CustomInteractorStyle* New ();

        CustomInteractorStyle ();

        vtkTypeMacro (CustomInteractorStyle, vtkInteractorStyleTrackballCamera)

        virtual void
        OnChar ();

        virtual void
        OnKeyDown ();

        virtual void
        OnKeyUp ();

        void
        resetCamera ();

        void
        transformCamera (const Quaternion& rotation, const Vec3& translation);

        void
        setPivot (const Vec3& pivot);

        const Vec3&
        getPivot () const;

        virtual void
        Rotate ();

        virtual void
        Spin ();

        virtual void
        Pan ();

        virtual void
        Dolly ();

        virtual void
        OnMouseWheelForward ();

        virtual void
        OnMouseWheelBackward ();

      protected:

        void
        updateCamera ();

      protected:

        Quaternion  R_;
        Vec3        t_;
        Vec3        pivot_;
    };

  } // End namespace ihs
} // End namespace pcl

#endif // PCL_IN_HAND_SCANNER_CUSTOM_INTERACTOR_STYLE_H
