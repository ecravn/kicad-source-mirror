/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jean-pierre.charras@gipsa-lab.inpg.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2011 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 1992-2011 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file class_drawsegment.cpp
 * @brief Class and functions to handle a graphic segments.
 */

#include <fctsys.h>
#include <macros.h>
#include <wxstruct.h>
#include <gr_basic.h>
#include <bezier_curves.h>
#include <class_drawpanel.h>
#include <class_pcb_screen.h>
#include <trigo.h>
#include <msgpanel.h>
#include <bitmaps.h>

#include <wxPcbStruct.h>

#include <pcbnew.h>

#include <class_board.h>
#include <class_module.h>
#include <class_drawsegment.h>
#include <base_units.h>


DRAWSEGMENT::DRAWSEGMENT( BOARD_ITEM* aParent, KICAD_T idtype ) :
    BOARD_ITEM( aParent, idtype )
{
    m_Type  = 0;
    m_Angle = 0;
    m_Flags = 0;
    m_Shape = S_SEGMENT;
    m_Width = Millimeter2iu( 0.15 );    // Gives a decent width
}


DRAWSEGMENT::~DRAWSEGMENT()
{
}


void DRAWSEGMENT::Rotate( const wxPoint& aRotCentre, double aAngle )
{
    switch( m_Shape )
    {
    case S_ARC:
    case S_SEGMENT:
    case S_CIRCLE:
        // these can all be done by just rotating the start and end points
        RotatePoint( &m_Start, aRotCentre, aAngle);
        RotatePoint( &m_End, aRotCentre, aAngle);
        break;

    case S_POLYGON:
        for( unsigned ii = 0; ii < m_PolyPoints.size(); ii++ )
        {
            RotatePoint( &m_PolyPoints[ii], aRotCentre, aAngle);
        }
        break;

    case S_CURVE:
        RotatePoint( &m_Start, aRotCentre, aAngle);
        RotatePoint( &m_End, aRotCentre, aAngle);

        for( unsigned int ii = 0; ii < m_BezierPoints.size(); ii++ )
        {
            RotatePoint( &m_BezierPoints[ii], aRotCentre, aAngle);
        }
        break;

    case S_RECT:
    default:
        // un-handled edge transform
        wxASSERT_MSG( false, wxT( "DRAWSEGMENT::Rotate not implemented for "
                + ShowShape( m_Shape ) ) );
        break;
    }
};

void DRAWSEGMENT::Flip( const wxPoint& aCentre )
{
    m_Start.y  = aCentre.y - (m_Start.y - aCentre.y);
    m_End.y  = aCentre.y - (m_End.y - aCentre.y);

    if( m_Shape == S_ARC )
        m_Angle = -m_Angle;

    // DRAWSEGMENT items are not allowed on copper layers, so
    // copper layers count is not taken in accoun in Flip transform
    SetLayer( FlipLayer( GetLayer() ) );
}

const wxPoint DRAWSEGMENT::GetCenter() const
{
    wxPoint c;

    switch( m_Shape )
    {
    case S_ARC:
    case S_CIRCLE:
        c = m_Start;
        break;

    case S_SEGMENT:
        // Midpoint of the line
        c = ( GetStart() + GetEnd() ) / 2;
        break;

    case S_POLYGON:
    case S_RECT:
    case S_CURVE:
        c = GetBoundingBox().Centre();
        break;

    default:
        wxASSERT_MSG( false, "DRAWSEGMENT::GetCentre not implemented for shape"
                + ShowShape( GetShape() ) );
        break;
    }

    return c;
}

const wxPoint DRAWSEGMENT::GetArcEnd() const
{
    wxPoint endPoint;         // start of arc

    switch( m_Shape )
    {
    case S_ARC:
        // rotate the starting point of the arc, given by m_End, through the
        // angle m_Angle to get the ending point of the arc.
        // m_Start is the arc centre
        endPoint  = m_End;         // m_End = start point of arc
        RotatePoint( &endPoint, m_Start, -m_Angle );
        break;

    default:
        ;
    }

    return endPoint;   // after rotation, the end of the arc.
}

double DRAWSEGMENT::GetArcAngleStart() const
{
    // due to the Y axis orient atan2 needs - y value
    double angleStart = ArcTangente( GetArcStart().y - GetCenter().y,
                                     GetArcStart().x - GetCenter().x );

    // Normalize it to 0 ... 360 deg, to avoid discontinuity for angles near 180 deg
    // because 180 deg and -180 are very near angles when ampping betewwen -180 ... 180 deg.
    // and this is not easy to handle in calculations
    NORMALIZE_ANGLE_POS( angleStart );

    return angleStart;
}


void DRAWSEGMENT::SetAngle( double aAngle )
{
    m_Angle = NormalizeAngle360( aAngle );
}


MODULE* DRAWSEGMENT::GetParentModule() const
{
    if( !m_Parent || m_Parent->Type() != PCB_MODULE_T )
        return NULL;

    return (MODULE*) m_Parent;
}


void DRAWSEGMENT::Draw( EDA_DRAW_PANEL* panel, wxDC* DC, GR_DRAWMODE draw_mode,
                        const wxPoint& aOffset )
{
    int ux0, uy0, dx, dy;
    int l_trace;
    int radius;

    PCB_LAYER_ID    curr_layer = ( (PCB_SCREEN*) panel->GetScreen() )->m_Active_Layer;

    BOARD * brd =  GetBoard( );

    if( brd->IsLayerVisible( GetLayer() ) == false )
        return;

    auto frame = static_cast<PCB_EDIT_FRAME*> ( panel->GetParent() );
    auto color = frame->Settings().Colors().GetLayerColor( GetLayer() );

    DISPLAY_OPTIONS* displ_opts = (DISPLAY_OPTIONS*)panel->GetDisplayOptions();

    if( ( draw_mode & GR_ALLOW_HIGHCONTRAST ) &&  displ_opts && displ_opts->m_ContrastModeDisplay )
    {
        if( !IsOnLayer( curr_layer ) && !IsOnLayer( Edge_Cuts ) )
            color = COLOR4D( DARKDARKGRAY );
    }

    GRSetDrawMode( DC, draw_mode );
    l_trace = m_Width >> 1;         // half trace width

    // Line start point or Circle and Arc center
    ux0 = m_Start.x + aOffset.x;
    uy0 = m_Start.y + aOffset.y;

    // Line end point or circle and arc start point
    dx = m_End.x + aOffset.x;
    dy = m_End.y + aOffset.y;

    bool filled = displ_opts ? displ_opts->m_DisplayDrawItemsFill : FILLED;

    if( m_Flags & FORCE_SKETCH )
        filled = SKETCH;

    switch( m_Shape )
    {
    case S_CIRCLE:
        radius = KiROUND( Distance( ux0, uy0, dx, dy ) );

        if( filled )
        {
            GRCircle( panel->GetClipBox(), DC, ux0, uy0, radius, m_Width, color );
        }
        else
        {
            GRCircle( panel->GetClipBox(), DC, ux0, uy0, radius - l_trace, color );
            GRCircle( panel->GetClipBox(), DC, ux0, uy0, radius + l_trace, color );
        }

        break;

    case S_ARC:
        double StAngle, EndAngle;
        radius   = KiROUND( Distance( ux0, uy0, dx, dy ) );
        StAngle  = ArcTangente( dy - uy0, dx - ux0 );
        EndAngle = StAngle + m_Angle;

        if( !panel->GetPrintMirrored() )
        {
            if( StAngle > EndAngle )
                std::swap( StAngle, EndAngle );
        }
        else    // Mirrored mode: arc orientation is reversed
        {
            if( StAngle < EndAngle )
                std::swap( StAngle, EndAngle );
        }

        if( filled )
        {
            GRArc( panel->GetClipBox(), DC, ux0, uy0, StAngle, EndAngle,
                   radius, m_Width, color );
        }
        else
        {
            GRArc( panel->GetClipBox(), DC, ux0, uy0, StAngle, EndAngle,
                   radius - l_trace, color );
            GRArc( panel->GetClipBox(), DC, ux0, uy0, StAngle, EndAngle,
                   radius + l_trace, color );
        }

        break;

    case S_CURVE:
        {
            std::vector<wxPoint> ctrlPoints = { m_Start, m_BezierC1, m_BezierC2, m_End };
            BEZIER_POLY converter( ctrlPoints );
            converter.GetPoly( m_BezierPoints );
        }

        for( unsigned int i=1; i < m_BezierPoints.size(); i++ )
        {
            if( filled )
            {
                GRFillCSegm( panel->GetClipBox(), DC,
                             m_BezierPoints[i].x, m_BezierPoints[i].y,
                             m_BezierPoints[i-1].x, m_BezierPoints[i-1].y,
                             m_Width, color );
            }
            else
            {
                GRCSegm( panel->GetClipBox(), DC,
                         m_BezierPoints[i].x, m_BezierPoints[i].y,
                         m_BezierPoints[i-1].x, m_BezierPoints[i-1].y,
                         m_Width, color );
            }
        }

        break;

    default:
        if( filled )
        {
            GRFillCSegm( panel->GetClipBox(), DC, ux0, uy0, dx, dy, m_Width, color );
        }
        else
        {
            GRCSegm( panel->GetClipBox(), DC, ux0, uy0, dx, dy, m_Width, color );
        }

        break;
    }
}


// see pcbstruct.h
void DRAWSEGMENT::GetMsgPanelInfo( std::vector< MSG_PANEL_ITEM >& aList )
{
    wxString msg;
    wxASSERT( m_Parent );

    msg = _( "Drawing" );

    aList.push_back( MSG_PANEL_ITEM( _( "Type" ), msg, DARKCYAN ) );

    wxString    shape = _( "Shape" );

    switch( m_Shape )
    {
    case S_CIRCLE:
        aList.push_back( MSG_PANEL_ITEM( shape, _( "Circle" ), RED ) );
        break;

    case S_ARC:
        aList.push_back( MSG_PANEL_ITEM( shape, _( "Arc" ), RED ) );
        msg.Printf( wxT( "%.1f" ), m_Angle / 10.0 );
        aList.push_back( MSG_PANEL_ITEM( _( "Angle" ), msg, RED ) );
        break;

    case S_CURVE:
        aList.push_back( MSG_PANEL_ITEM( shape, _( "Curve" ), RED ) );
        break;

    default:
    {
        aList.push_back( MSG_PANEL_ITEM( shape, _( "Segment" ), RED ) );

        msg = ::CoordinateToString( GetLineLength( m_Start, m_End ) );
        aList.push_back( MSG_PANEL_ITEM( _( "Length" ), msg, DARKGREEN ) );

        // angle counter-clockwise from 3'o-clock
        const double deg = RAD2DEG( atan2( (double)( m_Start.y - m_End.y ),
                                           (double)( m_End.x - m_Start.x ) ) );
        msg.Printf( wxT( "%.1f" ), deg );
        aList.push_back( MSG_PANEL_ITEM( _( "Angle" ), msg, DARKGREEN ) );
    }
    }

    wxString start;
    start << GetStart();

    wxString end;
    end << GetEnd();

    aList.push_back( MSG_PANEL_ITEM( start, end, DARKGREEN ) );
    aList.push_back( MSG_PANEL_ITEM( _( "Layer" ), GetLayerName(), DARKBROWN ) );
    msg = ::CoordinateToString( m_Width );
    aList.push_back( MSG_PANEL_ITEM( _( "Width" ), msg, DARKCYAN ) );
}


const EDA_RECT DRAWSEGMENT::GetBoundingBox() const
{
    EDA_RECT bbox;

    bbox.SetOrigin( m_Start );

    switch( m_Shape )
    {
    case S_SEGMENT:
        bbox.SetEnd( m_End );
        break;

    case S_CIRCLE:
        bbox.Inflate( GetRadius() );
        break;

    case S_ARC:
        computeArcBBox( bbox );
        break;

    case S_POLYGON:
    {
        wxPoint p_end;
        MODULE* module = GetParentModule();

        for( unsigned ii = 0; ii < m_PolyPoints.size(); ii++ )
        {
            wxPoint pt = m_PolyPoints[ii];

            if( module ) // Transform, if we belong to a module
            {
                RotatePoint( &pt, module->GetOrientation() );
                pt += module->GetPosition();
            }

            if( ii == 0 )
                p_end = pt;

            bbox.SetX( std::min( bbox.GetX(), pt.x ) );
            bbox.SetY( std::min( bbox.GetY(), pt.y ) );
            p_end.x   = std::max( p_end.x, pt.x );
            p_end.y   = std::max( p_end.y, pt.y );
        }

        bbox.SetEnd( p_end );
    }
        break;

    default:
        break;
    }

    bbox.Inflate( ((m_Width+1) / 2) + 1 );
    bbox.Normalize();

    return bbox;
}


bool DRAWSEGMENT::HitTest( const wxPoint& aPosition ) const
{
    switch( m_Shape )
    {
    case S_CIRCLE:
    case S_ARC:
    {
        wxPoint relPos = aPosition - GetCenter();
        int radius = GetRadius();
        int dist   = KiROUND( EuclideanNorm( relPos ) );

        if( abs( radius - dist ) <= ( m_Width / 2 ) )
        {
            if( m_Shape == S_CIRCLE )
                return true;

            // For arcs, the test point angle must be >= arc angle start
            // and <= arc angle end
            // However angle values > 360 deg are not easy to handle
            // so we calculate the relative angle between arc start point and teast point
            // this relative arc should be < arc angle if arc angle > 0 (CW arc)
            // and > arc angle if arc angle < 0 (CCW arc)
            double arc_angle_start = GetArcAngleStart();    // Always 0.0 ... 360 deg, in 0.1 deg

            double arc_hittest = ArcTangente( relPos.y, relPos.x );

            // Calculate relative angle between the starting point of the arc, and the test point
            arc_hittest -= arc_angle_start;

            // Normalise arc_hittest between 0 ... 360 deg
            NORMALIZE_ANGLE_POS( arc_hittest );

            // Check angle: inside the arc angle when it is > 0
            // and outside the not drawn arc when it is < 0
            if( GetAngle() >= 0.0 )
            {
                if( arc_hittest <= GetAngle() )
                    return true;
            }
            else
            {
                if( arc_hittest >= (3600.0 + GetAngle()) )
                    return true;
            }
        }
    }
        break;

    case S_CURVE:
        for( unsigned int i= 1; i < m_BezierPoints.size(); i++)
        {
            if( TestSegmentHit( aPosition, m_BezierPoints[i-1], m_BezierPoints[i-1], m_Width / 2 ) )
                return true;
        }
        break;

    case S_SEGMENT:
        if( TestSegmentHit( aPosition, m_Start, m_End, m_Width / 2 ) )
            return true;
        break;

    case S_POLYGON:     // not yet handled
        break;

    default:
        wxASSERT_MSG( 0, wxString::Format( "unknown DRAWSEGMENT shape: %d", m_Shape ) );
        break;
    }

    return false;
}


bool DRAWSEGMENT::HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy ) const
{
    EDA_RECT arect = aRect;
    arect.Normalize();
    arect.Inflate( aAccuracy );

    EDA_RECT arcRect;
    EDA_RECT bb = GetBoundingBox();

    switch( m_Shape )
    {
    case S_CIRCLE:
        // Test if area intersects or contains the circle:
        if( aContained )
            return arect.Contains( bb );
        else
        {
            // If the rectangle does not intersect the bounding box, this is a much quicker test
            if( !aRect.Intersects( bb ) )
            {
                return false;
            }
            else
            {
                return arect.IntersectsCircleEdge( GetCenter(), GetRadius(), GetWidth() );
            }

        }
        break;

    case S_ARC:

        // Test for full containment of this arc in the rect
        if( aContained )
        {
            return arect.Contains( bb );
        }
        // Test if the rect crosses the arc
        else
        {
            arcRect = bb.Common( arect );

            /* All following tests must pass:
             * 1. Rectangle must intersect arc BoundingBox
             * 2. Rectangle must cross the outside of the arc
             */
            return arcRect.Intersects( arect ) &&
                   arcRect.IntersectsCircleEdge( GetCenter(), GetRadius(), GetWidth() );
        }
        break;
    case S_SEGMENT:
        if( aContained )
        {
            return arect.Contains( GetStart() ) && aRect.Contains( GetEnd() );
        }
        else
        {
            // Account for the width of the line
            arect.Inflate( GetWidth() / 2 );
            return arect.Intersects( GetStart(), GetEnd() );
        }

        break;

    case S_CURVE:
    case S_POLYGON:     // not yet handled
        break;

    default:
        wxASSERT_MSG( 0, wxString::Format( "unknown DRAWSEGMENT shape: %d", m_Shape ) );
        break;
    }

    return false;
}


wxString DRAWSEGMENT::GetSelectMenuText() const
{
    wxString text;
    wxString temp = ::LengthDoubleToString( GetLength() );

    text.Printf( _( "Pcb Graphic: %s, length %s on %s" ),
                 GetChars( ShowShape( m_Shape ) ),
                 GetChars( temp ), GetChars( GetLayerName() ) );

    return text;
}


BITMAP_DEF DRAWSEGMENT::GetMenuImage() const
{
    return add_dashed_line_xpm;
}


EDA_ITEM* DRAWSEGMENT::Clone() const
{
    return new DRAWSEGMENT( *this );
}


const BOX2I DRAWSEGMENT::ViewBBox() const
{
    // For arcs - do not include the center point in the bounding box,
    // it is redundant for displaying an arc
    if( m_Shape == S_ARC )
    {
        EDA_RECT bbox;
        bbox.SetOrigin( m_End );
        computeArcBBox( bbox );
        return BOX2I( bbox.GetOrigin(), bbox.GetSize() );
    }

    return EDA_ITEM::ViewBBox();
}


void DRAWSEGMENT::computeArcBBox( EDA_RECT& aBBox ) const
{
    // Do not include the center, which is not necessarily
    // inside the BB of a arc with a small angle
    aBBox.SetOrigin( m_End );

    wxPoint end = m_End;
    RotatePoint( &end, m_Start, -m_Angle );
    aBBox.Merge( end );

    // Determine the starting quarter
    // 0 right-bottom
    // 1 left-bottom
    // 2 left-top
    // 3 right-top
    unsigned int quarter = 0;       // assume right-bottom

    if( m_End.x < m_Start.x )
    {
        if( m_End.y <= m_Start.y )
            quarter = 2;
        else // ( m_End.y > m_Start.y )
            quarter = 1;
    }
    else if( m_End.x >= m_Start.x )
    {
        if( m_End.y < m_Start.y )
            quarter = 3;
        else if( m_End.x == m_Start.x )
            quarter = 1;
    }

    int radius = GetRadius();
    int angle = (int) GetArcAngleStart() % 900 + m_Angle;
    bool directionCW = ( m_Angle > 0 );      // Is the direction of arc clockwise?

    // Make the angle positive, so we go clockwise and merge points belonging to the arc
    if( !directionCW )
    {
        angle = 900 - angle;
        quarter = ( quarter + 3 ) % 4;       // -1 modulo arithmetic
    }

    while( angle > 900 )
    {
        switch( quarter )
        {
        case 0:
            aBBox.Merge( wxPoint( m_Start.x, m_Start.y + radius ) );     // down
            break;

        case 1:
            aBBox.Merge( wxPoint( m_Start.x - radius, m_Start.y ) );     // left
            break;

        case 2:
            aBBox.Merge( wxPoint( m_Start.x, m_Start.y - radius ) );     // up
            break;

        case 3:
            aBBox.Merge( wxPoint( m_Start.x + radius, m_Start.y ) );     // right
            break;
        }

        if( directionCW )
            ++quarter;
        else
            quarter += 3;       // -1 modulo arithmetic

        quarter %= 4;
        angle -= 900;
    }
}
