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
 * $Id: mesh_topology.hpp 7870 2012-11-09 02:38:07Z Martin $
 *
 */

#ifndef PCL_GEOMETRY_MESH_TOPOLOGY_HPP
#define PCL_GEOMETRY_MESH_TOPOLOGY_HPP

#include <stack>
#include <algorithm>
#include <deque>

#include <pcl/geometry/boost.h>
#include <pcl/geometry/impl/mesh_base.hpp>

namespace pcl
{
  template <bool is_manifold, class VertexDataT, class FaceDataT, class HalfEdgeDataT>
  class MeshTopology : public pcl::MeshBase <VertexDataT, FaceDataT, HalfEdgeDataT>
  {
    public:

      typedef pcl::MeshTopology <is_manifold, VertexDataT, FaceDataT, HalfEdgeDataT> Self;
      typedef pcl::MeshBase     <             VertexDataT, FaceDataT, HalfEdgeDataT> Base;

      typedef boost::integral_constant <bool, true>        ManifoldMeshTag;
      typedef boost::integral_constant <bool, false>       NonManifoldMeshTag;
      typedef boost::integral_constant <bool, is_manifold> IsManifold;

      typedef typename Base::VertexData   VertexData;
      typedef typename Base::HalfEdgeData HalfEdgeData;
      typedef typename Base::FaceData     FaceData;

      typedef typename Base::VertexIndex   VertexIndex;
      typedef typename Base::HalfEdgeIndex HalfEdgeIndex;
      typedef typename Base::FaceIndex     FaceIndex;

      typedef typename Base::VertexIndexes   VertexIndexes;
      typedef typename Base::HalfEdgeIndexes HalfEdgeIndexes;
      typedef typename Base::FaceIndexes     FaceIndexes;

      typedef typename Base::Vertex   Vertex;
      typedef typename Base::HalfEdge HalfEdge;
      typedef typename Base::Face     Face;

      typedef typename Base::Vertexes  Vertexes;
      typedef typename Base::HalfEdges HalfEdges;
      typedef typename Base::Faces     Faces;

      typedef typename Base::SizeType SizeType;

      typedef typename Base::VertexIterator   VertexIterator;
      typedef typename Base::HalfEdgeIterator HalfEdgeIterator;
      typedef typename Base::FaceIterator     FaceIterator;

      typedef typename Base::VertexConstIterator   VertexConstIterator;
      typedef typename Base::HalfEdgeConstIterator HalfEdgeConstIterator;
      typedef typename Base::FaceConstIterator     FaceConstIterator;

      typedef typename Base::VertexIndexIterator   VertexIndexIterator;
      typedef typename Base::HalfEdgeIndexIterator HalfEdgeIndexIterator;
      typedef typename Base::FaceIndexIterator     FaceIndexIterator;

      typedef typename Base::VertexIndexConstIterator   VertexIndexConstIterator;
      typedef typename Base::HalfEdgeIndexConstIterator HalfEdgeIndexConstIterator;
      typedef typename Base::FaceIndexConstIterator     FaceIndexConstIterator;

      typedef typename Base::VertexAroundVertexCirculator           VertexAroundVertexCirculator;
      typedef typename Base::OutgoingHalfEdgeAroundVertexCirculator OutgoingHalfEdgeAroundVertexCirculator;
      typedef typename Base::IncomingHalfEdgeAroundVertexCirculator IncomingHalfEdgeAroundVertexCirculator;
      typedef typename Base::FaceAroundVertexCirculator             FaceAroundVertexCirculator;
      typedef typename Base::VertexAroundFaceCirculator             VertexAroundFaceCirculator;
      typedef typename Base::InnerHalfEdgeAroundFaceCirculator      InnerHalfEdgeAroundFaceCirculator;
      typedef typename Base::OuterHalfEdgeAroundFaceCirculator      OuterHalfEdgeAroundFaceCirculator;
      typedef typename Base::FaceAroundFaceCirculator               FaceAroundFaceCirculator;
      typedef typename Base::HalfEdgeAroundBoundaryCirculator       HalfEdgeAroundBoundaryCirculator;

      typedef typename Base::VertexAroundVertexConstCirculator           VertexAroundVertexConstCirculator;
      typedef typename Base::OutgoingHalfEdgeAroundVertexConstCirculator OutgoingHalfEdgeAroundVertexConstCirculator;
      typedef typename Base::IncomingHalfEdgeAroundVertexConstCirculator IncomingHalfEdgeAroundVertexConstCirculator;
      typedef typename Base::FaceAroundVertexConstCirculator             FaceAroundVertexConstCirculator;
      typedef typename Base::VertexAroundFaceConstCirculator             VertexAroundFaceConstCirculator;
      typedef typename Base::InnerHalfEdgeAroundFaceConstCirculator      InnerHalfEdgeAroundFaceConstCirculator;
      typedef typename Base::OuterHalfEdgeAroundFaceConstCirculator      OuterHalfEdgeAroundFaceConstCirculator;
      typedef typename Base::FaceAroundFaceConstCirculator               FaceAroundFaceConstCirculator;
      typedef typename Base::HalfEdgeAroundBoundaryConstCirculator       HalfEdgeAroundBoundaryConstCirculator;

    public:

      MeshTopology ()
        : Base ()
      {
      }

    public:

      //////////////////////////////////////////////////////////////////////////
      // isManifold
      //////////////////////////////////////////////////////////////////////////

      bool
      isManifold () const
      {
        return (this->isManifold (IsManifold ()));
      }

      //////////////////////////////////////////////////////////////////////////
      // addVertex
      //////////////////////////////////////////////////////////////////////////

      VertexIndex
      addVertex (const VertexData& vertex_data)
      {
        return (Base::pushBackVertex (vertex_data));
      }

      //////////////////////////////////////////////////////////////////////////
      // deleteVertex
      //////////////////////////////////////////////////////////////////////////

      void
      deleteVertex (const VertexIndex& idx_vertex)
      {
        assert (Base::validateMeshElementIndex (idx_vertex));
        if (idx_vertex.isValid ())
        {
          this->deleteVertex (Base::getElement (idx_vertex));
        }
      }

      void
      deleteVertex (Vertex& vertex)
      {
        if (vertex.getDeleted ()) return;
        if (vertex.isIsolated ()) {vertex.setDeleted (true); return;}

        FaceAroundVertexConstCirculator       circ     = Base::getFaceAroundVertexConstCirculator (vertex);
        const FaceAroundVertexConstCirculator circ_end = circ;
        assert (circ.isValid ());

        std::deque <FaceIndex> fi;
        do fi.push_back ((circ++).getDereferencedIndex ()); while (circ!=circ_end);

        for (typename std::deque <FaceIndex>::const_iterator it = fi.begin (); it!=fi.end (); ++it)
        {
          this->deleteFace (*it);
        }
      }

      //////////////////////////////////////////////////////////////////////////
      // deleteEdge
      //////////////////////////////////////////////////////////////////////////

      void
      deleteEdge (const HalfEdgeIndex& idx_half_edge)
      {
        assert (Base::validateMeshElementIndex (idx_half_edge));
        if (idx_half_edge.isValid ())
        {
          this->deleteEdge (Base::getElement (idx_half_edge));
        }
      }

      void
      deleteEdge (HalfEdge& half_edge)
      {
        HalfEdge& opposite = half_edge.getOppositeHalfEdge (*this);

        if (half_edge.isBoundary ()) half_edge.setDeleted (true);
        else                         this->deleteFace (half_edge.getFaceIndex ());
        if (opposite.isBoundary ())  opposite.setDeleted (true);
        else                         this->deleteFace (half_edge.getOppositeFaceIndex (*this));
      }

      //////////////////////////////////////////////////////////////////////////
      // deleteFace
      //////////////////////////////////////////////////////////////////////////

      void
      deleteFace (const FaceIndex& idx_face)
      {
        assert (Base::validateMeshElementIndex (idx_face));
        if (idx_face.isValid ())
        {
          std::stack <FaceIndex> delete_faces; // This is actually only needed for the manifold mesh.
          this->deleteFace (idx_face, delete_faces, IsManifold ());
        }
      }

      void
      deleteFace (const Face& face)
      {
        if (!face.getDeleted ())
        {
          this->deleteFace (face.getInnerHalfEdge (*this).getFaceIndex ());
        }
      }

      //////////////////////////////////////////////////////////////////////////
      // cleanUp
      //////////////////////////////////////////////////////////////////////////

      void cleanUp (const bool remove_isolated = true) // TODO: what is the better default value?
      {
        // Copy the non-deleted mesh elements and store the index to their new position
        const VertexIndexes   new_vertex_indexes    = this->removeMeshElements <Vertexes,  VertexIndexes>   (Base::vertexes_, remove_isolated);
        const HalfEdgeIndexes new_half_edge_indexes = this->removeMeshElements <HalfEdges, HalfEdgeIndexes> (Base::half_edges_, remove_isolated);
        const FaceIndexes     new_face_indexes      = this->removeMeshElements <Faces,     FaceIndexes>     (Base::faces_, remove_isolated);

        // Adjust the indexes
        for (VertexIterator it = Base::beginVertexes (); it!=Base::endVertexes (); ++it)
        {
          if (!it->isIsolated ())
          {
            it->setOutgoingHalfEdgeIndex (new_half_edge_indexes [it->getOutgoingHalfEdgeIndex ().getIndex ()]);
          }
        }

        for (HalfEdgeIterator it = Base::beginHalfEdges (); it!=Base::endHalfEdges (); ++it)
        {
          it->setTerminatingVertexIndex (new_vertex_indexes    [it->getTerminatingVertexIndex ().getIndex ()]);
          it->setOppositeHalfEdgeIndex  (new_half_edge_indexes [it->getOppositeHalfEdgeIndex ().getIndex ()]);
          it->setNextHalfEdgeIndex      (new_half_edge_indexes [it->getNextHalfEdgeIndex ().getIndex ()]);
          it->setPrevHalfEdgeIndex      (new_half_edge_indexes [it->getPrevHalfEdgeIndex ().getIndex ()]);
          if (!it->isBoundary ())
          {
            it->setFaceIndex (new_face_indexes [it->getFaceIndex ().getIndex ()]);
          }
        }

        for (FaceIterator it = Base::beginFaces (); it!=Base::endFaces (); ++it)
        {
          it->setInnerHalfEdgeIndex (new_half_edge_indexes [it->getInnerHalfEdgeIndex ().getIndex ()]);
        }
      }

    protected:

      //////////////////////////////////////////////////////////////////////////
      // uniqueCheck
      //////////////////////////////////////////////////////////////////////////

      bool
      uniqueCheck (VertexIndexes vertex_indexes) const
      {
        // TODO: Check if an unordered_set is faster (vertex_indexes can be passed by reference).
        std::sort (vertex_indexes.begin (), vertex_indexes.end ());
        return (std::adjacent_find (vertex_indexes.begin (), vertex_indexes.end ()) == vertex_indexes.end ());
      }

      //////////////////////////////////////////////////////////////////////////
      // checkTopology1
      //////////////////////////////////////////////////////////////////////////

      // Returns true if addFace may be continued
      bool
      checkTopology1 (const VertexIndex& idx_v_a,
                      const VertexIndex& idx_v_b,
                      HalfEdgeIndex&     idx_he_a_out,
                      bool&              is_new_ab) const
      {
        return (this->checkTopology1 (idx_v_a, idx_v_b, idx_he_a_out, is_new_ab, IsManifold ()));
      }

      //////////////////////////////////////////////////////////////////////////
      // checkTopology2
      //////////////////////////////////////////////////////////////////////////

      bool
      checkTopology2 (const bool is_new_ab,
                      const bool is_new_bc,
                      const bool is_isolated_b) const
      {
        return (this->checkTopology2 (is_new_ab, is_new_bc, is_isolated_b, IsManifold ()));
      }

      //////////////////////////////////////////////////////////////////////////
      // checkAdjacency
      //////////////////////////////////////////////////////////////////////////

      bool
      checkAdjacency (const HalfEdgeIndex& idx_he_ab,
                      const HalfEdgeIndex& idx_he_bc,
                      const bool           is_new_ab,
                      const bool           is_new_bc,
                      bool&                make_adjacent_ab_bc,
                      HalfEdgeIndex&       idx_he_boundary)
      {
        return (this->checkAdjacency (idx_he_ab,idx_he_bc, is_new_ab,is_new_bc, make_adjacent_ab_bc, idx_he_boundary, IsManifold ()));
      }

      //////////////////////////////////////////////////////////////////////////
      // makeAdjacent
      //////////////////////////////////////////////////////////////////////////

      void
      makeAdjacent (const HalfEdgeIndex& idx_he_ab,
                    const HalfEdgeIndex& idx_he_bc,
                    bool&                make_adjacent_ab_bc,
                    HalfEdgeIndex&       idx_he_boundary)
      {
        this->makeAdjacent (idx_he_ab,idx_he_bc, make_adjacent_ab_bc, idx_he_boundary, IsManifold ());
      }

      //////////////////////////////////////////////////////////////////////////
      // addHalfEdgePair
      //////////////////////////////////////////////////////////////////////////

      void
      addHalfEdgePair (const VertexIndex&  idx_v_a,
                       const VertexIndex&  idx_v_b,
                       const HalfEdgeData& he_data_ab,
                       const HalfEdgeData& he_data_ba,
                       HalfEdgeIndex&      idx_he_ab,
                       HalfEdgeIndex&      idx_he_ba)
      {
        // Only sets the unambiguous connections: OppositeHalfEdge and TerminatingVertex
        idx_he_ab = Base::pushBackHalfEdge (he_data_ab, idx_v_b);
        idx_he_ba = Base::pushBackHalfEdge (he_data_ba, idx_v_a, idx_he_ab);

        Base::getElement (idx_he_ab).setOppositeHalfEdgeIndex (idx_he_ba);
      }

      //////////////////////////////////////////////////////////////////////////
      // connectPrevNext
      //////////////////////////////////////////////////////////////////////////

      void
      connectPrevNext (const HalfEdgeIndex& idx_he_ab,
                       const HalfEdgeIndex& idx_he_bc)
      {
        Base::getElement (idx_he_ab).setNextHalfEdgeIndex (idx_he_bc);
        Base::getElement (idx_he_bc).setPrevHalfEdgeIndex (idx_he_ab);
      }

      //////////////////////////////////////////////////////////////////////////
      // connectHalfEdges
      //////////////////////////////////////////////////////////////////////////

      void
      connectHalfEdges (const bool           is_new_ab,
                        const bool           is_new_bc,
                        const HalfEdgeIndex& idx_he_ab,
                        const HalfEdgeIndex& idx_he_ba,
                        const HalfEdgeIndex& idx_he_bc,
                        const HalfEdgeIndex& idx_he_cb,
                        const VertexIndex&   idx_v_b)
      {
        if      ( is_new_ab &&  is_new_bc) this->connectNewNew (idx_he_ab,idx_he_ba, idx_he_bc,idx_he_cb, idx_v_b, IsManifold ());
        else if ( is_new_ab && !is_new_bc) this->connectNewOld (idx_he_ab,idx_he_ba, idx_he_bc,idx_he_cb, idx_v_b);
        else if (!is_new_ab &&  is_new_bc) this->connectOldNew (idx_he_ab,idx_he_ba, idx_he_bc,idx_he_cb, idx_v_b);
        else                               this->connectOldOld (idx_he_ab,idx_he_ba, idx_he_bc,idx_he_cb, idx_v_b, IsManifold ());
      }

      //////////////////////////////////////////////////////////////////////////
      // connectFace
      //////////////////////////////////////////////////////////////////////////

      FaceIndex
      connectFace (const FaceData&                  face_data,
                   HalfEdgeIndexConstIterator       it,
                   const HalfEdgeIndexConstIterator it_end)
      {
        // Add and connect the face
        const FaceIndex idx_face = Base::pushBackFace (face_data, *(it_end-1));

        while (it!=it_end)
        {
          Base::getElement (*it++).setFaceIndex (idx_face);
        }

        return (idx_face);
      }

    private:

      //////////////////////////////////////////////////////////////////////////
      // isManifold
      //////////////////////////////////////////////////////////////////////////

      bool
      isManifold (ManifoldMeshTag) const
      {
        return (true);
      }

      bool
      isManifold (NonManifoldMeshTag) const
      {
        VertexConstIterator       it     = Base::beginVertexes ();
        const VertexConstIterator it_end = Base::endVertexes ();

        for (; it!=it_end; ++it)
        {
          if (!it->isManifold (*this))
          {
            return (false);
          }
        }
        return (true);
      }

      //////////////////////////////////////////////////////////////////////////
      // deleteFace
      //////////////////////////////////////////////////////////////////////////

      void
      deleteFace (const FaceIndex&       idx_face,
                  std::stack<FaceIndex>& delete_faces,
                  ManifoldMeshTag)
      {
        delete_faces.push (idx_face);

        while (!delete_faces.empty ())
        {
          const FaceIndex& idx_face_cur = delete_faces.top ();
          delete_faces.pop ();

          // This calls the non-manifold version of deleteFace, which will call the manifold version of reconnect (avoid code duplication).
          this->deleteFace (idx_face_cur, delete_faces, NonManifoldMeshTag ());
        }
      }

      void
      deleteFace (const FaceIndex&       idx_face,
                  std::stack<FaceIndex>& delete_faces,
                  NonManifoldMeshTag)
      {
        Face& face = Base::getElement (idx_face);
        if (face.getDeleted ())
        {
          return; // already deleted
        }

        // Get all inner & outer half-edges in the face and store if the outer half-edge is a boundary
        typedef std::vector <EdgeWithBoundaryInfo>               EdgeWithBoundaryInfoVec;
        typedef typename EdgeWithBoundaryInfoVec::const_iterator EdgeWithBoundaryInfoConstIterator;

        // TODO: maybe use a deque instead of vector here (no reserve necessary)
        EdgeWithBoundaryInfoVec ewbiv; ewbiv.reserve (4); // Minimum for a triangle

        InnerHalfEdgeAroundFaceCirculator       circ     = Base::getInnerHalfEdgeAroundFaceCirculator(face);
        const InnerHalfEdgeAroundFaceCirculator circ_end = circ;

        do
        {
          ewbiv.push_back (EdgeWithBoundaryInfo (circ.getDereferencedIndex (),
                                                 circ->getOppositeHalfEdgeIndex (),
                                                 circ->getOppositeHalfEdge (*this).isBoundary ()));
          ++circ;
        } while (circ!=circ_end);

        // This is needed to reconnect the last half-edge with the first half-edge in the face
        ewbiv.push_back (ewbiv.front ());

        // Reconnect
        EdgeWithBoundaryInfoConstIterator       it     = ewbiv.begin ();
        const EdgeWithBoundaryInfoConstIterator it_end = ewbiv.end ()-1;

        while (it!=it_end)
        {
          const EdgeWithBoundaryInfo& cur  = *it++;
          const EdgeWithBoundaryInfo& next = *it;

          this->reconnect (cur.is_boundary_ba_,next.is_boundary_ba_, cur.idx_he_ab_,cur.idx_he_ba_, next.idx_he_ab_,next.idx_he_ba_, delete_faces);
        }

        // Delete the face
        face.setDeleted (true);
        it = ewbiv.begin ();
        while (it!=it_end)
        {
          Base::getElement ((*it++).idx_he_ab_).getFaceIndex ().invalidate ();
        }
      }

      //////////////////////////////////////////////////////////////////////////
      // checkTopology1
      //////////////////////////////////////////////////////////////////////////

      bool
      checkTopology1 (const VertexIndex& idx_v_a,
                      const VertexIndex& idx_v_b,
                      HalfEdgeIndex&     idx_he_a_out,
                      bool&              is_new_ab,
                      ManifoldMeshTag) const
      {
        const Vertex& v_a = Base::getElement (idx_v_a);

        if (v_a.isIsolated ()) return (true);

        idx_he_a_out          = v_a.getOutgoingHalfEdgeIndex ();
        const HalfEdge& he_ab = Base::getElement (idx_he_a_out);

        if (!he_ab.isBoundary ())                          return (false);
        if (he_ab.getTerminatingVertexIndex () == idx_v_b) is_new_ab = false;
        else                                               is_new_ab = true;

        return (true);
      }

      bool
      checkTopology1 (const VertexIndex& idx_v_a,
                      const VertexIndex& idx_v_b,
                      HalfEdgeIndex&     idx_he_a_out,
                      bool&              is_new_ab,
                      NonManifoldMeshTag) const
      {
        const Vertex& v_a = Base::getElement (idx_v_a);

        if (v_a.isIsolated ())                              return (true);
        if (!v_a.getOutgoingHalfEdge (*this).isBoundary ()) return (false);

        VertexAroundVertexConstCirculator       circ     = Base::getVertexAroundVertexConstCirculator (v_a.getOutgoingHalfEdgeIndex ());
        const VertexAroundVertexConstCirculator circ_end = circ;

        is_new_ab = true;

        do
        {
          if (circ.getDereferencedIndex () == idx_v_b)
          {
            idx_he_a_out = circ.getCurrentHalfEdgeIndex ();
            if (!Base::getElement (idx_he_a_out).isBoundary ())
            {
              return (false);
            }

            is_new_ab = false;
            return (true);
          }
          ++circ;
        } while (circ!=circ_end);

        return (true);
      }

      //////////////////////////////////////////////////////////////////////////
      // checkTopology2
      //////////////////////////////////////////////////////////////////////////

      bool
      checkTopology2 (const bool is_new_ab,
                      const bool is_new_bc,
                      const bool is_isolated_b,
                      ManifoldMeshTag) const
      {
        if (is_new_ab && is_new_bc && !is_isolated_b) return (false);
        else                                          return (true);
      }

      bool
      checkTopology2 (const bool /*is_new_ab*/,
                      const bool /*is_new_bc*/,
                      const bool /*is_isolated_b*/,
                      NonManifoldMeshTag) const
      {
        return (true);
      }

      //////////////////////////////////////////////////////////////////////////
      // checkAdjacency
      //////////////////////////////////////////////////////////////////////////

      bool
      checkAdjacency (const HalfEdgeIndex& /*idx_he_ab*/,
                      const HalfEdgeIndex& /*idx_he_bc*/,
                      const bool           /*is_new_ab*/,
                      const bool           /*is_new_bc*/,
                      bool&                /*make_adjacent_ab_bc*/,
                      HalfEdgeIndex&       /*idx_he_boundary*/,
                      ManifoldMeshTag)
      {
        return (true);
      }

      bool
      checkAdjacency (const HalfEdgeIndex& idx_he_ab,
                      const HalfEdgeIndex& idx_he_bc,
                      const bool           is_new_ab,
                      const bool           is_new_bc,
                      bool&                make_adjacent_ab_bc,
                      HalfEdgeIndex&       idx_he_boundary,
                      NonManifoldMeshTag)
      {
        if (is_new_ab || is_new_bc)
        {
          make_adjacent_ab_bc = false;
          return (true); // Make adjacent is only needed for two old half-edges
        }

        if (Base::getElement (idx_he_ab).getNextHalfEdgeIndex () == idx_he_bc)
        {
          make_adjacent_ab_bc = false;
          return (true); // already adjacent
        }

        make_adjacent_ab_bc = true;

        // Find the next boundary half edge (counter-clockwise around vertex b)
        OutgoingHalfEdgeAroundVertexConstCirculator circ = Base::getOutgoingHalfEdgeAroundVertexConstCirculator (Base::getElement (idx_he_ab).getOppositeHalfEdgeIndex ());

        do ++circ; while (!circ->isBoundary ());
        idx_he_boundary = circ.getDereferencedIndex ();

        // This would detach the faces around the vertex from each other.
        if (circ.getCurrentHalfEdgeIndex () == idx_he_bc) return (false);
        else                                              return (true);
      }

      //////////////////////////////////////////////////////////////////////////
      // makeAdjacent
      //////////////////////////////////////////////////////////////////////////

      void
      makeAdjacent (const HalfEdgeIndex& /*idx_he_ab*/,
                    const HalfEdgeIndex& /*idx_he_bc*/,
                    bool&                /*make_adjacent_ab_bc*/,
                    HalfEdgeIndex&       /*idx_he_boundary*/,
                    ManifoldMeshTag)
      {
        // Nothing to do here (manifold)
      }

      void
      makeAdjacent (const HalfEdgeIndex& idx_he_ab,
                    const HalfEdgeIndex& idx_he_bc,
                    bool&                make_adjacent_ab_bc,
                    HalfEdgeIndex&       idx_he_boundary,
                    NonManifoldMeshTag)
      {
        if (!make_adjacent_ab_bc) return;

        // Re-link. No references!
        const HalfEdgeIndex idx_he_ab_next       = Base::getElement (idx_he_ab).getNextHalfEdgeIndex ();
        const HalfEdgeIndex idx_he_bc_prev       = Base::getElement (idx_he_bc).getPrevHalfEdgeIndex ();
        const HalfEdgeIndex idx_he_boundary_prev = Base::getElement (idx_he_boundary).getPrevHalfEdgeIndex ();

        this->connectPrevNext (idx_he_ab, idx_he_bc);
        this->connectPrevNext (idx_he_boundary_prev, idx_he_ab_next);
        this->connectPrevNext (idx_he_bc_prev, idx_he_boundary);
      }

      //////////////////////////////////////////////////////////////////////////
      // connectNewNEw
      //////////////////////////////////////////////////////////////////////////

      void
      connectNewNew (const HalfEdgeIndex& idx_he_ab,
                     const HalfEdgeIndex& idx_he_ba,
                     const HalfEdgeIndex& idx_he_bc,
                     const HalfEdgeIndex& idx_he_cb,
                     const VertexIndex&   idx_v_b,
                     ManifoldMeshTag)
      {
        this->connectPrevNext (idx_he_ab, idx_he_bc);
        this->connectPrevNext (idx_he_cb, idx_he_ba);

        Base::getElement (idx_v_b).setOutgoingHalfEdgeIndex (idx_he_ba);
      }

      void
      connectNewNew (const HalfEdgeIndex& idx_he_ab,
                     const HalfEdgeIndex& idx_he_ba,
                     const HalfEdgeIndex& idx_he_bc,
                     const HalfEdgeIndex& idx_he_cb,
                     const VertexIndex&   idx_v_b,
                     NonManifoldMeshTag)
      {
        const Vertex& v_b = Base::getElement (idx_v_b);

        if (v_b.isIsolated ())
        {
          this->connectNewNew (idx_he_ab,idx_he_ba, idx_he_bc,idx_he_cb, idx_v_b, ManifoldMeshTag ());
        }
        else
        {
          // No references!
          const HalfEdgeIndex idx_he_b_out      = v_b.getOutgoingHalfEdgeIndex ();
          const HalfEdgeIndex idx_he_b_out_prev = Base::getElement (idx_he_b_out).getPrevHalfEdgeIndex ();

          this->connectPrevNext (idx_he_ab, idx_he_bc);
          this->connectPrevNext (idx_he_cb, idx_he_b_out);
          this->connectPrevNext (idx_he_b_out_prev, idx_he_ba);
        }
      }

      //////////////////////////////////////////////////////////////////////////
      // connectNewOld
      //////////////////////////////////////////////////////////////////////////

      void
      connectNewOld (const HalfEdgeIndex& idx_he_ab,
                     const HalfEdgeIndex& idx_he_ba,
                     const HalfEdgeIndex& idx_he_bc,
                     const HalfEdgeIndex& /*idx_he_cb*/,
                     const VertexIndex&   idx_v_b)
      {
        const HalfEdgeIndex idx_he_bc_prev = Base::getElement (idx_he_bc).getPrevHalfEdgeIndex (); // No reference!

        this->connectPrevNext (idx_he_ab, idx_he_bc);
        this->connectPrevNext (idx_he_bc_prev, idx_he_ba);

        Base::getElement (idx_v_b).setOutgoingHalfEdgeIndex (idx_he_ba);
      }

      //////////////////////////////////////////////////////////////////////////
      // connectOldNew
      //////////////////////////////////////////////////////////////////////////

      void
      connectOldNew (const HalfEdgeIndex& idx_he_ab,
                     const HalfEdgeIndex& /*idx_he_ba*/,
                     const HalfEdgeIndex& idx_he_bc,
                     const HalfEdgeIndex& idx_he_cb,
                     const VertexIndex&   idx_v_b)
      {
        const HalfEdgeIndex idx_he_ab_next = Base::getElement (idx_he_ab).getNextHalfEdgeIndex (); // No reference!

        this->connectPrevNext (idx_he_ab, idx_he_bc);
        this->connectPrevNext (idx_he_cb, idx_he_ab_next);

        Base::getElement (idx_v_b).setOutgoingHalfEdgeIndex (idx_he_ab_next);
      }

      //////////////////////////////////////////////////////////////////////////
      // connectOldOld
      //////////////////////////////////////////////////////////////////////////

      void
      connectOldOld (const HalfEdgeIndex& /*idx_he_ab*/,
                     const HalfEdgeIndex& /*idx_he_ba*/,
                     const HalfEdgeIndex& /*idx_he_bc*/,
                     const HalfEdgeIndex& /*idx_he_cb*/,
                     const VertexIndex&   /*idx_v_b*/,
                     ManifoldMeshTag)
      {
        // Nothing to do here (manifold)
      }

      void
      connectOldOld (const HalfEdgeIndex& /*idx_he_ab*/,
                     const HalfEdgeIndex& /*idx_he_ba*/,
                     const HalfEdgeIndex& idx_he_bc,
                     const HalfEdgeIndex& /*idx_he_cb*/,
                     const VertexIndex&   idx_v_b,
                     NonManifoldMeshTag)
      {
        Vertex& v_b = Base::getElement (idx_v_b);

        // The outgoing half edge MUST be a boundary half-edge (if there is one)
        if (v_b.getOutgoingHalfEdgeIndex () == idx_he_bc) // he_bc is no longer on the boundary
        {
          OutgoingHalfEdgeAroundVertexConstCirculator       circ     = Base::getOutgoingHalfEdgeAroundVertexConstCirculator (v_b);
          const OutgoingHalfEdgeAroundVertexConstCirculator circ_end = circ++;
          do
          {
            if (circ->isBoundary ())
            {
              v_b.setOutgoingHalfEdgeIndex (circ.getDereferencedIndex ());
              return;
            }
            ++circ;
          } while (circ!=circ_end);
        }
      }

      //////////////////////////////////////////////////////////////////////////
      // reconnect
      //////////////////////////////////////////////////////////////////////////

      void
      reconnect (const bool             is_boundary_ba,
                 const bool             is_boundary_cb,
                 const HalfEdgeIndex&   idx_he_ab,
                 const HalfEdgeIndex&   idx_he_ba,
                 const HalfEdgeIndex&   idx_he_bc,
                 const HalfEdgeIndex&   idx_he_cb,
                 std::stack<FaceIndex>& delete_faces)
      {
        if      ( is_boundary_ba &&  is_boundary_cb) this->reconnectBB   (idx_he_ab,idx_he_ba, idx_he_bc,idx_he_cb);
        else if ( is_boundary_ba && !is_boundary_cb) this->reconnectBNB  (idx_he_ab,idx_he_ba, idx_he_bc,idx_he_cb);
        else if (!is_boundary_ba &&  is_boundary_cb) this->reconnectNBB  (idx_he_ab,idx_he_ba, idx_he_bc,idx_he_cb);
        else                                         this->reconnectNBNB (idx_he_ab,idx_he_ba, idx_he_bc,idx_he_cb, delete_faces, IsManifold ());
      }

      //////////////////////////////////////////////////////////////////////////
      // reconnectBB (BoundaryBoundary)
      //////////////////////////////////////////////////////////////////////////

      void
      reconnectBB (const HalfEdgeIndex& idx_he_ab,
                   const HalfEdgeIndex& idx_he_ba,
                   const HalfEdgeIndex& /*idx_he_bc*/,
                   const HalfEdgeIndex& idx_he_cb)
      {
        const HalfEdgeIndex& idx_he_cb_next = Base::getElement (idx_he_cb).getNextHalfEdgeIndex ();

        if (idx_he_cb_next == idx_he_ba) // Vertex b is isolated
        {
          Base::getElement (idx_he_ab).getTerminatingVertex (*this).setDeleted (true);
        }
        else
        {
          this->connectPrevNext (Base::getElement (idx_he_ba).getPrevHalfEdgeIndex (), idx_he_cb_next);
          Base::getElement (idx_he_ab).getTerminatingVertex (*this).setOutgoingHalfEdgeIndex (idx_he_cb_next);
        }

        Base::getElement (idx_he_ab).setDeleted (true);
        Base::getElement (idx_he_ba).setDeleted (true);
        // he_bc.setDeleted (true); // already done in reconnectBNB or reconnectBB
        // he_cb.setDeleted (true);
      }

      //////////////////////////////////////////////////////////////////////////
      // reconnectBNB (BoundaryNoBoundary)
      //////////////////////////////////////////////////////////////////////////

      void
      reconnectBNB (const HalfEdgeIndex& idx_he_ab,
                    const HalfEdgeIndex& idx_he_ba,
                    const HalfEdgeIndex& idx_he_bc,
                    const HalfEdgeIndex& /*idx_he_cb*/)
      {
        this->connectPrevNext (Base::getElement (idx_he_ba).getPrevHalfEdgeIndex (), idx_he_bc);
        Base::getElement (idx_he_ab).getTerminatingVertex (*this).setOutgoingHalfEdgeIndex (idx_he_bc);

        Base::getElement (idx_he_ab).setDeleted (true);
        Base::getElement (idx_he_ba).setDeleted (true);
      }

      //////////////////////////////////////////////////////////////////////////
      // reconnectNBB (NoBoundaryBoundary)
      //////////////////////////////////////////////////////////////////////////

      void
      reconnectNBB (const HalfEdgeIndex& idx_he_ab,
                    const HalfEdgeIndex& /*idx_he_ba*/,
                    const HalfEdgeIndex& /*idx_he_bc*/,
                    const HalfEdgeIndex& idx_he_cb)
      {
        const HalfEdgeIndex& idx_he_cb_next = Base::getElement (idx_he_cb).getNextHalfEdgeIndex ();
        this->connectPrevNext (idx_he_ab, idx_he_cb_next);
        Base::getElement (idx_he_ab).getTerminatingVertex (*this).setOutgoingHalfEdgeIndex (idx_he_cb_next);

        // he_bc.setDeleted (true); // already done in reconnectBB
        // he_cb.setDeleted (true);
      }

      //////////////////////////////////////////////////////////////////////////
      // reconnectNBNB (NoBoundaryNoBoundary)
      //////////////////////////////////////////////////////////////////////////

      void
      reconnectNBNB (const HalfEdgeIndex&   idx_he_ab,
                     const HalfEdgeIndex&   idx_he_ba,
                     const HalfEdgeIndex&   idx_he_bc,
                     const HalfEdgeIndex&   /*idx_he_cb*/,
                     std::stack<FaceIndex>& delete_faces,
                     ManifoldMeshTag)
      {
        Vertex& v_b = Base::getElement (idx_he_ab).getTerminatingVertex (*this);

        if(v_b.isBoundary (*this))
        {
          // Deletion of this face makes the mesh non-manifold
          // -> delete the neighbouring faces until it is manifold again
          FaceAroundVertexConstCirculator circ = Base::getFaceAroundVertexConstCirculator (idx_he_ba);

          FaceIndex idx_face = (circ++).getDereferencedIndex ();

          while (idx_face.isValid ())
          {
            delete_faces.push (idx_face);
            idx_face = (circ++).getDereferencedIndex ();
          }
        }

        v_b.setOutgoingHalfEdgeIndex (idx_he_bc);
      }

      void
      reconnectNBNB (const HalfEdgeIndex&   idx_he_ab,
                     const HalfEdgeIndex&   /*idx_he_ba*/,
                     const HalfEdgeIndex&   idx_he_bc,
                     const HalfEdgeIndex&   /*idx_he_cb*/,
                     std::stack<FaceIndex>& /*delete_faces*/,
                     NonManifoldMeshTag)
      {
        Vertex& v_b = Base::getElement (idx_he_ab).getTerminatingVertex (*this);

        if(!v_b.isBoundary (*this))
        {
          v_b.setOutgoingHalfEdgeIndex (idx_he_bc);
        }
      }

      //////////////////////////////////////////////////////////////////////////
      // removeMeshElements
      //////////////////////////////////////////////////////////////////////////

      template <class MeshElementsT, class MeshElementIndexesT> MeshElementIndexesT
      removeMeshElements (MeshElementsT& mesh_elements,
                          const bool     remove_isolated) const
      {
        typedef MeshElementsT                         MeshElements;
        typedef typename MeshElements::iterator       MeshElementIterator;
        typedef typename MeshElements::const_iterator MeshElementConstIterator;

        typedef MeshElementIndexesT                     MeshElementIndexes;
        typedef typename MeshElementIndexes::value_type MeshElementIndex;
        typedef typename MeshElementIndexes::iterator   MeshElementIndexIterator;

        MeshElementIndexes new_mesh_element_indexes (mesh_elements.size (), MeshElementIndex ());
        MeshElementIndex   ind_new (0);

        MeshElementConstIterator it_me_old = mesh_elements.begin ();
        MeshElementIterator      it_me_new = mesh_elements.begin ();

        MeshElementIndexIterator       it_mei_new     = new_mesh_element_indexes.begin ();
        const MeshElementIndexIterator it_mei_new_end = new_mesh_element_indexes.end ();

        for (; it_mei_new!=it_mei_new_end; ++it_me_old, ++it_mei_new)
        {
          if (!(it_me_old->getDeleted () || (remove_isolated && it_me_old->isIsolated ())))
          {
            *it_me_new++ = *it_me_old; // test for self assignment?
            *it_mei_new  = ind_new++;
          }
        }

        mesh_elements.resize (ind_new.getIndex ());

        return (new_mesh_element_indexes);
      }

    private:

      struct EdgeWithBoundaryInfo
      {
          EdgeWithBoundaryInfo (const HalfEdgeIndex& idx_he_ab,
                                const HalfEdgeIndex& idx_he_ba,
                                const bool           is_boundary_ba)
            : idx_he_ab_      (idx_he_ab),
              idx_he_ba_      (idx_he_ba),
              is_boundary_ba_ (is_boundary_ba)
          {
          }

          HalfEdgeIndex idx_he_ab_;
          HalfEdgeIndex idx_he_ba_;
          bool          is_boundary_ba_;
      };
  };

} // End namespace pcl

#endif // PCL_GEOMETRY_MESH_TOPOLOGY_HPP
